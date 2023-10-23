// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_VIEWS_LABEL_BUTTON_VIEW_H_
#define HONEYCOMB_LIBHONEY_BROWSER_VIEWS_LABEL_BUTTON_VIEW_H_
#pragma once

#include "include/views/honey_label_button.h"
#include "libhoney/browser/views/button_view.h"

#include "base/logging.h"
#include "ui/gfx/font_list.h"

// Helpers for template boiler-plate.
#define HONEYCOMB_LABEL_BUTTON_VIEW_T HONEYCOMB_BUTTON_VIEW_T
#define HONEYCOMB_LABEL_BUTTON_VIEW_A HONEYCOMB_BUTTON_VIEW_A
#define HONEYCOMB_LABEL_BUTTON_VIEW_D HoneycombLabelButtonView<HONEYCOMB_LABEL_BUTTON_VIEW_A>

// Template for implementing views::View-derived classes that support adding and
// removing children (called a Panel in Honeycomb terminology). See comments in
// view_impl.h for a usage overview.
HONEYCOMB_LABEL_BUTTON_VIEW_T class HoneycombLabelButtonView : public HONEYCOMB_BUTTON_VIEW_D {
 public:
  using ParentClass = HONEYCOMB_BUTTON_VIEW_D;

  // |honey_delegate| may be nullptr.
  template <typename... Args>
  explicit HoneycombLabelButtonView(HoneycombViewDelegateClass* honey_delegate, Args... args)
      : ParentClass(honey_delegate, args...) {}

  void Initialize() override {
    ParentClass::Initialize();

    // Use our defaults instead of the Views framework defaults.
    ParentClass::SetFontList(gfx::FontList(view_util::kDefaultFontList));
  }

  // Returns the HoneycombLabelButton associated with this view. See comments on
  // HoneycombViewView::GetHoneycombView.
  HoneycombRefPtr<HoneycombLabelButton> GetHoneycombLabelButton() const {
    HoneycombRefPtr<HoneycombLabelButton> label_button =
        ParentClass::GetHoneycombButton()->AsLabelButton();
    DCHECK(label_button);
    return label_button;
  }

  // HoneycombViewView methods:
  bool HasMinimumSize() const override { return true; }
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_VIEWS_LABEL_BUTTON_VIEW_H_
