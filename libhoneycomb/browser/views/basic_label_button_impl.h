// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_BASIC_LABEL_BUTTON_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_BASIC_LABEL_BUTTON_IMPL_H_
#pragma once

#include "include/views/honey_button_delegate.h"
#include "include/views/honey_label_button.h"

#include "libhoneycomb/browser/views/label_button_impl.h"

#include "ui/views/controls/button/label_button.h"

class HoneycombBasicLabelButtonImpl : public HoneycombLabelButtonImpl<views::LabelButton,
                                                          HoneycombLabelButton,
                                                          HoneycombButtonDelegate> {
 public:
  using ParentClass =
      HoneycombLabelButtonImpl<views::LabelButton, HoneycombLabelButton, HoneycombButtonDelegate>;

  HoneycombBasicLabelButtonImpl(const HoneycombBasicLabelButtonImpl&) = delete;
  HoneycombBasicLabelButtonImpl& operator=(const HoneycombBasicLabelButtonImpl&) = delete;

  // Create a new HoneycombLabelButton instance. |delegate| may be nullptr.
  static HoneycombRefPtr<HoneycombBasicLabelButtonImpl> Create(
      HoneycombRefPtr<HoneycombButtonDelegate> delegate,
      const HoneycombString& text);

  // HoneycombViewAdapter methods:
  std::string GetDebugType() override { return "LabelButton"; }

 private:
  // Create a new implementation object.
  // Always call Initialize() after creation.
  // |delegate| may be nullptr.
  explicit HoneycombBasicLabelButtonImpl(HoneycombRefPtr<HoneycombButtonDelegate> delegate);

  // HoneycombViewImpl methods:
  views::LabelButton* CreateRootView() override;
  void InitializeRootView() override;

  IMPLEMENT_REFCOUNTING_DELETE_ON_UIT(HoneycombBasicLabelButtonImpl);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_BASIC_LABEL_BUTTON_IMPL_H_
