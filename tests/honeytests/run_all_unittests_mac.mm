// Copyright (c) 2011 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/honey_app.h"
#import "include/honey_application_mac.h"

#if !__has_feature(objc_arc)
// Memory AutoRelease pool.
static NSAutoreleasePool* g_autopool = nil;
#endif

// Provide the HoneycombAppProtocol implementation required by Honeycomb.
@interface TestApplication : NSApplication <HoneycombAppProtocol> {
 @private
  BOOL handlingSendEvent_;
}
@end

@implementation TestApplication
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
@end

void PlatformInit() {
#if !__has_feature(objc_arc)
  // Initialize the AutoRelease pool.
  g_autopool = [[NSAutoreleasePool alloc] init];
#endif

  // Initialize the TestApplication instance.
  [TestApplication sharedApplication];
}

void PlatformCleanup() {
#if !__has_feature(objc_arc)
  // Release the AutoRelease pool.
  [g_autopool release];
#endif
}
