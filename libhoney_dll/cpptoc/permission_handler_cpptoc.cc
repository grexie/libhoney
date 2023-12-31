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
// $hash=0386addd1e401049523c8b13439412a802fb4211$
//

#include "libhoney_dll/cpptoc/permission_handler_cpptoc.h"
#include "libhoney_dll/ctocpp/browser_ctocpp.h"
#include "libhoney_dll/ctocpp/frame_ctocpp.h"
#include "libhoney_dll/ctocpp/media_access_callback_ctocpp.h"
#include "libhoney_dll/ctocpp/permission_prompt_callback_ctocpp.h"
#include "libhoney_dll/shutdown_checker.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

int HONEYCOMB_CALLBACK permission_handler_on_request_media_access_permission(
    struct _honey_permission_handler_t* self,
    honey_browser_t* browser,
    honey_frame_t* frame,
    const honey_string_t* requesting_origin,
    uint32_t requested_permissions,
    honey_media_access_callback_t* callback) {
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
  // Verify param: frame; type: refptr_diff
  DCHECK(frame);
  if (!frame) {
    return 0;
  }
  // Verify param: requesting_origin; type: string_byref_const
  DCHECK(requesting_origin);
  if (!requesting_origin) {
    return 0;
  }
  // Verify param: callback; type: refptr_diff
  DCHECK(callback);
  if (!callback) {
    return 0;
  }

  // Execute
  bool _retval =
      HoneycombPermissionHandlerCppToC::Get(self)->OnRequestMediaAccessPermission(
          HoneycombBrowserCToCpp::Wrap(browser), HoneycombFrameCToCpp::Wrap(frame),
          HoneycombString(requesting_origin), requested_permissions,
          HoneycombMediaAccessCallbackCToCpp::Wrap(callback));

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK permission_handler_on_show_permission_prompt(
    struct _honey_permission_handler_t* self,
    honey_browser_t* browser,
    uint64_t prompt_id,
    const honey_string_t* requesting_origin,
    uint32_t requested_permissions,
    honey_permission_prompt_callback_t* callback) {
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
  // Verify param: requesting_origin; type: string_byref_const
  DCHECK(requesting_origin);
  if (!requesting_origin) {
    return 0;
  }
  // Verify param: callback; type: refptr_diff
  DCHECK(callback);
  if (!callback) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombPermissionHandlerCppToC::Get(self)->OnShowPermissionPrompt(
      HoneycombBrowserCToCpp::Wrap(browser), prompt_id, HoneycombString(requesting_origin),
      requested_permissions, HoneycombPermissionPromptCallbackCToCpp::Wrap(callback));

  // Return type: bool
  return _retval;
}

void HONEYCOMB_CALLBACK permission_handler_on_dismiss_permission_prompt(
    struct _honey_permission_handler_t* self,
    honey_browser_t* browser,
    uint64_t prompt_id,
    honey_permission_request_result_t result) {
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
  HoneycombPermissionHandlerCppToC::Get(self)->OnDismissPermissionPrompt(
      HoneycombBrowserCToCpp::Wrap(browser), prompt_id, result);
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

HoneycombPermissionHandlerCppToC::HoneycombPermissionHandlerCppToC() {
  GetStruct()->on_request_media_access_permission =
      permission_handler_on_request_media_access_permission;
  GetStruct()->on_show_permission_prompt =
      permission_handler_on_show_permission_prompt;
  GetStruct()->on_dismiss_permission_prompt =
      permission_handler_on_dismiss_permission_prompt;
}

// DESTRUCTOR - Do not edit by hand.

HoneycombPermissionHandlerCppToC::~HoneycombPermissionHandlerCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
HoneycombRefPtr<HoneycombPermissionHandler> HoneycombCppToCRefCounted<
    HoneycombPermissionHandlerCppToC,
    HoneycombPermissionHandler,
    honey_permission_handler_t>::UnwrapDerived(HoneycombWrapperType type,
                                             honey_permission_handler_t* s) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCppToCRefCounted<HoneycombPermissionHandlerCppToC,
                                   HoneycombPermissionHandler,
                                   honey_permission_handler_t>::kWrapperType =
    WT_PERMISSION_HANDLER;
