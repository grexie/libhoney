// Copyright (c) 2022 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeytests/message_router_unittest_utils.h"

extern const char kJSQueryFunc[] = "mrtQuery";
extern const char kJSQueryCancelFunc[] = "mrtQueryCancel";

namespace {

const char kTestDomainRoot[] = "https://tests-mr";
const char kDoneMessageName[] = "mrtNotifyMsg";
const char kJSNotifyFunc[] = "mrtNotify";
const char kJSAssertTotalCountFunc[] = "mrtAssertTotalCount";
const char kJSAssertBrowserCountFunc[] = "mrtAssertBrowserCount";
const char kJSAssertContextCountFunc[] = "mrtAssertContextCount";

void SetRouterConfig(HoneycombMessageRouterConfig& config) {
  config.js_query_function = kJSQueryFunc;
  config.js_cancel_function = kJSQueryCancelFunc;
}

}  // namespace

// Entry point for creating the test delegate.
// Called from client_app_delegates.cc.
void CreateMessageRouterRendererTests(
    ClientAppRenderer::DelegateSet& delegates) {
  delegates.insert(new MRRenderDelegate);
}

bool MRRenderDelegate::V8HandlerImpl::Execute(const HoneycombString& name,
                                              HoneycombRefPtr<HoneycombV8Value> object,
                                              const HoneycombV8ValueList& arguments,
                                              HoneycombRefPtr<HoneycombV8Value>& retval,
                                              HoneycombString& exception) {
  const std::string& message_name = name;
  if (message_name == kJSNotifyFunc) {
    EXPECT_EQ(1U, arguments.size());
    EXPECT_TRUE(arguments[0]->IsString());

    const HoneycombString& msg = arguments[0]->GetStringValue();
    HoneycombRefPtr<HoneycombV8Context> context = HoneycombV8Context::GetCurrentContext();
    HoneycombRefPtr<HoneycombFrame> frame = context->GetFrame();

    HoneycombRefPtr<HoneycombProcessMessage> message =
        HoneycombProcessMessage::Create(kDoneMessageName);
    HoneycombRefPtr<HoneycombListValue> args = message->GetArgumentList();
    args->SetString(0, msg);
    frame->SendProcessMessage(PID_BROWSER, message);
    return true;
  } else {
    EXPECT_EQ(2U, arguments.size());
    EXPECT_TRUE(arguments[0]->IsInt());
    EXPECT_TRUE(arguments[1]->IsInt());

    const int line_no = arguments[0]->GetIntValue();
    const int expected_count = arguments[1]->GetIntValue();
    int actual_count = -1;

    HoneycombRefPtr<HoneycombV8Context> context = HoneycombV8Context::GetCurrentContext();
    HoneycombRefPtr<HoneycombBrowser> browser = context->GetBrowser();

    if (name == kJSAssertTotalCountFunc) {
      actual_count =
          delegate_->message_router_->GetPendingCount(nullptr, nullptr);
    } else if (name == kJSAssertBrowserCountFunc) {
      actual_count =
          delegate_->message_router_->GetPendingCount(browser, nullptr);
    } else if (name == kJSAssertContextCountFunc) {
      actual_count =
          delegate_->message_router_->GetPendingCount(browser, context);
    }

    if (expected_count != actual_count) {
      std::stringstream ss;
      ss << message_name << " failed (line " << line_no << "); expected "
         << expected_count << ", got " << actual_count;
      exception = ss.str();
    }
  }

  return true;
}

void MRRenderDelegate::OnWebKitInitialized(HoneycombRefPtr<ClientAppRenderer> app) {
  // Create the renderer-side router for query handling.
  HoneycombMessageRouterConfig config;
  SetRouterConfig(config);
  message_router_ = HoneycombMessageRouterRendererSide::Create(config);
}

void MRRenderDelegate::OnContextCreated(HoneycombRefPtr<ClientAppRenderer> app,
                                        HoneycombRefPtr<HoneycombBrowser> browser,
                                        HoneycombRefPtr<HoneycombFrame> frame,
                                        HoneycombRefPtr<HoneycombV8Context> context) {
  const std::string& url = frame->GetURL();
  if (url.find(kTestDomainRoot) != 0) {
    return;
  }

  message_router_->OnContextCreated(browser, frame, context);

  // Register function handlers with the 'window' object.
  HoneycombRefPtr<HoneycombV8Value> window = context->GetGlobal();

  HoneycombRefPtr<V8HandlerImpl> handler = new V8HandlerImpl(this);
  HoneycombV8Value::PropertyAttribute attributes =
      static_cast<HoneycombV8Value::PropertyAttribute>(
          V8_PROPERTY_ATTRIBUTE_READONLY | V8_PROPERTY_ATTRIBUTE_DONTENUM |
          V8_PROPERTY_ATTRIBUTE_DONTDELETE);

  HoneycombRefPtr<HoneycombV8Value> notify_func =
      HoneycombV8Value::CreateFunction(kJSNotifyFunc, handler.get());
  window->SetValue(kJSNotifyFunc, notify_func, attributes);

  HoneycombRefPtr<HoneycombV8Value> total_count_func =
      HoneycombV8Value::CreateFunction(kJSAssertTotalCountFunc, handler.get());
  window->SetValue(kJSAssertTotalCountFunc, total_count_func, attributes);

  HoneycombRefPtr<HoneycombV8Value> browser_count_func =
      HoneycombV8Value::CreateFunction(kJSAssertBrowserCountFunc, handler.get());
  window->SetValue(kJSAssertBrowserCountFunc, browser_count_func, attributes);

  HoneycombRefPtr<HoneycombV8Value> context_count_func =
      HoneycombV8Value::CreateFunction(kJSAssertContextCountFunc, handler.get());
  window->SetValue(kJSAssertContextCountFunc, context_count_func, attributes);
}

void MRRenderDelegate::OnContextReleased(HoneycombRefPtr<ClientAppRenderer> app,
                                         HoneycombRefPtr<HoneycombBrowser> browser,
                                         HoneycombRefPtr<HoneycombFrame> frame,
                                         HoneycombRefPtr<HoneycombV8Context> context) {
  const std::string& url = frame->GetURL();
  if (url.find(kTestDomainRoot) != 0) {
    return;
  }

  message_router_->OnContextReleased(browser, frame, context);
}

bool MRRenderDelegate::OnProcessMessageReceived(
    HoneycombRefPtr<ClientAppRenderer> app,
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombFrame> frame,
    HoneycombProcessId source_process,
    HoneycombRefPtr<HoneycombProcessMessage> message) {
  const std::string& url = frame->GetURL();
  if (url.find(kTestDomainRoot) != 0) {
    return false;
  }

  return message_router_->OnProcessMessageReceived(browser, frame,
                                                   source_process, message);
}

void MRTestHandler::RunTest() {
  RunMRTest();

  // Time out the test after a reasonable period of time.
  SetTestTimeout(10000);
}

void MRTestHandler::OnAfterCreated(HoneycombRefPtr<HoneycombBrowser> browser) {
  if (!message_router_.get()) {
    // Create the browser-side router for query handling.
    HoneycombMessageRouterConfig config;

    SetRouterConfig(config);
    if (message_size_threshold_) {
      config.message_size_threshold = message_size_threshold_;
    }

    message_router_ = HoneycombMessageRouterBrowserSide::Create(config);
    AddHandlers(message_router_);
  }
  TestHandler::OnAfterCreated(browser);
}

void MRTestHandler::OnBeforeClose(HoneycombRefPtr<HoneycombBrowser> browser) {
  message_router_->OnBeforeClose(browser);
  TestHandler::OnBeforeClose(browser);
}

void MRTestHandler::OnRenderProcessTerminated(HoneycombRefPtr<HoneycombBrowser> browser,
                                              TerminationStatus status) {
  message_router_->OnRenderProcessTerminated(browser);
}

bool MRTestHandler::OnBeforeBrowse(HoneycombRefPtr<HoneycombBrowser> browser,
                                   HoneycombRefPtr<HoneycombFrame> frame,
                                   HoneycombRefPtr<HoneycombRequest> request,
                                   bool user_gesture,
                                   bool is_redirect) {
  message_router_->OnBeforeBrowse(browser, frame);
  return false;
}

// Returns true if the router handled the navigation.
bool MRTestHandler::OnProcessMessageReceived(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombFrame> frame,
    HoneycombProcessId source_process,
    HoneycombRefPtr<HoneycombProcessMessage> message) {
  const std::string& message_name = message->GetName();
  if (message_name == kDoneMessageName) {
    HoneycombRefPtr<HoneycombListValue> args = message->GetArgumentList();
    EXPECT_EQ(1U, args->GetSize());
    EXPECT_EQ(VTYPE_STRING, args->GetType(0));
    OnNotify(browser, frame, args->GetString(0));
    return true;
  }

  return message_router_->OnProcessMessageReceived(browser, frame,
                                                   source_process, message);
}

HoneycombRefPtr<HoneycombMessageRouterBrowserSide> MRTestHandler::GetRouter() const {
  return message_router_;
}

void MRTestHandler::SetMessageSizeThreshold(size_t message_size_threshold) {
  message_size_threshold_ = message_size_threshold;
}

bool MRTestHandler::AssertQueryCount(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombMessageRouterBrowserSide::Handler* handler,
    int expected_count) {
  int actual_count = message_router_->GetPendingCount(browser, handler);
  EXPECT_EQ(expected_count, actual_count);
  return (expected_count == actual_count);
}

void MRTestHandler::AssertMainBrowser(HoneycombRefPtr<HoneycombBrowser> browser) {
  EXPECT_TRUE(browser.get());
  EXPECT_EQ(GetBrowserId(), browser->GetIdentifier());
}

SingleLoadTestHandler::SingleLoadTestHandler()
    : main_url_("https://tests-mr.com/main.html") {}

void SingleLoadTestHandler::RunMRTest() {
  AddOtherResources();
  AddResource(main_url_, GetMainHTML(), "text/html");

  CreateBrowser(main_url_, nullptr);
}

void SingleLoadTestHandler::AddHandlers(
    HoneycombRefPtr<HoneycombMessageRouterBrowserSide> message_router) {
  message_router->AddHandler(this, false);
}

void SingleLoadTestHandler::AssertMainFrame(HoneycombRefPtr<HoneycombFrame> frame) {
  EXPECT_TRUE(frame.get());
  EXPECT_TRUE(frame->IsMain());
  EXPECT_STREQ(main_url_.c_str(), frame->GetURL().ToString().c_str());
}
