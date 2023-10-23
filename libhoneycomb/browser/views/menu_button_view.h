// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_MENU_BUTTON_VIEW_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_MENU_BUTTON_VIEW_H_
#pragma once

#include "include/views/honey_menu_button.h"
#include "include/views/honey_menu_button_delegate.h"

#include "libhoneycomb/browser/views/label_button_view.h"

#include "ui/views/controls/button/menu_button.h"

// Extend views::LabelButton with a no-argument constructor as required by the
// HoneycombViewView template and extend views::ButtonListener as required by the
// HoneycombButtonView template.
class MenuButtonEx : public views::MenuButton {
 public:
  MenuButtonEx()
      : views::MenuButton(base::BindRepeating(
            [](MenuButtonEx* self, const ui::Event& event) {
              self->ButtonPressed(event);
            },
            base::Unretained(this))) {}

  virtual void ButtonPressed(const ui::Event& event) = 0;
};

class HoneycombMenuButtonView
    : public HoneycombLabelButtonView<MenuButtonEx, HoneycombMenuButtonDelegate> {
 public:
  using ParentClass = HoneycombLabelButtonView<MenuButtonEx, HoneycombMenuButtonDelegate>;

  // |honey_delegate| must not be nullptr.
  explicit HoneycombMenuButtonView(HoneycombMenuButtonDelegate* honey_delegate);

  HoneycombMenuButtonView(const HoneycombMenuButtonView&) = delete;
  HoneycombMenuButtonView& operator=(const HoneycombMenuButtonView&) = delete;

  void Initialize() override;

  // Returns the HoneycombMenuButton associated with this view. See comments on
  // HoneycombViewView::GetHoneycombView.
  HoneycombRefPtr<HoneycombMenuButton> GetHoneycombMenuButton() const;

  // Set the flags that control display of accelerator characters.
  void SetDrawStringsFlags(int flags);

  // MenuButtonEx methods:
  void ButtonPressed(const ui::Event& event) override;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_MENU_BUTTON_VIEW_H_
