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
// $hash=2df390a069798f60909a11621b3984e7a02cce40$
//

#include "libhoney_dll/cpptoc/find_handler_cpptoc.h"
#include "libhoney_dll/ctocpp/browser_ctocpp.h"
#include "libhoney_dll/shutdown_checker.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

void HONEYCOMB_CALLBACK find_handler_on_find_result(struct _honey_find_handler_t* self,
                                              honey_browser_t* browser,
                                              int identifier,
                                              int count,
                                              const honey_rect_t* selectionRect,
                                              int activeMatchOrdinal,
                                              int finalUpdate) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: browser; type: refptr_diff
  DCHECK(browser);
  if (!browser) {
    return;
  }
  // Verify param: selectionRect; type: simple_byref_const
  DCHECK(selectionRect);
  if (!selectionRect) {
    return;
  }

  // Translate param: selectionRect; type: simple_byref_const
  HoneycombRect selectionRectVal = selectionRect ? *selectionRect : HoneycombRect();

  // Execute
  HoneycombFindHandlerCppToC::Get(self)->OnFindResult(
      HoneycombBrowserCToCpp::Wrap(browser), identifier, count, selectionRectVal,
      activeMatchOrdinal, finalUpdate ? true : false);
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

HoneycombFindHandlerCppToC::HoneycombFindHandlerCppToC() {
  GetStruct()->on_find_result = find_handler_on_find_result;
}

// DESTRUCTOR - Do not edit by hand.

HoneycombFindHandlerCppToC::~HoneycombFindHandlerCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
HoneycombRefPtr<HoneycombFindHandler>
HoneycombCppToCRefCounted<HoneycombFindHandlerCppToC, HoneycombFindHandler, honey_find_handler_t>::
    UnwrapDerived(HoneycombWrapperType type, honey_find_handler_t* s) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCppToCRefCounted<HoneycombFindHandlerCppToC,
                                   HoneycombFindHandler,
                                   honey_find_handler_t>::kWrapperType =
    WT_FIND_HANDLER;
