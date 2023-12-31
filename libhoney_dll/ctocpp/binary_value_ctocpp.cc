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
// $hash=caaf5dfe8d56c784e213b1590c21194f08084b70$
//

#include "libhoney_dll/ctocpp/binary_value_ctocpp.h"
#include "libhoney_dll/shutdown_checker.h"

// STATIC METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall")
HoneycombRefPtr<HoneycombBinaryValue> HoneycombBinaryValue::Create(const void* data,
                                                 size_t data_size) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: data; type: simple_byaddr
  DCHECK(data);
  if (!data) {
    return nullptr;
  }

  // Execute
  honey_binary_value_t* _retval = honey_binary_value_create(data, data_size);

  // Return type: refptr_same
  return HoneycombBinaryValueCToCpp::Wrap(_retval);
}

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall") bool HoneycombBinaryValueCToCpp::IsValid() {
  shutdown_checker::AssertNotShutdown();

  honey_binary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, is_valid)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->is_valid(_struct);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall") bool HoneycombBinaryValueCToCpp::IsOwned() {
  shutdown_checker::AssertNotShutdown();

  honey_binary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, is_owned)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->is_owned(_struct);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
bool HoneycombBinaryValueCToCpp::IsSame(HoneycombRefPtr<HoneycombBinaryValue> that) {
  shutdown_checker::AssertNotShutdown();

  honey_binary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, is_same)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: that; type: refptr_same
  DCHECK(that.get());
  if (!that.get()) {
    return false;
  }

  // Execute
  int _retval = _struct->is_same(_struct, HoneycombBinaryValueCToCpp::Unwrap(that));

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
bool HoneycombBinaryValueCToCpp::IsEqual(HoneycombRefPtr<HoneycombBinaryValue> that) {
  shutdown_checker::AssertNotShutdown();

  honey_binary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, is_equal)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: that; type: refptr_same
  DCHECK(that.get());
  if (!that.get()) {
    return false;
  }

  // Execute
  int _retval = _struct->is_equal(_struct, HoneycombBinaryValueCToCpp::Unwrap(that));

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
HoneycombRefPtr<HoneycombBinaryValue> HoneycombBinaryValueCToCpp::Copy() {
  shutdown_checker::AssertNotShutdown();

  honey_binary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, copy)) {
    return nullptr;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_binary_value_t* _retval = _struct->copy(_struct);

  // Return type: refptr_same
  return HoneycombBinaryValueCToCpp::Wrap(_retval);
}

NO_SANITIZE("cfi-icall") size_t HoneycombBinaryValueCToCpp::GetSize() {
  shutdown_checker::AssertNotShutdown();

  honey_binary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_size)) {
    return 0;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  size_t _retval = _struct->get_size(_struct);

  // Return type: simple
  return _retval;
}

NO_SANITIZE("cfi-icall")
size_t HoneycombBinaryValueCToCpp::GetData(void* buffer,
                                     size_t buffer_size,
                                     size_t data_offset) {
  shutdown_checker::AssertNotShutdown();

  honey_binary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_data)) {
    return 0;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: buffer; type: simple_byaddr
  DCHECK(buffer);
  if (!buffer) {
    return 0;
  }

  // Execute
  size_t _retval = _struct->get_data(_struct, buffer, buffer_size, data_offset);

  // Return type: simple
  return _retval;
}

// CONSTRUCTOR - Do not edit by hand.

HoneycombBinaryValueCToCpp::HoneycombBinaryValueCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

HoneycombBinaryValueCToCpp::~HoneycombBinaryValueCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
honey_binary_value_t*
HoneycombCToCppRefCounted<HoneycombBinaryValueCToCpp, HoneycombBinaryValue, honey_binary_value_t>::
    UnwrapDerived(HoneycombWrapperType type, HoneycombBinaryValue* c) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCToCppRefCounted<HoneycombBinaryValueCToCpp,
                                   HoneycombBinaryValue,
                                   honey_binary_value_t>::kWrapperType =
    WT_BINARY_VALUE;
