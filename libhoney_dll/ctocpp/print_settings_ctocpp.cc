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
// $hash=388d22bd24b0e340d9509abec2b654680ac6779d$
//

#include "libhoney_dll/ctocpp/print_settings_ctocpp.h"
#include <algorithm>
#include "libhoney_dll/shutdown_checker.h"

// STATIC METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall")
HoneycombRefPtr<HoneycombPrintSettings> HoneycombPrintSettings::Create() {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_print_settings_t* _retval = honey_print_settings_create();

  // Return type: refptr_same
  return HoneycombPrintSettingsCToCpp::Wrap(_retval);
}

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall") bool HoneycombPrintSettingsCToCpp::IsValid() {
  shutdown_checker::AssertNotShutdown();

  honey_print_settings_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, is_valid)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->is_valid(_struct);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall") bool HoneycombPrintSettingsCToCpp::IsReadOnly() {
  shutdown_checker::AssertNotShutdown();

  honey_print_settings_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, is_read_only)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->is_read_only(_struct);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
void HoneycombPrintSettingsCToCpp::SetOrientation(bool landscape) {
  shutdown_checker::AssertNotShutdown();

  honey_print_settings_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, set_orientation)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  _struct->set_orientation(_struct, landscape);
}

NO_SANITIZE("cfi-icall") bool HoneycombPrintSettingsCToCpp::IsLandscape() {
  shutdown_checker::AssertNotShutdown();

  honey_print_settings_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, is_landscape)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->is_landscape(_struct);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
void HoneycombPrintSettingsCToCpp::SetPrinterPrintableArea(
    const HoneycombSize& physical_size_device_units,
    const HoneycombRect& printable_area_device_units,
    bool landscape_needs_flip) {
  shutdown_checker::AssertNotShutdown();

  honey_print_settings_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, set_printer_printable_area)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  _struct->set_printer_printable_area(_struct, &physical_size_device_units,
                                      &printable_area_device_units,
                                      landscape_needs_flip);
}

NO_SANITIZE("cfi-icall")
void HoneycombPrintSettingsCToCpp::SetDeviceName(const HoneycombString& name) {
  shutdown_checker::AssertNotShutdown();

  honey_print_settings_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, set_device_name)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Unverified params: name

  // Execute
  _struct->set_device_name(_struct, name.GetStruct());
}

NO_SANITIZE("cfi-icall") HoneycombString HoneycombPrintSettingsCToCpp::GetDeviceName() {
  shutdown_checker::AssertNotShutdown();

  honey_print_settings_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_device_name)) {
    return HoneycombString();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_string_userfree_t _retval = _struct->get_device_name(_struct);

  // Return type: string
  HoneycombString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

NO_SANITIZE("cfi-icall") void HoneycombPrintSettingsCToCpp::SetDPI(int dpi) {
  shutdown_checker::AssertNotShutdown();

  honey_print_settings_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, set_dpi)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  _struct->set_dpi(_struct, dpi);
}

NO_SANITIZE("cfi-icall") int HoneycombPrintSettingsCToCpp::GetDPI() {
  shutdown_checker::AssertNotShutdown();

  honey_print_settings_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_dpi)) {
    return 0;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->get_dpi(_struct);

  // Return type: simple
  return _retval;
}

NO_SANITIZE("cfi-icall")
void HoneycombPrintSettingsCToCpp::SetPageRanges(const PageRangeList& ranges) {
  shutdown_checker::AssertNotShutdown();

  honey_print_settings_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, set_page_ranges)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Translate param: ranges; type: simple_vec_byref_const
  const size_t rangesCount = ranges.size();
  honey_range_t* rangesList = NULL;
  if (rangesCount > 0) {
    rangesList = new honey_range_t[rangesCount];
    DCHECK(rangesList);
    if (rangesList) {
      for (size_t i = 0; i < rangesCount; ++i) {
        rangesList[i] = ranges[i];
      }
    }
  }

  // Execute
  _struct->set_page_ranges(_struct, rangesCount, rangesList);

  // Restore param:ranges; type: simple_vec_byref_const
  if (rangesList) {
    delete[] rangesList;
  }
}

NO_SANITIZE("cfi-icall") size_t HoneycombPrintSettingsCToCpp::GetPageRangesCount() {
  shutdown_checker::AssertNotShutdown();

  honey_print_settings_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_page_ranges_count)) {
    return 0;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  size_t _retval = _struct->get_page_ranges_count(_struct);

  // Return type: simple
  return _retval;
}

NO_SANITIZE("cfi-icall")
void HoneycombPrintSettingsCToCpp::GetPageRanges(PageRangeList& ranges) {
  shutdown_checker::AssertNotShutdown();

  honey_print_settings_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_page_ranges)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Translate param: ranges; type: simple_vec_byref
  size_t rangesSize = ranges.size();
  size_t rangesCount = std::max(GetPageRangesCount(), rangesSize);
  honey_range_t* rangesList = NULL;
  if (rangesCount > 0) {
    rangesList = new honey_range_t[rangesCount];
    DCHECK(rangesList);
    if (rangesList) {
      memset(rangesList, 0, sizeof(honey_range_t) * rangesCount);
    }
    if (rangesList && rangesSize > 0) {
      for (size_t i = 0; i < rangesSize; ++i) {
        rangesList[i] = ranges[i];
      }
    }
  }

  // Execute
  _struct->get_page_ranges(_struct, &rangesCount, rangesList);

  // Restore param:ranges; type: simple_vec_byref
  ranges.clear();
  if (rangesCount > 0 && rangesList) {
    for (size_t i = 0; i < rangesCount; ++i) {
      ranges.push_back(rangesList[i]);
    }
    delete[] rangesList;
  }
}

NO_SANITIZE("cfi-icall")
void HoneycombPrintSettingsCToCpp::SetSelectionOnly(bool selection_only) {
  shutdown_checker::AssertNotShutdown();

  honey_print_settings_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, set_selection_only)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  _struct->set_selection_only(_struct, selection_only);
}

NO_SANITIZE("cfi-icall") bool HoneycombPrintSettingsCToCpp::IsSelectionOnly() {
  shutdown_checker::AssertNotShutdown();

  honey_print_settings_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, is_selection_only)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->is_selection_only(_struct);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall") void HoneycombPrintSettingsCToCpp::SetCollate(bool collate) {
  shutdown_checker::AssertNotShutdown();

  honey_print_settings_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, set_collate)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  _struct->set_collate(_struct, collate);
}

NO_SANITIZE("cfi-icall") bool HoneycombPrintSettingsCToCpp::WillCollate() {
  shutdown_checker::AssertNotShutdown();

  honey_print_settings_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, will_collate)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->will_collate(_struct);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
void HoneycombPrintSettingsCToCpp::SetColorModel(ColorModel model) {
  shutdown_checker::AssertNotShutdown();

  honey_print_settings_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, set_color_model)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  _struct->set_color_model(_struct, model);
}

NO_SANITIZE("cfi-icall")
HoneycombPrintSettings::ColorModel HoneycombPrintSettingsCToCpp::GetColorModel() {
  shutdown_checker::AssertNotShutdown();

  honey_print_settings_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_color_model)) {
    return COLOR_MODEL_UNKNOWN;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_color_model_t _retval = _struct->get_color_model(_struct);

  // Return type: simple
  return _retval;
}

NO_SANITIZE("cfi-icall") void HoneycombPrintSettingsCToCpp::SetCopies(int copies) {
  shutdown_checker::AssertNotShutdown();

  honey_print_settings_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, set_copies)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  _struct->set_copies(_struct, copies);
}

NO_SANITIZE("cfi-icall") int HoneycombPrintSettingsCToCpp::GetCopies() {
  shutdown_checker::AssertNotShutdown();

  honey_print_settings_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_copies)) {
    return 0;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->get_copies(_struct);

  // Return type: simple
  return _retval;
}

NO_SANITIZE("cfi-icall")
void HoneycombPrintSettingsCToCpp::SetDuplexMode(DuplexMode mode) {
  shutdown_checker::AssertNotShutdown();

  honey_print_settings_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, set_duplex_mode)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  _struct->set_duplex_mode(_struct, mode);
}

NO_SANITIZE("cfi-icall")
HoneycombPrintSettings::DuplexMode HoneycombPrintSettingsCToCpp::GetDuplexMode() {
  shutdown_checker::AssertNotShutdown();

  honey_print_settings_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_duplex_mode)) {
    return DUPLEX_MODE_UNKNOWN;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_duplex_mode_t _retval = _struct->get_duplex_mode(_struct);

  // Return type: simple
  return _retval;
}

// CONSTRUCTOR - Do not edit by hand.

HoneycombPrintSettingsCToCpp::HoneycombPrintSettingsCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

HoneycombPrintSettingsCToCpp::~HoneycombPrintSettingsCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
honey_print_settings_t*
HoneycombCToCppRefCounted<HoneycombPrintSettingsCToCpp,
                    HoneycombPrintSettings,
                    honey_print_settings_t>::UnwrapDerived(HoneycombWrapperType type,
                                                         HoneycombPrintSettings* c) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCToCppRefCounted<HoneycombPrintSettingsCToCpp,
                                   HoneycombPrintSettings,
                                   honey_print_settings_t>::kWrapperType =
    WT_PRINT_SETTINGS;