// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_VIEWS_MENU_BUTTON_IMPL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_VIEWS_MENU_BUTTON_IMPL_H_
#pragma once

#include "include/views/honey_menu_button.h"
#include "include/views/honey_menu_button_delegate.h"

#include "libhoney/browser/menu_model_impl.h"
#include "libhoney/browser/views/label_button_impl.h"

#include "ui/views/controls/button/menu_button.h"

class HoneycombMenuButtonImpl : public HoneycombLabelButtonImpl<views::MenuButton,
                                                    HoneycombMenuButton,
                                                    HoneycombMenuButtonDelegate> {
 public:
  using ParentClass = HoneycombLabelButtonImpl<views::MenuButton,
                                         HoneycombMenuButton,
                                         HoneycombMenuButtonDelegate>;

  HoneycombMenuButtonImpl(const HoneycombMenuButtonImpl&) = delete;
  HoneycombMenuButtonImpl& operator=(const HoneycombMenuButtonImpl&) = delete;

  // Create a new HoneycombMenuButton instance. |delegate| must not be nullptr.
  static HoneycombRefPtr<HoneycombMenuButtonImpl> Create(
      HoneycombRefPtr<HoneycombMenuButtonDelegate> delegate,
      const HoneycombString& text);

  // HoneycombMenuButton methods:
  void ShowMenu(HoneycombRefPtr<HoneycombMenuModel> menu_model,
                const HoneycombPoint& screen_point,
                honey_menu_anchor_position_t anchor_position) override;
  void TriggerMenu() override;

  // HoneycombLabelButton methods:
  HoneycombRefPtr<HoneycombMenuButton> AsMenuButton() override { return this; }

  // HoneycombViewAdapter methods:
  std::string GetDebugType() override { return "MenuButton"; }

  // HoneycombView methods:
  void SetFocusable(bool focusable) override;

 private:
  // Create a new implementation object.
  // Always call Initialize() after creation.
  // |delegate| must not be nullptr.
  explicit HoneycombMenuButtonImpl(HoneycombRefPtr<HoneycombMenuButtonDelegate> delegate);

  // HoneycombViewImpl methods:
  views::MenuButton* CreateRootView() override;
  void InitializeRootView() override;

  IMPLEMENT_REFCOUNTING_DELETE_ON_UIT(HoneycombMenuButtonImpl);
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_VIEWS_MENU_BUTTON_IMPL_H_
