// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_VIEWS_SCROLL_VIEW_VIEW_H_
#define HONEYCOMB_LIBHONEY_BROWSER_VIEWS_SCROLL_VIEW_VIEW_H_
#pragma once

#include "include/views/honey_panel_delegate.h"

#include "libhoney/browser/views/view_view.h"

#include "ui/views/controls/scroll_view.h"

class HoneycombScrollViewView
    : public HoneycombViewView<views::ScrollView, HoneycombViewDelegate> {
 public:
  using ParentClass = HoneycombViewView<views::ScrollView, HoneycombViewDelegate>;

  // |honey_delegate| may be nullptr.
  explicit HoneycombScrollViewView(HoneycombViewDelegate* honey_delegate);

  HoneycombScrollViewView(const HoneycombScrollViewView&) = delete;
  HoneycombScrollViewView& operator=(const HoneycombScrollViewView&) = delete;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_VIEWS_SCROLL_VIEW_VIEW_H_
