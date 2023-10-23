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
// $hash=8883a3a31a9a48371dcc5b35ea7820a52f105bf1$
//

#include "libhoney_dll/ctocpp/v8stack_frame_ctocpp.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall") bool HoneycombV8StackFrameCToCpp::IsValid() {
  honey_v8stack_frame_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, is_valid)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->is_valid(_struct);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall") HoneycombString HoneycombV8StackFrameCToCpp::GetScriptName() {
  honey_v8stack_frame_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_script_name)) {
    return HoneycombString();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_string_userfree_t _retval = _struct->get_script_name(_struct);

  // Return type: string
  HoneycombString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

NO_SANITIZE("cfi-icall")
HoneycombString HoneycombV8StackFrameCToCpp::GetScriptNameOrSourceURL() {
  honey_v8stack_frame_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_script_name_or_source_url)) {
    return HoneycombString();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_string_userfree_t _retval =
      _struct->get_script_name_or_source_url(_struct);

  // Return type: string
  HoneycombString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

NO_SANITIZE("cfi-icall") HoneycombString HoneycombV8StackFrameCToCpp::GetFunctionName() {
  honey_v8stack_frame_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_function_name)) {
    return HoneycombString();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_string_userfree_t _retval = _struct->get_function_name(_struct);

  // Return type: string
  HoneycombString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

NO_SANITIZE("cfi-icall") int HoneycombV8StackFrameCToCpp::GetLineNumber() {
  honey_v8stack_frame_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_line_number)) {
    return 0;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->get_line_number(_struct);

  // Return type: simple
  return _retval;
}

NO_SANITIZE("cfi-icall") int HoneycombV8StackFrameCToCpp::GetColumn() {
  honey_v8stack_frame_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_column)) {
    return 0;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->get_column(_struct);

  // Return type: simple
  return _retval;
}

NO_SANITIZE("cfi-icall") bool HoneycombV8StackFrameCToCpp::IsEval() {
  honey_v8stack_frame_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, is_eval)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->is_eval(_struct);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall") bool HoneycombV8StackFrameCToCpp::IsConstructor() {
  honey_v8stack_frame_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, is_constructor)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->is_constructor(_struct);

  // Return type: bool
  return _retval ? true : false;
}

// CONSTRUCTOR - Do not edit by hand.

HoneycombV8StackFrameCToCpp::HoneycombV8StackFrameCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

HoneycombV8StackFrameCToCpp::~HoneycombV8StackFrameCToCpp() {}

template <>
honey_v8stack_frame_t*
HoneycombCToCppRefCounted<HoneycombV8StackFrameCToCpp,
                    HoneycombV8StackFrame,
                    honey_v8stack_frame_t>::UnwrapDerived(HoneycombWrapperType type,
                                                        HoneycombV8StackFrame* c) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCToCppRefCounted<HoneycombV8StackFrameCToCpp,
                                   HoneycombV8StackFrame,
                                   honey_v8stack_frame_t>::kWrapperType =
    WT_V8STACK_FRAME;