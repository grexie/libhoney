// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_BUTTON_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_BUTTON_IMPL_H_
#pragma once

#include "include/views/honey_button.h"
#include "include/views/honey_label_button.h"

#include "libhoneycomb/browser/views/view_impl.h"

#include "base/logging.h"
#include "ui/gfx/color_utils.h"
#include "ui/views/animation/ink_drop.h"
#include "ui/views/controls/button/button.h"

// Helpers for template boiler-plate.
#define HONEYCOMB_BUTTON_IMPL_T HONEYCOMB_VIEW_IMPL_T
#define HONEYCOMB_BUTTON_IMPL_A HONEYCOMB_VIEW_IMPL_A
#define HONEYCOMB_BUTTON_IMPL_D HoneycombButtonImpl<HONEYCOMB_BUTTON_IMPL_A>

// Template for implementing HoneycombButton-derived classes. See comments in
// view_impl.h for a usage overview.
HONEYCOMB_BUTTON_IMPL_T class HoneycombButtonImpl : public HONEYCOMB_VIEW_IMPL_D {
 public:
  using ParentClass = HONEYCOMB_VIEW_IMPL_D;

  // HoneycombButton methods. When adding new As*() methods make sure to update
  // HoneycombViewAdapter::GetFor() in view_adapter.cc.
  HoneycombRefPtr<HoneycombLabelButton> AsLabelButton() override { return nullptr; }
  void SetState(honey_button_state_t state) override;
  honey_button_state_t GetState() override;
  void SetInkDropEnabled(bool enabled) override;
  void SetTooltipText(const HoneycombString& tooltip_text) override;
  void SetAccessibleName(const HoneycombString& name) override;

  // HoneycombView methods:
  HoneycombRefPtr<HoneycombButton> AsButton() override { return this; }
  void SetFocusable(bool focusable) override;

 protected:
  // Create a new implementation object.
  // Always call Initialize() after creation.
  // |delegate| may be nullptr.
  explicit HoneycombButtonImpl(HoneycombRefPtr<HoneycombViewDelegateClass> delegate)
      : ParentClass(delegate) {}
};

HONEYCOMB_BUTTON_IMPL_T void HONEYCOMB_BUTTON_IMPL_D::SetState(honey_button_state_t state) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  views::Button::ButtonState old_state = ParentClass::root_view()->GetState();
  views::Button::ButtonState new_state =
      static_cast<views::Button::ButtonState>(state);

  if (views::InkDrop::Get(ParentClass::root_view())->ink_drop_mode() !=
          views::InkDropHost::InkDropMode::OFF &&
      !ParentClass::root_view()->IsFocusable()) {
    // Ink drop state does not get set properly on state change when the button
    // is non-focusable.
    views::InkDropState ink_state = views::InkDropState::HIDDEN;
    if (new_state == views::Button::STATE_PRESSED) {
      ink_state = views::InkDropState::ACTIVATED;
    } else if (old_state == views::Button::STATE_PRESSED) {
      ink_state = views::InkDropState::DEACTIVATED;
    }
    views::InkDrop::Get(ParentClass::root_view())
        ->AnimateToState(ink_state, nullptr);
  }

  ParentClass::root_view()->SetState(new_state);
}

HONEYCOMB_BUTTON_IMPL_T honey_button_state_t HONEYCOMB_BUTTON_IMPL_D::GetState() {
  HONEYCOMB_REQUIRE_VALID_RETURN(HONEYCOMB_BUTTON_STATE_NORMAL);
  return static_cast<honey_button_state_t>(ParentClass::root_view()->GetState());
}

HONEYCOMB_BUTTON_IMPL_T void HONEYCOMB_BUTTON_IMPL_D::SetInkDropEnabled(bool enabled) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  views::InkDrop::Get(ParentClass::root_view())
      ->SetMode(enabled ? views::InkDropHost::InkDropMode::ON
                        : views::InkDropHost::InkDropMode::OFF);
  if (enabled) {
    views::InkDrop::Get(ParentClass::root_view())
        ->SetBaseColor(color_utils::BlendTowardMaxContrast(
            ParentClass::root_view()->background()->get_color(), 0x61));
  }
}

HONEYCOMB_BUTTON_IMPL_T void HONEYCOMB_BUTTON_IMPL_D::SetTooltipText(
    const HoneycombString& tooltip_text) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  ParentClass::root_view()->SetTooltipText(tooltip_text);
}

HONEYCOMB_BUTTON_IMPL_T void HONEYCOMB_BUTTON_IMPL_D::SetAccessibleName(
    const HoneycombString& name) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  ParentClass::root_view()->SetAccessibleName(name);
}

HONEYCOMB_BUTTON_IMPL_T void HONEYCOMB_BUTTON_IMPL_D::SetFocusable(bool focusable) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  ParentClass::root_view()->SetRequestFocusOnPress(focusable);
  ParentClass::SetFocusable(focusable);
}

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_BUTTON_IMPL_H_
