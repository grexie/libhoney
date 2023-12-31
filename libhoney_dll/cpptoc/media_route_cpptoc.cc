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
// $hash=3273e6b77a9d01d328b538480cfd2fb0ed45201c$
//

#include "libhoney_dll/cpptoc/media_route_cpptoc.h"
#include "libhoney_dll/cpptoc/media_sink_cpptoc.h"
#include "libhoney_dll/cpptoc/media_source_cpptoc.h"
#include "libhoney_dll/shutdown_checker.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

honey_string_userfree_t HONEYCOMB_CALLBACK
media_route_get_id(struct _honey_media_route_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombString _retval = HoneycombMediaRouteCppToC::Get(self)->GetId();

  // Return type: string
  return _retval.DetachToUserFree();
}

struct _honey_media_source_t* HONEYCOMB_CALLBACK
media_route_get_source(struct _honey_media_route_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombMediaSource> _retval =
      HoneycombMediaRouteCppToC::Get(self)->GetSource();

  // Return type: refptr_same
  return HoneycombMediaSourceCppToC::Wrap(_retval);
}

struct _honey_media_sink_t* HONEYCOMB_CALLBACK
media_route_get_sink(struct _honey_media_route_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombMediaSink> _retval = HoneycombMediaRouteCppToC::Get(self)->GetSink();

  // Return type: refptr_same
  return HoneycombMediaSinkCppToC::Wrap(_retval);
}

void HONEYCOMB_CALLBACK
media_route_send_route_message(struct _honey_media_route_t* self,
                               const void* message,
                               size_t message_size) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: message; type: simple_byaddr
  DCHECK(message);
  if (!message) {
    return;
  }

  // Execute
  HoneycombMediaRouteCppToC::Get(self)->SendRouteMessage(message, message_size);
}

void HONEYCOMB_CALLBACK media_route_terminate(struct _honey_media_route_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }

  // Execute
  HoneycombMediaRouteCppToC::Get(self)->Terminate();
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

HoneycombMediaRouteCppToC::HoneycombMediaRouteCppToC() {
  GetStruct()->get_id = media_route_get_id;
  GetStruct()->get_source = media_route_get_source;
  GetStruct()->get_sink = media_route_get_sink;
  GetStruct()->send_route_message = media_route_send_route_message;
  GetStruct()->terminate = media_route_terminate;
}

// DESTRUCTOR - Do not edit by hand.

HoneycombMediaRouteCppToC::~HoneycombMediaRouteCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
HoneycombRefPtr<HoneycombMediaRoute>
HoneycombCppToCRefCounted<HoneycombMediaRouteCppToC, HoneycombMediaRoute, honey_media_route_t>::
    UnwrapDerived(HoneycombWrapperType type, honey_media_route_t* s) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCppToCRefCounted<HoneycombMediaRouteCppToC,
                                   HoneycombMediaRoute,
                                   honey_media_route_t>::kWrapperType =
    WT_MEDIA_ROUTE;
