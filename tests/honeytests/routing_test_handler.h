// Copyright (c) 2014 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBTESTS_ROUTING_TEST_HANDLER_H_
#define HONEYCOMB_TESTS_HONEYCOMBTESTS_ROUTING_TEST_HANDLER_H_
#pragma once

#include "include/wrapper/honey_message_router.h"
#include "tests/honeytests/test_handler.h"

// Extends TestHandler to provide message routing functionality. The
// RoutingTestHandler implementation must be called from subclass
// overrides unless otherwise indicated.
class RoutingTestHandler : public TestHandler,
                           public HoneycombMessageRouterBrowserSide::Handler {
 public:
  RoutingTestHandler(CompletionState* completion_state = nullptr);

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

 private:
  HoneycombRefPtr<HoneycombMessageRouterBrowserSide> message_router_;
};

#endif  // HONEYCOMB_TESTS_HONEYCOMBTESTS_ROUTING_TEST_HANDLER_H_
