// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeysimple/simple_app.h"

#if defined(HONEYCOMB_X11)
#include <X11/Xlib.h>
#endif

#include "include/base/honey_logging.h"
#include "include/honey_command_line.h"

#if defined(HONEYCOMB_X11)
namespace {

int XErrorHandlerImpl(Display* display, XErrorEvent* event) {
  LOG(WARNING) << "X error received: "
               << "type " << event->type << ", "
               << "serial " << event->serial << ", "
               << "error_code " << static_cast<int>(event->error_code) << ", "
               << "request_code " << static_cast<int>(event->request_code)
               << ", "
               << "minor_code " << static_cast<int>(event->minor_code);
  return 0;
}

int XIOErrorHandlerImpl(Display* display) {
  return 0;
}

}  // namespace
#endif  // defined(HONEYCOMB_X11)

// Entry point function for all processes.
int main(int argc, char* argv[]) {
  // Provide Honeycomb with command-line arguments.
  HoneycombMainArgs main_args(argc, argv);

  // Honeycomb applications have multiple sub-processes (render, GPU, etc) that share
  // the same executable. This function checks the command-line and, if this is
  // a sub-process, executes the appropriate logic.
  int exit_code = HoneycombExecuteProcess(main_args, nullptr, nullptr);
  if (exit_code >= 0) {
    // The sub-process has completed so return here.
    return exit_code;
  }

#if defined(HONEYCOMB_X11)
  // Install xlib error handlers so that the application won't be terminated
  // on non-fatal errors.
  XSetErrorHandler(XErrorHandlerImpl);
  XSetIOErrorHandler(XIOErrorHandlerImpl);
#endif

  // Parse command-line arguments for use in this method.
  HoneycombRefPtr<HoneycombCommandLine> command_line = HoneycombCommandLine::CreateCommandLine();
  command_line->InitFromArgv(argc, argv);

  // Specify Honeycomb global settings here.
  HoneycombSettings settings;

  if (command_line->HasSwitch("enable-chrome-runtime")) {
    // Enable experimental Chrome runtime. See issue #2969 for details.
    settings.chrome_runtime = true;
  }

// When generating projects with CMake the HONEYCOMB_USE_SANDBOX value will be defined
// automatically. Pass -DUSE_SANDBOX=OFF to the CMake command-line to disable
// use of the sandbox.
#if !defined(HONEYCOMB_USE_SANDBOX)
  settings.no_sandbox = true;
#endif

  // SimpleApp implements application-level callbacks for the browser process.
  // It will create the first browser instance in OnContextInitialized() after
  // Honeycomb has initialized.
  HoneycombRefPtr<SimpleApp> app(new SimpleApp);

  // Initialize Honeycomb for the browser process.
  HoneycombInitialize(main_args, settings, app.get(), nullptr);

  // Run the Honeycomb message loop. This will block until HoneycombQuitMessageLoop() is
  // called.
  HoneycombRunMessageLoop();

  // Shut down Honeycomb.
  HoneycombShutdown();

  return 0;
}
