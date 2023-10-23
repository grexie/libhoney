// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_BASIC_LABEL_BUTTON_VIEW_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_BASIC_LABEL_BUTTON_VIEW_H_
#pragma once

#include "include/views/honey_button_delegate.h"

#include "libhoneycomb/browser/views/label_button_view.h"

#include "ui/views/controls/button/label_button.h"

// Extend views::LabelButton with a no-argument constructor as required by the
// HoneycombViewView template and extend views::ButtonListener as required by the
// HoneycombButtonView template.
class LabelButtonEx : public views::LabelButton {
 public:
  LabelButtonEx()
      : views::LabelButton(base::BindRepeating(
                               [](LabelButtonEx* self, const ui::Event& event) {
                                 self->ButtonPressed(event);
                               },
                               base::Unretained(this)),
                           std::u16string()) {}

  virtual void ButtonPressed(const ui::Event& event) = 0;
};

class HoneycombBasicLabelButtonView
    : public HoneycombLabelButtonView<LabelButtonEx, HoneycombButtonDelegate> {
 public:
  using ParentClass = HoneycombLabelButtonView<LabelButtonEx, HoneycombButtonDelegate>;

  // |honey_delegate| may be nullptr.
  explicit HoneycombBasicLabelButtonView(HoneycombButtonDelegate* honey_delegate);

  HoneycombBasicLabelButtonView(const HoneycombBasicLabelButtonView&) = delete;
  HoneycombBasicLabelButtonView& operator=(const HoneycombBasicLabelButtonView&) = delete;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_BASIC_LABEL_BUTTON_VIEW_H_
