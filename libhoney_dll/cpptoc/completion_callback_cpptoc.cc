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
// $hash=d1faca27c390ce09058aebddcf7e0a55bdbea45c$
//

#include "libhoney_dll/cpptoc/completion_callback_cpptoc.h"
#include "libhoney_dll/shutdown_checker.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

void HONEYCOMB_CALLBACK
completion_callback_on_complete(struct _honey_completion_callback_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }

  // Execute
  HoneycombCompletionCallbackCppToC::Get(self)->OnComplete();
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

HoneycombCompletionCallbackCppToC::HoneycombCompletionCallbackCppToC() {
  GetStruct()->on_complete = completion_callback_on_complete;
}

// DESTRUCTOR - Do not edit by hand.

HoneycombCompletionCallbackCppToC::~HoneycombCompletionCallbackCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
HoneycombRefPtr<HoneycombCompletionCallback> HoneycombCppToCRefCounted<
    HoneycombCompletionCallbackCppToC,
    HoneycombCompletionCallback,
    honey_completion_callback_t>::UnwrapDerived(HoneycombWrapperType type,
                                              honey_completion_callback_t* s) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCppToCRefCounted<HoneycombCompletionCallbackCppToC,
                                   HoneycombCompletionCallback,
                                   honey_completion_callback_t>::kWrapperType =
    WT_COMPLETION_CALLBACK;
