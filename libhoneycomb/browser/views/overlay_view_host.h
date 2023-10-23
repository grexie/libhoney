// Copyright 2021 The Honeycomb Authors. Portions copyright
// 2011 The Chromium Authors and 2016 the CEF Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_OVERLAY_VIEW_HOST_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_OVERLAY_VIEW_HOST_H_
#pragma once

#include <memory>

#include "include/views/honey_overlay_controller.h"
#include "include/views/honey_view.h"

#include "ui/views/view_observer.h"
#include "ui/views/widget/widget_delegate.h"

class HoneycombWindowView;

// Host class for a child Widget that behaves as an overlay control. Based on
// Chrome's DropdownBarHost.
class HoneycombOverlayViewHost : public views::WidgetDelegate,
                           public views::ViewObserver {
 public:
  // |window_view| is the top-level view that contains this overlay.
  HoneycombOverlayViewHost(HoneycombWindowView* window_view,
                     honey_docking_mode_t docking_mode);

  HoneycombOverlayViewHost(const HoneycombOverlayViewHost&) = delete;
  HoneycombOverlayViewHost& operator=(const HoneycombOverlayViewHost&) = delete;

  // Initializes the HoneycombOverlayViewHost. This creates the Widget that |view|
  // paints into. On Aura platforms, |host_view| is the view whose position in
  // the |window_view_| view hierarchy determines the z-order of the widget
  // relative to views with layers and views with associated NativeViews.
  void Init(views::View* host_view, HoneycombRefPtr<HoneycombView> view);

  void Destroy();

  void MoveIfNecessary();

  void SetOverlayBounds(const gfx::Rect& bounds);
  void SetOverlayInsets(const HoneycombInsets& insets);

  // views::ViewObserver methods:
  void OnViewBoundsChanged(views::View* observed_view) override;

  honey_docking_mode_t docking_mode() const { return docking_mode_; }
  HoneycombRefPtr<HoneycombOverlayController> controller() const { return honey_controller_; }
  HoneycombWindowView* window_view() const { return window_view_; }
  views::Widget* widget() const { return widget_.get(); }
  views::View* view() const { return view_; }
  gfx::Rect bounds() const { return bounds_; }
  HoneycombInsets insets() const { return insets_; }

 private:
  gfx::Rect ComputeBounds() const;

  // The HoneycombWindowView that created us.
  HoneycombWindowView* const window_view_;

  const honey_docking_mode_t docking_mode_;

  // Our view, which is responsible for drawing the UI.
  views::View* view_ = nullptr;

  // The Widget implementation that is created and maintained by the overlay.
  // It contains |view_|.
  std::unique_ptr<views::Widget> widget_;

  HoneycombRefPtr<HoneycombOverlayController> honey_controller_;

  gfx::Rect bounds_;
  bool bounds_changing_ = false;

  HoneycombInsets insets_;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_OVERLAY_VIEW_HOST_H_
