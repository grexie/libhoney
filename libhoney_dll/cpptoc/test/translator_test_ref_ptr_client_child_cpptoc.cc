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
// $hash=e94e27d12aa7fa48ea890cb8ab99f2e400d7ff23$
//

#include "libhoney_dll/cpptoc/test/translator_test_ref_ptr_client_child_cpptoc.h"
#include "libhoney_dll/shutdown_checker.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

int HONEYCOMB_CALLBACK translator_test_ref_ptr_client_child_get_other_value(
    struct _honey_translator_test_ref_ptr_client_child_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  int _retval =
      HoneycombTranslatorTestRefPtrClientChildCppToC::Get(self)->GetOtherValue();

  // Return type: simple
  return _retval;
}

int HONEYCOMB_CALLBACK translator_test_ref_ptr_client_child_get_value(
    struct _honey_translator_test_ref_ptr_client_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  int _retval =
      HoneycombTranslatorTestRefPtrClientChildCppToC::Get(
          reinterpret_cast<honey_translator_test_ref_ptr_client_child_t*>(self))
          ->GetValue();

  // Return type: simple
  return _retval;
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

HoneycombTranslatorTestRefPtrClientChildCppToC::
    HoneycombTranslatorTestRefPtrClientChildCppToC() {
  GetStruct()->get_other_value =
      translator_test_ref_ptr_client_child_get_other_value;
  GetStruct()->base.get_value = translator_test_ref_ptr_client_child_get_value;
}

// DESTRUCTOR - Do not edit by hand.

HoneycombTranslatorTestRefPtrClientChildCppToC::
    ~HoneycombTranslatorTestRefPtrClientChildCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
HoneycombRefPtr<HoneycombTranslatorTestRefPtrClientChild>
HoneycombCppToCRefCounted<HoneycombTranslatorTestRefPtrClientChildCppToC,
                    HoneycombTranslatorTestRefPtrClientChild,
                    honey_translator_test_ref_ptr_client_child_t>::
    UnwrapDerived(HoneycombWrapperType type,
                  honey_translator_test_ref_ptr_client_child_t* s) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCppToCRefCounted<
    HoneycombTranslatorTestRefPtrClientChildCppToC,
    HoneycombTranslatorTestRefPtrClientChild,
    honey_translator_test_ref_ptr_client_child_t>::kWrapperType =
    WT_TRANSLATOR_TEST_REF_PTR_CLIENT_CHILD;
