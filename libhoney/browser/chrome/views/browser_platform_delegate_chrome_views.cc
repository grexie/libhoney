// Copyright 2021 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoney/browser/chrome/views/browser_platform_delegate_chrome_views.h"

#include "include/views/honey_window.h"
#include "libhoney/browser/views/window_impl.h"

#include "chrome/browser/devtools/devtools_window.h"
#include "chrome/browser/ui/browser.h"
#include "components/zoom/zoom_controller.h"
#include "ui/views/widget/widget.h"

namespace {

// Default popup window delegate implementation.
class PopupWindowDelegate : public HoneycombWindowDelegate {
 public:
  explicit PopupWindowDelegate(HoneycombRefPtr<HoneycombBrowserView> browser_view)
      : browser_view_(browser_view) {}

  PopupWindowDelegate(const PopupWindowDelegate&) = delete;
  PopupWindowDelegate& operator=(const PopupWindowDelegate&) = delete;

  void OnWindowCreated(HoneycombRefPtr<HoneycombWindow> window) override {
    window->AddChildView(browser_view_);
    window->Show();
    browser_view_->RequestFocus();
  }

  void OnWindowDestroyed(HoneycombRefPtr<HoneycombWindow> window) override {
    browser_view_ = nullptr;
  }

  bool CanClose(HoneycombRefPtr<HoneycombWindow> window) override {
    HoneycombRefPtr<HoneycombBrowser> browser = browser_view_->GetBrowser();
    if (browser) {
      return browser->GetHost()->TryCloseBrowser();
    }
    return true;
  }

 private:
  HoneycombRefPtr<HoneycombBrowserView> browser_view_;

  IMPLEMENT_REFCOUNTING(PopupWindowDelegate);
};

}  // namespace

HoneycombBrowserPlatformDelegateChromeViews::HoneycombBrowserPlatformDelegateChromeViews(
    std::unique_ptr<HoneycombBrowserPlatformDelegateNative> native_delegate,
    HoneycombRefPtr<HoneycombBrowserViewImpl> browser_view)
    : HoneycombBrowserPlatformDelegateChrome(std::move(native_delegate)) {
  if (browser_view) {
    SetBrowserView(browser_view);
  }
}

void HoneycombBrowserPlatformDelegateChromeViews::SetBrowserView(
    HoneycombRefPtr<HoneycombBrowserViewImpl> browser_view) {
  DCHECK(!browser_view_);
  DCHECK(browser_view);
  browser_view_ = browser_view;
}

void HoneycombBrowserPlatformDelegateChromeViews::WebContentsCreated(
    content::WebContents* web_contents,
    bool owned) {
  HoneycombBrowserPlatformDelegateChrome::WebContentsCreated(web_contents, owned);
  browser_view_->WebContentsCreated(web_contents);
}

void HoneycombBrowserPlatformDelegateChromeViews::BrowserCreated(
    HoneycombBrowserHostBase* browser) {
  HoneycombBrowserPlatformDelegateChrome::BrowserCreated(browser);
  browser_view_->BrowserCreated(browser, base::RepeatingClosure());
}

void HoneycombBrowserPlatformDelegateChromeViews::NotifyBrowserCreated() {
  if (auto delegate = browser_view_->delegate()) {
    delegate->OnBrowserCreated(browser_view_, browser_);

    // DevTools windows hide the notification bubble by default. However, we
    // don't currently have the ability to intercept WebContents creation via
    // DevToolsWindow::Create(), so |show_by_default| will always be true here.
    const bool show_by_default =
        !DevToolsWindow::IsDevToolsWindow(web_contents_);

    bool show_zoom_bubble = show_by_default;
    const auto& state = browser_->settings().chrome_zoom_bubble;
    if (show_by_default && state == STATE_DISABLED) {
      show_zoom_bubble = false;
    } else if (!show_by_default && state == STATE_ENABLED) {
      show_zoom_bubble = true;
    }

    if (show_zoom_bubble != show_by_default) {
      // We may be called before TabHelpers::AttachTabHelpers(), so create
      // the ZoomController if necessary.
      zoom::ZoomController::CreateForWebContents(web_contents_);
      zoom::ZoomController::FromWebContents(web_contents_)
          ->SetShowsNotificationBubble(show_zoom_bubble);
    }
  }
}

void HoneycombBrowserPlatformDelegateChromeViews::NotifyBrowserDestroyed() {
  if (browser_view_->delegate()) {
    browser_view_->delegate()->OnBrowserDestroyed(browser_view_, browser_);
  }
}

void HoneycombBrowserPlatformDelegateChromeViews::BrowserDestroyed(
    HoneycombBrowserHostBase* browser) {
  HoneycombBrowserPlatformDelegateChrome::BrowserDestroyed(browser);
  browser_view_->BrowserDestroyed(browser);
}

void HoneycombBrowserPlatformDelegateChromeViews::CloseHostWindow() {
  views::Widget* widget = GetWindowWidget();
  if (widget && !widget->IsClosed()) {
    widget->Close();
  }
}

HoneycombWindowHandle HoneycombBrowserPlatformDelegateChromeViews::GetHostWindowHandle()
    const {
  return view_util::GetWindowHandle(GetWindowWidget());
}

views::Widget* HoneycombBrowserPlatformDelegateChromeViews::GetWindowWidget() const {
  if (browser_view_->root_view()) {
    return browser_view_->root_view()->GetWidget();
  }
  return nullptr;
}

HoneycombRefPtr<HoneycombBrowserView>
HoneycombBrowserPlatformDelegateChromeViews::GetBrowserView() const {
  return browser_view_.get();
}

void HoneycombBrowserPlatformDelegateChromeViews::PopupWebContentsCreated(
    const HoneycombBrowserSettings& settings,
    HoneycombRefPtr<HoneycombClient> client,
    content::WebContents* new_web_contents,
    HoneycombBrowserPlatformDelegate* new_platform_delegate,
    bool is_devtools) {
  // Default popup handling may not be Views-hosted.
  if (!new_platform_delegate->IsViewsHosted()) {
    return;
  }

  auto* new_platform_delegate_impl =
      static_cast<HoneycombBrowserPlatformDelegateChromeViews*>(
          new_platform_delegate);

  HoneycombRefPtr<HoneycombBrowserViewDelegate> new_delegate;
  if (browser_view_->delegate()) {
    new_delegate = browser_view_->delegate()->GetDelegateForPopupBrowserView(
        browser_view_.get(), settings, client, is_devtools);
  }

  // Create a new BrowserView for the popup.
  HoneycombRefPtr<HoneycombBrowserViewImpl> new_browser_view =
      HoneycombBrowserViewImpl::CreateForPopup(settings, new_delegate);

  // Associate the PlatformDelegate with the new BrowserView.
  new_platform_delegate_impl->SetBrowserView(new_browser_view);
}

void HoneycombBrowserPlatformDelegateChromeViews::PopupBrowserCreated(
    HoneycombBrowserHostBase* new_browser,
    bool is_devtools) {
  // Default popup handling may not be Views-hosted.
  if (!new_browser->HasView()) {
    return;
  }

  HoneycombRefPtr<HoneycombBrowserView> new_browser_view =
      HoneycombBrowserView::GetForBrowser(new_browser);
  DCHECK(new_browser_view);

  bool popup_handled = false;
  if (browser_view_->delegate()) {
    popup_handled = browser_view_->delegate()->OnPopupBrowserViewCreated(
        browser_view_.get(), new_browser_view.get(), is_devtools);
  }

  if (!popup_handled) {
    HoneycombWindow::CreateTopLevelWindow(
        new PopupWindowDelegate(new_browser_view.get()));
  }
}

void HoneycombBrowserPlatformDelegateChromeViews::UpdateFindBarBoundingBox(
    gfx::Rect* bounds) const {
  if (auto* window_impl = GetWindowImpl()) {
    if (window_impl->root_view()) {
      window_impl->root_view()->UpdateFindBarBoundingBox(bounds);
    }
  }
}

bool HoneycombBrowserPlatformDelegateChromeViews::IsViewsHosted() const {
  return true;
}

HoneycombWindowImpl* HoneycombBrowserPlatformDelegateChromeViews::GetWindowImpl() const {
  if (auto* widget = GetWindowWidget()) {
    HoneycombRefPtr<HoneycombWindow> window = view_util::GetWindowFor(widget);
    return static_cast<HoneycombWindowImpl*>(window.get());
  }
  return nullptr;
}
