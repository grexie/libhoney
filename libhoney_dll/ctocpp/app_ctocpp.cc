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
// $hash=fb2f03e369354152db16f5a6b6c6b93e87c42fa5$
//

#include "libhoney_dll/ctocpp/app_ctocpp.h"
#include "libhoney_dll/cpptoc/command_line_cpptoc.h"
#include "libhoney_dll/cpptoc/scheme_registrar_cpptoc.h"
#include "libhoney_dll/ctocpp/browser_process_handler_ctocpp.h"
#include "libhoney_dll/ctocpp/render_process_handler_ctocpp.h"
#include "libhoney_dll/ctocpp/resource_bundle_handler_ctocpp.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall")
void HoneycombAppCToCpp::OnBeforeCommandLineProcessing(
    const HoneycombString& process_type,
    HoneycombRefPtr<HoneycombCommandLine> command_line) {
  honey_app_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_before_command_line_processing)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: command_line; type: refptr_diff
  DCHECK(command_line.get());
  if (!command_line.get()) {
    return;
  }
  // Unverified params: process_type

  // Execute
  _struct->on_before_command_line_processing(
      _struct, process_type.GetStruct(),
      HoneycombCommandLineCppToC::Wrap(command_line));
}

NO_SANITIZE("cfi-icall")
void HoneycombAppCToCpp::OnRegisterCustomSchemes(
    HoneycombRawPtr<HoneycombSchemeRegistrar> registrar) {
  honey_app_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_register_custom_schemes)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: registrar; type: rawptr_diff
  DCHECK(registrar);
  if (!registrar) {
    return;
  }

  // Translate param: registrar; type: rawptr_diff
  HoneycombOwnPtr<HoneycombSchemeRegistrarCppToC> registrarPtr(
      HoneycombSchemeRegistrarCppToC::WrapRaw(registrar));

  // Execute
  _struct->on_register_custom_schemes(_struct, registrarPtr->GetStruct());
}

NO_SANITIZE("cfi-icall")
HoneycombRefPtr<HoneycombResourceBundleHandler> HoneycombAppCToCpp::GetResourceBundleHandler() {
  honey_app_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_resource_bundle_handler)) {
    return nullptr;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_resource_bundle_handler_t* _retval =
      _struct->get_resource_bundle_handler(_struct);

  // Return type: refptr_same
  return HoneycombResourceBundleHandlerCToCpp::Wrap(_retval);
}

NO_SANITIZE("cfi-icall")
HoneycombRefPtr<HoneycombBrowserProcessHandler> HoneycombAppCToCpp::GetBrowserProcessHandler() {
  honey_app_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_browser_process_handler)) {
    return nullptr;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_browser_process_handler_t* _retval =
      _struct->get_browser_process_handler(_struct);

  // Return type: refptr_same
  return HoneycombBrowserProcessHandlerCToCpp::Wrap(_retval);
}

NO_SANITIZE("cfi-icall")
HoneycombRefPtr<HoneycombRenderProcessHandler> HoneycombAppCToCpp::GetRenderProcessHandler() {
  honey_app_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_render_process_handler)) {
    return nullptr;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_render_process_handler_t* _retval =
      _struct->get_render_process_handler(_struct);

  // Return type: refptr_same
  return HoneycombRenderProcessHandlerCToCpp::Wrap(_retval);
}

// CONSTRUCTOR - Do not edit by hand.

HoneycombAppCToCpp::HoneycombAppCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

HoneycombAppCToCpp::~HoneycombAppCToCpp() {}

template <>
honey_app_t* HoneycombCToCppRefCounted<HoneycombAppCToCpp, HoneycombApp, honey_app_t>::UnwrapDerived(
    HoneycombWrapperType type,
    HoneycombApp* c) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType
    HoneycombCToCppRefCounted<HoneycombAppCToCpp, HoneycombApp, honey_app_t>::kWrapperType = WT_APP;
