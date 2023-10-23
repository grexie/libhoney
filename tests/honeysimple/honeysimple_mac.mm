// Copyright (c) 2013 The Honeycomb Authors.
// Portions copyright (c) 2010 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import <Cocoa/Cocoa.h>

#include "include/honey_application_mac.h"
#include "include/honey_command_line.h"
#include "include/wrapper/honey_helpers.h"
#include "include/wrapper/honey_library_loader.h"
#include "tests/honeysimple/simple_app.h"
#include "tests/honeysimple/simple_handler.h"

// Receives notifications from the application.
@interface SimpleAppDelegate : NSObject <NSApplicationDelegate>

- (void)createApplication:(id)object;
- (void)tryToTerminateApplication:(NSApplication*)app;
@end

// Provide the HoneycombAppProtocol implementation required by Honeycomb.
@interface SimpleApplication : NSApplication <HoneycombAppProtocol> {
 @private
  BOOL handlingSendEvent_;
}
@end

@implementation SimpleApplication
- (BOOL)isHandlingSendEvent {
  return handlingSendEvent_;
}

- (void)setHandlingSendEvent:(BOOL)handlingSendEvent {
  handlingSendEvent_ = handlingSendEvent;
}

- (void)sendEvent:(NSEvent*)event {
  HoneycombScopedSendingEvent sendingEventScoper;
  [super sendEvent:event];
}

// |-terminate:| is the entry point for orderly "quit" operations in Cocoa. This
// includes the application menu's quit menu item and keyboard equivalent, the
// application's dock icon menu's quit menu item, "quit" (not "force quit") in
// the Activity Monitor, and quits triggered by user logout and system restart
// and shutdown.
//
// The default |-terminate:| implementation ends the process by calling exit(),
// and thus never leaves the main run loop. This is unsuitable for Chromium
// since Chromium depends on leaving the main run loop to perform an orderly
// shutdown. We support the normal |-terminate:| interface by overriding the
// default implementation. Our implementation, which is very specific to the
// needs of Chromium, works by asking the application delegate to terminate
// using its |-tryToTerminateApplication:| method.
//
// |-tryToTerminateApplication:| differs from the standard
// |-applicationShouldTerminate:| in that no special event loop is run in the
// case that immediate termination is not possible (e.g., if dialog boxes
// allowing the user to cancel have to be shown). Instead, this method tries to
// close all browsers by calling CloseBrowser(false) via
// ClientHandler::CloseAllBrowsers. Calling CloseBrowser will result in a call
// to ClientHandler::DoClose and execution of |-performClose:| on the NSWindow.
// DoClose sets a flag that is used to differentiate between new close events
// (e.g., user clicked the window close button) and in-progress close events
// (e.g., user approved the close window dialog). The NSWindowDelegate
// |-windowShouldClose:| method checks this flag and either calls
// CloseBrowser(false) in the case of a new close event or destructs the
// NSWindow in the case of an in-progress close event.
// ClientHandler::OnBeforeClose will be called after the Honeycomb NSView hosted in
// the NSWindow is dealloc'ed.
//
// After the final browser window has closed ClientHandler::OnBeforeClose will
// begin actual tear-down of the application by calling HoneycombQuitMessageLoop.
// This ends the NSApplication event loop and execution then returns to the
// main() function for cleanup before application termination.
//
// The standard |-applicationShouldTerminate:| is not supported, and code paths
// leading to it must be redirected.
- (void)terminate:(id)sender {
  SimpleAppDelegate* delegate =
      static_cast<SimpleAppDelegate*>([NSApp delegate]);
  [delegate tryToTerminateApplication:self];
  // Return, don't exit. The application is responsible for exiting on its own.
}
@end

@implementation SimpleAppDelegate

// Create the application on the UI thread.
- (void)createApplication:(id)object {
  [[NSBundle mainBundle] loadNibNamed:@"MainMenu"
                                owner:NSApp
                      topLevelObjects:nil];

  // Set the delegate for application events.
  [[NSApplication sharedApplication] setDelegate:self];
}

- (void)tryToTerminateApplication:(NSApplication*)app {
  SimpleHandler* handler = SimpleHandler::GetInstance();
  if (handler && !handler->IsClosing()) {
    handler->CloseAllBrowsers(false);
  }
}

- (NSApplicationTerminateReply)applicationShouldTerminate:
    (NSApplication*)sender {
  return NSTerminateNow;
}
@end

// Entry point function for the browser process.
int main(int argc, char* argv[]) {
  // Load the Honeycomb framework library at runtime instead of linking directly
  // as required by the macOS sandbox implementation.
  HoneycombScopedLibraryLoader library_loader;
  if (!library_loader.LoadInMain()) {
    return 1;
  }

  // Provide Honeycomb with command-line arguments.
  HoneycombMainArgs main_args(argc, argv);

  @autoreleasepool {
    // Initialize the SimpleApplication instance.
    [SimpleApplication sharedApplication];

    // If there was an invocation to NSApp prior to this method, then the NSApp
    // will not be a SimpleApplication, but will instead be an NSApplication.
    // This is undesirable and we must enforce that this doesn't happen.
    CHECK([NSApp isKindOfClass:[SimpleApplication class]]);

    // Parse command-line arguments for use in this method.
    HoneycombRefPtr<HoneycombCommandLine> command_line =
        HoneycombCommandLine::CreateCommandLine();
    command_line->InitFromArgv(argc, argv);

    // Specify Honeycomb global settings here.
    HoneycombSettings settings;

    const bool with_chrome_runtime =
        command_line->HasSwitch("enable-chrome-runtime");

    if (with_chrome_runtime) {
      // Enable experimental Chrome runtime. See issue #2969 for details.
      settings.chrome_runtime = true;
    }

    // When generating projects with CMake the HONEYCOMB_USE_SANDBOX value will be
    // defined automatically. Pass -DUSE_SANDBOX=OFF to the CMake command-line
    // to disable use of the sandbox.
#if !defined(HONEYCOMB_USE_SANDBOX)
    settings.no_sandbox = true;
#endif

    // SimpleApp implements application-level callbacks for the browser process.
    // It will create the first browser instance in OnContextInitialized() after
    // Honeycomb has initialized.
    HoneycombRefPtr<SimpleApp> app(new SimpleApp);

    // Initialize Honeycomb for the browser process.
    HoneycombInitialize(main_args, settings, app.get(), nullptr);

    // Create the application delegate.
    NSObject* delegate = [[SimpleAppDelegate alloc] init];
    [delegate performSelectorOnMainThread:@selector(createApplication:)
                               withObject:nil
                            waitUntilDone:NO];

    // Run the Honeycomb message loop. This will block until HoneycombQuitMessageLoop() is
    // called.
    HoneycombRunMessageLoop();

    // Shut down Honeycomb.
    HoneycombShutdown();

    // Release the delegate.
#if !__has_feature(objc_arc)
    [delegate release];
#endif  // !__has_feature(objc_arc)
    delegate = nil;
  }  // @autoreleasepool

  return 0;
}
