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
// $hash=3ad767f0b424a886e1783b416f532fbaf6b8cbbe$
//

#include "libhoney_dll/cpptoc/accessibility_handler_cpptoc.h"
#include "libhoney_dll/ctocpp/value_ctocpp.h"
#include "libhoney_dll/shutdown_checker.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

void HONEYCOMB_CALLBACK accessibility_handler_on_accessibility_tree_change(
    struct _honey_accessibility_handler_t* self,
    struct _honey_value_t* value) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: value; type: refptr_diff
  DCHECK(value);
  if (!value) {
    return;
  }

  // Execute
  HoneycombAccessibilityHandlerCppToC::Get(self)->OnAccessibilityTreeChange(
      HoneycombValueCToCpp::Wrap(value));
}

void HONEYCOMB_CALLBACK accessibility_handler_on_accessibility_location_change(
    struct _honey_accessibility_handler_t* self,
    struct _honey_value_t* value) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: value; type: refptr_diff
  DCHECK(value);
  if (!value) {
    return;
  }

  // Execute
  HoneycombAccessibilityHandlerCppToC::Get(self)->OnAccessibilityLocationChange(
      HoneycombValueCToCpp::Wrap(value));
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

HoneycombAccessibilityHandlerCppToC::HoneycombAccessibilityHandlerCppToC() {
  GetStruct()->on_accessibility_tree_change =
      accessibility_handler_on_accessibility_tree_change;
  GetStruct()->on_accessibility_location_change =
      accessibility_handler_on_accessibility_location_change;
}

// DESTRUCTOR - Do not edit by hand.

HoneycombAccessibilityHandlerCppToC::~HoneycombAccessibilityHandlerCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
HoneycombRefPtr<HoneycombAccessibilityHandler> HoneycombCppToCRefCounted<
    HoneycombAccessibilityHandlerCppToC,
    HoneycombAccessibilityHandler,
    honey_accessibility_handler_t>::UnwrapDerived(HoneycombWrapperType type,
                                                honey_accessibility_handler_t*
                                                    s) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCppToCRefCounted<HoneycombAccessibilityHandlerCppToC,
                                   HoneycombAccessibilityHandler,
                                   honey_accessibility_handler_t>::kWrapperType =
    WT_ACCESSIBILITY_HANDLER;