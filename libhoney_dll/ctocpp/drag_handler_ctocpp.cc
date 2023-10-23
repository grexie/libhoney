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
// $hash=de7454fdcb2d48f43bb22d95d989050c3dce0f16$
//

#include "libhoney_dll/ctocpp/drag_handler_ctocpp.h"
#include "libhoney_dll/cpptoc/browser_cpptoc.h"
#include "libhoney_dll/cpptoc/drag_data_cpptoc.h"
#include "libhoney_dll/cpptoc/frame_cpptoc.h"
#include "libhoney_dll/shutdown_checker.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall")
bool HoneycombDragHandlerCToCpp::OnDragEnter(HoneycombRefPtr<HoneycombBrowser> browser,
                                       HoneycombRefPtr<HoneycombDragData> dragData,
                                       DragOperationsMask mask) {
  shutdown_checker::AssertNotShutdown();

  honey_drag_handler_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_drag_enter)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: browser; type: refptr_diff
  DCHECK(browser.get());
  if (!browser.get()) {
    return false;
  }
  // Verify param: dragData; type: refptr_diff
  DCHECK(dragData.get());
  if (!dragData.get()) {
    return false;
  }

  // Execute
  int _retval = _struct->on_drag_enter(_struct, HoneycombBrowserCppToC::Wrap(browser),
                                       HoneycombDragDataCppToC::Wrap(dragData), mask);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
void HoneycombDragHandlerCToCpp::OnDraggableRegionsChanged(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombFrame> frame,
    const std::vector<HoneycombDraggableRegion>& regions) {
  shutdown_checker::AssertNotShutdown();

  honey_drag_handler_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_draggable_regions_changed)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: browser; type: refptr_diff
  DCHECK(browser.get());
  if (!browser.get()) {
    return;
  }
  // Verify param: frame; type: refptr_diff
  DCHECK(frame.get());
  if (!frame.get()) {
    return;
  }

  // Translate param: regions; type: simple_vec_byref_const
  const size_t regionsCount = regions.size();
  honey_draggable_region_t* regionsList = NULL;
  if (regionsCount > 0) {
    regionsList = new honey_draggable_region_t[regionsCount];
    DCHECK(regionsList);
    if (regionsList) {
      for (size_t i = 0; i < regionsCount; ++i) {
        regionsList[i] = regions[i];
      }
    }
  }

  // Execute
  _struct->on_draggable_regions_changed(
      _struct, HoneycombBrowserCppToC::Wrap(browser), HoneycombFrameCppToC::Wrap(frame),
      regionsCount, regionsList);

  // Restore param:regions; type: simple_vec_byref_const
  if (regionsList) {
    delete[] regionsList;
  }
}

// CONSTRUCTOR - Do not edit by hand.

HoneycombDragHandlerCToCpp::HoneycombDragHandlerCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

HoneycombDragHandlerCToCpp::~HoneycombDragHandlerCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
honey_drag_handler_t*
HoneycombCToCppRefCounted<HoneycombDragHandlerCToCpp, HoneycombDragHandler, honey_drag_handler_t>::
    UnwrapDerived(HoneycombWrapperType type, HoneycombDragHandler* c) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCToCppRefCounted<HoneycombDragHandlerCToCpp,
                                   HoneycombDragHandler,
                                   honey_drag_handler_t>::kWrapperType =
    WT_DRAG_HANDLER;