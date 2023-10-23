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
// $hash=ce7854af973f443c580f9f026be9db6c4cff6182$
//

#include "libhoney_dll/ctocpp/test/translator_test_ref_ptr_client_ctocpp.h"
#include "libhoney_dll/ctocpp/test/translator_test_ref_ptr_client_child_ctocpp.h"
#include "libhoney_dll/shutdown_checker.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall") int HoneycombTranslatorTestRefPtrClientCToCpp::GetValue() {
  shutdown_checker::AssertNotShutdown();

  honey_translator_test_ref_ptr_client_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_value)) {
    return 0;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->get_value(_struct);

  // Return type: simple
  return _retval;
}

// CONSTRUCTOR - Do not edit by hand.

HoneycombTranslatorTestRefPtrClientCToCpp::HoneycombTranslatorTestRefPtrClientCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

HoneycombTranslatorTestRefPtrClientCToCpp::~HoneycombTranslatorTestRefPtrClientCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
honey_translator_test_ref_ptr_client_t*
HoneycombCToCppRefCounted<HoneycombTranslatorTestRefPtrClientCToCpp,
                    HoneycombTranslatorTestRefPtrClient,
                    honey_translator_test_ref_ptr_client_t>::
    UnwrapDerived(HoneycombWrapperType type, HoneycombTranslatorTestRefPtrClient* c) {
  if (type == WT_TRANSLATOR_TEST_REF_PTR_CLIENT_CHILD) {
    return reinterpret_cast<honey_translator_test_ref_ptr_client_t*>(
        HoneycombTranslatorTestRefPtrClientChildCToCpp::Unwrap(
            reinterpret_cast<HoneycombTranslatorTestRefPtrClientChild*>(c)));
  }
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType
    HoneycombCToCppRefCounted<HoneycombTranslatorTestRefPtrClientCToCpp,
                        HoneycombTranslatorTestRefPtrClient,
                        honey_translator_test_ref_ptr_client_t>::kWrapperType =
        WT_TRANSLATOR_TEST_REF_PTR_CLIENT;