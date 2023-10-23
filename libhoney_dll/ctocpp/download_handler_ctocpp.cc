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
// $hash=23264ad881e05de87c12279841ec0f8c071c5503$
//

#include "libhoney_dll/ctocpp/download_handler_ctocpp.h"
#include "libhoney_dll/cpptoc/before_download_callback_cpptoc.h"
#include "libhoney_dll/cpptoc/browser_cpptoc.h"
#include "libhoney_dll/cpptoc/download_item_callback_cpptoc.h"
#include "libhoney_dll/cpptoc/download_item_cpptoc.h"
#include "libhoney_dll/shutdown_checker.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall")
bool HoneycombDownloadHandlerCToCpp::CanDownload(HoneycombRefPtr<HoneycombBrowser> browser,
                                           const HoneycombString& url,
                                           const HoneycombString& request_method) {
  shutdown_checker::AssertNotShutdown();

  honey_download_handler_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, can_download)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: browser; type: refptr_diff
  DCHECK(browser.get());
  if (!browser.get()) {
    return false;
  }
  // Verify param: url; type: string_byref_const
  DCHECK(!url.empty());
  if (url.empty()) {
    return false;
  }
  // Verify param: request_method; type: string_byref_const
  DCHECK(!request_method.empty());
  if (request_method.empty()) {
    return false;
  }

  // Execute
  int _retval =
      _struct->can_download(_struct, HoneycombBrowserCppToC::Wrap(browser),
                            url.GetStruct(), request_method.GetStruct());

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
void HoneycombDownloadHandlerCToCpp::OnBeforeDownload(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombDownloadItem> download_item,
    const HoneycombString& suggested_name,
    HoneycombRefPtr<HoneycombBeforeDownloadCallback> callback) {
  shutdown_checker::AssertNotShutdown();

  honey_download_handler_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_before_download)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: browser; type: refptr_diff
  DCHECK(browser.get());
  if (!browser.get()) {
    return;
  }
  // Verify param: download_item; type: refptr_diff
  DCHECK(download_item.get());
  if (!download_item.get()) {
    return;
  }
  // Verify param: suggested_name; type: string_byref_const
  DCHECK(!suggested_name.empty());
  if (suggested_name.empty()) {
    return;
  }
  // Verify param: callback; type: refptr_diff
  DCHECK(callback.get());
  if (!callback.get()) {
    return;
  }

  // Execute
  _struct->on_before_download(_struct, HoneycombBrowserCppToC::Wrap(browser),
                              HoneycombDownloadItemCppToC::Wrap(download_item),
                              suggested_name.GetStruct(),
                              HoneycombBeforeDownloadCallbackCppToC::Wrap(callback));
}

NO_SANITIZE("cfi-icall")
void HoneycombDownloadHandlerCToCpp::OnDownloadUpdated(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombDownloadItem> download_item,
    HoneycombRefPtr<HoneycombDownloadItemCallback> callback) {
  shutdown_checker::AssertNotShutdown();

  honey_download_handler_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_download_updated)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: browser; type: refptr_diff
  DCHECK(browser.get());
  if (!browser.get()) {
    return;
  }
  // Verify param: download_item; type: refptr_diff
  DCHECK(download_item.get());
  if (!download_item.get()) {
    return;
  }
  // Verify param: callback; type: refptr_diff
  DCHECK(callback.get());
  if (!callback.get()) {
    return;
  }

  // Execute
  _struct->on_download_updated(_struct, HoneycombBrowserCppToC::Wrap(browser),
                               HoneycombDownloadItemCppToC::Wrap(download_item),
                               HoneycombDownloadItemCallbackCppToC::Wrap(callback));
}

// CONSTRUCTOR - Do not edit by hand.

HoneycombDownloadHandlerCToCpp::HoneycombDownloadHandlerCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

HoneycombDownloadHandlerCToCpp::~HoneycombDownloadHandlerCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
honey_download_handler_t* HoneycombCToCppRefCounted<
    HoneycombDownloadHandlerCToCpp,
    HoneycombDownloadHandler,
    honey_download_handler_t>::UnwrapDerived(HoneycombWrapperType type,
                                           HoneycombDownloadHandler* c) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCToCppRefCounted<HoneycombDownloadHandlerCToCpp,
                                   HoneycombDownloadHandler,
                                   honey_download_handler_t>::kWrapperType =
    WT_DOWNLOAD_HANDLER;