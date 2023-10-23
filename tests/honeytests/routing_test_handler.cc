// Copyright (c) 2014 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeytests/routing_test_handler.h"
#include "tests/shared/renderer/client_app_renderer.h"

using client::ClientAppRenderer;

namespace {

void SetRouterConfig(HoneycombMessageRouterConfig& config) {
  config.js_query_function = "testQuery";
  config.js_cancel_function = "testQueryCancel";
}

// Handle the renderer side of the routing implementation.
class RoutingRenderDelegate : public ClientAppRenderer::Delegate {
 public:
  RoutingRenderDelegate() {}

  void OnWebKitInitialized(HoneycombRefPtr<ClientAppRenderer> app) override {
    // Create the renderer-side router for query handling.
    HoneycombMessageRouterConfig config;
    SetRouterConfig(config);
    message_router_ = HoneycombMessageRouterRendererSide::Create(config);
  }

  void OnContextCreated(HoneycombRefPtr<ClientAppRenderer> app,
                        HoneycombRefPtr<HoneycombBrowser> browser,
                        HoneycombRefPtr<HoneycombFrame> frame,
                        HoneycombRefPtr<HoneycombV8Context> context) override {
    message_router_->OnContextCreated(browser, frame, context);
  }

  void OnContextReleased(HoneycombRefPtr<ClientAppRenderer> app,
                         HoneycombRefPtr<HoneycombBrowser> browser,
                         HoneycombRefPtr<HoneycombFrame> frame,
                         HoneycombRefPtr<HoneycombV8Context> context) override {
    message_router_->OnContextReleased(browser, frame, context);
  }

  bool OnProcessMessageReceived(HoneycombRefPtr<ClientAppRenderer> app,
                                HoneycombRefPtr<HoneycombBrowser> browser,
                                HoneycombRefPtr<HoneycombFrame> frame,
                                HoneycombProcessId source_process,
                                HoneycombRefPtr<HoneycombProcessMessage> message) override {
    return message_router_->OnProcessMessageReceived(browser, frame,
                                                     source_process, message);
  }

 private:
  HoneycombRefPtr<HoneycombMessageRouterRendererSide> message_router_;

  IMPLEMENT_REFCOUNTING(RoutingRenderDelegate);
};

}  // namespace

RoutingTestHandler::RoutingTestHandler(CompletionState* completion_state)
    : TestHandler(completion_state) {}

void RoutingTestHandler::OnAfterCreated(HoneycombRefPtr<HoneycombBrowser> browser) {
  if (!message_router_.get()) {
    // Create the browser-side router for query handling.
    HoneycombMessageRouterConfig config;
    SetRouterConfig(config);
    message_router_ = HoneycombMessageRouterBrowserSide::Create(config);
    message_router_->AddHandler(this, false);
  }
  TestHandler::OnAfterCreated(browser);
}

void RoutingTestHandler::OnBeforeClose(HoneycombRefPtr<HoneycombBrowser> browser) {
  message_router_->OnBeforeClose(browser);
  TestHandler::OnBeforeClose(browser);
}

void RoutingTestHandler::OnRenderProcessTerminated(
    HoneycombRefPtr<HoneycombBrowser> browser,
    TerminationStatus status) {
  message_router_->OnRenderProcessTerminated(browser);
}

bool RoutingTestHandler::OnBeforeBrowse(HoneycombRefPtr<HoneycombBrowser> browser,
                                        HoneycombRefPtr<HoneycombFrame> frame,
                                        HoneycombRefPtr<HoneycombRequest> request,
                                        bool user_gesture,
                                        bool is_redirect) {
  message_router_->OnBeforeBrowse(browser, frame);
  return false;
}

bool RoutingTestHandler::OnProcessMessageReceived(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombFrame> frame,
    HoneycombProcessId source_process,
    HoneycombRefPtr<HoneycombProcessMessage> message) {
  return message_router_->OnProcessMessageReceived(browser, frame,
                                                   source_process, message);
}

// Entry point for creating the test delegate.
// Called from client_app_delegates.cc.
void CreateRoutingTestHandlerDelegate(
    ClientAppRenderer::DelegateSet& delegates) {
  delegates.insert(new RoutingRenderDelegate);
}
