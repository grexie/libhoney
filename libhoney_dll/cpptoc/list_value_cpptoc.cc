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
// $hash=1431751cc07f3678e8e893c638391c9701a9e4fb$
//

#include "libhoney_dll/cpptoc/list_value_cpptoc.h"
#include "libhoney_dll/cpptoc/binary_value_cpptoc.h"
#include "libhoney_dll/cpptoc/dictionary_value_cpptoc.h"
#include "libhoney_dll/cpptoc/value_cpptoc.h"
#include "libhoney_dll/shutdown_checker.h"

// GLOBAL FUNCTIONS - Body may be edited by hand.

HONEYCOMB_EXPORT honey_list_value_t* honey_list_value_create() {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  HoneycombRefPtr<HoneycombListValue> _retval = HoneycombListValue::Create();

  // Return type: refptr_same
  return HoneycombListValueCppToC::Wrap(_retval);
}

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

int HONEYCOMB_CALLBACK list_value_is_valid(struct _honey_list_value_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombListValueCppToC::Get(self)->IsValid();

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK list_value_is_owned(struct _honey_list_value_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombListValueCppToC::Get(self)->IsOwned();

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK list_value_is_read_only(struct _honey_list_value_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombListValueCppToC::Get(self)->IsReadOnly();

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK list_value_is_same(struct _honey_list_value_t* self,
                                    struct _honey_list_value_t* that) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }
  // Verify param: that; type: refptr_same
  DCHECK(that);
  if (!that) {
    return 0;
  }

  // Execute
  bool _retval =
      HoneycombListValueCppToC::Get(self)->IsSame(HoneycombListValueCppToC::Unwrap(that));

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK list_value_is_equal(struct _honey_list_value_t* self,
                                     struct _honey_list_value_t* that) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }
  // Verify param: that; type: refptr_same
  DCHECK(that);
  if (!that) {
    return 0;
  }

  // Execute
  bool _retval =
      HoneycombListValueCppToC::Get(self)->IsEqual(HoneycombListValueCppToC::Unwrap(that));

  // Return type: bool
  return _retval;
}

struct _honey_list_value_t* HONEYCOMB_CALLBACK
list_value_copy(struct _honey_list_value_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombListValue> _retval = HoneycombListValueCppToC::Get(self)->Copy();

  // Return type: refptr_same
  return HoneycombListValueCppToC::Wrap(_retval);
}

int HONEYCOMB_CALLBACK list_value_set_size(struct _honey_list_value_t* self,
                                     size_t size) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombListValueCppToC::Get(self)->SetSize(size);

  // Return type: bool
  return _retval;
}

size_t HONEYCOMB_CALLBACK list_value_get_size(struct _honey_list_value_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  size_t _retval = HoneycombListValueCppToC::Get(self)->GetSize();

  // Return type: simple
  return _retval;
}

int HONEYCOMB_CALLBACK list_value_clear(struct _honey_list_value_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombListValueCppToC::Get(self)->Clear();

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK list_value_remove(struct _honey_list_value_t* self,
                                   size_t index) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombListValueCppToC::Get(self)->Remove(index);

  // Return type: bool
  return _retval;
}

honey_value_type_t HONEYCOMB_CALLBACK
list_value_get_type(struct _honey_list_value_t* self, size_t index) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return VTYPE_INVALID;
  }

  // Execute
  honey_value_type_t _retval = HoneycombListValueCppToC::Get(self)->GetType(index);

  // Return type: simple
  return _retval;
}

honey_value_t* HONEYCOMB_CALLBACK list_value_get_value(struct _honey_list_value_t* self,
                                               size_t index) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombValue> _retval = HoneycombListValueCppToC::Get(self)->GetValue(index);

  // Return type: refptr_same
  return HoneycombValueCppToC::Wrap(_retval);
}

int HONEYCOMB_CALLBACK list_value_get_bool(struct _honey_list_value_t* self,
                                     size_t index) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombListValueCppToC::Get(self)->GetBool(index);

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK list_value_get_int(struct _honey_list_value_t* self,
                                    size_t index) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  int _retval = HoneycombListValueCppToC::Get(self)->GetInt(index);

  // Return type: simple
  return _retval;
}

double HONEYCOMB_CALLBACK list_value_get_double(struct _honey_list_value_t* self,
                                          size_t index) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  double _retval = HoneycombListValueCppToC::Get(self)->GetDouble(index);

  // Return type: simple
  return _retval;
}

honey_string_userfree_t HONEYCOMB_CALLBACK
list_value_get_string(struct _honey_list_value_t* self, size_t index) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombString _retval = HoneycombListValueCppToC::Get(self)->GetString(index);

  // Return type: string
  return _retval.DetachToUserFree();
}

honey_binary_value_t* HONEYCOMB_CALLBACK
list_value_get_binary(struct _honey_list_value_t* self, size_t index) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombBinaryValue> _retval =
      HoneycombListValueCppToC::Get(self)->GetBinary(index);

  // Return type: refptr_same
  return HoneycombBinaryValueCppToC::Wrap(_retval);
}

honey_dictionary_value_t* HONEYCOMB_CALLBACK
list_value_get_dictionary(struct _honey_list_value_t* self, size_t index) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombDictionaryValue> _retval =
      HoneycombListValueCppToC::Get(self)->GetDictionary(index);

  // Return type: refptr_same
  return HoneycombDictionaryValueCppToC::Wrap(_retval);
}

struct _honey_list_value_t* HONEYCOMB_CALLBACK
list_value_get_list(struct _honey_list_value_t* self, size_t index) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombListValue> _retval =
      HoneycombListValueCppToC::Get(self)->GetList(index);

  // Return type: refptr_same
  return HoneycombListValueCppToC::Wrap(_retval);
}

int HONEYCOMB_CALLBACK list_value_set_value(struct _honey_list_value_t* self,
                                      size_t index,
                                      honey_value_t* value) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }
  // Verify param: value; type: refptr_same
  DCHECK(value);
  if (!value) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombListValueCppToC::Get(self)->SetValue(
      index, HoneycombValueCppToC::Unwrap(value));

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK list_value_set_null(struct _honey_list_value_t* self,
                                     size_t index) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombListValueCppToC::Get(self)->SetNull(index);

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK list_value_set_bool(struct _honey_list_value_t* self,
                                     size_t index,
                                     int value) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  bool _retval =
      HoneycombListValueCppToC::Get(self)->SetBool(index, value ? true : false);

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK list_value_set_int(struct _honey_list_value_t* self,
                                    size_t index,
                                    int value) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombListValueCppToC::Get(self)->SetInt(index, value);

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK list_value_set_double(struct _honey_list_value_t* self,
                                       size_t index,
                                       double value) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombListValueCppToC::Get(self)->SetDouble(index, value);

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK list_value_set_string(struct _honey_list_value_t* self,
                                       size_t index,
                                       const honey_string_t* value) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }
  // Unverified params: value

  // Execute
  bool _retval =
      HoneycombListValueCppToC::Get(self)->SetString(index, HoneycombString(value));

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK list_value_set_binary(struct _honey_list_value_t* self,
                                       size_t index,
                                       honey_binary_value_t* value) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }
  // Verify param: value; type: refptr_same
  DCHECK(value);
  if (!value) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombListValueCppToC::Get(self)->SetBinary(
      index, HoneycombBinaryValueCppToC::Unwrap(value));

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK list_value_set_dictionary(struct _honey_list_value_t* self,
                                           size_t index,
                                           honey_dictionary_value_t* value) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }
  // Verify param: value; type: refptr_same
  DCHECK(value);
  if (!value) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombListValueCppToC::Get(self)->SetDictionary(
      index, HoneycombDictionaryValueCppToC::Unwrap(value));

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK list_value_set_list(struct _honey_list_value_t* self,
                                     size_t index,
                                     struct _honey_list_value_t* value) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }
  // Verify param: value; type: refptr_same
  DCHECK(value);
  if (!value) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombListValueCppToC::Get(self)->SetList(
      index, HoneycombListValueCppToC::Unwrap(value));

  // Return type: bool
  return _retval;
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

HoneycombListValueCppToC::HoneycombListValueCppToC() {
  GetStruct()->is_valid = list_value_is_valid;
  GetStruct()->is_owned = list_value_is_owned;
  GetStruct()->is_read_only = list_value_is_read_only;
  GetStruct()->is_same = list_value_is_same;
  GetStruct()->is_equal = list_value_is_equal;
  GetStruct()->copy = list_value_copy;
  GetStruct()->set_size = list_value_set_size;
  GetStruct()->get_size = list_value_get_size;
  GetStruct()->clear = list_value_clear;
  GetStruct()->remove = list_value_remove;
  GetStruct()->get_type = list_value_get_type;
  GetStruct()->get_value = list_value_get_value;
  GetStruct()->get_bool = list_value_get_bool;
  GetStruct()->get_int = list_value_get_int;
  GetStruct()->get_double = list_value_get_double;
  GetStruct()->get_string = list_value_get_string;
  GetStruct()->get_binary = list_value_get_binary;
  GetStruct()->get_dictionary = list_value_get_dictionary;
  GetStruct()->get_list = list_value_get_list;
  GetStruct()->set_value = list_value_set_value;
  GetStruct()->set_null = list_value_set_null;
  GetStruct()->set_bool = list_value_set_bool;
  GetStruct()->set_int = list_value_set_int;
  GetStruct()->set_double = list_value_set_double;
  GetStruct()->set_string = list_value_set_string;
  GetStruct()->set_binary = list_value_set_binary;
  GetStruct()->set_dictionary = list_value_set_dictionary;
  GetStruct()->set_list = list_value_set_list;
}

// DESTRUCTOR - Do not edit by hand.

HoneycombListValueCppToC::~HoneycombListValueCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
HoneycombRefPtr<HoneycombListValue>
HoneycombCppToCRefCounted<HoneycombListValueCppToC, HoneycombListValue, honey_list_value_t>::
    UnwrapDerived(HoneycombWrapperType type, honey_list_value_t* s) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCppToCRefCounted<HoneycombListValueCppToC,
                                   HoneycombListValue,
                                   honey_list_value_t>::kWrapperType =
    WT_LIST_VALUE;