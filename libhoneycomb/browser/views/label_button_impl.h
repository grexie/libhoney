// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_LABEL_BUTTON_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_LABEL_BUTTON_IMPL_H_
#pragma once

#include "include/views/honey_button.h"
#include "include/views/honey_label_button.h"
#include "include/views/honey_menu_button.h"

#include "libhoneycomb/browser/image_impl.h"
#include "libhoneycomb/browser/views/button_impl.h"

#include "base/logging.h"
#include "ui/views/controls/button/label_button.h"

// Helpers for template boiler-plate.
#define HONEYCOMB_LABEL_BUTTON_IMPL_T HONEYCOMB_BUTTON_IMPL_T
#define HONEYCOMB_LABEL_BUTTON_IMPL_A HONEYCOMB_BUTTON_IMPL_A
#define HONEYCOMB_LABEL_BUTTON_IMPL_D HoneycombLabelButtonImpl<HONEYCOMB_LABEL_BUTTON_IMPL_A>

// Template for implementing HoneycombLabelButton-derived classes. See comments in
// view_impl.h for a usage overview.
HONEYCOMB_LABEL_BUTTON_IMPL_T class HoneycombLabelButtonImpl : public HONEYCOMB_BUTTON_IMPL_D {
 public:
  using ParentClass = HONEYCOMB_BUTTON_IMPL_D;

  // HoneycombLabelButton methods. When adding new As*() methods make sure to update
  // HoneycombViewAdapter::GetFor() in view_adapter.cc.
  void SetText(const HoneycombString& text) override;
  HoneycombString GetText() override;
  void SetImage(honey_button_state_t button_state,
                HoneycombRefPtr<HoneycombImage> image) override;
  HoneycombRefPtr<HoneycombImage> GetImage(honey_button_state_t button_state) override;
  void SetTextColor(honey_button_state_t for_state, honey_color_t color) override;
  void SetEnabledTextColors(honey_color_t color) override;
  void SetFontList(const HoneycombString& font_list) override;
  void SetHorizontalAlignment(honey_horizontal_alignment_t alignment) override;
  void SetMinimumSize(const HoneycombSize& size) override;
  void SetMaximumSize(const HoneycombSize& size) override;

  // HoneycombLabelButton methods:
  HoneycombRefPtr<HoneycombMenuButton> AsMenuButton() override { return nullptr; }

  // HoneycombButton methods:
  HoneycombRefPtr<HoneycombLabelButton> AsLabelButton() override { return this; }

  // HoneycombViewAdapter methods:
  void GetDebugInfo(base::Value::Dict* info, bool include_children) override {
    ParentClass::GetDebugInfo(info, include_children);
    info->Set("text", ParentClass::root_view()->GetText());
  }

 protected:
  // Create a new implementation object.
  // Always call Initialize() after creation.
  // |delegate| may be nullptr.
  explicit HoneycombLabelButtonImpl(HoneycombRefPtr<HoneycombViewDelegateClass> delegate)
      : ParentClass(delegate) {}
};

HONEYCOMB_LABEL_BUTTON_IMPL_T void HONEYCOMB_LABEL_BUTTON_IMPL_D::SetText(
    const HoneycombString& text) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  ParentClass::root_view()->SetText(text);
}

HONEYCOMB_LABEL_BUTTON_IMPL_T HoneycombString HONEYCOMB_LABEL_BUTTON_IMPL_D::GetText() {
  HONEYCOMB_REQUIRE_VALID_RETURN(HoneycombString());
  return ParentClass::root_view()->GetText();
}

HONEYCOMB_LABEL_BUTTON_IMPL_T void HONEYCOMB_LABEL_BUTTON_IMPL_D::SetImage(
    honey_button_state_t button_state,
    HoneycombRefPtr<HoneycombImage> image) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  gfx::ImageSkia image_skia;
  if (image) {
    image_skia = static_cast<HoneycombImageImpl*>(image.get())->AsImageSkia();
  }
  ParentClass::root_view()->SetImage(
      static_cast<views::Button::ButtonState>(button_state), image_skia);
}

HONEYCOMB_LABEL_BUTTON_IMPL_T HoneycombRefPtr<HoneycombImage> HONEYCOMB_LABEL_BUTTON_IMPL_D::GetImage(
    honey_button_state_t button_state) {
  HONEYCOMB_REQUIRE_VALID_RETURN(nullptr);
  const gfx::ImageSkia& image_skia = ParentClass::root_view()->GetImage(
      static_cast<views::Button::ButtonState>(button_state));
  if (image_skia.isNull()) {
    return nullptr;
  }
  return new HoneycombImageImpl(image_skia);
}

HONEYCOMB_LABEL_BUTTON_IMPL_T void HONEYCOMB_LABEL_BUTTON_IMPL_D::SetTextColor(
    honey_button_state_t for_state,
    honey_color_t color) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  ParentClass::root_view()->SetTextColor(
      static_cast<views::Button::ButtonState>(for_state), color);
}

HONEYCOMB_LABEL_BUTTON_IMPL_T void HONEYCOMB_LABEL_BUTTON_IMPL_D::SetEnabledTextColors(
    honey_color_t color) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  ParentClass::root_view()->SetEnabledTextColors(color);
}

HONEYCOMB_LABEL_BUTTON_IMPL_T void HONEYCOMB_LABEL_BUTTON_IMPL_D::SetFontList(
    const HoneycombString& font_list) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  ParentClass::root_view()->SetFontList(gfx::FontList(font_list));
}

HONEYCOMB_LABEL_BUTTON_IMPL_T void HONEYCOMB_LABEL_BUTTON_IMPL_D::SetHorizontalAlignment(
    honey_horizontal_alignment_t alignment) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  ParentClass::root_view()->SetHorizontalAlignment(
      static_cast<gfx::HorizontalAlignment>(alignment));
}

HONEYCOMB_LABEL_BUTTON_IMPL_T void HONEYCOMB_LABEL_BUTTON_IMPL_D::SetMinimumSize(
    const HoneycombSize& size) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  ParentClass::root_view()->SetMinSize(gfx::Size(size.width, size.height));
}

HONEYCOMB_LABEL_BUTTON_IMPL_T void HONEYCOMB_LABEL_BUTTON_IMPL_D::SetMaximumSize(
    const HoneycombSize& size) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  ParentClass::root_view()->SetMaxSize(gfx::Size(size.width, size.height));
}

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_LABEL_BUTTON_IMPL_H_
