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
// $hash=0fc307ce2060ccf455514543b4dfd694300cf79a$
//

#include "libhoney_dll/ctocpp/keyboard_handler_ctocpp.h"
#include "libhoney_dll/cpptoc/browser_cpptoc.h"
#include "libhoney_dll/shutdown_checker.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall")
bool HoneycombKeyboardHandlerCToCpp::OnPreKeyEvent(HoneycombRefPtr<HoneycombBrowser> browser,
                                             const HoneycombKeyEvent& event,
                                             HoneycombEventHandle os_event,
                                             bool* is_keyboard_shortcut) {
  shutdown_checker::AssertNotShutdown();

  honey_keyboard_handler_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_pre_key_event)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: browser; type: refptr_diff
  DCHECK(browser.get());
  if (!browser.get()) {
    return false;
  }
  // Verify param: is_keyboard_shortcut; type: bool_byaddr
  DCHECK(is_keyboard_shortcut);
  if (!is_keyboard_shortcut) {
    return false;
  }

  // Translate param: is_keyboard_shortcut; type: bool_byaddr
  int is_keyboard_shortcutInt =
      is_keyboard_shortcut ? *is_keyboard_shortcut : 0;

  // Execute
  int _retval =
      _struct->on_pre_key_event(_struct, HoneycombBrowserCppToC::Wrap(browser),
                                &event, os_event, &is_keyboard_shortcutInt);

  // Restore param:is_keyboard_shortcut; type: bool_byaddr
  if (is_keyboard_shortcut) {
    *is_keyboard_shortcut = is_keyboard_shortcutInt ? true : false;
  }

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
bool HoneycombKeyboardHandlerCToCpp::OnKeyEvent(HoneycombRefPtr<HoneycombBrowser> browser,
                                          const HoneycombKeyEvent& event,
                                          HoneycombEventHandle os_event) {
  shutdown_checker::AssertNotShutdown();

  honey_keyboard_handler_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_key_event)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: browser; type: refptr_diff
  DCHECK(browser.get());
  if (!browser.get()) {
    return false;
  }

  // Execute
  int _retval = _struct->on_key_event(_struct, HoneycombBrowserCppToC::Wrap(browser),
                                      &event, os_event);

  // Return type: bool
  return _retval ? true : false;
}

// CONSTRUCTOR - Do not edit by hand.

HoneycombKeyboardHandlerCToCpp::HoneycombKeyboardHandlerCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

HoneycombKeyboardHandlerCToCpp::~HoneycombKeyboardHandlerCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
honey_keyboard_handler_t* HoneycombCToCppRefCounted<
    HoneycombKeyboardHandlerCToCpp,
    HoneycombKeyboardHandler,
    honey_keyboard_handler_t>::UnwrapDerived(HoneycombWrapperType type,
                                           HoneycombKeyboardHandler* c) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCToCppRefCounted<HoneycombKeyboardHandlerCToCpp,
                                   HoneycombKeyboardHandler,
                                   honey_keyboard_handler_t>::kWrapperType =
    WT_KEYBOARD_HANDLER;
