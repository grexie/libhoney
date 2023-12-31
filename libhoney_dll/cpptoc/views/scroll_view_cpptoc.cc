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
// $hash=171b3ef450c160ece605980988dd80af9dd97958$
//

#include "libhoney_dll/cpptoc/views/scroll_view_cpptoc.h"
#include "libhoney_dll/cpptoc/views/browser_view_cpptoc.h"
#include "libhoney_dll/cpptoc/views/button_cpptoc.h"
#include "libhoney_dll/cpptoc/views/panel_cpptoc.h"
#include "libhoney_dll/cpptoc/views/textfield_cpptoc.h"
#include "libhoney_dll/cpptoc/views/view_cpptoc.h"
#include "libhoney_dll/cpptoc/views/window_cpptoc.h"
#include "libhoney_dll/ctocpp/views/view_delegate_ctocpp.h"
#include "libhoney_dll/shutdown_checker.h"

// GLOBAL FUNCTIONS - Body may be edited by hand.

HONEYCOMB_EXPORT honey_scroll_view_t* honey_scroll_view_create(
    struct _honey_view_delegate_t* delegate) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Unverified params: delegate

  // Execute
  HoneycombRefPtr<HoneycombScrollView> _retval =
      HoneycombScrollView::CreateScrollView(HoneycombViewDelegateCToCpp::Wrap(delegate));

  // Return type: refptr_same
  return HoneycombScrollViewCppToC::Wrap(_retval);
}

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

void HONEYCOMB_CALLBACK scroll_view_set_content_view(struct _honey_scroll_view_t* self,
                                               struct _honey_view_t* view) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: view; type: refptr_same
  DCHECK(view);
  if (!view) {
    return;
  }

  // Execute
  HoneycombScrollViewCppToC::Get(self)->SetContentView(HoneycombViewCppToC::Unwrap(view));
}

struct _honey_view_t* HONEYCOMB_CALLBACK
scroll_view_get_content_view(struct _honey_scroll_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombView> _retval = HoneycombScrollViewCppToC::Get(self)->GetContentView();

  // Return type: refptr_same
  return HoneycombViewCppToC::Wrap(_retval);
}

honey_rect_t HONEYCOMB_CALLBACK
scroll_view_get_visible_content_rect(struct _honey_scroll_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return HoneycombRect();
  }

  // Execute
  honey_rect_t _retval = HoneycombScrollViewCppToC::Get(self)->GetVisibleContentRect();

  // Return type: simple
  return _retval;
}

int HONEYCOMB_CALLBACK
scroll_view_has_horizontal_scrollbar(struct _honey_scroll_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombScrollViewCppToC::Get(self)->HasHorizontalScrollbar();

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK
scroll_view_get_horizontal_scrollbar_height(struct _honey_scroll_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  int _retval = HoneycombScrollViewCppToC::Get(self)->GetHorizontalScrollbarHeight();

  // Return type: simple
  return _retval;
}

int HONEYCOMB_CALLBACK
scroll_view_has_vertical_scrollbar(struct _honey_scroll_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombScrollViewCppToC::Get(self)->HasVerticalScrollbar();

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK
scroll_view_get_vertical_scrollbar_width(struct _honey_scroll_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  int _retval = HoneycombScrollViewCppToC::Get(self)->GetVerticalScrollbarWidth();

  // Return type: simple
  return _retval;
}

honey_browser_view_t* HONEYCOMB_CALLBACK
scroll_view_as_browser_view(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombBrowserView> _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->AsBrowserView();

  // Return type: refptr_same
  return HoneycombBrowserViewCppToC::Wrap(_retval);
}

honey_button_t* HONEYCOMB_CALLBACK scroll_view_as_button(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombButton> _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->AsButton();

  // Return type: refptr_same
  return HoneycombButtonCppToC::Wrap(_retval);
}

honey_panel_t* HONEYCOMB_CALLBACK scroll_view_as_panel(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombPanel> _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->AsPanel();

  // Return type: refptr_same
  return HoneycombPanelCppToC::Wrap(_retval);
}

honey_scroll_view_t* HONEYCOMB_CALLBACK
scroll_view_as_scroll_view(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombScrollView> _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->AsScrollView();

  // Return type: refptr_same
  return HoneycombScrollViewCppToC::Wrap(_retval);
}

honey_textfield_t* HONEYCOMB_CALLBACK
scroll_view_as_textfield(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombTextfield> _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->AsTextfield();

  // Return type: refptr_same
  return HoneycombTextfieldCppToC::Wrap(_retval);
}

honey_string_userfree_t HONEYCOMB_CALLBACK
scroll_view_get_type_string(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombString _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->GetTypeString();

  // Return type: string
  return _retval.DetachToUserFree();
}

honey_string_userfree_t HONEYCOMB_CALLBACK
scroll_view_to_string(struct _honey_view_t* self, int include_children) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombString _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->ToString(include_children ? true : false);

  // Return type: string
  return _retval.DetachToUserFree();
}

int HONEYCOMB_CALLBACK scroll_view_is_valid(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  bool _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->IsValid();

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK scroll_view_is_attached(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  bool _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->IsAttached();

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK scroll_view_is_same(struct _honey_view_t* self,
                                     struct _honey_view_t* that) {
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
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->IsSame(HoneycombViewCppToC::Unwrap(that));

  // Return type: bool
  return _retval;
}

struct _honey_view_delegate_t* HONEYCOMB_CALLBACK
scroll_view_get_delegate(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombViewDelegate> _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->GetDelegate();

  // Return type: refptr_diff
  return HoneycombViewDelegateCToCpp::Unwrap(_retval);
}

struct _honey_window_t* HONEYCOMB_CALLBACK
scroll_view_get_window(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombWindow> _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->GetWindow();

  // Return type: refptr_same
  return HoneycombWindowCppToC::Wrap(_retval);
}

int HONEYCOMB_CALLBACK scroll_view_get_id(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  int _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->GetID();

  // Return type: simple
  return _retval;
}

void HONEYCOMB_CALLBACK scroll_view_set_id(struct _honey_view_t* self, int id) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }

  // Execute
  HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
      ->SetID(id);
}

int HONEYCOMB_CALLBACK scroll_view_get_group_id(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  int _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->GetGroupID();

  // Return type: simple
  return _retval;
}

void HONEYCOMB_CALLBACK scroll_view_set_group_id(struct _honey_view_t* self,
                                           int group_id) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }

  // Execute
  HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
      ->SetGroupID(group_id);
}

struct _honey_view_t* HONEYCOMB_CALLBACK
scroll_view_get_parent_view(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombView> _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->GetParentView();

  // Return type: refptr_same
  return HoneycombViewCppToC::Wrap(_retval);
}

struct _honey_view_t* HONEYCOMB_CALLBACK
scroll_view_get_view_for_id(struct _honey_view_t* self, int id) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombView> _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->GetViewForID(id);

  // Return type: refptr_same
  return HoneycombViewCppToC::Wrap(_retval);
}

void HONEYCOMB_CALLBACK scroll_view_set_bounds(struct _honey_view_t* self,
                                         const honey_rect_t* bounds) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: bounds; type: simple_byref_const
  DCHECK(bounds);
  if (!bounds) {
    return;
  }

  // Translate param: bounds; type: simple_byref_const
  HoneycombRect boundsVal = bounds ? *bounds : HoneycombRect();

  // Execute
  HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
      ->SetBounds(boundsVal);
}

honey_rect_t HONEYCOMB_CALLBACK scroll_view_get_bounds(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return HoneycombRect();
  }

  // Execute
  honey_rect_t _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->GetBounds();

  // Return type: simple
  return _retval;
}

honey_rect_t HONEYCOMB_CALLBACK
scroll_view_get_bounds_in_screen(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return HoneycombRect();
  }

  // Execute
  honey_rect_t _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->GetBoundsInScreen();

  // Return type: simple
  return _retval;
}

void HONEYCOMB_CALLBACK scroll_view_set_size(struct _honey_view_t* self,
                                       const honey_size_t* size) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: size; type: simple_byref_const
  DCHECK(size);
  if (!size) {
    return;
  }

  // Translate param: size; type: simple_byref_const
  HoneycombSize sizeVal = size ? *size : HoneycombSize();

  // Execute
  HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
      ->SetSize(sizeVal);
}

honey_size_t HONEYCOMB_CALLBACK scroll_view_get_size(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return HoneycombSize();
  }

  // Execute
  honey_size_t _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->GetSize();

  // Return type: simple
  return _retval;
}

void HONEYCOMB_CALLBACK scroll_view_set_position(struct _honey_view_t* self,
                                           const honey_point_t* position) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: position; type: simple_byref_const
  DCHECK(position);
  if (!position) {
    return;
  }

  // Translate param: position; type: simple_byref_const
  HoneycombPoint positionVal = position ? *position : HoneycombPoint();

  // Execute
  HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
      ->SetPosition(positionVal);
}

honey_point_t HONEYCOMB_CALLBACK scroll_view_get_position(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return HoneycombPoint();
  }

  // Execute
  honey_point_t _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->GetPosition();

  // Return type: simple
  return _retval;
}

void HONEYCOMB_CALLBACK scroll_view_set_insets(struct _honey_view_t* self,
                                         const honey_insets_t* insets) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: insets; type: simple_byref_const
  DCHECK(insets);
  if (!insets) {
    return;
  }

  // Translate param: insets; type: simple_byref_const
  HoneycombInsets insetsVal = insets ? *insets : HoneycombInsets();

  // Execute
  HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
      ->SetInsets(insetsVal);
}

honey_insets_t HONEYCOMB_CALLBACK scroll_view_get_insets(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return HoneycombInsets();
  }

  // Execute
  honey_insets_t _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->GetInsets();

  // Return type: simple
  return _retval;
}

honey_size_t HONEYCOMB_CALLBACK
scroll_view_get_preferred_size(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return HoneycombSize();
  }

  // Execute
  honey_size_t _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->GetPreferredSize();

  // Return type: simple
  return _retval;
}

void HONEYCOMB_CALLBACK scroll_view_size_to_preferred_size(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }

  // Execute
  HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
      ->SizeToPreferredSize();
}

honey_size_t HONEYCOMB_CALLBACK scroll_view_get_minimum_size(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return HoneycombSize();
  }

  // Execute
  honey_size_t _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->GetMinimumSize();

  // Return type: simple
  return _retval;
}

honey_size_t HONEYCOMB_CALLBACK scroll_view_get_maximum_size(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return HoneycombSize();
  }

  // Execute
  honey_size_t _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->GetMaximumSize();

  // Return type: simple
  return _retval;
}

int HONEYCOMB_CALLBACK scroll_view_get_height_for_width(struct _honey_view_t* self,
                                                  int width) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  int _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->GetHeightForWidth(width);

  // Return type: simple
  return _retval;
}

void HONEYCOMB_CALLBACK scroll_view_invalidate_layout(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }

  // Execute
  HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
      ->InvalidateLayout();
}

void HONEYCOMB_CALLBACK scroll_view_set_visible(struct _honey_view_t* self,
                                          int visible) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }

  // Execute
  HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
      ->SetVisible(visible ? true : false);
}

int HONEYCOMB_CALLBACK scroll_view_is_visible(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  bool _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->IsVisible();

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK scroll_view_is_drawn(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  bool _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->IsDrawn();

  // Return type: bool
  return _retval;
}

void HONEYCOMB_CALLBACK scroll_view_set_enabled(struct _honey_view_t* self,
                                          int enabled) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }

  // Execute
  HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
      ->SetEnabled(enabled ? true : false);
}

int HONEYCOMB_CALLBACK scroll_view_is_enabled(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  bool _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->IsEnabled();

  // Return type: bool
  return _retval;
}

void HONEYCOMB_CALLBACK scroll_view_set_focusable(struct _honey_view_t* self,
                                            int focusable) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }

  // Execute
  HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
      ->SetFocusable(focusable ? true : false);
}

int HONEYCOMB_CALLBACK scroll_view_is_focusable(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  bool _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->IsFocusable();

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK
scroll_view_is_accessibility_focusable(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  bool _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->IsAccessibilityFocusable();

  // Return type: bool
  return _retval;
}

void HONEYCOMB_CALLBACK scroll_view_request_focus(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }

  // Execute
  HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
      ->RequestFocus();
}

void HONEYCOMB_CALLBACK scroll_view_set_background_color(struct _honey_view_t* self,
                                                   honey_color_t color) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }

  // Execute
  HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
      ->SetBackgroundColor(color);
}

honey_color_t HONEYCOMB_CALLBACK
scroll_view_get_background_color(struct _honey_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  honey_color_t _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->GetBackgroundColor();

  // Return type: simple
  return _retval;
}

int HONEYCOMB_CALLBACK scroll_view_convert_point_to_screen(struct _honey_view_t* self,
                                                     honey_point_t* point) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }
  // Verify param: point; type: simple_byref
  DCHECK(point);
  if (!point) {
    return 0;
  }

  // Translate param: point; type: simple_byref
  HoneycombPoint pointVal = point ? *point : HoneycombPoint();

  // Execute
  bool _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->ConvertPointToScreen(pointVal);

  // Restore param: point; type: simple_byref
  if (point) {
    *point = pointVal;
  }

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK scroll_view_convert_point_from_screen(struct _honey_view_t* self,
                                                       honey_point_t* point) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }
  // Verify param: point; type: simple_byref
  DCHECK(point);
  if (!point) {
    return 0;
  }

  // Translate param: point; type: simple_byref
  HoneycombPoint pointVal = point ? *point : HoneycombPoint();

  // Execute
  bool _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->ConvertPointFromScreen(pointVal);

  // Restore param: point; type: simple_byref
  if (point) {
    *point = pointVal;
  }

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK scroll_view_convert_point_to_window(struct _honey_view_t* self,
                                                     honey_point_t* point) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }
  // Verify param: point; type: simple_byref
  DCHECK(point);
  if (!point) {
    return 0;
  }

  // Translate param: point; type: simple_byref
  HoneycombPoint pointVal = point ? *point : HoneycombPoint();

  // Execute
  bool _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->ConvertPointToWindow(pointVal);

  // Restore param: point; type: simple_byref
  if (point) {
    *point = pointVal;
  }

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK scroll_view_convert_point_from_window(struct _honey_view_t* self,
                                                       honey_point_t* point) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }
  // Verify param: point; type: simple_byref
  DCHECK(point);
  if (!point) {
    return 0;
  }

  // Translate param: point; type: simple_byref
  HoneycombPoint pointVal = point ? *point : HoneycombPoint();

  // Execute
  bool _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->ConvertPointFromWindow(pointVal);

  // Restore param: point; type: simple_byref
  if (point) {
    *point = pointVal;
  }

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK scroll_view_convert_point_to_view(struct _honey_view_t* self,
                                                   struct _honey_view_t* view,
                                                   honey_point_t* point) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }
  // Verify param: view; type: refptr_same
  DCHECK(view);
  if (!view) {
    return 0;
  }
  // Verify param: point; type: simple_byref
  DCHECK(point);
  if (!point) {
    return 0;
  }

  // Translate param: point; type: simple_byref
  HoneycombPoint pointVal = point ? *point : HoneycombPoint();

  // Execute
  bool _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->ConvertPointToView(HoneycombViewCppToC::Unwrap(view), pointVal);

  // Restore param: point; type: simple_byref
  if (point) {
    *point = pointVal;
  }

  // Return type: bool
  return _retval;
}

int HONEYCOMB_CALLBACK scroll_view_convert_point_from_view(struct _honey_view_t* self,
                                                     struct _honey_view_t* view,
                                                     honey_point_t* point) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }
  // Verify param: view; type: refptr_same
  DCHECK(view);
  if (!view) {
    return 0;
  }
  // Verify param: point; type: simple_byref
  DCHECK(point);
  if (!point) {
    return 0;
  }

  // Translate param: point; type: simple_byref
  HoneycombPoint pointVal = point ? *point : HoneycombPoint();

  // Execute
  bool _retval =
      HoneycombScrollViewCppToC::Get(reinterpret_cast<honey_scroll_view_t*>(self))
          ->ConvertPointFromView(HoneycombViewCppToC::Unwrap(view), pointVal);

  // Restore param: point; type: simple_byref
  if (point) {
    *point = pointVal;
  }

  // Return type: bool
  return _retval;
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

HoneycombScrollViewCppToC::HoneycombScrollViewCppToC() {
  GetStruct()->set_content_view = scroll_view_set_content_view;
  GetStruct()->get_content_view = scroll_view_get_content_view;
  GetStruct()->get_visible_content_rect = scroll_view_get_visible_content_rect;
  GetStruct()->has_horizontal_scrollbar = scroll_view_has_horizontal_scrollbar;
  GetStruct()->get_horizontal_scrollbar_height =
      scroll_view_get_horizontal_scrollbar_height;
  GetStruct()->has_vertical_scrollbar = scroll_view_has_vertical_scrollbar;
  GetStruct()->get_vertical_scrollbar_width =
      scroll_view_get_vertical_scrollbar_width;
  GetStruct()->base.as_browser_view = scroll_view_as_browser_view;
  GetStruct()->base.as_button = scroll_view_as_button;
  GetStruct()->base.as_panel = scroll_view_as_panel;
  GetStruct()->base.as_scroll_view = scroll_view_as_scroll_view;
  GetStruct()->base.as_textfield = scroll_view_as_textfield;
  GetStruct()->base.get_type_string = scroll_view_get_type_string;
  GetStruct()->base.to_string = scroll_view_to_string;
  GetStruct()->base.is_valid = scroll_view_is_valid;
  GetStruct()->base.is_attached = scroll_view_is_attached;
  GetStruct()->base.is_same = scroll_view_is_same;
  GetStruct()->base.get_delegate = scroll_view_get_delegate;
  GetStruct()->base.get_window = scroll_view_get_window;
  GetStruct()->base.get_id = scroll_view_get_id;
  GetStruct()->base.set_id = scroll_view_set_id;
  GetStruct()->base.get_group_id = scroll_view_get_group_id;
  GetStruct()->base.set_group_id = scroll_view_set_group_id;
  GetStruct()->base.get_parent_view = scroll_view_get_parent_view;
  GetStruct()->base.get_view_for_id = scroll_view_get_view_for_id;
  GetStruct()->base.set_bounds = scroll_view_set_bounds;
  GetStruct()->base.get_bounds = scroll_view_get_bounds;
  GetStruct()->base.get_bounds_in_screen = scroll_view_get_bounds_in_screen;
  GetStruct()->base.set_size = scroll_view_set_size;
  GetStruct()->base.get_size = scroll_view_get_size;
  GetStruct()->base.set_position = scroll_view_set_position;
  GetStruct()->base.get_position = scroll_view_get_position;
  GetStruct()->base.set_insets = scroll_view_set_insets;
  GetStruct()->base.get_insets = scroll_view_get_insets;
  GetStruct()->base.get_preferred_size = scroll_view_get_preferred_size;
  GetStruct()->base.size_to_preferred_size = scroll_view_size_to_preferred_size;
  GetStruct()->base.get_minimum_size = scroll_view_get_minimum_size;
  GetStruct()->base.get_maximum_size = scroll_view_get_maximum_size;
  GetStruct()->base.get_height_for_width = scroll_view_get_height_for_width;
  GetStruct()->base.invalidate_layout = scroll_view_invalidate_layout;
  GetStruct()->base.set_visible = scroll_view_set_visible;
  GetStruct()->base.is_visible = scroll_view_is_visible;
  GetStruct()->base.is_drawn = scroll_view_is_drawn;
  GetStruct()->base.set_enabled = scroll_view_set_enabled;
  GetStruct()->base.is_enabled = scroll_view_is_enabled;
  GetStruct()->base.set_focusable = scroll_view_set_focusable;
  GetStruct()->base.is_focusable = scroll_view_is_focusable;
  GetStruct()->base.is_accessibility_focusable =
      scroll_view_is_accessibility_focusable;
  GetStruct()->base.request_focus = scroll_view_request_focus;
  GetStruct()->base.set_background_color = scroll_view_set_background_color;
  GetStruct()->base.get_background_color = scroll_view_get_background_color;
  GetStruct()->base.convert_point_to_screen =
      scroll_view_convert_point_to_screen;
  GetStruct()->base.convert_point_from_screen =
      scroll_view_convert_point_from_screen;
  GetStruct()->base.convert_point_to_window =
      scroll_view_convert_point_to_window;
  GetStruct()->base.convert_point_from_window =
      scroll_view_convert_point_from_window;
  GetStruct()->base.convert_point_to_view = scroll_view_convert_point_to_view;
  GetStruct()->base.convert_point_from_view =
      scroll_view_convert_point_from_view;
}

// DESTRUCTOR - Do not edit by hand.

HoneycombScrollViewCppToC::~HoneycombScrollViewCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
HoneycombRefPtr<HoneycombScrollView>
HoneycombCppToCRefCounted<HoneycombScrollViewCppToC, HoneycombScrollView, honey_scroll_view_t>::
    UnwrapDerived(HoneycombWrapperType type, honey_scroll_view_t* s) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCppToCRefCounted<HoneycombScrollViewCppToC,
                                   HoneycombScrollView,
                                   honey_scroll_view_t>::kWrapperType =
    WT_SCROLL_VIEW;
