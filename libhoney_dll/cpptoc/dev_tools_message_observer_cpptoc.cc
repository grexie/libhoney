// Copyright (c) 2023 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the Honeycomb translator tool. If making changes by
// hand only do so within the body of existing method and function
// implementations. See the translator.README.txt file in the tools directory
// for more information.
//
// $hash=d32b3a49e312971ee0c825fe765c584c033d65e9$
//

#include "libhoney_dll/cpptoc/dev_tools_message_observer_cpptoc.h"
#include "libhoney_dll/ctocpp/browser_ctocpp.h"
#include "libhoney_dll/shutdown_checker.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

int HONEYCOMB_CALLBACK dev_tools_message_observer_on_dev_tools_message(
    struct _honey_dev_tools_message_observer_t* self,
    honey_browser_t* browser,
    const void* message,
    size_t message_size) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }
  // Verify param: browser; type: refptr_diff
  DCHECK(browser);
  if (!browser) {
    return 0;
  }
  // Verify param: message; type: simple_byaddr
  DCHECK(message);
  if (!message) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombDevToolsMessageObserverCppToC::Get(self)->OnDevToolsMessage(
      HoneycombBrowserCToCpp::Wrap(browser), message, message_size);

  // Return type: bool
  return _retval;
}

void HONEYCOMB_CALLBACK dev_tools_message_observer_on_dev_tools_method_result(
    struct _honey_dev_tools_message_observer_t* self,
    honey_browser_t* browser,
    int message_id,
    int success,
    const void* result,
    size_t result_size) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: browser; type: refptr_diff
  DCHECK(browser);
  if (!browser) {
    return;
  }
  // Unverified params: result

  // Execute
  HoneycombDevToolsMessageObserverCppToC::Get(self)->OnDevToolsMethodResult(
      HoneycombBrowserCToCpp::Wrap(browser), message_id, success ? true : false,
      result, result_size);
}

void HONEYCOMB_CALLBACK dev_tools_message_observer_on_dev_tools_event(
    struct _honey_dev_tools_message_observer_t* self,
    honey_browser_t* browser,
    const honey_string_t* method,
    const void* params,
    size_t params_size) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: browser; type: refptr_diff
  DCHECK(browser);
  if (!browser) {
    return;
  }
  // Verify param: method; type: string_byref_const
  DCHECK(method);
  if (!method) {
    return;
  }
  // Unverified params: params

  // Execute
  HoneycombDevToolsMessageObserverCppToC::Get(self)->OnDevToolsEvent(
      HoneycombBrowserCToCpp::Wrap(browser), HoneycombString(method), params, params_size);
}

void HONEYCOMB_CALLBACK dev_tools_message_observer_on_dev_tools_agent_attached(
    struct _honey_dev_tools_message_observer_t* self,
    honey_browser_t* browser) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: browser; type: refptr_diff
  DCHECK(browser);
  if (!browser) {
    return;
  }

  // Execute
  HoneycombDevToolsMessageObserverCppToC::Get(self)->OnDevToolsAgentAttached(
      HoneycombBrowserCToCpp::Wrap(browser));
}

void HONEYCOMB_CALLBACK dev_tools_message_observer_on_dev_tools_agent_detached(
    struct _honey_dev_tools_message_observer_t* self,
    honey_browser_t* browser) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: browser; type: refptr_diff
  DCHECK(browser);
  if (!browser) {
    return;
  }

  // Execute
  HoneycombDevToolsMessageObserverCppToC::Get(self)->OnDevToolsAgentDetached(
      HoneycombBrowserCToCpp::Wrap(browser));
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

HoneycombDevToolsMessageObserverCppToC::HoneycombDevToolsMessageObserverCppToC() {
  GetStruct()->on_dev_tools_message =
      dev_tools_message_observer_on_dev_tools_message;
  GetStruct()->on_dev_tools_method_result =
      dev_tools_message_observer_on_dev_tools_method_result;
  GetStruct()->on_dev_tools_event =
      dev_tools_message_observer_on_dev_tools_event;
  GetStruct()->on_dev_tools_agent_attached =
      dev_tools_message_observer_on_dev_tools_agent_attached;
  GetStruct()->on_dev_tools_agent_detached =
      dev_tools_message_observer_on_dev_tools_agent_detached;
}

// DESTRUCTOR - Do not edit by hand.

HoneycombDevToolsMessageObserverCppToC::~HoneycombDevToolsMessageObserverCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
HoneycombRefPtr<HoneycombDevToolsMessageObserver>
HoneycombCppToCRefCounted<HoneycombDevToolsMessageObserverCppToC,
                    HoneycombDevToolsMessageObserver,
                    honey_dev_tools_message_observer_t>::
    UnwrapDerived(HoneycombWrapperType type, honey_dev_tools_message_observer_t* s) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType
    HoneycombCppToCRefCounted<HoneycombDevToolsMessageObserverCppToC,
                        HoneycombDevToolsMessageObserver,
                        honey_dev_tools_message_observer_t>::kWrapperType =
        WT_DEV_TOOLS_MESSAGE_OBSERVER;