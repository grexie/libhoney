// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_WINDOW_TEST_RUNNER_VIEWS_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_WINDOW_TEST_RUNNER_VIEWS_H_
#pragma once

#include "tests/honeyclient/browser/window_test_runner.h"

namespace client {
namespace window_test {

// Views platform implementation.
class WindowTestRunnerViews : public WindowTestRunner {
 public:
  WindowTestRunnerViews();

  void SetPos(HoneycombRefPtr<HoneycombBrowser> browser,
              int x,
              int y,
              int width,
              int height) override;
  void Minimize(HoneycombRefPtr<HoneycombBrowser> browser) override;
  void Maximize(HoneycombRefPtr<HoneycombBrowser> browser) override;
  void Restore(HoneycombRefPtr<HoneycombBrowser> browser) override;
  void Fullscreen(HoneycombRefPtr<HoneycombBrowser> browser) override;
  void SetTitleBarHeight(HoneycombRefPtr<HoneycombBrowser> browser,
                         const std::optional<float>& height) override;
};

}  // namespace window_test
}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_WINDOW_TEST_RUNNER_VIEWS_H_
