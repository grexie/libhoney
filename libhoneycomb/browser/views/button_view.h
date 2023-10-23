// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_BUTTON_VIEW_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_BUTTON_VIEW_H_
#pragma once

#include "include/views/honey_button_delegate.h"

#include "libhoneycomb/browser/thread_util.h"
#include "libhoneycomb/browser/views/view_view.h"

#include "base/logging.h"
#include "ui/views/animation/ink_drop.h"
#include "ui/views/controls/button/button.h"

// Helpers for template boiler-plate.
#define HONEYCOMB_BUTTON_VIEW_T HONEYCOMB_VIEW_VIEW_T
#define HONEYCOMB_BUTTON_VIEW_A HONEYCOMB_VIEW_VIEW_A
#define HONEYCOMB_BUTTON_VIEW_D HoneycombButtonView<HONEYCOMB_BUTTON_VIEW_A>

// Template for implementing views::Button-derived classes. The
// views::Button-derived type passed to this template must extend
// views::ButtonListener (for example, see LabelButtonEx from
// basic_label_button_view.h). See comments in view_impl.h for a usage overview.
HONEYCOMB_BUTTON_VIEW_T class HoneycombButtonView : public HONEYCOMB_VIEW_VIEW_D {
 public:
  using ParentClass = HONEYCOMB_VIEW_VIEW_D;

  // |honey_delegate| may be nullptr.
  template <typename... Args>
  explicit HoneycombButtonView(HoneycombViewDelegateClass* honey_delegate, Args... args)
      : ParentClass(honey_delegate, args...) {}

  // Returns the HoneycombButton associated with this view. See comments on
  // HoneycombViewView::GetHoneycombView.
  HoneycombRefPtr<HoneycombButton> GetHoneycombButton() const {
    HoneycombRefPtr<HoneycombButton> button = ParentClass::GetHoneycombView()->AsButton();
    DCHECK(button);
    return button;
  }

  // views::Button methods:
  void StateChanged(views::Button::ButtonState old_state) override;

  // LabelButtonEx methods:
  void ButtonPressed(const ui::Event& event) override;
};

HONEYCOMB_BUTTON_VIEW_T void HONEYCOMB_BUTTON_VIEW_D::StateChanged(
    views::Button::ButtonState old_state) {
  ParentClass::StateChanged(old_state);
  if (ParentClass::honey_delegate()) {
    ParentClass::honey_delegate()->OnButtonStateChanged(GetHoneycombButton());
  }
}

HONEYCOMB_BUTTON_VIEW_T void HONEYCOMB_BUTTON_VIEW_D::ButtonPressed(
    const ui::Event& event) {
  // Callback may trigger new animation state.
  if (ParentClass::honey_delegate()) {
    ParentClass::honey_delegate()->OnButtonPressed(GetHoneycombButton());
  }
  if (views::InkDrop::Get(this)->ink_drop_mode() !=
          views::InkDropHost::InkDropMode::OFF &&
      !ParentClass::IsFocusable() &&
      ParentClass::GetState() != views::Button::STATE_PRESSED) {
    // Ink drop state does not get reset properly on click when the button is
    // non-focusable. Reset the ink drop state here if the state has not been
    // explicitly set to pressed by the OnButtonPressed callback calling
    // SetState (which also sets the ink drop state).
    views::InkDrop::Get(this)->AnimateToState(
        views::InkDropState::HIDDEN, ui::LocatedEvent::FromIfValid(&event));
  }
}

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_BUTTON_VIEW_H_
