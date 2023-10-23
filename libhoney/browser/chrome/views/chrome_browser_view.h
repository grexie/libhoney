// Copyright 2021 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_CHROME_VIEWS_CHROME_BROWSER_VIEW_H_
#define HONEYCOMB_LIBHONEY_BROWSER_CHROME_VIEWS_CHROME_BROWSER_VIEW_H_
#pragma once

#include "include/views/honey_browser_view.h"
#include "include/views/honey_browser_view_delegate.h"
#include "libhoney/browser/chrome/views/toolbar_view_impl.h"
#include "libhoney/browser/views/browser_view_view.h"
#include "libhoney/browser/views/view_view.h"

#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/views/frame/browser_view.h"

namespace views {
class WebView;
}  // namespace views

class HoneycombBrowserViewImpl;

// A variant of HoneycombBrowserViewView that extends BrowserView instead of
// views::WebView. See chrome_browser_frame.h for related documentation.
class ChromeBrowserView
    : public HoneycombViewView<BrowserView, HoneycombBrowserViewDelegate> {
 public:
  using ParentClass = HoneycombViewView<BrowserView, HoneycombBrowserViewDelegate>;

  // |honey_browser_view| is non-nullptr and will outlive this object.
  explicit ChromeBrowserView(HoneycombBrowserViewImpl* honey_browser_view);

  ChromeBrowserView(const ChromeBrowserView&) = delete;
  ChromeBrowserView& operator=(const ChromeBrowserView&) = delete;

  // Called by ChromeBrowserHostImpl.
  void InitBrowser(std::unique_ptr<Browser> browser);
  void Destroyed();

  // View methods:
  void ViewHierarchyChanged(
      const views::ViewHierarchyChangedDetails& details) override;
  void AddedToWidget() override;
  void OnBoundsChanged(const gfx::Rect& previous_bounds) override;
  void OnGestureEvent(ui::GestureEvent* event) override;

  // BrowserView methods:
  ToolbarView* OverrideCreateToolbar() override;

  HoneycombRefPtr<HoneycombToolbarViewImpl> honey_toolbar() const { return honey_toolbar_; }
  HoneycombBrowserViewImpl* honey_browser_view() const { return honey_browser_view_; }

 private:
  HoneycombBrowserViewImpl* const honey_browser_view_;

  views::WebView* web_view_ = nullptr;

  bool destroyed_ = false;

  HoneycombRefPtr<HoneycombToolbarViewImpl> honey_toolbar_;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_CHROME_VIEWS_CHROME_BROWSER_VIEW_H_
