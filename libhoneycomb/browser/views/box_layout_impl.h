// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_BOX_LAYOUT_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_BOX_LAYOUT_IMPL_H_
#pragma once

#include "include/views/honey_box_layout.h"

#include "libhoneycomb/browser/views/layout_impl.h"
#include "ui/views/layout/box_layout.h"

class HoneycombBoxLayoutImpl : public HoneycombLayoutImpl<views::BoxLayout, HoneycombBoxLayout> {
 public:
  // Necessary for the HONEYCOMB_REQUIRE_VALID_*() macros to compile.
  using ParentClass = HoneycombLayoutImpl<views::BoxLayout, HoneycombBoxLayout>;

  HoneycombBoxLayoutImpl(const HoneycombBoxLayoutImpl&) = delete;
  HoneycombBoxLayoutImpl& operator=(const HoneycombBoxLayoutImpl&) = delete;

  // Create a new HoneycombBoxLayout insance. |owner_view| must be non-nullptr.
  static HoneycombRefPtr<HoneycombBoxLayoutImpl> Create(
      const HoneycombBoxLayoutSettings& settings,
      views::View* owner_view);

  // HoneycombBoxLayout methods:
  void SetFlexForView(HoneycombRefPtr<HoneycombView> view, int flex) override;
  void ClearFlexForView(HoneycombRefPtr<HoneycombView> view) override;

  // HoneycombLayout methods:
  HoneycombRefPtr<HoneycombBoxLayout> AsBoxLayout() override { return this; }

 private:
  explicit HoneycombBoxLayoutImpl(const HoneycombBoxLayoutSettings& settings);

  views::BoxLayout* CreateLayout() override;

  HoneycombBoxLayoutSettings settings_;

  IMPLEMENT_REFCOUNTING_DELETE_ON_UIT(HoneycombBoxLayoutImpl);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_BOX_LAYOUT_IMPL_H_
