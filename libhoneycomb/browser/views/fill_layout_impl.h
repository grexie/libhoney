// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_FILL_LAYOUT_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_FILL_LAYOUT_IMPL_H_
#pragma once

#include "include/views/honey_fill_layout.h"

#include "libhoneycomb/browser/views/layout_impl.h"
#include "ui/views/layout/fill_layout.h"

class HoneycombFillLayoutImpl
    : public HoneycombLayoutImpl<views::FillLayout, HoneycombFillLayout> {
 public:
  HoneycombFillLayoutImpl(const HoneycombFillLayoutImpl&) = delete;
  HoneycombFillLayoutImpl& operator=(const HoneycombFillLayoutImpl&) = delete;

  // Create a new HoneycombFillLayout insance. |owner_view| must be non-nullptr.
  static HoneycombRefPtr<HoneycombFillLayout> Create(views::View* owner_view);

  // HoneycombLayout methods:
  HoneycombRefPtr<HoneycombFillLayout> AsFillLayout() override { return this; }

 private:
  HoneycombFillLayoutImpl();

  views::FillLayout* CreateLayout() override;

  IMPLEMENT_REFCOUNTING_DELETE_ON_UIT(HoneycombFillLayoutImpl);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_FILL_LAYOUT_IMPL_H_
