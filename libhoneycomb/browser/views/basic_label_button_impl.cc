// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoneycomb/browser/views/basic_label_button_impl.h"

#include "libhoneycomb/browser/views/basic_label_button_view.h"

// static
HoneycombRefPtr<HoneycombLabelButton> HoneycombLabelButton::CreateLabelButton(
    HoneycombRefPtr<HoneycombButtonDelegate> delegate,
    const HoneycombString& text) {
  return HoneycombBasicLabelButtonImpl::Create(delegate, text);
}

// static
HoneycombRefPtr<HoneycombBasicLabelButtonImpl> HoneycombBasicLabelButtonImpl::Create(
    HoneycombRefPtr<HoneycombButtonDelegate> delegate,
    const HoneycombString& text) {
  HONEYCOMB_REQUIRE_UIT_RETURN(nullptr);
  HoneycombRefPtr<HoneycombBasicLabelButtonImpl> label_button =
      new HoneycombBasicLabelButtonImpl(delegate);
  label_button->Initialize();
  if (!text.empty()) {
    label_button->SetText(text);
  }
  return label_button;
}

HoneycombBasicLabelButtonImpl::HoneycombBasicLabelButtonImpl(
    HoneycombRefPtr<HoneycombButtonDelegate> delegate)
    : ParentClass(delegate) {}

views::LabelButton* HoneycombBasicLabelButtonImpl::CreateRootView() {
  return new HoneycombBasicLabelButtonView(delegate());
}

void HoneycombBasicLabelButtonImpl::InitializeRootView() {
  static_cast<HoneycombBasicLabelButtonView*>(root_view())->Initialize();
}
