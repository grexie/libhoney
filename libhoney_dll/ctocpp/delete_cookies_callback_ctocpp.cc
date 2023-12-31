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
// $hash=26d697d88a0312f47a3fe212b4c77cdb4e8b5800$
//

#include "libhoney_dll/ctocpp/delete_cookies_callback_ctocpp.h"
#include "libhoney_dll/shutdown_checker.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall")
void HoneycombDeleteCookiesCallbackCToCpp::OnComplete(int num_deleted) {
  shutdown_checker::AssertNotShutdown();

  honey_delete_cookies_callback_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_complete)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  _struct->on_complete(_struct, num_deleted);
}

// CONSTRUCTOR - Do not edit by hand.

HoneycombDeleteCookiesCallbackCToCpp::HoneycombDeleteCookiesCallbackCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

HoneycombDeleteCookiesCallbackCToCpp::~HoneycombDeleteCookiesCallbackCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
honey_delete_cookies_callback_t* HoneycombCToCppRefCounted<
    HoneycombDeleteCookiesCallbackCToCpp,
    HoneycombDeleteCookiesCallback,
    honey_delete_cookies_callback_t>::UnwrapDerived(HoneycombWrapperType type,
                                                  HoneycombDeleteCookiesCallback* c) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType
    HoneycombCToCppRefCounted<HoneycombDeleteCookiesCallbackCToCpp,
                        HoneycombDeleteCookiesCallback,
                        honey_delete_cookies_callback_t>::kWrapperType =
        WT_DELETE_COOKIES_CALLBACK;
