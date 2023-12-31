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
// $hash=92edc8064b198b90008142f6d839810a498350d1$
//

#include "libhoney_dll/ctocpp/media_route_ctocpp.h"
#include "libhoney_dll/ctocpp/media_sink_ctocpp.h"
#include "libhoney_dll/ctocpp/media_source_ctocpp.h"
#include "libhoney_dll/shutdown_checker.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall") HoneycombString HoneycombMediaRouteCToCpp::GetId() {
  shutdown_checker::AssertNotShutdown();

  honey_media_route_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_id)) {
    return HoneycombString();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_string_userfree_t _retval = _struct->get_id(_struct);

  // Return type: string
  HoneycombString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

NO_SANITIZE("cfi-icall")
HoneycombRefPtr<HoneycombMediaSource> HoneycombMediaRouteCToCpp::GetSource() {
  shutdown_checker::AssertNotShutdown();

  honey_media_route_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_source)) {
    return nullptr;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_media_source_t* _retval = _struct->get_source(_struct);

  // Return type: refptr_same
  return HoneycombMediaSourceCToCpp::Wrap(_retval);
}

NO_SANITIZE("cfi-icall")
HoneycombRefPtr<HoneycombMediaSink> HoneycombMediaRouteCToCpp::GetSink() {
  shutdown_checker::AssertNotShutdown();

  honey_media_route_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_sink)) {
    return nullptr;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_media_sink_t* _retval = _struct->get_sink(_struct);

  // Return type: refptr_same
  return HoneycombMediaSinkCToCpp::Wrap(_retval);
}

NO_SANITIZE("cfi-icall")
void HoneycombMediaRouteCToCpp::SendRouteMessage(const void* message,
                                           size_t message_size) {
  shutdown_checker::AssertNotShutdown();

  honey_media_route_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, send_route_message)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: message; type: simple_byaddr
  DCHECK(message);
  if (!message) {
    return;
  }

  // Execute
  _struct->send_route_message(_struct, message, message_size);
}

NO_SANITIZE("cfi-icall") void HoneycombMediaRouteCToCpp::Terminate() {
  shutdown_checker::AssertNotShutdown();

  honey_media_route_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, terminate)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  _struct->terminate(_struct);
}

// CONSTRUCTOR - Do not edit by hand.

HoneycombMediaRouteCToCpp::HoneycombMediaRouteCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

HoneycombMediaRouteCToCpp::~HoneycombMediaRouteCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
honey_media_route_t*
HoneycombCToCppRefCounted<HoneycombMediaRouteCToCpp, HoneycombMediaRoute, honey_media_route_t>::
    UnwrapDerived(HoneycombWrapperType type, HoneycombMediaRoute* c) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCToCppRefCounted<HoneycombMediaRouteCToCpp,
                                   HoneycombMediaRoute,
                                   honey_media_route_t>::kWrapperType =
    WT_MEDIA_ROUTE;
