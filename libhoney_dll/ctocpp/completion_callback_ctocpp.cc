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
// $hash=3c645623a6fdcc8eb1dc721fc84ca63a85cdbe3c$
//

#include "libhoney_dll/ctocpp/completion_callback_ctocpp.h"
#include "libhoney_dll/shutdown_checker.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall") void HoneycombCompletionCallbackCToCpp::OnComplete() {
  shutdown_checker::AssertNotShutdown();

  honey_completion_callback_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_complete)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  _struct->on_complete(_struct);
}

// CONSTRUCTOR - Do not edit by hand.

HoneycombCompletionCallbackCToCpp::HoneycombCompletionCallbackCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

HoneycombCompletionCallbackCToCpp::~HoneycombCompletionCallbackCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
honey_completion_callback_t* HoneycombCToCppRefCounted<
    HoneycombCompletionCallbackCToCpp,
    HoneycombCompletionCallback,
    honey_completion_callback_t>::UnwrapDerived(HoneycombWrapperType type,
                                              HoneycombCompletionCallback* c) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCToCppRefCounted<HoneycombCompletionCallbackCToCpp,
                                   HoneycombCompletionCallback,
                                   honey_completion_callback_t>::kWrapperType =
    WT_COMPLETION_CALLBACK;
