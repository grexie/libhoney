// Copyright 2021 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_CHROME_VIEWS_TOOLBAR_VIEW_IMPL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_CHROME_VIEWS_TOOLBAR_VIEW_IMPL_H_
#pragma once

#include "include/views/honey_view_delegate.h"

#include "libhoney/browser/chrome/views/toolbar_view_view.h"
#include "libhoney/browser/views/view_impl.h"

class Browser;
class BrowserView;

class HoneycombToolbarViewImpl
    : public HoneycombViewImpl<HoneycombToolbarViewView, HoneycombView, HoneycombViewDelegate> {
 public:
  using ParentClass = HoneycombViewImpl<HoneycombToolbarViewView, HoneycombView, HoneycombViewDelegate>;

  HoneycombToolbarViewImpl(const HoneycombToolbarViewImpl&) = delete;
  HoneycombToolbarViewImpl& operator=(const HoneycombToolbarViewImpl&) = delete;

  // Create a new HoneycombToolbarViewImpl instance. |delegate| may be nullptr.
  static HoneycombRefPtr<HoneycombToolbarViewImpl> Create(
      HoneycombRefPtr<HoneycombViewDelegate> delegate,
      Browser* browser,
      BrowserView* browser_view,
      absl::optional<ToolbarView::DisplayMode> display_mode);

  static const char* const kTypeString;

  // HoneycombViewAdapter methods:
  std::string GetDebugType() override { return kTypeString; }

 private:
  // Create a new implementation object.
  // Always call Initialize() after creation.
  // |delegate| may be nullptr.
  HoneycombToolbarViewImpl(HoneycombRefPtr<HoneycombViewDelegate> delegate,
                     Browser* browser,
                     BrowserView* browser_view,
                     absl::optional<ToolbarView::DisplayMode> display_mode);

  // HoneycombViewImpl methods:
  HoneycombToolbarViewView* CreateRootView() override;
  void InitializeRootView() override;

  Browser* const browser_;
  BrowserView* const browser_view_;
  absl::optional<ToolbarView::DisplayMode> const display_mode_;

  IMPLEMENT_REFCOUNTING_DELETE_ON_UIT(HoneycombToolbarViewImpl);
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_VIEWS_SCROLL_VIEW_IMPL_H_
