// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file.

#import <Cocoa/Cocoa.h>

#include "tests/honeytests/os_rendering_unittest_mac.h"

namespace osr_unittests {

HoneycombWindowHandle GetFakeView() {
  NSScreen* mainScreen = [NSScreen mainScreen];
  NSRect screenRect = [mainScreen visibleFrame];
  NSView* fakeView = [[NSView alloc] initWithFrame:screenRect];
  return CAST_NSVIEW_TO_HONEYCOMB_WINDOW_HANDLE(fakeView);
}

}  // namespace osr_unittests
