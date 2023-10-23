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
// $hash=c677cabce869cc187866e07f53fcf235018d8978$
//

#include "libhoney_dll/ctocpp/preference_registrar_ctocpp.h"
#include "libhoney_dll/ctocpp/value_ctocpp.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall")
bool HoneycombPreferenceRegistrarCToCpp::AddPreference(
    const HoneycombString& name,
    HoneycombRefPtr<HoneycombValue> default_value) {
  honey_preference_registrar_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, add_preference)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: name; type: string_byref_const
  DCHECK(!name.empty());
  if (name.empty()) {
    return false;
  }
  // Verify param: default_value; type: refptr_same
  DCHECK(default_value.get());
  if (!default_value.get()) {
    return false;
  }

  // Execute
  int _retval = _struct->add_preference(_struct, name.GetStruct(),
                                        HoneycombValueCToCpp::Unwrap(default_value));

  // Return type: bool
  return _retval ? true : false;
}

// CONSTRUCTOR - Do not edit by hand.

HoneycombPreferenceRegistrarCToCpp::HoneycombPreferenceRegistrarCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

HoneycombPreferenceRegistrarCToCpp::~HoneycombPreferenceRegistrarCToCpp() {}

template <>
honey_preference_registrar_t* HoneycombCToCppScoped<HoneycombPreferenceRegistrarCToCpp,
                                            HoneycombPreferenceRegistrar,
                                            honey_preference_registrar_t>::
    UnwrapDerivedOwn(HoneycombWrapperType type, HoneycombOwnPtr<HoneycombPreferenceRegistrar> c) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
honey_preference_registrar_t* HoneycombCToCppScoped<HoneycombPreferenceRegistrarCToCpp,
                                            HoneycombPreferenceRegistrar,
                                            honey_preference_registrar_t>::
    UnwrapDerivedRaw(HoneycombWrapperType type, HoneycombRawPtr<HoneycombPreferenceRegistrar> c) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCToCppScoped<HoneycombPreferenceRegistrarCToCpp,
                               HoneycombPreferenceRegistrar,
                               honey_preference_registrar_t>::kWrapperType =
    WT_PREFERENCE_REGISTRAR;