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
// $hash=9040a3e4e5208ec11e2b29195a0bfa1c3f5ecffa$
//

#include "libhoney_dll/cpptoc/callback_cpptoc.h"
#include "libhoney_dll/shutdown_checker.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

void HONEYCOMB_CALLBACK callback_cont(struct _honey_callback_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }

  // Execute
  HoneycombCallbackCppToC::Get(self)->Continue();
}

void HONEYCOMB_CALLBACK callback_cancel(struct _honey_callback_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }

  // Execute
  HoneycombCallbackCppToC::Get(self)->Cancel();
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

HoneycombCallbackCppToC::HoneycombCallbackCppToC() {
  GetStruct()->cont = callback_cont;
  GetStruct()->cancel = callback_cancel;
}

// DESTRUCTOR - Do not edit by hand.

HoneycombCallbackCppToC::~HoneycombCallbackCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
HoneycombRefPtr<HoneycombCallback>
HoneycombCppToCRefCounted<HoneycombCallbackCppToC, HoneycombCallback, honey_callback_t>::
    UnwrapDerived(HoneycombWrapperType type, honey_callback_t* s) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCppToCRefCounted<HoneycombCallbackCppToC,
                                   HoneycombCallback,
                                   honey_callback_t>::kWrapperType = WT_CALLBACK;
