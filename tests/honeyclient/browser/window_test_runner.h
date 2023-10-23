// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_WINDOW_TEST_RUNNER_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_WINDOW_TEST_RUNNER_H_
#pragma once

#include "include/honey_browser.h"

#include <optional>

namespace client {
namespace window_test {

// Implement this interface for different platforms. Methods will be called on
// the browser process UI thread unless otherwise indicated.
class WindowTestRunner {
 public:
  virtual ~WindowTestRunner() = default;

  virtual void SetPos(HoneycombRefPtr<HoneycombBrowser> browser,
                      int x,
                      int y,
                      int width,
                      int height) = 0;
  virtual void Minimize(HoneycombRefPtr<HoneycombBrowser> browser) = 0;
  virtual void Maximize(HoneycombRefPtr<HoneycombBrowser> browser) = 0;
  virtual void Restore(HoneycombRefPtr<HoneycombBrowser> browser) = 0;
  virtual void Fullscreen(HoneycombRefPtr<HoneycombBrowser> browser);

  // Fit |window| inside |display|. Coordinates are relative to the upper-left
  // corner of the display.
  static void ModifyBounds(const HoneycombRect& display, HoneycombRect& window);

  virtual void SetTitleBarHeight(HoneycombRefPtr<HoneycombBrowser> browser,
                                 const std::optional<float>& height);
};

}  // namespace window_test
}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_WINDOW_TEST_RUNNER_H_
