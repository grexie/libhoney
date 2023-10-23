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
// $hash=6c1d9cfcafa7425bf5530eba7be024d9d07c466b$
//

#include "libhoney_dll/cpptoc/views/button_delegate_cpptoc.h"
#include "libhoney_dll/cpptoc/views/menu_button_delegate_cpptoc.h"
#include "libhoney_dll/ctocpp/views/button_ctocpp.h"
#include "libhoney_dll/ctocpp/views/view_ctocpp.h"
#include "libhoney_dll/shutdown_checker.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

void HONEYCOMB_CALLBACK
button_delegate_on_button_pressed(struct _honey_button_delegate_t* self,
                                  honey_button_t* button) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: button; type: refptr_diff
  DCHECK(button);
  if (!button) {
    return;
  }

  // Execute
  HoneycombButtonDelegateCppToC::Get(self)->OnButtonPressed(
      HoneycombButtonCToCpp::Wrap(button));
}

void HONEYCOMB_CALLBACK
button_delegate_on_button_state_changed(struct _honey_button_delegate_t* self,
                                        honey_button_t* button) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: button; type: refptr_diff
  DCHECK(button);
  if (!button) {
    return;
  }

  // Execute
  HoneycombButtonDelegateCppToC::Get(self)->OnButtonStateChanged(
      HoneycombButtonCToCpp::Wrap(button));
}

honey_size_t HONEYCOMB_CALLBACK
button_delegate_get_preferred_size(struct _honey_view_delegate_t* self,
                                   honey_view_t* view) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return HoneycombSize();
  }
  // Verify param: view; type: refptr_diff
  DCHECK(view);
  if (!view) {
    return HoneycombSize();
  }

  // Execute
  honey_size_t _retval = HoneycombButtonDelegateCppToC::Get(
                           reinterpret_cast<honey_button_delegate_t*>(self))
                           ->GetPreferredSize(HoneycombViewCToCpp::Wrap(view));

  // Return type: simple
  return _retval;
}

honey_size_t HONEYCOMB_CALLBACK
button_delegate_get_minimum_size(struct _honey_view_delegate_t* self,
                                 honey_view_t* view) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return HoneycombSize();
  }
  // Verify param: view; type: refptr_diff
  DCHECK(view);
  if (!view) {
    return HoneycombSize();
  }

  // Execute
  honey_size_t _retval = HoneycombButtonDelegateCppToC::Get(
                           reinterpret_cast<honey_button_delegate_t*>(self))
                           ->GetMinimumSize(HoneycombViewCToCpp::Wrap(view));

  // Return type: simple
  return _retval;
}

honey_size_t HONEYCOMB_CALLBACK
button_delegate_get_maximum_size(struct _honey_view_delegate_t* self,
                                 honey_view_t* view) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return HoneycombSize();
  }
  // Verify param: view; type: refptr_diff
  DCHECK(view);
  if (!view) {
    return HoneycombSize();
  }

  // Execute
  honey_size_t _retval = HoneycombButtonDelegateCppToC::Get(
                           reinterpret_cast<honey_button_delegate_t*>(self))
                           ->GetMaximumSize(HoneycombViewCToCpp::Wrap(view));

  // Return type: simple
  return _retval;
}

int HONEYCOMB_CALLBACK
button_delegate_get_height_for_width(struct _honey_view_delegate_t* self,
                                     honey_view_t* view,
                                     int width) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }
  // Verify param: view; type: refptr_diff
  DCHECK(view);
  if (!view) {
    return 0;
  }

  // Execute
  int _retval = HoneycombButtonDelegateCppToC::Get(
                    reinterpret_cast<honey_button_delegate_t*>(self))
                    ->GetHeightForWidth(HoneycombViewCToCpp::Wrap(view), width);

  // Return type: simple
  return _retval;
}

void HONEYCOMB_CALLBACK
button_delegate_on_parent_view_changed(struct _honey_view_delegate_t* self,
                                       honey_view_t* view,
                                       int added,
                                       honey_view_t* parent) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: view; type: refptr_diff
  DCHECK(view);
  if (!view) {
    return;
  }
  // Verify param: parent; type: refptr_diff
  DCHECK(parent);
  if (!parent) {
    return;
  }

  // Execute
  HoneycombButtonDelegateCppToC::Get(reinterpret_cast<honey_button_delegate_t*>(self))
      ->OnParentViewChanged(HoneycombViewCToCpp::Wrap(view), added ? true : false,
                            HoneycombViewCToCpp::Wrap(parent));
}

void HONEYCOMB_CALLBACK
button_delegate_on_child_view_changed(struct _honey_view_delegate_t* self,
                                      honey_view_t* view,
                                      int added,
                                      honey_view_t* child) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: view; type: refptr_diff
  DCHECK(view);
  if (!view) {
    return;
  }
  // Verify param: child; type: refptr_diff
  DCHECK(child);
  if (!child) {
    return;
  }

  // Execute
  HoneycombButtonDelegateCppToC::Get(reinterpret_cast<honey_button_delegate_t*>(self))
      ->OnChildViewChanged(HoneycombViewCToCpp::Wrap(view), added ? true : false,
                           HoneycombViewCToCpp::Wrap(child));
}

void HONEYCOMB_CALLBACK
button_delegate_on_window_changed(struct _honey_view_delegate_t* self,
                                  honey_view_t* view,
                                  int added) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: view; type: refptr_diff
  DCHECK(view);
  if (!view) {
    return;
  }

  // Execute
  HoneycombButtonDelegateCppToC::Get(reinterpret_cast<honey_button_delegate_t*>(self))
      ->OnWindowChanged(HoneycombViewCToCpp::Wrap(view), added ? true : false);
}

void HONEYCOMB_CALLBACK
button_delegate_on_layout_changed(struct _honey_view_delegate_t* self,
                                  honey_view_t* view,
                                  const honey_rect_t* new_bounds) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: view; type: refptr_diff
  DCHECK(view);
  if (!view) {
    return;
  }
  // Verify param: new_bounds; type: simple_byref_const
  DCHECK(new_bounds);
  if (!new_bounds) {
    return;
  }

  // Translate param: new_bounds; type: simple_byref_const
  HoneycombRect new_boundsVal = new_bounds ? *new_bounds : HoneycombRect();

  // Execute
  HoneycombButtonDelegateCppToC::Get(reinterpret_cast<honey_button_delegate_t*>(self))
      ->OnLayoutChanged(HoneycombViewCToCpp::Wrap(view), new_boundsVal);
}

void HONEYCOMB_CALLBACK button_delegate_on_focus(struct _honey_view_delegate_t* self,
                                           honey_view_t* view) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: view; type: refptr_diff
  DCHECK(view);
  if (!view) {
    return;
  }

  // Execute
  HoneycombButtonDelegateCppToC::Get(reinterpret_cast<honey_button_delegate_t*>(self))
      ->OnFocus(HoneycombViewCToCpp::Wrap(view));
}

void HONEYCOMB_CALLBACK button_delegate_on_blur(struct _honey_view_delegate_t* self,
                                          honey_view_t* view) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: view; type: refptr_diff
  DCHECK(view);
  if (!view) {
    return;
  }

  // Execute
  HoneycombButtonDelegateCppToC::Get(reinterpret_cast<honey_button_delegate_t*>(self))
      ->OnBlur(HoneycombViewCToCpp::Wrap(view));
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

HoneycombButtonDelegateCppToC::HoneycombButtonDelegateCppToC() {
  GetStruct()->on_button_pressed = button_delegate_on_button_pressed;
  GetStruct()->on_button_state_changed =
      button_delegate_on_button_state_changed;
  GetStruct()->base.get_preferred_size = button_delegate_get_preferred_size;
  GetStruct()->base.get_minimum_size = button_delegate_get_minimum_size;
  GetStruct()->base.get_maximum_size = button_delegate_get_maximum_size;
  GetStruct()->base.get_height_for_width = button_delegate_get_height_for_width;
  GetStruct()->base.on_parent_view_changed =
      button_delegate_on_parent_view_changed;
  GetStruct()->base.on_child_view_changed =
      button_delegate_on_child_view_changed;
  GetStruct()->base.on_window_changed = button_delegate_on_window_changed;
  GetStruct()->base.on_layout_changed = button_delegate_on_layout_changed;
  GetStruct()->base.on_focus = button_delegate_on_focus;
  GetStruct()->base.on_blur = button_delegate_on_blur;
}

// DESTRUCTOR - Do not edit by hand.

HoneycombButtonDelegateCppToC::~HoneycombButtonDelegateCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
HoneycombRefPtr<HoneycombButtonDelegate> HoneycombCppToCRefCounted<
    HoneycombButtonDelegateCppToC,
    HoneycombButtonDelegate,
    honey_button_delegate_t>::UnwrapDerived(HoneycombWrapperType type,
                                          honey_button_delegate_t* s) {
  if (type == WT_MENU_BUTTON_DELEGATE) {
    return HoneycombMenuButtonDelegateCppToC::Unwrap(
        reinterpret_cast<honey_menu_button_delegate_t*>(s));
  }
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCppToCRefCounted<HoneycombButtonDelegateCppToC,
                                   HoneycombButtonDelegate,
                                   honey_button_delegate_t>::kWrapperType =
    WT_BUTTON_DELEGATE;