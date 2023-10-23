// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_VIEWS_WINDOW_VIEW_H_
#define HONEYCOMB_LIBHONEY_BROWSER_VIEWS_WINDOW_VIEW_H_
#pragma once

#include <vector>

#include "include/views/honey_window.h"
#include "include/views/honey_window_delegate.h"

#include "libhoney/browser/views/overlay_view_host.h"
#include "libhoney/browser/views/panel_view.h"
#include "libhoney/browser/views/widget_destruction_observer.h"

#include "third_party/skia/include/core/SkRegion.h"
#include "ui/display/display.h"
#include "ui/views/widget/widget_delegate.h"
#include "ui/views/widget/widget_observer.h"

// Manages the views-based root window. This object will be deleted
// automatically when the associated root window is destroyed.
class HoneycombWindowView
    : public HoneycombPanelView<views::WidgetDelegateView, HoneycombWindowDelegate>,
      public views::WidgetObserver {
 public:
  using ParentClass =
      HoneycombPanelView<views::WidgetDelegateView, HoneycombWindowDelegate>;

  class Delegate {
   public:
    // Returns true to signal that the Widget can be closed.
    virtual bool CanWidgetClose() = 0;

    // Called when the underlying platform window is closing.
    virtual void OnWindowClosing() = 0;

    // Called when the WindowView is about to be deleted.
    virtual void OnWindowViewDeleted() = 0;

   protected:
    virtual ~Delegate() {}
  };

  // |honey_delegate| may be nullptr.
  // |window_delegate| must be non-nullptr.
  HoneycombWindowView(HoneycombWindowDelegate* honey_delegate, Delegate* window_delegate);

  HoneycombWindowView(const HoneycombWindowView&) = delete;
  HoneycombWindowView& operator=(const HoneycombWindowView&) = delete;

  // Create the Widget.
  void CreateWidget(gfx::AcceleratedWidget parent_widget);

  // Returns the HoneycombWindow associated with this view. See comments on
  // HoneycombViewView::GetHoneycombView.
  HoneycombRefPtr<HoneycombWindow> GetHoneycombWindow() const;

  // views::WidgetDelegate methods:
  bool CanMinimize() const override;
  bool CanMaximize() const override;
  std::u16string GetWindowTitle() const override;
  ui::ImageModel GetWindowIcon() override;
  ui::ImageModel GetWindowAppIcon() override;
  void WindowClosing() override;
  views::View* GetContentsView() override;
  views::ClientView* CreateClientView(views::Widget* widget) override;
  std::unique_ptr<views::NonClientFrameView> CreateNonClientFrameView(
      views::Widget* widget) override;
  bool ShouldDescendIntoChildForEventHandling(
      gfx::NativeView child,
      const gfx::Point& location) override;
  bool MaybeGetMinimumSize(gfx::Size* size) const override;
  bool MaybeGetMaximumSize(gfx::Size* size) const override;

  // views::View methods:
  void ViewHierarchyChanged(
      const views::ViewHierarchyChangedDetails& details) override;

  // views::WidgetObserver methods:
  void OnWidgetActivationChanged(views::Widget* widget, bool active) override;
  void OnWidgetBoundsChanged(views::Widget* widget,
                             const gfx::Rect& new_bounds) override;

  // Returns the Display containing this Window.
  display::Display GetDisplay() const;

  // Set/get the window title.
  void SetTitle(const std::u16string& title);
  std::u16string title() const { return title_; }

  // Set/get the window icon. This should be a 16x16 icon suitable for use in
  // the Windows's title bar.
  void SetWindowIcon(HoneycombRefPtr<HoneycombImage> window_icon);
  HoneycombRefPtr<HoneycombImage> window_icon() const { return window_icon_; }

  // Set/get the window app icon. This should be a larger icon for use in the
  // host environment app switching UI. On Windows, this is the ICON_BIG used in
  // Alt-Tab list and Windows taskbar. The Window icon will be used by default
  // if no Window App icon is specified.
  void SetWindowAppIcon(HoneycombRefPtr<HoneycombImage> window_app_icon);
  HoneycombRefPtr<HoneycombImage> window_app_icon() const { return window_app_icon_; }

  HoneycombRefPtr<HoneycombOverlayController> AddOverlayView(
      HoneycombRefPtr<HoneycombView> view,
      honey_docking_mode_t docking_mode);

  // Set/get the draggable regions.
  void SetDraggableRegions(const std::vector<HoneycombDraggableRegion>& regions);
  SkRegion* draggable_region() const { return draggable_region_.get(); }

  // Returns the NonClientFrameView for this Window. May be nullptr.
  views::NonClientFrameView* GetNonClientFrameView() const;

  // Optionally modify the bounding box for the Chrome Find bar.
  void UpdateFindBarBoundingBox(gfx::Rect* bounds) const;

  absl::optional<float> GetTitlebarHeight() const;
  bool IsFrameless() const { return is_frameless_; }

  // The Widget that hosts us, if we're a modal dialog. May return nullptr
  // during initialization and destruction.
  views::Widget* host_widget() const;

 private:
  // Called when removed from the Widget and before |this| is deleted.
  void DeleteDelegate();

  void MoveOverlaysIfNecessary();

  // Not owned by this object.
  Delegate* window_delegate_;

  // True if the window is frameless. It might still be resizable and draggable.
  bool is_frameless_ = false;

  std::u16string title_;
  HoneycombRefPtr<HoneycombImage> window_icon_;
  HoneycombRefPtr<HoneycombImage> window_app_icon_;

  std::unique_ptr<SkRegion> draggable_region_;
  std::vector<gfx::Rect> draggable_rects_;

  // Tracks the Widget that hosts us, if we're a modal dialog.
  std::unique_ptr<WidgetDestructionObserver> host_widget_destruction_observer_;

  // Hosts for overlay widgets.
  std::vector<std::unique_ptr<HoneycombOverlayViewHost>> overlay_hosts_;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_VIEWS_WINDOW_VIEW_H_
