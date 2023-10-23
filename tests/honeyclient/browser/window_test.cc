// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeyclient/browser/window_test.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "include/base/honey_callback.h"
#include "include/wrapper/honey_stream_resource_handler.h"
#include "tests/honeyclient/browser/main_context.h"
#include "tests/honeyclient/browser/test_runner.h"
#include "tests/honeyclient/browser/window_test_runner.h"

#include "tests/honeyclient/browser/window_test_runner_views.h"

#if defined(OS_WIN)
#include "tests/honeyclient/browser/window_test_runner_win.h"
#elif defined(OS_LINUX)
#include "tests/honeyclient/browser/window_test_runner_gtk.h"
#elif defined(OS_MAC)
#include "tests/honeyclient/browser/window_test_runner_mac.h"
#endif

namespace client {
namespace window_test {

namespace {

const char kTestUrlPath[] = "/window";
const char kMessagePositionName[] = "WindowTest.Position";
const char kMessageMinimizeName[] = "WindowTest.Minimize";
const char kMessageMaximizeName[] = "WindowTest.Maximize";
const char kMessageRestoreName[] = "WindowTest.Restore";
const char kMessageFullscreenName[] = "WindowTest.Fullscreen";
const char kMessageTitlebarHeightName[] = "WindowTest.TitlebarHeight";

// Create the appropriate platform test runner object.
std::unique_ptr<WindowTestRunner> CreateWindowTestRunner() {
  if (MainContext::Get()->UseViews()) {
    return std::make_unique<WindowTestRunnerViews>();
  }

#if defined(OS_WIN)
  return std::make_unique<WindowTestRunnerWin>();
#elif defined(OS_LINUX)
  return std::make_unique<WindowTestRunnerGtk>();
#elif defined(OS_MAC)
  return std::make_unique<WindowTestRunnerMac>();
#else
#error "No implementation available for your platform."
#endif
}

// Parse the comma-delimited list of integer values.
std::vector<int> ParsePosition(const std::string& message_name) {
  std::vector<int> vec;
  const std::string& vals = message_name.substr(sizeof(kMessagePositionName));
  std::stringstream ss(vals);
  int i;
  while (ss >> i) {
    vec.push_back(i);
    if (ss.peek() == ',') {
      ss.ignore();
    }
  }

  return vec;
}

std::optional<float> ParseHeight(const std::string& message) {
  if (message.size() > sizeof(kMessageTitlebarHeightName)) {
    const std::string& val = message.substr(sizeof(kMessageTitlebarHeightName));
    return std::stof(val);
  } else {
    return std::nullopt;
  }
}

// Handle messages in the browser process.
class Handler : public HoneycombMessageRouterBrowserSide::Handler {
 public:
  Handler() : runner_(CreateWindowTestRunner()) {}

  // Called due to honeyBroadcast execution in window.html.
  virtual bool OnQuery(HoneycombRefPtr<HoneycombBrowser> browser,
                       HoneycombRefPtr<HoneycombFrame> frame,
                       int64_t query_id,
                       const HoneycombString& request,
                       bool persistent,
                       HoneycombRefPtr<Callback> callback) override {
    // Only handle messages from the test URL.
    const std::string& url = frame->GetURL();
    if (!test_runner::IsTestURL(url, kTestUrlPath)) {
      return false;
    }

    const std::string& message_name = request;
    if (message_name.find(kMessagePositionName) == 0) {
      const auto vec = ParsePosition(message_name);
      if (vec.size() == 4) {
        runner_->SetPos(browser, vec[0], vec[1], vec[2], vec[3]);
      }
    } else if (message_name == kMessageMinimizeName) {
      runner_->Minimize(browser);
    } else if (message_name == kMessageMaximizeName) {
      runner_->Maximize(browser);
    } else if (message_name == kMessageRestoreName) {
      runner_->Restore(browser);
    } else if (message_name == kMessageFullscreenName) {
      runner_->Fullscreen(browser);
    } else if (message_name.find(kMessageTitlebarHeightName) == 0) {
      const auto height = ParseHeight(message_name);
      runner_->SetTitleBarHeight(browser, height);
    } else {
      NOTREACHED();
    }

    callback->Success("");
    return true;
  }

 private:
  std::unique_ptr<WindowTestRunner> runner_;
};

}  // namespace

void CreateMessageHandlers(test_runner::MessageHandlerSet& handlers) {
  handlers.insert(new Handler());
}

}  // namespace window_test
}  // namespace client
