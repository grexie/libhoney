// Copyright (c) 2022 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBTESTS_MESSAGE_ROUTER_UNITTEST_UTILS_H_
#define HONEYCOMB_TESTS_HONEYCOMBTESTS_MESSAGE_ROUTER_UNITTEST_UTILS_H_
#pragma once

#include <cstdlib>

#include "include/base/honey_callback.h"
#include "include/base/honey_weak_ptr.h"
#include "include/honey_v8.h"
#include "include/wrapper/honey_closure_task.h"
#include "tests/honeytests/routing_test_handler.h"
#include "tests/honeytests/test_util.h"
#include "tests/gtest/include/gtest/gtest.h"
#include "tests/shared/renderer/client_app_renderer.h"

using client::ClientAppRenderer;

extern const char kJSQueryFunc[];
extern const char kJSQueryCancelFunc[];

#define S1(N) #N
#define S2(N) S1(N)
#define LINESTR S2(__LINE__)

// Handle the renderer side of the routing implementation.
class MRRenderDelegate : public ClientAppRenderer::Delegate {
 public:
  class V8HandlerImpl : public HoneycombV8Handler {
   public:
    explicit V8HandlerImpl(HoneycombRefPtr<MRRenderDelegate> delegate)
        : delegate_(delegate) {}

    bool Execute(const HoneycombString& name,
                 HoneycombRefPtr<HoneycombV8Value> object,
                 const HoneycombV8ValueList& arguments,
                 HoneycombRefPtr<HoneycombV8Value>& retval,
                 HoneycombString& exception) override;

   private:
    HoneycombRefPtr<MRRenderDelegate> delegate_;

    IMPLEMENT_REFCOUNTING(V8HandlerImpl);
  };

  void OnWebKitInitialized(HoneycombRefPtr<ClientAppRenderer> app) override;

  void OnContextCreated(HoneycombRefPtr<ClientAppRenderer> app,
                        HoneycombRefPtr<HoneycombBrowser> browser,
                        HoneycombRefPtr<HoneycombFrame> frame,
                        HoneycombRefPtr<HoneycombV8Context> context) override;

  void OnContextReleased(HoneycombRefPtr<ClientAppRenderer> app,
                         HoneycombRefPtr<HoneycombBrowser> browser,
                         HoneycombRefPtr<HoneycombFrame> frame,
                         HoneycombRefPtr<HoneycombV8Context> context) override;

  bool OnProcessMessageReceived(HoneycombRefPtr<ClientAppRenderer> app,
                                HoneycombRefPtr<HoneycombBrowser> browser,
                                HoneycombRefPtr<HoneycombFrame> frame,
                                HoneycombProcessId source_process,
                                HoneycombRefPtr<HoneycombProcessMessage> message) override;

 private:
  HoneycombRefPtr<HoneycombMessageRouterRendererSide> message_router_;

  IMPLEMENT_REFCOUNTING(MRRenderDelegate);
};

class MRTestHandler : public TestHandler {
 public:
  void RunTest() override;
  void OnAfterCreated(HoneycombRefPtr<HoneycombBrowser> browser) override;
  void OnBeforeClose(HoneycombRefPtr<HoneycombBrowser> browser) override;
  void OnRenderProcessTerminated(HoneycombRefPtr<HoneycombBrowser> browser,
                                 TerminationStatus status) override;

  // Only call this method if the navigation isn't canceled.
  bool OnBeforeBrowse(HoneycombRefPtr<HoneycombBrowser> browser,
                      HoneycombRefPtr<HoneycombFrame> frame,
                      HoneycombRefPtr<HoneycombRequest> request,
                      bool user_gesture,
                      bool is_redirect) override;
  // Returns true if the router handled the navigation.
  bool OnProcessMessageReceived(HoneycombRefPtr<HoneycombBrowser> browser,
                                HoneycombRefPtr<HoneycombFrame> frame,
                                HoneycombProcessId source_process,
                                HoneycombRefPtr<HoneycombProcessMessage> message) override;

  HoneycombRefPtr<HoneycombMessageRouterBrowserSide> GetRouter() const;
  void SetMessageSizeThreshold(size_t message_size_treshold);

 protected:
  virtual void RunMRTest() = 0;

  virtual void AddHandlers(
      HoneycombRefPtr<HoneycombMessageRouterBrowserSide> message_router) = 0;

  virtual void OnNotify(HoneycombRefPtr<HoneycombBrowser> browser,
                        HoneycombRefPtr<HoneycombFrame> frame,
                        const std::string& message) = 0;

  bool AssertQueryCount(HoneycombRefPtr<HoneycombBrowser> browser,
                        HoneycombMessageRouterBrowserSide::Handler* handler,
                        int expected_count);
  void AssertMainBrowser(HoneycombRefPtr<HoneycombBrowser> browser);

 private:
  HoneycombRefPtr<HoneycombMessageRouterBrowserSide> message_router_;
  size_t message_size_threshold_ = 0;

  IMPLEMENT_REFCOUNTING(MRTestHandler);
};

// Implementation of MRTestHandler that loads a single page.
class SingleLoadTestHandler : public MRTestHandler,
                              public HoneycombMessageRouterBrowserSide::Handler {
 public:
  SingleLoadTestHandler();
  const std::string& GetMainURL() { return main_url_; }

 protected:
  void RunMRTest() override;
  void AddHandlers(
      HoneycombRefPtr<HoneycombMessageRouterBrowserSide> message_router) override;
  virtual void AddOtherResources() {}
  virtual std::string GetMainHTML() = 0;
  void AssertMainFrame(HoneycombRefPtr<HoneycombFrame> frame);

 private:
  const std::string main_url_;
};

#endif  // HONEYCOMB_TESTS_HONEYCOMBTESTS_MESSAGE_ROUTER_UNITTEST_UTILS_H_
