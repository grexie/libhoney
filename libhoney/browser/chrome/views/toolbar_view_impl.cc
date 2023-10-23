// Copyright 2021 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoney/browser/chrome/views/toolbar_view_impl.h"

// static
HoneycombRefPtr<HoneycombToolbarViewImpl> HoneycombToolbarViewImpl::Create(
    HoneycombRefPtr<HoneycombViewDelegate> delegate,
    Browser* browser,
    BrowserView* browser_view,
    absl::optional<ToolbarView::DisplayMode> display_mode) {
  HONEYCOMB_REQUIRE_UIT_RETURN(nullptr);
  HoneycombRefPtr<HoneycombToolbarViewImpl> view =
      new HoneycombToolbarViewImpl(delegate, browser, browser_view, display_mode);
  view->Initialize();
  return view;
}

// static
const char* const HoneycombToolbarViewImpl::kTypeString = "ToolbarView";

HoneycombToolbarViewImpl::HoneycombToolbarViewImpl(
    HoneycombRefPtr<HoneycombViewDelegate> delegate,
    Browser* browser,
    BrowserView* browser_view,
    absl::optional<ToolbarView::DisplayMode> display_mode)
    : ParentClass(delegate),
      browser_(browser),
      browser_view_(browser_view),
      display_mode_(display_mode) {}

HoneycombToolbarViewView* HoneycombToolbarViewImpl::CreateRootView() {
  return new HoneycombToolbarViewView(delegate(), browser_, browser_view_,
                                display_mode_);
}

void HoneycombToolbarViewImpl::InitializeRootView() {
  static_cast<HoneycombToolbarViewView*>(root_view())->Initialize();
}
