// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_VIEWS_WINDOW_IMPL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_VIEWS_WINDOW_IMPL_H_
#pragma once

#include <map>

#include "include/views/honey_window.h"
#include "include/views/honey_window_delegate.h"

#include "libhoney/browser/menu_model_impl.h"
#include "libhoney/browser/views/panel_impl.h"
#include "libhoney/browser/views/window_view.h"

#include "ui/base/accelerators/accelerator.h"
#include "ui/views/controls/menu/menu_runner.h"
#include "ui/views/widget/widget.h"

namespace views {
class MenuButton;
}

class HoneycombWindowImpl
    : public HoneycombPanelImpl<HoneycombWindowView, HoneycombWindow, HoneycombWindowDelegate>,
      public HoneycombWindowView::Delegate,
      public ui::AcceleratorTarget {
 public:
  using ParentClass = HoneycombPanelImpl<HoneycombWindowView, HoneycombWindow, HoneycombWindowDelegate>;

  HoneycombWindowImpl(const HoneycombWindowImpl&) = delete;
  HoneycombWindowImpl& operator=(const HoneycombWindowImpl&) = delete;

  // Create a new HoneycombWindow instance. |delegate| may be nullptr. |parent_widget|
  // will be used when creating a Chrome child window.
  static HoneycombRefPtr<HoneycombWindowImpl> Create(HoneycombRefPtr<HoneycombWindowDelegate> delegate,
                                         gfx::AcceleratedWidget parent_widget);

  // HoneycombWindow methods:
  void Show() override;
  void ShowAsBrowserModalDialog(
      HoneycombRefPtr<HoneycombBrowserView> browser_view) override;
  void Hide() override;
  void CenterWindow(const HoneycombSize& size) override;
  void Close() override;
  bool IsClosed() override;
  void Activate() override;
  void Deactivate() override;
  bool IsActive() override;
  void BringToTop() override;
  void SetAlwaysOnTop(bool on_top) override;
  bool IsAlwaysOnTop() override;
  void Maximize() override;
  void Minimize() override;
  void Restore() override;
  void SetFullscreen(bool fullscreen) override;
  bool IsMaximized() override;
  bool IsMinimized() override;
  bool IsFullscreen() override;
  void SetTitle(const HoneycombString& title) override;
  HoneycombString GetTitle() override;
  void SetWindowIcon(HoneycombRefPtr<HoneycombImage> image) override;
  HoneycombRefPtr<HoneycombImage> GetWindowIcon() override;
  void SetWindowAppIcon(HoneycombRefPtr<HoneycombImage> image) override;
  HoneycombRefPtr<HoneycombImage> GetWindowAppIcon() override;
  HoneycombRefPtr<HoneycombOverlayController> AddOverlayView(
      HoneycombRefPtr<HoneycombView> view,
      honey_docking_mode_t docking_mode) override;
  void ShowMenu(HoneycombRefPtr<HoneycombMenuModel> menu_model,
                const HoneycombPoint& screen_point,
                honey_menu_anchor_position_t anchor_position) override;
  void CancelMenu() override;
  HoneycombRefPtr<HoneycombDisplay> GetDisplay() override;
  HoneycombRect GetClientAreaBoundsInScreen() override;
  void SetDraggableRegions(
      const std::vector<HoneycombDraggableRegion>& regions) override;
  HoneycombWindowHandle GetWindowHandle() override;
  void SendKeyPress(int key_code, uint32_t event_flags) override;
  void SendMouseMove(int screen_x, int screen_y) override;
  void SendMouseEvents(honey_mouse_button_type_t button,
                       bool mouse_down,
                       bool mouse_up) override;
  void SetAccelerator(int command_id,
                      int key_code,
                      bool shift_pressed,
                      bool ctrl_pressed,
                      bool alt_pressed) override;
  void RemoveAccelerator(int command_id) override;
  void RemoveAllAccelerators() override;

  // HoneycombViewAdapter methods:
  void Detach() override;

  // HoneycombPanel methods:
  HoneycombRefPtr<HoneycombWindow> AsWindow() override { return this; }

  // HoneycombView methods:
  void SetBounds(const HoneycombRect& bounds) override;
  HoneycombRect GetBounds() override;
  HoneycombRect GetBoundsInScreen() override;
  void SetSize(const HoneycombSize& bounds) override;
  void SetPosition(const HoneycombPoint& position) override;
  void SizeToPreferredSize() override;
  void SetVisible(bool visible) override;
  bool IsVisible() override;
  bool IsDrawn() override;
  void SetBackgroundColor(honey_color_t color) override;

  // HoneycombWindowView::Delegate methods:
  bool CanWidgetClose() override;
  void OnWindowClosing() override;
  void OnWindowViewDeleted() override;

  // HoneycombViewAdapter methods:
  std::string GetDebugType() override { return "Window"; }
  void GetDebugInfo(base::Value::Dict* info, bool include_children) override;

  // ui::AcceleratorTarget methods:
  bool AcceleratorPressed(const ui::Accelerator& accelerator) override;
  bool CanHandleAccelerators() const override;

  // Called for key events that have not been handled by other controls in the
  // window. Returns true if the event was handled.
  bool OnKeyEvent(const HoneycombKeyEvent& event);

  void ShowMenu(views::MenuButton* menu_button,
                HoneycombRefPtr<HoneycombMenuModel> menu_model,
                const HoneycombPoint& screen_point,
                honey_menu_anchor_position_t anchor_position);
  void MenuClosed();

  views::Widget* widget() const { return widget_; }
  bool initialized() const { return initialized_; }

 private:
  // Create a new implementation object.
  // Always call Initialize() after creation.
  // |delegate| may be nullptr.
  explicit HoneycombWindowImpl(HoneycombRefPtr<HoneycombWindowDelegate> delegate);

  // HoneycombViewImpl methods:
  HoneycombWindowView* CreateRootView() override;
  void InitializeRootView() override;

  // Initialize the Widget.
  void CreateWidget(gfx::AcceleratedWidget parent_widget);

  views::Widget* widget_ = nullptr;

  // True if the window has been initialized.
  bool initialized_ = false;

  // True if the window has been destroyed.
  bool destroyed_ = false;

  // The currently active menu model and runner.
  HoneycombRefPtr<HoneycombMenuModelImpl> menu_model_;
  std::unique_ptr<views::MenuRunner> menu_runner_;

  // Map of command_id to accelerator.
  using AcceleratorMap = std::map<int, ui::Accelerator>;
  AcceleratorMap accelerator_map_;

#if defined(USE_AURA)
  // Native widget's handler to receive events after the event target.
  std::unique_ptr<ui::EventHandler> unhandled_key_event_handler_;
#endif

  // True if this window was shown using ShowAsBrowserModalDialog().
  bool shown_as_browser_modal_ = false;

  IMPLEMENT_REFCOUNTING_DELETE_ON_UIT(HoneycombWindowImpl);
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_VIEWS_WINDOW_IMPL_H_
