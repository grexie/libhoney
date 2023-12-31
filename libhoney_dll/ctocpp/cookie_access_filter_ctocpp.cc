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
// $hash=2982f6957159b86ce039dfe9e67726a30bb98636$
//

#include "libhoney_dll/ctocpp/cookie_access_filter_ctocpp.h"
#include "libhoney_dll/cpptoc/browser_cpptoc.h"
#include "libhoney_dll/cpptoc/frame_cpptoc.h"
#include "libhoney_dll/cpptoc/request_cpptoc.h"
#include "libhoney_dll/cpptoc/response_cpptoc.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall")
bool HoneycombCookieAccessFilterCToCpp::CanSendCookie(HoneycombRefPtr<HoneycombBrowser> browser,
                                                HoneycombRefPtr<HoneycombFrame> frame,
                                                HoneycombRefPtr<HoneycombRequest> request,
                                                const HoneycombCookie& cookie) {
  honey_cookie_access_filter_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, can_send_cookie)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: request; type: refptr_diff
  DCHECK(request.get());
  if (!request.get()) {
    return false;
  }
  // Unverified params: browser, frame

  // Execute
  int _retval = _struct->can_send_cookie(
      _struct, HoneycombBrowserCppToC::Wrap(browser), HoneycombFrameCppToC::Wrap(frame),
      HoneycombRequestCppToC::Wrap(request), &cookie);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
bool HoneycombCookieAccessFilterCToCpp::CanSaveCookie(HoneycombRefPtr<HoneycombBrowser> browser,
                                                HoneycombRefPtr<HoneycombFrame> frame,
                                                HoneycombRefPtr<HoneycombRequest> request,
                                                HoneycombRefPtr<HoneycombResponse> response,
                                                const HoneycombCookie& cookie) {
  honey_cookie_access_filter_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, can_save_cookie)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: request; type: refptr_diff
  DCHECK(request.get());
  if (!request.get()) {
    return false;
  }
  // Verify param: response; type: refptr_diff
  DCHECK(response.get());
  if (!response.get()) {
    return false;
  }
  // Unverified params: browser, frame

  // Execute
  int _retval = _struct->can_save_cookie(
      _struct, HoneycombBrowserCppToC::Wrap(browser), HoneycombFrameCppToC::Wrap(frame),
      HoneycombRequestCppToC::Wrap(request), HoneycombResponseCppToC::Wrap(response),
      &cookie);

  // Return type: bool
  return _retval ? true : false;
}

// CONSTRUCTOR - Do not edit by hand.

HoneycombCookieAccessFilterCToCpp::HoneycombCookieAccessFilterCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

HoneycombCookieAccessFilterCToCpp::~HoneycombCookieAccessFilterCToCpp() {}

template <>
honey_cookie_access_filter_t* HoneycombCToCppRefCounted<
    HoneycombCookieAccessFilterCToCpp,
    HoneycombCookieAccessFilter,
    honey_cookie_access_filter_t>::UnwrapDerived(HoneycombWrapperType type,
                                               HoneycombCookieAccessFilter* c) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCToCppRefCounted<HoneycombCookieAccessFilterCToCpp,
                                   HoneycombCookieAccessFilter,
                                   honey_cookie_access_filter_t>::kWrapperType =
    WT_COOKIE_ACCESS_FILTER;
