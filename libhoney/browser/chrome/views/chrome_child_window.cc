// Copyright 2022 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoney/browser/chrome/views/chrome_child_window.h"

#include "libhoney/browser/chrome/views/browser_platform_delegate_chrome_views.h"
#include "libhoney/browser/views/browser_view_impl.h"
#include "libhoney/browser/views/window_impl.h"

#if BUILDFLAG(IS_WIN)
#include "libhoney/browser/native/browser_platform_delegate_native_win.h"
#include "ui/views/win/hwnd_util.h"
#endif

namespace {

class ChildWindowDelegate : public HoneycombWindowDelegate {
 public:
  ChildWindowDelegate(const ChildWindowDelegate&) = delete;
  ChildWindowDelegate& operator=(const ChildWindowDelegate&) = delete;

  static void Create(HoneycombRefPtr<HoneycombBrowserView> browser_view,
                     const HoneycombWindowInfo& window_info,
                     gfx::AcceleratedWidget parent_handle) {
    DCHECK(parent_handle != gfx::kNullAcceleratedWidget);

    // Create the Window. It will show itself after creation.
    HoneycombWindowImpl::Create(new ChildWindowDelegate(browser_view, window_info),
                          parent_handle);
  }

  void OnWindowCreated(HoneycombRefPtr<HoneycombWindow> window) override {
    DCHECK(!window_);
    window_ = window;

    // Add the browser view. It will now have an associated Widget.
    window_->AddChildView(browser_view_);

    ShowWindow();
  }

  void OnWindowDestroyed(HoneycombRefPtr<HoneycombWindow> window) override {
    browser_view_ = nullptr;
    window_ = nullptr;
  }

  HoneycombRect GetInitialBounds(HoneycombRefPtr<HoneycombWindow> window) override {
    HoneycombRect initial_bounds(window_info_.bounds);
    if (initial_bounds.IsEmpty()) {
      return HoneycombRect(0, 0, 800, 600);
    }
    return initial_bounds;
  }

  void ShowWindow() {
#if BUILDFLAG(IS_WIN)
    auto widget = static_cast<HoneycombWindowImpl*>(window_.get())->widget();
    DCHECK(widget);
    const HWND widget_hwnd = HWNDForWidget(widget);
    DCHECK(widget_hwnd);

    // The native delegate needs state to perform some actions.
    auto browser =
        static_cast<HoneycombBrowserHostBase*>(browser_view_->GetBrowser().get());
    auto platform_delegate = browser->platform_delegate();
    DCHECK(platform_delegate->IsViewsHosted());
    auto chrome_delegate =
        static_cast<HoneycombBrowserPlatformDelegateChromeViews*>(platform_delegate);
    auto native_delegate = static_cast<HoneycombBrowserPlatformDelegateNativeWin*>(
        chrome_delegate->native_delegate());
    native_delegate->set_widget(widget, widget_hwnd);

    if (window_info_.ex_style & WS_EX_NOACTIVATE) {
      const DWORD widget_ex_styles = GetWindowLongPtr(widget_hwnd, GWL_EXSTYLE);

      // Add the WS_EX_NOACTIVATE style on the DesktopWindowTreeHostWin HWND
      // so that HWNDMessageHandler::Show() called via Widget::Show() does not
      // activate the window.
      SetWindowLongPtr(widget_hwnd, GWL_EXSTYLE,
                       widget_ex_styles | WS_EX_NOACTIVATE);

      window_->Show();

      // Remove the WS_EX_NOACTIVATE style so that future mouse clicks inside
      // the browser correctly activate and focus the window.
      SetWindowLongPtr(widget_hwnd, GWL_EXSTYLE, widget_ex_styles);
      return;
    }
#endif  // BUILDFLAG(IS_WIN)

    window_->Show();

    // Give keyboard focus to the browser view.
    browser_view_->RequestFocus();
  }

 private:
  ChildWindowDelegate(HoneycombRefPtr<HoneycombBrowserView> browser_view,
                      const HoneycombWindowInfo& window_info)
      : browser_view_(browser_view), window_info_(window_info) {}

  HoneycombRefPtr<HoneycombBrowserView> browser_view_;
  const HoneycombWindowInfo window_info_;

  HoneycombRefPtr<HoneycombWindow> window_;

  IMPLEMENT_REFCOUNTING(ChildWindowDelegate);
};

class ChildBrowserViewDelegate : public HoneycombBrowserViewDelegate {
 public:
  ChildBrowserViewDelegate() = default;

  ChildBrowserViewDelegate(const ChildBrowserViewDelegate&) = delete;
  ChildBrowserViewDelegate& operator=(const ChildBrowserViewDelegate&) = delete;

  HoneycombRefPtr<HoneycombBrowserViewDelegate> GetDelegateForPopupBrowserView(
      HoneycombRefPtr<HoneycombBrowserView> browser_view,
      const HoneycombBrowserSettings& settings,
      HoneycombRefPtr<HoneycombClient> client,
      bool is_devtools) override {
    return new ChildBrowserViewDelegate();
  }

  bool OnPopupBrowserViewCreated(HoneycombRefPtr<HoneycombBrowserView> browser_view,
                                 HoneycombRefPtr<HoneycombBrowserView> popup_browser_view,
                                 bool is_devtools) override {
    DCHECK(!is_devtools);

    auto new_browser = static_cast<HoneycombBrowserHostBase*>(
        popup_browser_view->GetBrowser().get());
    auto new_platform_delegate = new_browser->platform_delegate();
    DCHECK(new_platform_delegate->IsViewsHosted());
    auto new_platform_delegate_impl =
        static_cast<HoneycombBrowserPlatformDelegateChromeViews*>(
            new_platform_delegate);

    const auto& window_info =
        new_platform_delegate_impl->native_delegate()->window_info();
    const auto parent_handle =
        chrome_child_window::GetParentHandle(window_info);
    if (parent_handle != gfx::kNullAcceleratedWidget) {
      ChildWindowDelegate::Create(popup_browser_view, window_info,
                                  parent_handle);
      return true;
    }

    // Use the default implementation that creates a new Views-hosted top-level
    // window.
    return false;
  }

 private:
  IMPLEMENT_REFCOUNTING(ChildBrowserViewDelegate);
};

}  // namespace

namespace chrome_child_window {

bool HasParentHandle(const HoneycombWindowInfo& window_info) {
  return GetParentHandle(window_info) != gfx::kNullAcceleratedWidget;
}

gfx::AcceleratedWidget GetParentHandle(const HoneycombWindowInfo& window_info) {
#if !BUILDFLAG(IS_MAC)
  return window_info.parent_window;
#else
  return gfx::kNullAcceleratedWidget;
#endif
}

HoneycombRefPtr<HoneycombBrowserHostBase> MaybeCreateChildBrowser(
    const HoneycombBrowserCreateParams& create_params) {
  // If the BrowserView already exists it means that we're dealing with a popup
  // and we'll instead create the Window in OnPopupBrowserViewCreated.
  if (create_params.browser_view) {
    return nullptr;
  }

  if (!create_params.window_info) {
    return nullptr;
  }

  const auto parent_handle = GetParentHandle(*create_params.window_info);
  if (parent_handle == gfx::kNullAcceleratedWidget) {
    return nullptr;
  }

  // Create the BrowserView.
  auto browser_view = HoneycombBrowserViewImpl::Create(
      *create_params.window_info, create_params.client, create_params.url,
      create_params.settings, create_params.extra_info,
      create_params.request_context, new ChildBrowserViewDelegate());

  ChildWindowDelegate::Create(browser_view, *create_params.window_info,
                              parent_handle);

  return static_cast<HoneycombBrowserHostBase*>(browser_view->GetBrowser().get());
}

}  // namespace chrome_child_window
