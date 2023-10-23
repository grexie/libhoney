// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeyclient/browser/binding_test.h"

#include <algorithm>
#include <string>

#include "tests/honeyclient/browser/test_runner.h"

namespace client {
namespace binding_test {

namespace {

const char kTestUrlPath[] = "/binding";
const char kTestMessageName[] = "BindingTest";

// Handle messages in the browser process.
class Handler : public HoneycombMessageRouterBrowserSide::Handler {
 public:
  Handler() {}

  // Called due to honeyQuery execution in binding.html.
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
    if (message_name.find(kTestMessageName) == 0) {
      // Reverse the string and return.
      std::string result = message_name.substr(sizeof(kTestMessageName));
      std::reverse(result.begin(), result.end());
      callback->Success(result);
      return true;
    }

    return false;
  }
};

}  // namespace

void CreateMessageHandlers(test_runner::MessageHandlerSet& handlers) {
  handlers.insert(new Handler());
}

}  // namespace binding_test
}  // namespace client
