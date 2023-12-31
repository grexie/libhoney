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
// $hash=329506b8051faec631eb200b1a0b8da46bcb4185$
//

#include "libhoney_dll/ctocpp/jsdialog_handler_ctocpp.h"
#include "libhoney_dll/cpptoc/browser_cpptoc.h"
#include "libhoney_dll/cpptoc/jsdialog_callback_cpptoc.h"
#include "libhoney_dll/shutdown_checker.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall")
bool HoneycombJSDialogHandlerCToCpp::OnJSDialog(
    HoneycombRefPtr<HoneycombBrowser> browser,
    const HoneycombString& origin_url,
    JSDialogType dialog_type,
    const HoneycombString& message_text,
    const HoneycombString& default_prompt_text,
    HoneycombRefPtr<HoneycombJSDialogCallback> callback,
    bool& suppress_message) {
  shutdown_checker::AssertNotShutdown();

  honey_jsdialog_handler_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_jsdialog)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: browser; type: refptr_diff
  DCHECK(browser.get());
  if (!browser.get()) {
    return false;
  }
  // Verify param: callback; type: refptr_diff
  DCHECK(callback.get());
  if (!callback.get()) {
    return false;
  }
  // Unverified params: origin_url, message_text, default_prompt_text

  // Translate param: suppress_message; type: bool_byref
  int suppress_messageInt = suppress_message;

  // Execute
  int _retval = _struct->on_jsdialog(
      _struct, HoneycombBrowserCppToC::Wrap(browser), origin_url.GetStruct(),
      dialog_type, message_text.GetStruct(), default_prompt_text.GetStruct(),
      HoneycombJSDialogCallbackCppToC::Wrap(callback), &suppress_messageInt);

  // Restore param:suppress_message; type: bool_byref
  suppress_message = suppress_messageInt ? true : false;

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
bool HoneycombJSDialogHandlerCToCpp::OnBeforeUnloadDialog(
    HoneycombRefPtr<HoneycombBrowser> browser,
    const HoneycombString& message_text,
    bool is_reload,
    HoneycombRefPtr<HoneycombJSDialogCallback> callback) {
  shutdown_checker::AssertNotShutdown();

  honey_jsdialog_handler_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_before_unload_dialog)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: browser; type: refptr_diff
  DCHECK(browser.get());
  if (!browser.get()) {
    return false;
  }
  // Verify param: callback; type: refptr_diff
  DCHECK(callback.get());
  if (!callback.get()) {
    return false;
  }
  // Unverified params: message_text

  // Execute
  int _retval = _struct->on_before_unload_dialog(
      _struct, HoneycombBrowserCppToC::Wrap(browser), message_text.GetStruct(),
      is_reload, HoneycombJSDialogCallbackCppToC::Wrap(callback));

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
void HoneycombJSDialogHandlerCToCpp::OnResetDialogState(
    HoneycombRefPtr<HoneycombBrowser> browser) {
  shutdown_checker::AssertNotShutdown();

  honey_jsdialog_handler_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_reset_dialog_state)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: browser; type: refptr_diff
  DCHECK(browser.get());
  if (!browser.get()) {
    return;
  }

  // Execute
  _struct->on_reset_dialog_state(_struct, HoneycombBrowserCppToC::Wrap(browser));
}

NO_SANITIZE("cfi-icall")
void HoneycombJSDialogHandlerCToCpp::OnDialogClosed(HoneycombRefPtr<HoneycombBrowser> browser) {
  shutdown_checker::AssertNotShutdown();

  honey_jsdialog_handler_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_dialog_closed)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: browser; type: refptr_diff
  DCHECK(browser.get());
  if (!browser.get()) {
    return;
  }

  // Execute
  _struct->on_dialog_closed(_struct, HoneycombBrowserCppToC::Wrap(browser));
}

// CONSTRUCTOR - Do not edit by hand.

HoneycombJSDialogHandlerCToCpp::HoneycombJSDialogHandlerCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

HoneycombJSDialogHandlerCToCpp::~HoneycombJSDialogHandlerCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
honey_jsdialog_handler_t* HoneycombCToCppRefCounted<
    HoneycombJSDialogHandlerCToCpp,
    HoneycombJSDialogHandler,
    honey_jsdialog_handler_t>::UnwrapDerived(HoneycombWrapperType type,
                                           HoneycombJSDialogHandler* c) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCToCppRefCounted<HoneycombJSDialogHandlerCToCpp,
                                   HoneycombJSDialogHandler,
                                   honey_jsdialog_handler_t>::kWrapperType =
    WT_JSDIALOG_HANDLER;
