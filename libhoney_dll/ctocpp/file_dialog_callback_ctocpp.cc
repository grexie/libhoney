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
// $hash=01a59b2a4850603dcb6df74f33431d13bcf2c5c9$
//

#include "libhoney_dll/ctocpp/file_dialog_callback_ctocpp.h"
#include "libhoney_dll/shutdown_checker.h"
#include "libhoney_dll/transfer_util.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall")
void HoneycombFileDialogCallbackCToCpp::Continue(
    const std::vector<HoneycombString>& file_paths) {
  shutdown_checker::AssertNotShutdown();

  honey_file_dialog_callback_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, cont)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Unverified params: file_paths

  // Translate param: file_paths; type: string_vec_byref_const
  honey_string_list_t file_pathsList = honey_string_list_alloc();
  DCHECK(file_pathsList);
  if (file_pathsList) {
    transfer_string_list_contents(file_paths, file_pathsList);
  }

  // Execute
  _struct->cont(_struct, file_pathsList);

  // Restore param:file_paths; type: string_vec_byref_const
  if (file_pathsList) {
    honey_string_list_free(file_pathsList);
  }
}

NO_SANITIZE("cfi-icall") void HoneycombFileDialogCallbackCToCpp::Cancel() {
  shutdown_checker::AssertNotShutdown();

  honey_file_dialog_callback_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, cancel)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  _struct->cancel(_struct);
}

// CONSTRUCTOR - Do not edit by hand.

HoneycombFileDialogCallbackCToCpp::HoneycombFileDialogCallbackCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

HoneycombFileDialogCallbackCToCpp::~HoneycombFileDialogCallbackCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
honey_file_dialog_callback_t* HoneycombCToCppRefCounted<
    HoneycombFileDialogCallbackCToCpp,
    HoneycombFileDialogCallback,
    honey_file_dialog_callback_t>::UnwrapDerived(HoneycombWrapperType type,
                                               HoneycombFileDialogCallback* c) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCToCppRefCounted<HoneycombFileDialogCallbackCToCpp,
                                   HoneycombFileDialogCallback,
                                   honey_file_dialog_callback_t>::kWrapperType =
    WT_FILE_DIALOG_CALLBACK;