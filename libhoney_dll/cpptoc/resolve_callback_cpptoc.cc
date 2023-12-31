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
// $hash=e71369876af6d78b7f94e54b7ea5cf19c091a12a$
//

#include "libhoney_dll/cpptoc/resolve_callback_cpptoc.h"
#include "libhoney_dll/shutdown_checker.h"
#include "libhoney_dll/transfer_util.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

void HONEYCOMB_CALLBACK
resolve_callback_on_resolve_completed(struct _honey_resolve_callback_t* self,
                                      honey_errorcode_t result,
                                      honey_string_list_t resolved_ips) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Unverified params: resolved_ips

  // Translate param: resolved_ips; type: string_vec_byref_const
  std::vector<HoneycombString> resolved_ipsList;
  transfer_string_list_contents(resolved_ips, resolved_ipsList);

  // Execute
  HoneycombResolveCallbackCppToC::Get(self)->OnResolveCompleted(result,
                                                          resolved_ipsList);
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

HoneycombResolveCallbackCppToC::HoneycombResolveCallbackCppToC() {
  GetStruct()->on_resolve_completed = resolve_callback_on_resolve_completed;
}

// DESTRUCTOR - Do not edit by hand.

HoneycombResolveCallbackCppToC::~HoneycombResolveCallbackCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
HoneycombRefPtr<HoneycombResolveCallback> HoneycombCppToCRefCounted<
    HoneycombResolveCallbackCppToC,
    HoneycombResolveCallback,
    honey_resolve_callback_t>::UnwrapDerived(HoneycombWrapperType type,
                                           honey_resolve_callback_t* s) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCppToCRefCounted<HoneycombResolveCallbackCppToC,
                                   HoneycombResolveCallback,
                                   honey_resolve_callback_t>::kWrapperType =
    WT_RESOLVE_CALLBACK;
