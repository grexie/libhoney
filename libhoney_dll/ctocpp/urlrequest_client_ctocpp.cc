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
// $hash=4d593132f79049ee1f5bc752b17d0a5c3a9aff91$
//

#include "libhoney_dll/ctocpp/urlrequest_client_ctocpp.h"
#include "libhoney_dll/cpptoc/auth_callback_cpptoc.h"
#include "libhoney_dll/cpptoc/urlrequest_cpptoc.h"
#include "libhoney_dll/shutdown_checker.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall")
void HoneycombURLRequestClientCToCpp::OnRequestComplete(
    HoneycombRefPtr<HoneycombURLRequest> request) {
  shutdown_checker::AssertNotShutdown();

  honey_urlrequest_client_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_request_complete)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: request; type: refptr_diff
  DCHECK(request.get());
  if (!request.get()) {
    return;
  }

  // Execute
  _struct->on_request_complete(_struct, HoneycombURLRequestCppToC::Wrap(request));
}

NO_SANITIZE("cfi-icall")
void HoneycombURLRequestClientCToCpp::OnUploadProgress(
    HoneycombRefPtr<HoneycombURLRequest> request,
    int64_t current,
    int64_t total) {
  shutdown_checker::AssertNotShutdown();

  honey_urlrequest_client_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_upload_progress)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: request; type: refptr_diff
  DCHECK(request.get());
  if (!request.get()) {
    return;
  }

  // Execute
  _struct->on_upload_progress(_struct, HoneycombURLRequestCppToC::Wrap(request),
                              current, total);
}

NO_SANITIZE("cfi-icall")
void HoneycombURLRequestClientCToCpp::OnDownloadProgress(
    HoneycombRefPtr<HoneycombURLRequest> request,
    int64_t current,
    int64_t total) {
  shutdown_checker::AssertNotShutdown();

  honey_urlrequest_client_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_download_progress)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: request; type: refptr_diff
  DCHECK(request.get());
  if (!request.get()) {
    return;
  }

  // Execute
  _struct->on_download_progress(_struct, HoneycombURLRequestCppToC::Wrap(request),
                                current, total);
}

NO_SANITIZE("cfi-icall")
void HoneycombURLRequestClientCToCpp::OnDownloadData(HoneycombRefPtr<HoneycombURLRequest> request,
                                               const void* data,
                                               size_t data_length) {
  shutdown_checker::AssertNotShutdown();

  honey_urlrequest_client_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_download_data)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: request; type: refptr_diff
  DCHECK(request.get());
  if (!request.get()) {
    return;
  }
  // Verify param: data; type: simple_byaddr
  DCHECK(data);
  if (!data) {
    return;
  }

  // Execute
  _struct->on_download_data(_struct, HoneycombURLRequestCppToC::Wrap(request), data,
                            data_length);
}

NO_SANITIZE("cfi-icall")
bool HoneycombURLRequestClientCToCpp::GetAuthCredentials(
    bool isProxy,
    const HoneycombString& host,
    int port,
    const HoneycombString& realm,
    const HoneycombString& scheme,
    HoneycombRefPtr<HoneycombAuthCallback> callback) {
  shutdown_checker::AssertNotShutdown();

  honey_urlrequest_client_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_auth_credentials)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: host; type: string_byref_const
  DCHECK(!host.empty());
  if (host.empty()) {
    return false;
  }
  // Verify param: scheme; type: string_byref_const
  DCHECK(!scheme.empty());
  if (scheme.empty()) {
    return false;
  }
  // Verify param: callback; type: refptr_diff
  DCHECK(callback.get());
  if (!callback.get()) {
    return false;
  }
  // Unverified params: realm

  // Execute
  int _retval = _struct->get_auth_credentials(
      _struct, isProxy, host.GetStruct(), port, realm.GetStruct(),
      scheme.GetStruct(), HoneycombAuthCallbackCppToC::Wrap(callback));

  // Return type: bool
  return _retval ? true : false;
}

// CONSTRUCTOR - Do not edit by hand.

HoneycombURLRequestClientCToCpp::HoneycombURLRequestClientCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

HoneycombURLRequestClientCToCpp::~HoneycombURLRequestClientCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
honey_urlrequest_client_t* HoneycombCToCppRefCounted<
    HoneycombURLRequestClientCToCpp,
    HoneycombURLRequestClient,
    honey_urlrequest_client_t>::UnwrapDerived(HoneycombWrapperType type,
                                            HoneycombURLRequestClient* c) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCToCppRefCounted<HoneycombURLRequestClientCToCpp,
                                   HoneycombURLRequestClient,
                                   honey_urlrequest_client_t>::kWrapperType =
    WT_URLREQUEST_CLIENT;
