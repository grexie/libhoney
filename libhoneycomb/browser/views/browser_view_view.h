// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_BROWSER_VIEW_VIEW_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_BROWSER_VIEW_VIEW_H_
#pragma once

#include "include/views/honey_browser_view_delegate.h"

#include "libhoneycomb/browser/views/view_view.h"

#include "ui/views/controls/webview/webview.h"

// Extend views::WebView with a no-argument constructor as required by the
// HoneycombViewView template.
class WebViewEx : public views::WebView {
 public:
  WebViewEx() : views::WebView(nullptr) {
    // Mouse events on draggable regions will not be handled by the WebView.
    // Avoid the resulting DCHECK in NativeViewHost::OnMousePressed by
    // configuring the NativeViewHost not to process events via the view
    // hierarchy.
    holder()->SetCanProcessEventsWithinSubtree(false);
  }
};

class HoneycombBrowserViewView
    : public HoneycombViewView<WebViewEx, HoneycombBrowserViewDelegate> {
 public:
  using ParentClass = HoneycombViewView<WebViewEx, HoneycombBrowserViewDelegate>;

  HoneycombBrowserViewView(const HoneycombBrowserViewView&) = delete;
  HoneycombBrowserViewView& operator=(const HoneycombBrowserViewView&) = delete;

  class Delegate {
   public:
    // Called when the BrowserView has been added to a parent view.
    virtual void OnBrowserViewAdded() = 0;

    // Called when the BrowserView bounds have changed.
    virtual void OnBoundsChanged() = 0;

    // Called when the BrowserView receives a gesture event.
    // Returns true if the gesture was handled.
    virtual bool OnGestureEvent(ui::GestureEvent* event) = 0;

   protected:
    virtual ~Delegate() {}
  };

  // |honey_delegate| may be nullptr.
  // |browser_view_delegate| must be non-nullptr.
  HoneycombBrowserViewView(HoneycombBrowserViewDelegate* honey_delegate,
                     Delegate* browser_view_delegate);

  // View methods:
  void ViewHierarchyChanged(
      const views::ViewHierarchyChangedDetails& details) override;
  void OnBoundsChanged(const gfx::Rect& previous_bounds) override;
  void OnGestureEvent(ui::GestureEvent* event) override;

 private:
  // Not owned by this object.
  Delegate* browser_view_delegate_;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_BROWSER_VIEW_VIEW_H_
