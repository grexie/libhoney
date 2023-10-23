// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_VIEWS_PANEL_VIEW_H_
#define HONEYCOMB_LIBHONEY_BROWSER_VIEWS_PANEL_VIEW_H_
#pragma once

#include "include/views/honey_panel_delegate.h"

#include "libhoney/browser/thread_util.h"
#include "libhoney/browser/views/view_view.h"

#include "base/logging.h"

// Helpers for template boiler-plate.
#define HONEYCOMB_PANEL_VIEW_T HONEYCOMB_VIEW_VIEW_T
#define HONEYCOMB_PANEL_VIEW_A HONEYCOMB_VIEW_VIEW_A
#define HONEYCOMB_PANEL_VIEW_D HoneycombPanelView<HONEYCOMB_PANEL_VIEW_A>

// Template for implementing views::View-derived classes that support adding and
// removing children (called a Panel in Honeycomb terminology). See comments in
// view_impl.h for a usage overview.
HONEYCOMB_PANEL_VIEW_T class HoneycombPanelView : public HONEYCOMB_VIEW_VIEW_D {
 public:
  using ParentClass = HONEYCOMB_VIEW_VIEW_D;

  // |honey_delegate| may be nullptr.
  template <typename... Args>
  explicit HoneycombPanelView(HoneycombViewDelegateClass* honey_delegate, Args... args)
      : ParentClass(honey_delegate, args...) {}

  // Returns the HoneycombPanel associated with this view. See comments on
  // HoneycombViewView::GetHoneycombView.
  HoneycombRefPtr<HoneycombPanel> GetHoneycombPanel() const {
    HoneycombRefPtr<HoneycombPanel> panel = ParentClass::GetHoneycombView()->AsPanel();
    DCHECK(panel);
    return panel;
  }
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_VIEWS_PANEL_VIEW_H_
