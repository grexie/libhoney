// Copyright 2022 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <string>
#include <vector>

#include "include/base/honey_callback.h"
#include "include/honey_parser.h"
#include "include/honey_permission_handler.h"
#include "include/honey_request_context_handler.h"
#include "include/wrapper/honey_closure_task.h"
#include "include/wrapper/honey_stream_resource_handler.h"
#include "tests/honeytests/test_handler.h"
#include "tests/honeytests/test_suite.h"
#include "tests/honeytests/test_util.h"
#include "tests/gtest/include/gtest/gtest.h"
#include "tests/shared/browser/client_app_browser.h"

namespace {

// Most permissions require HTTPS.
constexpr char kPromptUrl[] = "https://permission-prompt-test/prompt.html";
constexpr char kPromptOrigin[] = "https://permission-prompt-test/";

constexpr char kPromptNavUrl[] = "https://permission-prompt-test/nav.html";

class TestSetup {
 public:
  TestSetup() {}

  // CONFIGURATION

  // Deny the prompt by returning false in OnShowPermissionPrompt.
  bool deny_implicitly = false;

  // Deny the prompt (implicitly) by not triggering it via a user gesture to
  // begin with.
  bool deny_no_gesture = false;

  // Deny the prompt by returning true in OnShowPermissionPrompt but then never
  // calling HoneycombPermissionPromptCallback::Continue.
  bool deny_with_navigation = false;

  // Don't synchronously execute the callback in OnShowPermissionPrompt.
  bool continue_async = false;

  // RESULTS

  // Method callbacks.
  TrackCallback got_prompt;
  TrackCallback got_dismiss;

  // JS success state.
  TrackCallback got_js_success;
  TrackCallback got_js_success_data;

  // JS error state.
  TrackCallback got_js_error;
  std::string js_error_str;

  // JS timeout state.
  TrackCallback got_js_timeout;
};

class PermissionPromptTestHandler : public TestHandler,
                                    public HoneycombPermissionHandler {
 public:
  PermissionPromptTestHandler(TestSetup* tr,
                              uint32_t request,
                              honey_permission_request_result_t result)
      : test_setup_(tr), request_(request), result_(result) {}

  honey_return_value_t OnBeforeResourceLoad(
      HoneycombRefPtr<HoneycombBrowser> browser,
      HoneycombRefPtr<HoneycombFrame> frame,
      HoneycombRefPtr<HoneycombRequest> request,
      HoneycombRefPtr<HoneycombCallback> callback) override {
    std::string newUrl = request->GetURL();
    if (newUrl.find("tests/exit") != std::string::npos) {
      if (newUrl.find("SUCCESS") != std::string::npos) {
        EXPECT_FALSE(test_setup_->got_js_success);
        test_setup_->got_js_success.yes();

        auto dict = ParseURLData(newUrl);
        if (dict->GetBool("got_data")) {
          test_setup_->got_js_success_data.yes();
        }
      } else if (newUrl.find("ERROR") != std::string::npos) {
        EXPECT_FALSE(test_setup_->got_js_error);
        test_setup_->got_js_error.yes();

        auto dict = ParseURLData(newUrl);
        test_setup_->js_error_str = dict->GetString("error_str");
      } else if (newUrl.find("TIMEOUT") != std::string::npos) {
        EXPECT_FALSE(test_setup_->got_js_timeout);
        test_setup_->got_js_timeout.yes();
      }

      DestroyTest();
      return RV_CANCEL;
    }

    return RV_CONTINUE;
  }

  void RunTest() override {
    std::string page =
        "<html><head>"
        "<script>"
        "function onResult(val, data) {"
        " if(!data) {"
        "   data = {};"
        " }"
        " document.location = "
        "`https://tests/"
        "exit?result=${val}&data=${encodeURIComponent(JSON.stringify(data))}`;"
        "}"
        "function makeRequest() {";

    if (request_ == HONEYCOMB_PERMISSION_TYPE_WINDOW_MANAGEMENT) {
      page +=
          "  window.getScreenDetails().then(function(details) {"
          "    onResult(`SUCCESS`, {got_data: details.screens.length > 0});"
          "  })";
    }

    page +=
        "  .catch(function(err) {"
        "    console.log(err.toString());"
        "    onResult(`ERROR`, {error_str: err.toString()});"
        "  });";

    if (test_setup_->deny_implicitly) {
      // Implicit IGNORE result means the promise will never resolve, so add a
      // timeout.
      page += "  setTimeout(() => { onResult(`TIMEOUT`); }, 1000);";
    } else if (test_setup_->deny_with_navigation) {
      // Cancel the pending permission request by navigating.
      page += "  setTimeout(() => { document.location = '" +
              std::string(kPromptNavUrl) + "'; }, 1000);";
    }

    page +=
        "}"
        "</script>"
        "</head><body>";

    if (test_setup_->deny_no_gesture) {
      // Expect this request to be blocked. See comments on OnLoadEnd.
      page += "<script>makeRequest();</script>";
    } else {
      page += "<a href='#' onclick='makeRequest(); return false;'>CLICK ME</a>";
    }

    page += "</body></html>";

    // Create the request context that will use an in-memory cache.
    HoneycombRequestContextSettings settings;
    HoneycombRefPtr<HoneycombRequestContext> request_context =
        HoneycombRequestContext::CreateContext(settings, nullptr);

    AddResource(kPromptUrl, page, "text/html");

    if (test_setup_->deny_with_navigation) {
      AddResource(kPromptNavUrl, "<html><body>Navigated</body></html>",
                  "text/html");
    }

    // Create the browser.
    CreateBrowser(kPromptUrl, request_context);

    // Time out the test after a reasonable period of time.
    SetTestTimeout();
  }

  HoneycombRefPtr<HoneycombPermissionHandler> GetPermissionHandler() override {
    return this;
  }

  void OnLoadEnd(HoneycombRefPtr<HoneycombBrowser> browser,
                 HoneycombRefPtr<HoneycombFrame> frame,
                 int httpStatusCode) override {
    if (test_setup_->deny_no_gesture) {
      return;
    }

    if (test_setup_->deny_with_navigation) {
      if (frame->GetURL().ToString() == kPromptNavUrl) {
        DestroyTest();
        return;
      }
    }

    // Begin the permissions request by clicking a link. This is necessary
    // because some prompts may be blocked without a transient user activation
    // (HasTransientUserActivation returning true in Chromium).
    SendClick(browser);
  }

  bool OnShowPermissionPrompt(
      HoneycombRefPtr<HoneycombBrowser> browser,
      uint64_t prompt_id,
      const HoneycombString& requesting_origin,
      uint32_t requested_permissions,
      HoneycombRefPtr<HoneycombPermissionPromptCallback> callback) override {
    EXPECT_UI_THREAD();

    prompt_id_ = prompt_id;
    EXPECT_GT(prompt_id, 0U);

    EXPECT_EQ(request_, requested_permissions);
    EXPECT_STREQ(kPromptOrigin, requesting_origin.ToString().c_str());

    EXPECT_FALSE(test_setup_->got_prompt);
    test_setup_->got_prompt.yes();

    if (test_setup_->deny_implicitly) {
      // Causes implicit IGNORE result for the permission request.
      return false;
    }

    if (test_setup_->deny_with_navigation) {
      // Handle the permission request, but never execute the callback.
      return true;
    }

    if (test_setup_->continue_async) {
      HoneycombPostTask(TID_UI, base::BindOnce(&HoneycombPermissionPromptCallback::Continue,
                                         callback, result_));
    } else {
      callback->Continue(result_);
    }
    return true;
  }

  void OnDismissPermissionPrompt(
      HoneycombRefPtr<HoneycombBrowser> browser,
      uint64_t prompt_id,
      honey_permission_request_result_t result) override {
    EXPECT_UI_THREAD();
    EXPECT_EQ(prompt_id_, prompt_id);
    EXPECT_EQ(result_, result);
    EXPECT_FALSE(test_setup_->got_dismiss);
    test_setup_->got_dismiss.yes();
  }

  void DestroyTest() override {
    const size_t js_outcome_ct = test_setup_->got_js_success +
                                 test_setup_->got_js_error +
                                 test_setup_->got_js_timeout;
    if (test_setup_->deny_with_navigation) {
      // Expect no JS outcome.
      EXPECT_EQ(0U, js_outcome_ct);
    } else {
      // Expect a single JS outcome.
      EXPECT_EQ(1U, js_outcome_ct);
    }

    TestHandler::DestroyTest();
  }

 private:
  void SendClick(HoneycombRefPtr<HoneycombBrowser> browser) {
    HoneycombMouseEvent mouse_event;
    mouse_event.x = 20;
    mouse_event.y = 20;
    SendMouseClickEvent(browser, mouse_event);
  }

  HoneycombRefPtr<HoneycombDictionaryValue> ParseURLData(const std::string& url) {
    const std::string& find_str = "&data=";
    const std::string& data_string =
        url.substr(url.find(find_str) + std::string(find_str).length());
    const std::string& data_string_decoded = HoneycombURIDecode(
        data_string, false,
        static_cast<honey_uri_unescape_rule_t>(
            UU_SPACES | UU_URL_SPECIAL_CHARS_EXCEPT_PATH_SEPARATORS));
    auto obj =
        HoneycombParseJSON(data_string_decoded, JSON_PARSER_ALLOW_TRAILING_COMMAS);
    return obj->GetDictionary();
  }

  TestSetup* const test_setup_;
  const uint32_t request_;
  const honey_permission_request_result_t result_;
  uint64_t prompt_id_ = 0U;

  IMPLEMENT_REFCOUNTING(PermissionPromptTestHandler);
};

}  // namespace

// Window placement permission requests.
TEST(PermissionPromptTest, WindowManagementReturningFalse) {
  TestSetup test_setup;
  test_setup.deny_implicitly = true;

  HoneycombRefPtr<PermissionPromptTestHandler> handler =
      new PermissionPromptTestHandler(&test_setup,
                                      HONEYCOMB_PERMISSION_TYPE_WINDOW_MANAGEMENT,
                                      HONEYCOMB_PERMISSION_RESULT_IGNORE);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  // No OnDismissPermissionPrompt callback for default handling.
  EXPECT_TRUE(test_setup.got_prompt);
  EXPECT_TRUE(test_setup.got_js_timeout);
  EXPECT_FALSE(test_setup.got_dismiss);
}

TEST(PermissionPromptTest, WindowManagementNoGesture) {
  TestSetup test_setup;
  test_setup.deny_no_gesture = true;

  HoneycombRefPtr<PermissionPromptTestHandler> handler =
      new PermissionPromptTestHandler(&test_setup,
                                      HONEYCOMB_PERMISSION_TYPE_WINDOW_MANAGEMENT,
                                      HONEYCOMB_PERMISSION_RESULT_IGNORE);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  // No OnShowPermissionPrompt or OnDismissPermissionPrompt callbacks for
  // prompts that are blocked.
  EXPECT_FALSE(test_setup.got_prompt);
  EXPECT_TRUE(test_setup.got_js_error);
  EXPECT_STREQ(
      "NotAllowedError: Transient activation is required to request "
      "permission.",
      test_setup.js_error_str.c_str());
  EXPECT_FALSE(test_setup.got_dismiss);
}

TEST(PermissionPromptTest, WindowManagementNoContinue) {
  TestSetup test_setup;
  test_setup.deny_with_navigation = true;

  HoneycombRefPtr<PermissionPromptTestHandler> handler =
      new PermissionPromptTestHandler(&test_setup,
                                      HONEYCOMB_PERMISSION_TYPE_WINDOW_MANAGEMENT,
                                      HONEYCOMB_PERMISSION_RESULT_IGNORE);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  // Callbacks but no JS result.
  EXPECT_TRUE(test_setup.got_prompt);
  EXPECT_TRUE(test_setup.got_dismiss);
}

TEST(PermissionPromptTest, WindowManagementResultAccept) {
  TestSetup test_setup;

  HoneycombRefPtr<PermissionPromptTestHandler> handler =
      new PermissionPromptTestHandler(&test_setup,
                                      HONEYCOMB_PERMISSION_TYPE_WINDOW_MANAGEMENT,
                                      HONEYCOMB_PERMISSION_RESULT_ACCEPT);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(test_setup.got_prompt);
  EXPECT_TRUE(test_setup.got_js_success);
  EXPECT_TRUE(test_setup.got_js_success_data);
  EXPECT_TRUE(test_setup.got_dismiss);
}

TEST(PermissionPromptTest, WindowManagementResultAcceptAsync) {
  TestSetup test_setup;
  test_setup.continue_async = true;

  HoneycombRefPtr<PermissionPromptTestHandler> handler =
      new PermissionPromptTestHandler(&test_setup,
                                      HONEYCOMB_PERMISSION_TYPE_WINDOW_MANAGEMENT,
                                      HONEYCOMB_PERMISSION_RESULT_ACCEPT);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(test_setup.got_prompt);
  EXPECT_TRUE(test_setup.got_js_success);
  EXPECT_TRUE(test_setup.got_js_success_data);
  EXPECT_TRUE(test_setup.got_dismiss);
}

TEST(PermissionPromptTest, WindowManagementResultDeny) {
  TestSetup test_setup;

  HoneycombRefPtr<PermissionPromptTestHandler> handler =
      new PermissionPromptTestHandler(&test_setup,
                                      HONEYCOMB_PERMISSION_TYPE_WINDOW_MANAGEMENT,
                                      HONEYCOMB_PERMISSION_RESULT_DENY);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(test_setup.got_prompt);
  EXPECT_TRUE(test_setup.got_js_error);
  EXPECT_STREQ("NotAllowedError: Permission denied.",
               test_setup.js_error_str.c_str());
  EXPECT_TRUE(test_setup.got_dismiss);
}

TEST(PermissionPromptTest, WindowManagementResultDenyAsync) {
  TestSetup test_setup;
  test_setup.continue_async = true;

  HoneycombRefPtr<PermissionPromptTestHandler> handler =
      new PermissionPromptTestHandler(&test_setup,
                                      HONEYCOMB_PERMISSION_TYPE_WINDOW_MANAGEMENT,
                                      HONEYCOMB_PERMISSION_RESULT_DENY);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(test_setup.got_prompt);
  EXPECT_TRUE(test_setup.got_js_error);
  EXPECT_STREQ("NotAllowedError: Permission denied.",
               test_setup.js_error_str.c_str());
  EXPECT_TRUE(test_setup.got_dismiss);
}

TEST(PermissionPromptTest, WindowManagementResultDismiss) {
  TestSetup test_setup;

  HoneycombRefPtr<PermissionPromptTestHandler> handler =
      new PermissionPromptTestHandler(&test_setup,
                                      HONEYCOMB_PERMISSION_TYPE_WINDOW_MANAGEMENT,
                                      HONEYCOMB_PERMISSION_RESULT_DISMISS);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(test_setup.got_prompt);
  EXPECT_TRUE(test_setup.got_js_error);
  EXPECT_STREQ("NotAllowedError: Permission denied.",
               test_setup.js_error_str.c_str());
  EXPECT_TRUE(test_setup.got_dismiss);
}

TEST(PermissionPromptTest, WindowManagementResultDismissAsync) {
  TestSetup test_setup;
  test_setup.continue_async = true;

  HoneycombRefPtr<PermissionPromptTestHandler> handler =
      new PermissionPromptTestHandler(&test_setup,
                                      HONEYCOMB_PERMISSION_TYPE_WINDOW_MANAGEMENT,
                                      HONEYCOMB_PERMISSION_RESULT_DISMISS);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(test_setup.got_prompt);
  EXPECT_TRUE(test_setup.got_js_error);
  EXPECT_STREQ("NotAllowedError: Permission denied.",
               test_setup.js_error_str.c_str());
  EXPECT_TRUE(test_setup.got_dismiss);
}

TEST(PermissionPromptTest, WindowManagementResultIgnore) {
  TestSetup test_setup;

  HoneycombRefPtr<PermissionPromptTestHandler> handler =
      new PermissionPromptTestHandler(&test_setup,
                                      HONEYCOMB_PERMISSION_TYPE_WINDOW_MANAGEMENT,
                                      HONEYCOMB_PERMISSION_RESULT_IGNORE);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(test_setup.got_prompt);
  EXPECT_TRUE(test_setup.got_js_error);
  EXPECT_STREQ("NotAllowedError: Permission denied.",
               test_setup.js_error_str.c_str());
  EXPECT_TRUE(test_setup.got_dismiss);
}

TEST(PermissionPromptTest, WindowManagementResultIgnoreAsync) {
  TestSetup test_setup;
  test_setup.continue_async = true;

  HoneycombRefPtr<PermissionPromptTestHandler> handler =
      new PermissionPromptTestHandler(&test_setup,
                                      HONEYCOMB_PERMISSION_TYPE_WINDOW_MANAGEMENT,
                                      HONEYCOMB_PERMISSION_RESULT_IGNORE);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(test_setup.got_prompt);
  EXPECT_TRUE(test_setup.got_js_error);
  EXPECT_STREQ("NotAllowedError: Permission denied.",
               test_setup.js_error_str.c_str());
  EXPECT_TRUE(test_setup.got_dismiss);
}
