// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeyclient/renderer/client_renderer.h"

#include <sstream>
#include <string>

#include "include/honey_crash_util.h"
#include "include/honey_dom.h"
#include "include/wrapper/honey_helpers.h"
#include "include/wrapper/honey_message_router.h"

namespace client {
namespace renderer {

namespace {

// Must match the value in client_handler.cc.
const char kFocusedNodeChangedMessage[] = "ClientRenderer.FocusedNodeChanged";

class ClientRenderDelegate : public ClientAppRenderer::Delegate {
 public:
  ClientRenderDelegate() : last_node_is_editable_(false) {}

  void OnWebKitInitialized(HoneycombRefPtr<ClientAppRenderer> app) override {
    if (HoneycombCrashReportingEnabled()) {
      // Set some crash keys for testing purposes. Keys must be defined in the
      // "crash_reporter.cfg" file. See honey_crash_util.h for details.
      HoneycombSetCrashKeyValue("testkey_small1", "value1_small_renderer");
      HoneycombSetCrashKeyValue("testkey_small2", "value2_small_renderer");
      HoneycombSetCrashKeyValue("testkey_medium1", "value1_medium_renderer");
      HoneycombSetCrashKeyValue("testkey_medium2", "value2_medium_renderer");
      HoneycombSetCrashKeyValue("testkey_large1", "value1_large_renderer");
      HoneycombSetCrashKeyValue("testkey_large2", "value2_large_renderer");
    }

    // Create the renderer-side router for query handling.
    HoneycombMessageRouterConfig config;
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

  void OnFocusedNodeChanged(HoneycombRefPtr<ClientAppRenderer> app,
                            HoneycombRefPtr<HoneycombBrowser> browser,
                            HoneycombRefPtr<HoneycombFrame> frame,
                            HoneycombRefPtr<HoneycombDOMNode> node) override {
    bool is_editable = (node.get() && node->IsEditable());
    if (is_editable != last_node_is_editable_) {
      // Notify the browser of the change in focused element type.
      last_node_is_editable_ = is_editable;
      HoneycombRefPtr<HoneycombProcessMessage> message =
          HoneycombProcessMessage::Create(kFocusedNodeChangedMessage);
      message->GetArgumentList()->SetBool(0, is_editable);
      frame->SendProcessMessage(PID_BROWSER, message);
    }
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
  bool last_node_is_editable_;

  // Handles the renderer side of query routing.
  HoneycombRefPtr<HoneycombMessageRouterRendererSide> message_router_;

  DISALLOW_COPY_AND_ASSIGN(ClientRenderDelegate);
  IMPLEMENT_REFCOUNTING(ClientRenderDelegate);
};

}  // namespace

void CreateDelegates(ClientAppRenderer::DelegateSet& delegates) {
  delegates.insert(new ClientRenderDelegate);
}

}  // namespace renderer
}  // namespace client
