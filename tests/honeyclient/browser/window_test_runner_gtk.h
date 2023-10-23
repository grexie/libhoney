// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_WINDOW_TEST_RUNNER_GTK_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_WINDOW_TEST_RUNNER_GTK_H_
#pragma once

#include "tests/honeyclient/browser/window_test_runner.h"

namespace client {
namespace window_test {

// GTK platform implementation.
class WindowTestRunnerGtk : public WindowTestRunner {
 public:
  WindowTestRunnerGtk();

  void SetPos(HoneycombRefPtr<HoneycombBrowser> browser,
              int x,
              int y,
              int width,
              int height) override;
  void Minimize(HoneycombRefPtr<HoneycombBrowser> browser) override;
  void Maximize(HoneycombRefPtr<HoneycombBrowser> browser) override;
  void Restore(HoneycombRefPtr<HoneycombBrowser> browser) override;
};

}  // namespace window_test
}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_WINDOW_TEST_RUNNER_GTK_H_
