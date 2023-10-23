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
// $hash=21357f7bdec9fe05f50147f4605572ed795e6bcc$
//

#include "libhoney_dll/ctocpp/dictionary_value_ctocpp.h"
#include "libhoney_dll/ctocpp/binary_value_ctocpp.h"
#include "libhoney_dll/ctocpp/list_value_ctocpp.h"
#include "libhoney_dll/ctocpp/value_ctocpp.h"
#include "libhoney_dll/shutdown_checker.h"
#include "libhoney_dll/transfer_util.h"

// STATIC METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall")
HoneycombRefPtr<HoneycombDictionaryValue> HoneycombDictionaryValue::Create() {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_dictionary_value_t* _retval = honey_dictionary_value_create();

  // Return type: refptr_same
  return HoneycombDictionaryValueCToCpp::Wrap(_retval);
}

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall") bool HoneycombDictionaryValueCToCpp::IsValid() {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, is_valid)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->is_valid(_struct);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall") bool HoneycombDictionaryValueCToCpp::IsOwned() {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, is_owned)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->is_owned(_struct);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall") bool HoneycombDictionaryValueCToCpp::IsReadOnly() {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
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
bool HoneycombDictionaryValueCToCpp::IsSame(HoneycombRefPtr<HoneycombDictionaryValue> that) {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
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
  int _retval =
      _struct->is_same(_struct, HoneycombDictionaryValueCToCpp::Unwrap(that));

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
bool HoneycombDictionaryValueCToCpp::IsEqual(HoneycombRefPtr<HoneycombDictionaryValue> that) {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
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
  int _retval =
      _struct->is_equal(_struct, HoneycombDictionaryValueCToCpp::Unwrap(that));

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
HoneycombRefPtr<HoneycombDictionaryValue> HoneycombDictionaryValueCToCpp::Copy(
    bool exclude_empty_children) {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, copy)) {
    return nullptr;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_dictionary_value_t* _retval =
      _struct->copy(_struct, exclude_empty_children);

  // Return type: refptr_same
  return HoneycombDictionaryValueCToCpp::Wrap(_retval);
}

NO_SANITIZE("cfi-icall") size_t HoneycombDictionaryValueCToCpp::GetSize() {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_size)) {
    return 0;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  size_t _retval = _struct->get_size(_struct);

  // Return type: simple
  return _retval;
}

NO_SANITIZE("cfi-icall") bool HoneycombDictionaryValueCToCpp::Clear() {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, clear)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->clear(_struct);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
bool HoneycombDictionaryValueCToCpp::HasKey(const HoneycombString& key) {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, has_key)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: key; type: string_byref_const
  DCHECK(!key.empty());
  if (key.empty()) {
    return false;
  }

  // Execute
  int _retval = _struct->has_key(_struct, key.GetStruct());

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall") bool HoneycombDictionaryValueCToCpp::GetKeys(KeyList& keys) {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_keys)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Translate param: keys; type: string_vec_byref
  honey_string_list_t keysList = honey_string_list_alloc();
  DCHECK(keysList);
  if (keysList) {
    transfer_string_list_contents(keys, keysList);
  }

  // Execute
  int _retval = _struct->get_keys(_struct, keysList);

  // Restore param:keys; type: string_vec_byref
  if (keysList) {
    keys.clear();
    transfer_string_list_contents(keysList, keys);
    honey_string_list_free(keysList);
  }

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
bool HoneycombDictionaryValueCToCpp::Remove(const HoneycombString& key) {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, remove)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: key; type: string_byref_const
  DCHECK(!key.empty());
  if (key.empty()) {
    return false;
  }

  // Execute
  int _retval = _struct->remove(_struct, key.GetStruct());

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
HoneycombValueType HoneycombDictionaryValueCToCpp::GetType(const HoneycombString& key) {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_type)) {
    return VTYPE_INVALID;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: key; type: string_byref_const
  DCHECK(!key.empty());
  if (key.empty()) {
    return VTYPE_INVALID;
  }

  // Execute
  honey_value_type_t _retval = _struct->get_type(_struct, key.GetStruct());

  // Return type: simple
  return _retval;
}

NO_SANITIZE("cfi-icall")
HoneycombRefPtr<HoneycombValue> HoneycombDictionaryValueCToCpp::GetValue(const HoneycombString& key) {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_value)) {
    return nullptr;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: key; type: string_byref_const
  DCHECK(!key.empty());
  if (key.empty()) {
    return nullptr;
  }

  // Execute
  honey_value_t* _retval = _struct->get_value(_struct, key.GetStruct());

  // Return type: refptr_same
  return HoneycombValueCToCpp::Wrap(_retval);
}

NO_SANITIZE("cfi-icall")
bool HoneycombDictionaryValueCToCpp::GetBool(const HoneycombString& key) {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_bool)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: key; type: string_byref_const
  DCHECK(!key.empty());
  if (key.empty()) {
    return false;
  }

  // Execute
  int _retval = _struct->get_bool(_struct, key.GetStruct());

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
int HoneycombDictionaryValueCToCpp::GetInt(const HoneycombString& key) {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_int)) {
    return 0;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: key; type: string_byref_const
  DCHECK(!key.empty());
  if (key.empty()) {
    return 0;
  }

  // Execute
  int _retval = _struct->get_int(_struct, key.GetStruct());

  // Return type: simple
  return _retval;
}

NO_SANITIZE("cfi-icall")
double HoneycombDictionaryValueCToCpp::GetDouble(const HoneycombString& key) {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_double)) {
    return 0;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: key; type: string_byref_const
  DCHECK(!key.empty());
  if (key.empty()) {
    return 0;
  }

  // Execute
  double _retval = _struct->get_double(_struct, key.GetStruct());

  // Return type: simple
  return _retval;
}

NO_SANITIZE("cfi-icall")
HoneycombString HoneycombDictionaryValueCToCpp::GetString(const HoneycombString& key) {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_string)) {
    return HoneycombString();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: key; type: string_byref_const
  DCHECK(!key.empty());
  if (key.empty()) {
    return HoneycombString();
  }

  // Execute
  honey_string_userfree_t _retval = _struct->get_string(_struct, key.GetStruct());

  // Return type: string
  HoneycombString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

NO_SANITIZE("cfi-icall")
HoneycombRefPtr<HoneycombBinaryValue> HoneycombDictionaryValueCToCpp::GetBinary(
    const HoneycombString& key) {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_binary)) {
    return nullptr;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: key; type: string_byref_const
  DCHECK(!key.empty());
  if (key.empty()) {
    return nullptr;
  }

  // Execute
  honey_binary_value_t* _retval = _struct->get_binary(_struct, key.GetStruct());

  // Return type: refptr_same
  return HoneycombBinaryValueCToCpp::Wrap(_retval);
}

NO_SANITIZE("cfi-icall")
HoneycombRefPtr<HoneycombDictionaryValue> HoneycombDictionaryValueCToCpp::GetDictionary(
    const HoneycombString& key) {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_dictionary)) {
    return nullptr;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: key; type: string_byref_const
  DCHECK(!key.empty());
  if (key.empty()) {
    return nullptr;
  }

  // Execute
  honey_dictionary_value_t* _retval =
      _struct->get_dictionary(_struct, key.GetStruct());

  // Return type: refptr_same
  return HoneycombDictionaryValueCToCpp::Wrap(_retval);
}

NO_SANITIZE("cfi-icall")
HoneycombRefPtr<HoneycombListValue> HoneycombDictionaryValueCToCpp::GetList(
    const HoneycombString& key) {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_list)) {
    return nullptr;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: key; type: string_byref_const
  DCHECK(!key.empty());
  if (key.empty()) {
    return nullptr;
  }

  // Execute
  honey_list_value_t* _retval = _struct->get_list(_struct, key.GetStruct());

  // Return type: refptr_same
  return HoneycombListValueCToCpp::Wrap(_retval);
}

NO_SANITIZE("cfi-icall")
bool HoneycombDictionaryValueCToCpp::SetValue(const HoneycombString& key,
                                        HoneycombRefPtr<HoneycombValue> value) {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, set_value)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: key; type: string_byref_const
  DCHECK(!key.empty());
  if (key.empty()) {
    return false;
  }
  // Verify param: value; type: refptr_same
  DCHECK(value.get());
  if (!value.get()) {
    return false;
  }

  // Execute
  int _retval = _struct->set_value(_struct, key.GetStruct(),
                                   HoneycombValueCToCpp::Unwrap(value));

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
bool HoneycombDictionaryValueCToCpp::SetNull(const HoneycombString& key) {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, set_null)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: key; type: string_byref_const
  DCHECK(!key.empty());
  if (key.empty()) {
    return false;
  }

  // Execute
  int _retval = _struct->set_null(_struct, key.GetStruct());

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
bool HoneycombDictionaryValueCToCpp::SetBool(const HoneycombString& key, bool value) {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, set_bool)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: key; type: string_byref_const
  DCHECK(!key.empty());
  if (key.empty()) {
    return false;
  }

  // Execute
  int _retval = _struct->set_bool(_struct, key.GetStruct(), value);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
bool HoneycombDictionaryValueCToCpp::SetInt(const HoneycombString& key, int value) {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, set_int)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: key; type: string_byref_const
  DCHECK(!key.empty());
  if (key.empty()) {
    return false;
  }

  // Execute
  int _retval = _struct->set_int(_struct, key.GetStruct(), value);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
bool HoneycombDictionaryValueCToCpp::SetDouble(const HoneycombString& key, double value) {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, set_double)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: key; type: string_byref_const
  DCHECK(!key.empty());
  if (key.empty()) {
    return false;
  }

  // Execute
  int _retval = _struct->set_double(_struct, key.GetStruct(), value);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
bool HoneycombDictionaryValueCToCpp::SetString(const HoneycombString& key,
                                         const HoneycombString& value) {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, set_string)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: key; type: string_byref_const
  DCHECK(!key.empty());
  if (key.empty()) {
    return false;
  }
  // Unverified params: value

  // Execute
  int _retval =
      _struct->set_string(_struct, key.GetStruct(), value.GetStruct());

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
bool HoneycombDictionaryValueCToCpp::SetBinary(const HoneycombString& key,
                                         HoneycombRefPtr<HoneycombBinaryValue> value) {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, set_binary)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: key; type: string_byref_const
  DCHECK(!key.empty());
  if (key.empty()) {
    return false;
  }
  // Verify param: value; type: refptr_same
  DCHECK(value.get());
  if (!value.get()) {
    return false;
  }

  // Execute
  int _retval = _struct->set_binary(_struct, key.GetStruct(),
                                    HoneycombBinaryValueCToCpp::Unwrap(value));

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
bool HoneycombDictionaryValueCToCpp::SetDictionary(
    const HoneycombString& key,
    HoneycombRefPtr<HoneycombDictionaryValue> value) {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, set_dictionary)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: key; type: string_byref_const
  DCHECK(!key.empty());
  if (key.empty()) {
    return false;
  }
  // Verify param: value; type: refptr_same
  DCHECK(value.get());
  if (!value.get()) {
    return false;
  }

  // Execute
  int _retval = _struct->set_dictionary(
      _struct, key.GetStruct(), HoneycombDictionaryValueCToCpp::Unwrap(value));

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
bool HoneycombDictionaryValueCToCpp::SetList(const HoneycombString& key,
                                       HoneycombRefPtr<HoneycombListValue> value) {
  shutdown_checker::AssertNotShutdown();

  honey_dictionary_value_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, set_list)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: key; type: string_byref_const
  DCHECK(!key.empty());
  if (key.empty()) {
    return false;
  }
  // Verify param: value; type: refptr_same
  DCHECK(value.get());
  if (!value.get()) {
    return false;
  }

  // Execute
  int _retval = _struct->set_list(_struct, key.GetStruct(),
                                  HoneycombListValueCToCpp::Unwrap(value));

  // Return type: bool
  return _retval ? true : false;
}

// CONSTRUCTOR - Do not edit by hand.

HoneycombDictionaryValueCToCpp::HoneycombDictionaryValueCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

HoneycombDictionaryValueCToCpp::~HoneycombDictionaryValueCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
honey_dictionary_value_t* HoneycombCToCppRefCounted<
    HoneycombDictionaryValueCToCpp,
    HoneycombDictionaryValue,
    honey_dictionary_value_t>::UnwrapDerived(HoneycombWrapperType type,
                                           HoneycombDictionaryValue* c) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCToCppRefCounted<HoneycombDictionaryValueCToCpp,
                                   HoneycombDictionaryValue,
                                   honey_dictionary_value_t>::kWrapperType =
    WT_DICTIONARY_VALUE;