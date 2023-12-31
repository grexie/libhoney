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
// $hash=74394106da192b6b9fc284bbf8899ae3d5ea0b8d$
//

#include "libhoney_dll/cpptoc/x509cert_principal_cpptoc.h"
#include "libhoney_dll/shutdown_checker.h"
#include "libhoney_dll/transfer_util.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

honey_string_userfree_t HONEYCOMB_CALLBACK
x509cert_principal_get_display_name(struct _honey_x509cert_principal_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombString _retval = HoneycombX509CertPrincipalCppToC::Get(self)->GetDisplayName();

  // Return type: string
  return _retval.DetachToUserFree();
}

honey_string_userfree_t HONEYCOMB_CALLBACK
x509cert_principal_get_common_name(struct _honey_x509cert_principal_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombString _retval = HoneycombX509CertPrincipalCppToC::Get(self)->GetCommonName();

  // Return type: string
  return _retval.DetachToUserFree();
}

honey_string_userfree_t HONEYCOMB_CALLBACK
x509cert_principal_get_locality_name(struct _honey_x509cert_principal_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombString _retval = HoneycombX509CertPrincipalCppToC::Get(self)->GetLocalityName();

  // Return type: string
  return _retval.DetachToUserFree();
}

honey_string_userfree_t HONEYCOMB_CALLBACK
x509cert_principal_get_state_or_province_name(
    struct _honey_x509cert_principal_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombString _retval =
      HoneycombX509CertPrincipalCppToC::Get(self)->GetStateOrProvinceName();

  // Return type: string
  return _retval.DetachToUserFree();
}

honey_string_userfree_t HONEYCOMB_CALLBACK
x509cert_principal_get_country_name(struct _honey_x509cert_principal_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombString _retval = HoneycombX509CertPrincipalCppToC::Get(self)->GetCountryName();

  // Return type: string
  return _retval.DetachToUserFree();
}

void HONEYCOMB_CALLBACK x509cert_principal_get_organization_names(
    struct _honey_x509cert_principal_t* self,
    honey_string_list_t names) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: names; type: string_vec_byref
  DCHECK(names);
  if (!names) {
    return;
  }

  // Translate param: names; type: string_vec_byref
  std::vector<HoneycombString> namesList;
  transfer_string_list_contents(names, namesList);

  // Execute
  HoneycombX509CertPrincipalCppToC::Get(self)->GetOrganizationNames(namesList);

  // Restore param: names; type: string_vec_byref
  honey_string_list_clear(names);
  transfer_string_list_contents(namesList, names);
}

void HONEYCOMB_CALLBACK x509cert_principal_get_organization_unit_names(
    struct _honey_x509cert_principal_t* self,
    honey_string_list_t names) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: names; type: string_vec_byref
  DCHECK(names);
  if (!names) {
    return;
  }

  // Translate param: names; type: string_vec_byref
  std::vector<HoneycombString> namesList;
  transfer_string_list_contents(names, namesList);

  // Execute
  HoneycombX509CertPrincipalCppToC::Get(self)->GetOrganizationUnitNames(namesList);

  // Restore param: names; type: string_vec_byref
  honey_string_list_clear(names);
  transfer_string_list_contents(namesList, names);
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

HoneycombX509CertPrincipalCppToC::HoneycombX509CertPrincipalCppToC() {
  GetStruct()->get_display_name = x509cert_principal_get_display_name;
  GetStruct()->get_common_name = x509cert_principal_get_common_name;
  GetStruct()->get_locality_name = x509cert_principal_get_locality_name;
  GetStruct()->get_state_or_province_name =
      x509cert_principal_get_state_or_province_name;
  GetStruct()->get_country_name = x509cert_principal_get_country_name;
  GetStruct()->get_organization_names =
      x509cert_principal_get_organization_names;
  GetStruct()->get_organization_unit_names =
      x509cert_principal_get_organization_unit_names;
}

// DESTRUCTOR - Do not edit by hand.

HoneycombX509CertPrincipalCppToC::~HoneycombX509CertPrincipalCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
HoneycombRefPtr<HoneycombX509CertPrincipal> HoneycombCppToCRefCounted<
    HoneycombX509CertPrincipalCppToC,
    HoneycombX509CertPrincipal,
    honey_x509cert_principal_t>::UnwrapDerived(HoneycombWrapperType type,
                                             honey_x509cert_principal_t* s) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCppToCRefCounted<HoneycombX509CertPrincipalCppToC,
                                   HoneycombX509CertPrincipal,
                                   honey_x509cert_principal_t>::kWrapperType =
    WT_X509CERT_PRINCIPAL;
