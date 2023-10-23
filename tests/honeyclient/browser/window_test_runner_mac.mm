// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeyclient/browser/window_test_runner_mac.h"

#import <Cocoa/Cocoa.h>

#include "include/wrapper/honey_helpers.h"
#include "tests/shared/browser/main_message_loop.h"

namespace client {
namespace window_test {

namespace {

NSWindow* GetWindow(HoneycombRefPtr<HoneycombBrowser> browser) {
  NSView* view =
      CAST_HONEYCOMB_WINDOW_HANDLE_TO_NSVIEW(browser->GetHost()->GetWindowHandle());
  return [view window];
}

}  // namespace

WindowTestRunnerMac::WindowTestRunnerMac() {}

void WindowTestRunnerMac::SetPos(HoneycombRefPtr<HoneycombBrowser> browser,
                                 int x,
                                 int y,
                                 int width,
                                 int height) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  REQUIRE_MAIN_THREAD();

  NSWindow* window = GetWindow(browser);

  // Make sure the window isn't minimized or maximized.
  if ([window isMiniaturized]) {
    [window deminiaturize:nil];
  } else if ([window isZoomed]) {
    [window performZoom:nil];
  }

  // Retrieve information for the display that contains the window.
  NSScreen* screen = [window screen];
  if (screen == nil) {
    screen = [NSScreen mainScreen];
  }
  NSRect frame = [screen frame];
  NSRect visibleFrame = [screen visibleFrame];

  // Make sure the window is inside the display.
  HoneycombRect display_rect(
      visibleFrame.origin.x,
      frame.size.height - visibleFrame.size.height - visibleFrame.origin.y,
      visibleFrame.size.width, visibleFrame.size.height);
  HoneycombRect window_rect(x, y, width, height);
  ModifyBounds(display_rect, window_rect);

  NSRect newRect;
  newRect.origin.x = window_rect.x;
  newRect.origin.y = frame.size.height - window_rect.height - window_rect.y;
  newRect.size.width = window_rect.width;
  newRect.size.height = window_rect.height;
  [window setFrame:newRect display:YES];
}

void WindowTestRunnerMac::Minimize(HoneycombRefPtr<HoneycombBrowser> browser) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  REQUIRE_MAIN_THREAD();

  [GetWindow(browser) performMiniaturize:nil];
}

void WindowTestRunnerMac::Maximize(HoneycombRefPtr<HoneycombBrowser> browser) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  REQUIRE_MAIN_THREAD();

  [GetWindow(browser) performZoom:nil];
}

void WindowTestRunnerMac::Restore(HoneycombRefPtr<HoneycombBrowser> browser) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  REQUIRE_MAIN_THREAD();

  NSWindow* window = GetWindow(browser);
  if ([window isMiniaturized]) {
    [window deminiaturize:nil];
  } else if ([window isZoomed]) {
    [window performZoom:nil];
  }
}

}  // namespace window_test
}  // namespace client
