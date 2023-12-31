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
// $hash=b1531352b058e448d89cee9e34c9ff512f7b61dc$
//

#include "libhoney_dll/ctocpp/run_quick_menu_callback_ctocpp.h"
#include "libhoney_dll/shutdown_checker.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall")
void HoneycombRunQuickMenuCallbackCToCpp::Continue(int command_id,
                                             honey_event_flags_t event_flags) {
  shutdown_checker::AssertNotShutdown();

  honey_run_quick_menu_callback_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, cont)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  _struct->cont(_struct, command_id, event_flags);
}

NO_SANITIZE("cfi-icall") void HoneycombRunQuickMenuCallbackCToCpp::Cancel() {
  shutdown_checker::AssertNotShutdown();

  honey_run_quick_menu_callback_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, cancel)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  _struct->cancel(_struct);
}

// CONSTRUCTOR - Do not edit by hand.

HoneycombRunQuickMenuCallbackCToCpp::HoneycombRunQuickMenuCallbackCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

HoneycombRunQuickMenuCallbackCToCpp::~HoneycombRunQuickMenuCallbackCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
honey_run_quick_menu_callback_t* HoneycombCToCppRefCounted<
    HoneycombRunQuickMenuCallbackCToCpp,
    HoneycombRunQuickMenuCallback,
    honey_run_quick_menu_callback_t>::UnwrapDerived(HoneycombWrapperType type,
                                                  HoneycombRunQuickMenuCallback* c) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType
    HoneycombCToCppRefCounted<HoneycombRunQuickMenuCallbackCToCpp,
                        HoneycombRunQuickMenuCallback,
                        honey_run_quick_menu_callback_t>::kWrapperType =
        WT_RUN_QUICK_MENU_CALLBACK;
