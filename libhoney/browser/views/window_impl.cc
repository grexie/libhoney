// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoney/browser/views/window_impl.h"

#include "libhoney/browser/browser_util.h"
#include "libhoney/browser/chrome/views/chrome_browser_frame.h"
#include "libhoney/browser/thread_util.h"
#include "libhoney/browser/views/browser_view_impl.h"
#include "libhoney/browser/views/display_impl.h"
#include "libhoney/browser/views/fill_layout_impl.h"
#include "libhoney/browser/views/layout_util.h"
#include "libhoney/browser/views/view_util.h"
#include "libhoney/browser/views/window_view.h"
#include "libhoney/features/runtime.h"

#include "base/i18n/rtl.h"
#include "components/constrained_window/constrained_window_views.h"
#include "ui/base/test/ui_controls.h"
#include "ui/compositor/compositor.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/views/controls/button/menu_button.h"
#include "ui/views/controls/menu/menu_runner.h"
#include "ui/views/controls/webview/webview.h"

#if defined(USE_AURA)
#include "ui/aura/window.h"
#endif  // defined(USE_AURA)

#if BUILDFLAG(IS_WIN)
#include "ui/aura/test/ui_controls_aurawin.h"
#include "ui/display/win/screen_win.h"
#endif

namespace {

// Based on chrome/test/base/interactive_ui_tests_main.cc.
void InitializeUITesting() {
  static bool initialized = false;
  if (!initialized) {
#if BUILDFLAG(IS_WIN)
    aura::test::EnableUIControlsAuraWin();
#else
    ui_controls::EnableUIControls();
#endif

    initialized = true;
  }
}

#if defined(USE_AURA)

// This class forwards KeyEvents to the HoneycombWindowImpl associated with a widget.
// This allows KeyEvents to be processed after all other targets.
// Events originating from HoneycombBrowserView will instead be delivered via
// HoneycombBrowserViewImpl::HandleKeyboardEvent.
class HoneycombUnhandledKeyEventHandler : public ui::EventHandler {
 public:
  HoneycombUnhandledKeyEventHandler(HoneycombWindowImpl* window_impl, views::Widget* widget)
      : window_impl_(window_impl),
        widget_(widget),
        window_(widget->GetNativeWindow()) {
    DCHECK(window_);
    window_->AddPostTargetHandler(this);
  }

  HoneycombUnhandledKeyEventHandler(const HoneycombUnhandledKeyEventHandler&) = delete;
  HoneycombUnhandledKeyEventHandler& operator=(const HoneycombUnhandledKeyEventHandler&) =
      delete;

  ~HoneycombUnhandledKeyEventHandler() override {
    window_->RemovePostTargetHandler(this);
  }

  // Implementation of ui::EventHandler:
  void OnKeyEvent(ui::KeyEvent* event) override {
    // Give the FocusManager a chance to handle accelerators first.
    // Widget::OnKeyEvent would normally call this after all EventHandlers have
    // had a shot but we don't want to wait.
    if (widget_->GetFocusManager() &&
        !widget_->GetFocusManager()->OnKeyEvent(*event)) {
      event->StopPropagation();
      return;
    }

    HoneycombKeyEvent honey_event;
    if (browser_util::GetHoneycombKeyEvent(*event, honey_event) &&
        window_impl_->OnKeyEvent(honey_event)) {
      event->StopPropagation();
    }
  }

 private:
  // Members are guaranteed to outlive this object.
  HoneycombWindowImpl* window_impl_;
  views::Widget* widget_;

  // |window_| is the event target that is associated with this class.
  aura::Window* window_;
};

#endif  // defined(USE_AURA)

}  // namespace

// static
HoneycombRefPtr<HoneycombWindow> HoneycombWindow::CreateTopLevelWindow(
    HoneycombRefPtr<HoneycombWindowDelegate> delegate) {
  return HoneycombWindowImpl::Create(delegate, gfx::kNullAcceleratedWidget);
}

// static
HoneycombRefPtr<HoneycombWindowImpl> HoneycombWindowImpl::Create(
    HoneycombRefPtr<HoneycombWindowDelegate> delegate,
    gfx::AcceleratedWidget parent_widget) {
  HONEYCOMB_REQUIRE_UIT_RETURN(nullptr);
  HoneycombRefPtr<HoneycombWindowImpl> window = new HoneycombWindowImpl(delegate);
  window->Initialize();
  window->CreateWidget(parent_widget);
  return window;
}

void HoneycombWindowImpl::Show() {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (widget_) {
    shown_as_browser_modal_ = false;
    widget_->Show();
  }
}

void HoneycombWindowImpl::ShowAsBrowserModalDialog(
    HoneycombRefPtr<HoneycombBrowserView> browser_view) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (widget_) {
    auto* browser_view_impl =
        static_cast<HoneycombBrowserViewImpl*>(browser_view.get());

    // |browser_view| must belong to the host widget.
    auto* host_widget = static_cast<HoneycombWindowView*>(root_view())->host_widget();
    CHECK(host_widget &&
          browser_view_impl->root_view()->GetWidget() == host_widget);

    if (auto web_view = browser_view_impl->web_view()) {
      if (auto web_contents = web_view->web_contents()) {
        shown_as_browser_modal_ = true;
        constrained_window::ShowModalDialog(widget_->GetNativeWindow(),
                                            web_contents);

        // NativeWebContentsModalDialogManagerViews::ManageDialog() disables
        // movement. That has no impact on native frames but interferes with
        // draggable regions.
        widget_->set_movement_disabled(false);
      }
    }
  }
}

void HoneycombWindowImpl::Hide() {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (widget_) {
    widget_->Hide();
  }
}

void HoneycombWindowImpl::CenterWindow(const HoneycombSize& size) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (widget_) {
    widget_->CenterWindow(gfx::Size(size.width, size.height));
  }
}

void HoneycombWindowImpl::Close() {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (widget_ && !widget_->IsClosed()) {
    widget_->Close();
  }
}

bool HoneycombWindowImpl::IsClosed() {
  HONEYCOMB_REQUIRE_UIT_RETURN(false);
  return destroyed_ || (widget_ && widget_->IsClosed());
}

void HoneycombWindowImpl::Activate() {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (widget_ && widget_->CanActivate() && !widget_->IsActive()) {
    widget_->Activate();
  }
}

void HoneycombWindowImpl::Deactivate() {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (widget_ && widget_->CanActivate() && widget_->IsActive()) {
    widget_->Deactivate();
  }
}

bool HoneycombWindowImpl::IsActive() {
  HONEYCOMB_REQUIRE_VALID_RETURN(false);
  if (widget_) {
    return widget_->IsActive();
  }
  return false;
}

void HoneycombWindowImpl::BringToTop() {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (widget_) {
    widget_->StackAtTop();
  }
}

void HoneycombWindowImpl::SetAlwaysOnTop(bool on_top) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (widget_ && on_top != (widget_->GetZOrderLevel() ==
                            ui::ZOrderLevel::kFloatingWindow)) {
    widget_->SetZOrderLevel(on_top ? ui::ZOrderLevel::kFloatingWindow
                                   : ui::ZOrderLevel::kNormal);
  }
}

bool HoneycombWindowImpl::IsAlwaysOnTop() {
  HONEYCOMB_REQUIRE_VALID_RETURN(false);
  if (widget_) {
    return widget_->GetZOrderLevel() == ui::ZOrderLevel::kFloatingWindow;
  }
  return false;
}

void HoneycombWindowImpl::Maximize() {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (widget_ && !widget_->IsMaximized()) {
    widget_->Maximize();
  }
}

void HoneycombWindowImpl::Minimize() {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (widget_ && !widget_->IsMinimized()) {
    widget_->Minimize();
  }
}

void HoneycombWindowImpl::Restore() {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (widget_ && (widget_->IsMaximized() || widget_->IsMinimized())) {
    widget_->Restore();
  }
}

void HoneycombWindowImpl::SetFullscreen(bool fullscreen) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (widget_ && fullscreen != widget_->IsFullscreen()) {
    if (honey::IsChromeRuntimeEnabled()) {
      // If a BrowserView exists, toggle fullscreen mode via the Chrome command
      // for consistent behavior.
      auto* browser_frame = static_cast<ChromeBrowserFrame*>(widget_);
      if (browser_frame->browser_view()) {
        browser_frame->ToggleFullscreenMode();
        return;
      }
    }

    // Call the Widget method directly with Alloy runtime, or Chrome runtime
    // when no BrowserView exists.
    widget_->SetFullscreen(fullscreen);

    // Use a synchronous callback notification on Windows/Linux. Chrome runtime
    // on Windows/Linux gets notified synchronously via ChromeBrowserDelegate
    // callbacks when a BrowserView exists. MacOS (both runtimes) gets notified
    // asynchronously via HoneycombNativeWidgetMac callbacks.
#if !BUILDFLAG(IS_MAC)
    if (delegate()) {
      delegate()->OnWindowFullscreenTransition(this, /*is_completed=*/true);
    }
#endif
  }
}

bool HoneycombWindowImpl::IsMaximized() {
  HONEYCOMB_REQUIRE_VALID_RETURN(false);
  if (widget_) {
    return widget_->IsMaximized();
  }
  return false;
}

bool HoneycombWindowImpl::IsMinimized() {
  HONEYCOMB_REQUIRE_VALID_RETURN(false);
  if (widget_) {
    return widget_->IsMinimized();
  }
  return false;
}

bool HoneycombWindowImpl::IsFullscreen() {
  HONEYCOMB_REQUIRE_VALID_RETURN(false);
  if (widget_) {
    return widget_->IsFullscreen();
  }
  return false;
}

void HoneycombWindowImpl::SetTitle(const HoneycombString& title) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (root_view()) {
    root_view()->SetTitle(title);
  }
}

HoneycombString HoneycombWindowImpl::GetTitle() {
  HONEYCOMB_REQUIRE_VALID_RETURN(HoneycombString());
  if (root_view()) {
    return root_view()->title();
  }
  return HoneycombString();
}

void HoneycombWindowImpl::SetWindowIcon(HoneycombRefPtr<HoneycombImage> image) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (root_view()) {
    root_view()->SetWindowIcon(image);
  }
}

HoneycombRefPtr<HoneycombImage> HoneycombWindowImpl::GetWindowIcon() {
  HONEYCOMB_REQUIRE_VALID_RETURN(nullptr);
  if (root_view()) {
    return root_view()->window_icon();
  }
  return nullptr;
}

void HoneycombWindowImpl::SetWindowAppIcon(HoneycombRefPtr<HoneycombImage> image) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (root_view()) {
    root_view()->SetWindowAppIcon(image);
  }
}

HoneycombRefPtr<HoneycombImage> HoneycombWindowImpl::GetWindowAppIcon() {
  HONEYCOMB_REQUIRE_VALID_RETURN(nullptr);
  if (root_view()) {
    return root_view()->window_app_icon();
  }
  return nullptr;
}

HoneycombRefPtr<HoneycombOverlayController> HoneycombWindowImpl::AddOverlayView(
    HoneycombRefPtr<HoneycombView> view,
    honey_docking_mode_t docking_mode) {
  HONEYCOMB_REQUIRE_VALID_RETURN(nullptr);
  if (root_view()) {
    return root_view()->AddOverlayView(view, docking_mode);
  }
  return nullptr;
}

void HoneycombWindowImpl::GetDebugInfo(base::Value::Dict* info,
                                 bool include_children) {
  ParentClass::GetDebugInfo(info, include_children);
  if (root_view()) {
    info->Set("title", root_view()->title());
  }
}

void HoneycombWindowImpl::ShowMenu(HoneycombRefPtr<HoneycombMenuModel> menu_model,
                             const HoneycombPoint& screen_point,
                             honey_menu_anchor_position_t anchor_position) {
  ShowMenu(nullptr, menu_model, screen_point, anchor_position);
}

void HoneycombWindowImpl::Detach() {
  // OnDeleteDelegate should always be called before Detach().
  DCHECK(!widget_);

  ParentClass::Detach();
}

void HoneycombWindowImpl::SetBounds(const HoneycombRect& bounds) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (widget_) {
    widget_->SetBounds(
        gfx::Rect(bounds.x, bounds.y, bounds.width, bounds.height));
  }
}

HoneycombRect HoneycombWindowImpl::GetBounds() {
  HONEYCOMB_REQUIRE_VALID_RETURN(HoneycombRect());
  gfx::Rect bounds;
  if (widget_) {
    bounds = widget_->GetWindowBoundsInScreen();
  }
  return HoneycombRect(bounds.x(), bounds.y(), bounds.width(), bounds.height());
}

HoneycombRect HoneycombWindowImpl::GetBoundsInScreen() {
  return GetBounds();
}

void HoneycombWindowImpl::SetSize(const HoneycombSize& size) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (widget_) {
    widget_->SetSize(gfx::Size(size.width, size.height));
  }
}

void HoneycombWindowImpl::SetPosition(const HoneycombPoint& position) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (widget_) {
    gfx::Rect bounds = widget_->GetWindowBoundsInScreen();
    bounds.set_origin(gfx::Point(position.x, position.y));
    widget_->SetBounds(bounds);
  }
}

void HoneycombWindowImpl::SizeToPreferredSize() {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (widget_) {
    if (widget_->non_client_view()) {
      widget_->SetSize(widget_->non_client_view()->GetPreferredSize());
    } else {
      widget_->SetSize(root_view()->GetPreferredSize());
    }
  }
}

void HoneycombWindowImpl::SetVisible(bool visible) {
  if (visible) {
    Show();
  } else {
    Hide();
  }
}

bool HoneycombWindowImpl::IsVisible() {
  HONEYCOMB_REQUIRE_VALID_RETURN(false);
  if (widget_) {
    return widget_->IsVisible();
  }
  return false;
}

bool HoneycombWindowImpl::IsDrawn() {
  return IsVisible();
}

void HoneycombWindowImpl::SetBackgroundColor(honey_color_t color) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  ParentClass::SetBackgroundColor(color);
  if (widget_ && widget_->GetCompositor()) {
    widget_->GetCompositor()->SetBackgroundColor(color);
  }
}

bool HoneycombWindowImpl::CanWidgetClose() {
  if (shown_as_browser_modal_) {
    // Always allow the close for browser modal dialogs to avoid an infinite
    // loop in WebContentsModalDialogManager::CloseAllDialogs().
    return true;
  }
  if (delegate()) {
    return delegate()->CanClose(this);
  }
  return true;
}

void HoneycombWindowImpl::OnWindowClosing() {
#if defined(USE_AURA)
  unhandled_key_event_handler_.reset();
#endif

  if (delegate()) {
    delegate()->OnWindowClosing(this);
  }
}

void HoneycombWindowImpl::OnWindowViewDeleted() {
  CancelMenu();

  destroyed_ = true;
  widget_ = nullptr;

  if (delegate()) {
    delegate()->OnWindowDestroyed(this);
  }

  // Call Detach() here instead of waiting for the root View to be deleted so
  // that any following attempts to call HoneycombWindow methods from the delegate
  // will fail.
  Detach();
}

// Will only be called if CanHandleAccelerators() returns true.
bool HoneycombWindowImpl::AcceleratorPressed(const ui::Accelerator& accelerator) {
  for (const auto& entry : accelerator_map_) {
    if (entry.second == accelerator) {
      return delegate()->OnAccelerator(this, entry.first);
    }
  }
  return false;
}

bool HoneycombWindowImpl::CanHandleAccelerators() const {
  if (delegate() && widget_) {
    return widget_->IsActive();
  }
  return false;
}

bool HoneycombWindowImpl::OnKeyEvent(const HoneycombKeyEvent& event) {
  if (delegate()) {
    return delegate()->OnKeyEvent(this, event);
  }
  return false;
}

void HoneycombWindowImpl::ShowMenu(views::MenuButton* menu_button,
                             HoneycombRefPtr<HoneycombMenuModel> menu_model,
                             const HoneycombPoint& screen_point,
                             honey_menu_anchor_position_t anchor_position) {
  CancelMenu();

  if (!widget_) {
    return;
  }

  HoneycombMenuModelImpl* menu_model_impl =
      static_cast<HoneycombMenuModelImpl*>(menu_model.get());
  if (!menu_model_impl || !menu_model_impl->model()) {
    return;
  }

  menu_model_ = menu_model_impl;

  // We'll send the MenuClosed notification manually for better accuracy.
  menu_model_->set_auto_notify_menu_closed(false);

  menu_runner_.reset(new views::MenuRunner(
      menu_model_impl->model(),
      menu_button ? views::MenuRunner::HAS_MNEMONICS
                  : views::MenuRunner::CONTEXT_MENU,
      base::BindRepeating(&HoneycombWindowImpl::MenuClosed, this)));

  menu_runner_->RunMenuAt(
      widget_, menu_button ? menu_button->button_controller() : nullptr,
      gfx::Rect(gfx::Point(screen_point.x, screen_point.y), gfx::Size()),
      static_cast<views::MenuAnchorPosition>(anchor_position),
      ui::MENU_SOURCE_NONE);
}

void HoneycombWindowImpl::MenuClosed() {
  menu_model_->NotifyMenuClosed();
  menu_model_ = nullptr;
  menu_runner_.reset(nullptr);
}

void HoneycombWindowImpl::CancelMenu() {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (menu_runner_) {
    menu_runner_->Cancel();
  }
  DCHECK(!menu_model_);
  DCHECK(!menu_runner_);
}

HoneycombRefPtr<HoneycombDisplay> HoneycombWindowImpl::GetDisplay() {
  HONEYCOMB_REQUIRE_VALID_RETURN(nullptr);
  if (widget_ && root_view()) {
    const display::Display& display = root_view()->GetDisplay();
    if (display.is_valid()) {
      return new HoneycombDisplayImpl(display);
    }
  }
  return nullptr;
}

HoneycombRect HoneycombWindowImpl::GetClientAreaBoundsInScreen() {
  HONEYCOMB_REQUIRE_VALID_RETURN(HoneycombRect());
  if (widget_) {
    gfx::Rect bounds = widget_->GetClientAreaBoundsInScreen();

    views::NonClientFrameView* non_client_frame_view =
        root_view()->GetNonClientFrameView();
    if (non_client_frame_view) {
      // When using a custom drawn NonClientFrameView the native Window will not
      // know the actual client bounds. Adjust the native Window bounds for the
      // reported client bounds.
      const gfx::Rect& client_bounds =
          non_client_frame_view->GetBoundsForClientView();
      bounds.set_origin(bounds.origin() + client_bounds.OffsetFromOrigin());
      bounds.set_size(client_bounds.size());
    }

    return HoneycombRect(bounds.x(), bounds.y(), bounds.width(), bounds.height());
  }
  return HoneycombRect();
}

void HoneycombWindowImpl::SetDraggableRegions(
    const std::vector<HoneycombDraggableRegion>& regions) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (root_view()) {
    root_view()->SetDraggableRegions(regions);
  }
}

HoneycombWindowHandle HoneycombWindowImpl::GetWindowHandle() {
  HONEYCOMB_REQUIRE_VALID_RETURN(kNullWindowHandle);
  return view_util::GetWindowHandle(widget_);
}

void HoneycombWindowImpl::SendKeyPress(int key_code, uint32_t event_flags) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  InitializeUITesting();

  gfx::NativeWindow native_window = view_util::GetNativeWindow(widget_);
  if (!native_window) {
    return;
  }

  ui_controls::SendKeyPress(native_window,
                            static_cast<ui::KeyboardCode>(key_code),
                            !!(event_flags & EVENTFLAG_CONTROL_DOWN),
                            !!(event_flags & EVENTFLAG_SHIFT_DOWN),
                            !!(event_flags & EVENTFLAG_ALT_DOWN),
                            false);  // Command key is not supported by Aura.
}

void HoneycombWindowImpl::SendMouseMove(int screen_x, int screen_y) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  InitializeUITesting();

  // Converts to pixel coordinates internally on Windows.
  gfx::Point point(screen_x, screen_y);
  ui_controls::SendMouseMove(point.x(), point.y());
}

void HoneycombWindowImpl::SendMouseEvents(honey_mouse_button_type_t button,
                                    bool mouse_down,
                                    bool mouse_up) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (!mouse_down && !mouse_up) {
    return;
  }

  InitializeUITesting();

  ui_controls::MouseButton type = ui_controls::LEFT;
  if (button == MBT_MIDDLE) {
    type = ui_controls::MIDDLE;
  } else if (button == MBT_RIGHT) {
    type = ui_controls::RIGHT;
  }

  int state = 0;
  if (mouse_down) {
    state |= ui_controls::DOWN;
  }
  if (mouse_up) {
    state |= ui_controls::UP;
  }

  ui_controls::SendMouseEvents(type, state);
}

void HoneycombWindowImpl::SetAccelerator(int command_id,
                                   int key_code,
                                   bool shift_pressed,
                                   bool ctrl_pressed,
                                   bool alt_pressed) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (!widget_) {
    return;
  }

  AcceleratorMap::const_iterator it = accelerator_map_.find(command_id);
  if (it != accelerator_map_.end()) {
    RemoveAccelerator(command_id);
  }

  int modifiers = 0;
  if (shift_pressed) {
    modifiers |= ui::EF_SHIFT_DOWN;
  }
  if (ctrl_pressed) {
    modifiers |= ui::EF_CONTROL_DOWN;
  }
  if (alt_pressed) {
    modifiers |= ui::EF_ALT_DOWN;
  }
  ui::Accelerator accelerator(static_cast<ui::KeyboardCode>(key_code),
                              modifiers);

  accelerator_map_.insert(std::make_pair(command_id, accelerator));

  views::FocusManager* focus_manager = widget_->GetFocusManager();
  DCHECK(focus_manager);
  focus_manager->RegisterAccelerator(
      accelerator, ui::AcceleratorManager::kNormalPriority, this);
}

void HoneycombWindowImpl::RemoveAccelerator(int command_id) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (!widget_) {
    return;
  }

  AcceleratorMap::iterator it = accelerator_map_.find(command_id);
  if (it == accelerator_map_.end()) {
    return;
  }

  ui::Accelerator accelerator = it->second;

  accelerator_map_.erase(it);

  views::FocusManager* focus_manager = widget_->GetFocusManager();
  DCHECK(focus_manager);
  focus_manager->UnregisterAccelerator(accelerator, this);
}

void HoneycombWindowImpl::RemoveAllAccelerators() {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (!widget_) {
    return;
  }

  accelerator_map_.clear();

  views::FocusManager* focus_manager = widget_->GetFocusManager();
  DCHECK(focus_manager);
  focus_manager->UnregisterAccelerators(this);
}

HoneycombWindowImpl::HoneycombWindowImpl(HoneycombRefPtr<HoneycombWindowDelegate> delegate)
    : ParentClass(delegate) {}

HoneycombWindowView* HoneycombWindowImpl::CreateRootView() {
  return new HoneycombWindowView(delegate(), this);
}

void HoneycombWindowImpl::InitializeRootView() {
  static_cast<HoneycombWindowView*>(root_view())->Initialize();
}

void HoneycombWindowImpl::CreateWidget(gfx::AcceleratedWidget parent_widget) {
  DCHECK(!widget_);

  root_view()->CreateWidget(parent_widget);
  widget_ = root_view()->GetWidget();
  DCHECK(widget_);

#if defined(USE_AURA)
  unhandled_key_event_handler_ =
      std::make_unique<HoneycombUnhandledKeyEventHandler>(this, widget_);
#endif

  // The Widget and root View are owned by the native window. Therefore don't
  // keep an owned reference.
  std::unique_ptr<views::View> view_ptr = view_util::PassOwnership(this);
  [[maybe_unused]] views::View* view = view_ptr.release();

  initialized_ = true;

  if (delegate()) {
    delegate()->OnWindowCreated(this);
  }
}
