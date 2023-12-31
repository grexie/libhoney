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
// $hash=5a5997b69936ad47fb9ab5b6f4016f33c91110c7$
//

#include "libhoney_dll/ctocpp/x509cert_principal_ctocpp.h"
#include "libhoney_dll/shutdown_checker.h"
#include "libhoney_dll/transfer_util.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall")
HoneycombString HoneycombX509CertPrincipalCToCpp::GetDisplayName() {
  shutdown_checker::AssertNotShutdown();

  honey_x509cert_principal_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_display_name)) {
    return HoneycombString();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_string_userfree_t _retval = _struct->get_display_name(_struct);

  // Return type: string
  HoneycombString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

NO_SANITIZE("cfi-icall") HoneycombString HoneycombX509CertPrincipalCToCpp::GetCommonName() {
  shutdown_checker::AssertNotShutdown();

  honey_x509cert_principal_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_common_name)) {
    return HoneycombString();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_string_userfree_t _retval = _struct->get_common_name(_struct);

  // Return type: string
  HoneycombString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

NO_SANITIZE("cfi-icall")
HoneycombString HoneycombX509CertPrincipalCToCpp::GetLocalityName() {
  shutdown_checker::AssertNotShutdown();

  honey_x509cert_principal_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_locality_name)) {
    return HoneycombString();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_string_userfree_t _retval = _struct->get_locality_name(_struct);

  // Return type: string
  HoneycombString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

NO_SANITIZE("cfi-icall")
HoneycombString HoneycombX509CertPrincipalCToCpp::GetStateOrProvinceName() {
  shutdown_checker::AssertNotShutdown();

  honey_x509cert_principal_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_state_or_province_name)) {
    return HoneycombString();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_string_userfree_t _retval = _struct->get_state_or_province_name(_struct);

  // Return type: string
  HoneycombString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

NO_SANITIZE("cfi-icall")
HoneycombString HoneycombX509CertPrincipalCToCpp::GetCountryName() {
  shutdown_checker::AssertNotShutdown();

  honey_x509cert_principal_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_country_name)) {
    return HoneycombString();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_string_userfree_t _retval = _struct->get_country_name(_struct);

  // Return type: string
  HoneycombString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

NO_SANITIZE("cfi-icall")
void HoneycombX509CertPrincipalCToCpp::GetOrganizationNames(
    std::vector<HoneycombString>& names) {
  shutdown_checker::AssertNotShutdown();

  honey_x509cert_principal_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_organization_names)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Translate param: names; type: string_vec_byref
  honey_string_list_t namesList = honey_string_list_alloc();
  DCHECK(namesList);
  if (namesList) {
    transfer_string_list_contents(names, namesList);
  }

  // Execute
  _struct->get_organization_names(_struct, namesList);

  // Restore param:names; type: string_vec_byref
  if (namesList) {
    names.clear();
    transfer_string_list_contents(namesList, names);
    honey_string_list_free(namesList);
  }
}

NO_SANITIZE("cfi-icall")
void HoneycombX509CertPrincipalCToCpp::GetOrganizationUnitNames(
    std::vector<HoneycombString>& names) {
  shutdown_checker::AssertNotShutdown();

  honey_x509cert_principal_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_organization_unit_names)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Translate param: names; type: string_vec_byref
  honey_string_list_t namesList = honey_string_list_alloc();
  DCHECK(namesList);
  if (namesList) {
    transfer_string_list_contents(names, namesList);
  }

  // Execute
  _struct->get_organization_unit_names(_struct, namesList);

  // Restore param:names; type: string_vec_byref
  if (namesList) {
    names.clear();
    transfer_string_list_contents(namesList, names);
    honey_string_list_free(namesList);
  }
}

// CONSTRUCTOR - Do not edit by hand.

HoneycombX509CertPrincipalCToCpp::HoneycombX509CertPrincipalCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

HoneycombX509CertPrincipalCToCpp::~HoneycombX509CertPrincipalCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
honey_x509cert_principal_t* HoneycombCToCppRefCounted<
    HoneycombX509CertPrincipalCToCpp,
    HoneycombX509CertPrincipal,
    honey_x509cert_principal_t>::UnwrapDerived(HoneycombWrapperType type,
                                             HoneycombX509CertPrincipal* c) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCToCppRefCounted<HoneycombX509CertPrincipalCToCpp,
                                   HoneycombX509CertPrincipal,
                                   honey_x509cert_principal_t>::kWrapperType =
    WT_X509CERT_PRINCIPAL;
