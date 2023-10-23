// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoney/browser/views/menu_button_impl.h"

#include "libhoney/browser/views/menu_button_view.h"
#include "libhoney/browser/views/window_impl.h"

#include "ui/gfx/canvas.h"

// static
HoneycombRefPtr<HoneycombMenuButton> HoneycombMenuButton::CreateMenuButton(
    HoneycombRefPtr<HoneycombMenuButtonDelegate> delegate,
    const HoneycombString& text) {
  return HoneycombMenuButtonImpl::Create(delegate, text);
}

// static
HoneycombRefPtr<HoneycombMenuButtonImpl> HoneycombMenuButtonImpl::Create(
    HoneycombRefPtr<HoneycombMenuButtonDelegate> delegate,
    const HoneycombString& text) {
  HONEYCOMB_REQUIRE_UIT_RETURN(nullptr);
  DCHECK(delegate);
  if (!delegate) {
    return nullptr;
  }
  HoneycombRefPtr<HoneycombMenuButtonImpl> menu_button = new HoneycombMenuButtonImpl(delegate);
  menu_button->Initialize();
  if (!text.empty()) {
    menu_button->SetText(text);
  }
  return menu_button;
}

void HoneycombMenuButtonImpl::ShowMenu(HoneycombRefPtr<HoneycombMenuModel> menu_model,
                                 const HoneycombPoint& screen_point,
                                 honey_menu_anchor_position_t anchor_position) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  HoneycombRefPtr<HoneycombWindow> window =
      view_util::GetWindowFor(root_view()->GetWidget());
  HoneycombWindowImpl* window_impl = static_cast<HoneycombWindowImpl*>(window.get());
  if (window_impl) {
    window_impl->ShowMenu(root_view(), menu_model, screen_point,
                          anchor_position);
  }
}

void HoneycombMenuButtonImpl::TriggerMenu() {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->Activate(nullptr);
}

void HoneycombMenuButtonImpl::SetFocusable(bool focusable) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  static_cast<HoneycombMenuButtonView*>(root_view())
      ->SetDrawStringsFlags(focusable ? gfx::Canvas::SHOW_PREFIX
                                      : gfx::Canvas::HIDE_PREFIX);
  ParentClass::SetFocusable(focusable);
}

HoneycombMenuButtonImpl::HoneycombMenuButtonImpl(HoneycombRefPtr<HoneycombMenuButtonDelegate> delegate)
    : ParentClass(delegate) {
  DCHECK(delegate);
}

views::MenuButton* HoneycombMenuButtonImpl::CreateRootView() {
  return new HoneycombMenuButtonView(delegate());
}

void HoneycombMenuButtonImpl::InitializeRootView() {
  static_cast<HoneycombMenuButtonView*>(root_view())->Initialize();
}
