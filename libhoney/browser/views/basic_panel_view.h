// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_VIEWS_BASIC_PANEL_VIEW_H_
#define HONEYCOMB_LIBHONEY_BROWSER_VIEWS_BASIC_PANEL_VIEW_H_
#pragma once

#include "include/views/honey_panel_delegate.h"

#include "libhoney/browser/views/panel_view.h"

class HoneycombBasicPanelView : public HoneycombPanelView<views::View, HoneycombPanelDelegate> {
 public:
  using ParentClass = HoneycombPanelView<views::View, HoneycombPanelDelegate>;

  // |honey_delegate| may be nullptr.
  explicit HoneycombBasicPanelView(HoneycombPanelDelegate* honey_delegate);

  HoneycombBasicPanelView(const HoneycombBasicPanelView&) = delete;
  HoneycombBasicPanelView& operator=(const HoneycombBasicPanelView&) = delete;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_VIEWS_BASIC_PANEL_VIEW_H_
