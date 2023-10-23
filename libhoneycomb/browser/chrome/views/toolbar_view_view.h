// Copyright 2021 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_CHROME_VIEWS_TOOLBAR_VIEW_VIEW_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_CHROME_VIEWS_TOOLBAR_VIEW_VIEW_H_
#pragma once

#include "libhoneycomb/browser/views/view_view.h"

#include "chrome/browser/ui/views/toolbar/toolbar_view.h"

class HoneycombToolbarViewView : public HoneycombViewView<ToolbarView, HoneycombViewDelegate> {
 public:
  using ParentClass = HoneycombViewView<ToolbarView, HoneycombViewDelegate>;

  // |honey_delegate| may be nullptr.
  explicit HoneycombToolbarViewView(HoneycombViewDelegate* honey_delegate,
                              Browser* browser,
                              BrowserView* browser_view,
                              absl::optional<DisplayMode> display_mode);

  HoneycombToolbarViewView(const HoneycombToolbarViewView&) = delete;
  HoneycombToolbarViewView& operator=(const HoneycombToolbarViewView&) = delete;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_CHROME_VIEWS_TOOLBAR_VIEW_VIEW_H_
