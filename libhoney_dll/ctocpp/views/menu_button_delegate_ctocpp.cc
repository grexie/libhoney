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
// $hash=a27cc5bc8d6400e77100d275ba2609bbf3ef9e0f$
//

#include "libhoney_dll/ctocpp/views/menu_button_delegate_ctocpp.h"
#include "libhoney_dll/cpptoc/views/button_cpptoc.h"
#include "libhoney_dll/cpptoc/views/menu_button_cpptoc.h"
#include "libhoney_dll/cpptoc/views/menu_button_pressed_lock_cpptoc.h"
#include "libhoney_dll/cpptoc/views/view_cpptoc.h"
#include "libhoney_dll/shutdown_checker.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall")
void HoneycombMenuButtonDelegateCToCpp::OnMenuButtonPressed(
    HoneycombRefPtr<HoneycombMenuButton> menu_button,
    const HoneycombPoint& screen_point,
    HoneycombRefPtr<HoneycombMenuButtonPressedLock> button_pressed_lock) {
  shutdown_checker::AssertNotShutdown();

  honey_menu_button_delegate_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_menu_button_pressed)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: menu_button; type: refptr_diff
  DCHECK(menu_button.get());
  if (!menu_button.get()) {
    return;
  }
  // Verify param: button_pressed_lock; type: refptr_diff
  DCHECK(button_pressed_lock.get());
  if (!button_pressed_lock.get()) {
    return;
  }

  // Execute
  _struct->on_menu_button_pressed(
      _struct, HoneycombMenuButtonCppToC::Wrap(menu_button), &screen_point,
      HoneycombMenuButtonPressedLockCppToC::Wrap(button_pressed_lock));
}

NO_SANITIZE("cfi-icall")
void HoneycombMenuButtonDelegateCToCpp::OnButtonPressed(HoneycombRefPtr<HoneycombButton> button) {
  shutdown_checker::AssertNotShutdown();

  honey_button_delegate_t* _struct =
      reinterpret_cast<honey_button_delegate_t*>(GetStruct());
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_button_pressed)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: button; type: refptr_diff
  DCHECK(button.get());
  if (!button.get()) {
    return;
  }

  // Execute
  _struct->on_button_pressed(_struct, HoneycombButtonCppToC::Wrap(button));
}

NO_SANITIZE("cfi-icall")
void HoneycombMenuButtonDelegateCToCpp::OnButtonStateChanged(
    HoneycombRefPtr<HoneycombButton> button) {
  shutdown_checker::AssertNotShutdown();

  honey_button_delegate_t* _struct =
      reinterpret_cast<honey_button_delegate_t*>(GetStruct());
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_button_state_changed)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: button; type: refptr_diff
  DCHECK(button.get());
  if (!button.get()) {
    return;
  }

  // Execute
  _struct->on_button_state_changed(_struct, HoneycombButtonCppToC::Wrap(button));
}

NO_SANITIZE("cfi-icall")
HoneycombSize HoneycombMenuButtonDelegateCToCpp::GetPreferredSize(HoneycombRefPtr<HoneycombView> view) {
  shutdown_checker::AssertNotShutdown();

  honey_view_delegate_t* _struct =
      reinterpret_cast<honey_view_delegate_t*>(GetStruct());
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_preferred_size)) {
    return HoneycombSize();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: view; type: refptr_diff
  DCHECK(view.get());
  if (!view.get()) {
    return HoneycombSize();
  }

  // Execute
  honey_size_t _retval =
      _struct->get_preferred_size(_struct, HoneycombViewCppToC::Wrap(view));

  // Return type: simple
  return _retval;
}

NO_SANITIZE("cfi-icall")
HoneycombSize HoneycombMenuButtonDelegateCToCpp::GetMinimumSize(HoneycombRefPtr<HoneycombView> view) {
  shutdown_checker::AssertNotShutdown();

  honey_view_delegate_t* _struct =
      reinterpret_cast<honey_view_delegate_t*>(GetStruct());
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_minimum_size)) {
    return HoneycombSize();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: view; type: refptr_diff
  DCHECK(view.get());
  if (!view.get()) {
    return HoneycombSize();
  }

  // Execute
  honey_size_t _retval =
      _struct->get_minimum_size(_struct, HoneycombViewCppToC::Wrap(view));

  // Return type: simple
  return _retval;
}

NO_SANITIZE("cfi-icall")
HoneycombSize HoneycombMenuButtonDelegateCToCpp::GetMaximumSize(HoneycombRefPtr<HoneycombView> view) {
  shutdown_checker::AssertNotShutdown();

  honey_view_delegate_t* _struct =
      reinterpret_cast<honey_view_delegate_t*>(GetStruct());
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_maximum_size)) {
    return HoneycombSize();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: view; type: refptr_diff
  DCHECK(view.get());
  if (!view.get()) {
    return HoneycombSize();
  }

  // Execute
  honey_size_t _retval =
      _struct->get_maximum_size(_struct, HoneycombViewCppToC::Wrap(view));

  // Return type: simple
  return _retval;
}

NO_SANITIZE("cfi-icall")
int HoneycombMenuButtonDelegateCToCpp::GetHeightForWidth(HoneycombRefPtr<HoneycombView> view,
                                                   int width) {
  shutdown_checker::AssertNotShutdown();

  honey_view_delegate_t* _struct =
      reinterpret_cast<honey_view_delegate_t*>(GetStruct());
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_height_for_width)) {
    return 0;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: view; type: refptr_diff
  DCHECK(view.get());
  if (!view.get()) {
    return 0;
  }

  // Execute
  int _retval =
      _struct->get_height_for_width(_struct, HoneycombViewCppToC::Wrap(view), width);

  // Return type: simple
  return _retval;
}

NO_SANITIZE("cfi-icall")
void HoneycombMenuButtonDelegateCToCpp::OnParentViewChanged(
    HoneycombRefPtr<HoneycombView> view,
    bool added,
    HoneycombRefPtr<HoneycombView> parent) {
  shutdown_checker::AssertNotShutdown();

  honey_view_delegate_t* _struct =
      reinterpret_cast<honey_view_delegate_t*>(GetStruct());
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_parent_view_changed)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: view; type: refptr_diff
  DCHECK(view.get());
  if (!view.get()) {
    return;
  }
  // Verify param: parent; type: refptr_diff
  DCHECK(parent.get());
  if (!parent.get()) {
    return;
  }

  // Execute
  _struct->on_parent_view_changed(_struct, HoneycombViewCppToC::Wrap(view), added,
                                  HoneycombViewCppToC::Wrap(parent));
}

NO_SANITIZE("cfi-icall")
void HoneycombMenuButtonDelegateCToCpp::OnChildViewChanged(HoneycombRefPtr<HoneycombView> view,
                                                     bool added,
                                                     HoneycombRefPtr<HoneycombView> child) {
  shutdown_checker::AssertNotShutdown();

  honey_view_delegate_t* _struct =
      reinterpret_cast<honey_view_delegate_t*>(GetStruct());
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_child_view_changed)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: view; type: refptr_diff
  DCHECK(view.get());
  if (!view.get()) {
    return;
  }
  // Verify param: child; type: refptr_diff
  DCHECK(child.get());
  if (!child.get()) {
    return;
  }

  // Execute
  _struct->on_child_view_changed(_struct, HoneycombViewCppToC::Wrap(view), added,
                                 HoneycombViewCppToC::Wrap(child));
}

NO_SANITIZE("cfi-icall")
void HoneycombMenuButtonDelegateCToCpp::OnWindowChanged(HoneycombRefPtr<HoneycombView> view,
                                                  bool added) {
  shutdown_checker::AssertNotShutdown();

  honey_view_delegate_t* _struct =
      reinterpret_cast<honey_view_delegate_t*>(GetStruct());
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_window_changed)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: view; type: refptr_diff
  DCHECK(view.get());
  if (!view.get()) {
    return;
  }

  // Execute
  _struct->on_window_changed(_struct, HoneycombViewCppToC::Wrap(view), added);
}

NO_SANITIZE("cfi-icall")
void HoneycombMenuButtonDelegateCToCpp::OnLayoutChanged(HoneycombRefPtr<HoneycombView> view,
                                                  const HoneycombRect& new_bounds) {
  shutdown_checker::AssertNotShutdown();

  honey_view_delegate_t* _struct =
      reinterpret_cast<honey_view_delegate_t*>(GetStruct());
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_layout_changed)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: view; type: refptr_diff
  DCHECK(view.get());
  if (!view.get()) {
    return;
  }

  // Execute
  _struct->on_layout_changed(_struct, HoneycombViewCppToC::Wrap(view), &new_bounds);
}

NO_SANITIZE("cfi-icall")
void HoneycombMenuButtonDelegateCToCpp::OnFocus(HoneycombRefPtr<HoneycombView> view) {
  shutdown_checker::AssertNotShutdown();

  honey_view_delegate_t* _struct =
      reinterpret_cast<honey_view_delegate_t*>(GetStruct());
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_focus)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: view; type: refptr_diff
  DCHECK(view.get());
  if (!view.get()) {
    return;
  }

  // Execute
  _struct->on_focus(_struct, HoneycombViewCppToC::Wrap(view));
}

NO_SANITIZE("cfi-icall")
void HoneycombMenuButtonDelegateCToCpp::OnBlur(HoneycombRefPtr<HoneycombView> view) {
  shutdown_checker::AssertNotShutdown();

  honey_view_delegate_t* _struct =
      reinterpret_cast<honey_view_delegate_t*>(GetStruct());
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_blur)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: view; type: refptr_diff
  DCHECK(view.get());
  if (!view.get()) {
    return;
  }

  // Execute
  _struct->on_blur(_struct, HoneycombViewCppToC::Wrap(view));
}

// CONSTRUCTOR - Do not edit by hand.

HoneycombMenuButtonDelegateCToCpp::HoneycombMenuButtonDelegateCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

HoneycombMenuButtonDelegateCToCpp::~HoneycombMenuButtonDelegateCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
honey_menu_button_delegate_t* HoneycombCToCppRefCounted<
    HoneycombMenuButtonDelegateCToCpp,
    HoneycombMenuButtonDelegate,
    honey_menu_button_delegate_t>::UnwrapDerived(HoneycombWrapperType type,
                                               HoneycombMenuButtonDelegate* c) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCToCppRefCounted<HoneycombMenuButtonDelegateCToCpp,
                                   HoneycombMenuButtonDelegate,
                                   honey_menu_button_delegate_t>::kWrapperType =
    WT_MENU_BUTTON_DELEGATE;
