// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoneycomb/browser/views/browser_platform_delegate_views.h"

#include <utility>

#include "include/views/honey_window.h"
#include "libhoneycomb/browser/alloy/alloy_browser_host_impl.h"
#include "libhoneycomb/browser/views/browser_view_impl.h"
#include "libhoneycomb/browser/views/menu_runner_views.h"

#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host.h"
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

HoneycombBrowserPlatformDelegateViews::HoneycombBrowserPlatformDelegateViews(
    std::unique_ptr<HoneycombBrowserPlatformDelegateNative> native_delegate,
    HoneycombRefPtr<HoneycombBrowserViewImpl> browser_view)
    : native_delegate_(std::move(native_delegate)) {
  if (browser_view) {
    SetBrowserView(browser_view);
  }
  native_delegate_->set_windowless_handler(this);
}

void HoneycombBrowserPlatformDelegateViews::SetBrowserView(
    HoneycombRefPtr<HoneycombBrowserViewImpl> browser_view) {
  DCHECK(!browser_view_);
  DCHECK(browser_view);
  browser_view_ = browser_view;
}

void HoneycombBrowserPlatformDelegateViews::WebContentsCreated(
    content::WebContents* web_contents,
    bool owned) {
  HoneycombBrowserPlatformDelegateAlloy::WebContentsCreated(web_contents, owned);
  native_delegate_->WebContentsCreated(web_contents, /*owned=*/false);
  browser_view_->WebContentsCreated(web_contents);
}

void HoneycombBrowserPlatformDelegateViews::WebContentsDestroyed(
    content::WebContents* web_contents) {
  HoneycombBrowserPlatformDelegateAlloy::WebContentsDestroyed(web_contents);
  native_delegate_->WebContentsDestroyed(web_contents);
}

void HoneycombBrowserPlatformDelegateViews::BrowserCreated(
    HoneycombBrowserHostBase* browser) {
  HoneycombBrowserPlatformDelegateAlloy::BrowserCreated(browser);

  native_delegate_->BrowserCreated(browser);
  browser_view_->BrowserCreated(browser, GetBoundsChangedCallback());
}

void HoneycombBrowserPlatformDelegateViews::NotifyBrowserCreated() {
  DCHECK(browser_view_);
  DCHECK(browser_);
  if (browser_view_->delegate()) {
    browser_view_->delegate()->OnBrowserCreated(browser_view_, browser_);
  }
}

void HoneycombBrowserPlatformDelegateViews::NotifyBrowserDestroyed() {
  DCHECK(browser_view_);
  DCHECK(browser_);
  if (browser_view_->delegate()) {
    browser_view_->delegate()->OnBrowserDestroyed(browser_view_, browser_);
  }
}

void HoneycombBrowserPlatformDelegateViews::BrowserDestroyed(
    HoneycombBrowserHostBase* browser) {
  HoneycombBrowserPlatformDelegateAlloy::BrowserDestroyed(browser);

  browser_view_->BrowserDestroyed(browser);
  browser_view_ = nullptr;
  native_delegate_->BrowserDestroyed(browser);
}

bool HoneycombBrowserPlatformDelegateViews::CreateHostWindow() {
  // Nothing to do here.
  return true;
}

void HoneycombBrowserPlatformDelegateViews::CloseHostWindow() {
  views::Widget* widget = GetWindowWidget();
  if (widget && !widget->IsClosed()) {
    widget->Close();
  }
}

HoneycombWindowHandle HoneycombBrowserPlatformDelegateViews::GetHostWindowHandle() const {
  return view_util::GetWindowHandle(GetWindowWidget());
}

views::Widget* HoneycombBrowserPlatformDelegateViews::GetWindowWidget() const {
  if (browser_view_->root_view()) {
    return browser_view_->root_view()->GetWidget();
  }
  return nullptr;
}

HoneycombRefPtr<HoneycombBrowserView> HoneycombBrowserPlatformDelegateViews::GetBrowserView()
    const {
  return browser_view_.get();
}

void HoneycombBrowserPlatformDelegateViews::PopupWebContentsCreated(
    const HoneycombBrowserSettings& settings,
    HoneycombRefPtr<HoneycombClient> client,
    content::WebContents* new_web_contents,
    HoneycombBrowserPlatformDelegate* new_platform_delegate,
    bool is_devtools) {
  DCHECK(new_platform_delegate->IsViewsHosted());
  HoneycombBrowserPlatformDelegateViews* new_platform_delegate_impl =
      static_cast<HoneycombBrowserPlatformDelegateViews*>(new_platform_delegate);

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

void HoneycombBrowserPlatformDelegateViews::PopupBrowserCreated(
    HoneycombBrowserHostBase* new_browser,
    bool is_devtools) {
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

SkColor HoneycombBrowserPlatformDelegateViews::GetBackgroundColor() const {
  return native_delegate_->GetBackgroundColor();
}

void HoneycombBrowserPlatformDelegateViews::WasResized() {
  native_delegate_->WasResized();
}

void HoneycombBrowserPlatformDelegateViews::SendKeyEvent(const HoneycombKeyEvent& event) {
  native_delegate_->SendKeyEvent(event);
}

void HoneycombBrowserPlatformDelegateViews::SendMouseClickEvent(
    const HoneycombMouseEvent& event,
    HoneycombBrowserHost::MouseButtonType type,
    bool mouseUp,
    int clickCount) {
  native_delegate_->SendMouseClickEvent(event, type, mouseUp, clickCount);
}

void HoneycombBrowserPlatformDelegateViews::SendMouseMoveEvent(
    const HoneycombMouseEvent& event,
    bool mouseLeave) {
  native_delegate_->SendMouseMoveEvent(event, mouseLeave);
}

void HoneycombBrowserPlatformDelegateViews::SendMouseWheelEvent(
    const HoneycombMouseEvent& event,
    int deltaX,
    int deltaY) {
  native_delegate_->SendMouseWheelEvent(event, deltaX, deltaY);
}

void HoneycombBrowserPlatformDelegateViews::SendTouchEvent(
    const HoneycombTouchEvent& event) {
  native_delegate_->SendTouchEvent(event);
}

void HoneycombBrowserPlatformDelegateViews::SetFocus(bool setFocus) {
  // Will activate the Widget and result in a call to WebContents::Focus().
  if (setFocus && browser_view_->root_view()) {
    if (auto widget = GetWindowWidget()) {
      // Don't activate a minimized Widget, or it will be shown.
      if (widget->IsMinimized()) {
        return;
      }
    }
    browser_view_->root_view()->RequestFocus();
  }
}

gfx::Point HoneycombBrowserPlatformDelegateViews::GetScreenPoint(
    const gfx::Point& view_pt,
    bool want_dip_coords) const {
  if (!browser_view_->root_view()) {
    return view_pt;
  }

  gfx::Point screen_point = view_pt;
  view_util::ConvertPointToScreen(browser_view_->root_view(), &screen_point,
                                  /*output_pixel_coords=*/!want_dip_coords);
  return screen_point;
}

void HoneycombBrowserPlatformDelegateViews::ViewText(const std::string& text) {
  native_delegate_->ViewText(text);
}

bool HoneycombBrowserPlatformDelegateViews::HandleKeyboardEvent(
    const content::NativeWebKeyboardEvent& event) {
  // The BrowserView will handle accelerators.
  return browser_view_->HandleKeyboardEvent(event);
}

HoneycombEventHandle HoneycombBrowserPlatformDelegateViews::GetEventHandle(
    const content::NativeWebKeyboardEvent& event) const {
  return native_delegate_->GetEventHandle(event);
}

std::unique_ptr<HoneycombMenuRunner>
HoneycombBrowserPlatformDelegateViews::CreateMenuRunner() {
  return base::WrapUnique(new HoneycombMenuRunnerViews(browser_view_.get()));
}

bool HoneycombBrowserPlatformDelegateViews::IsViewsHosted() const {
  return true;
}

gfx::Point HoneycombBrowserPlatformDelegateViews::GetDialogPosition(
    const gfx::Size& size) {
  const gfx::Rect& bounds = browser_view_->root_view()->GetBoundsInScreen();

  // Offset relative to the top-level content view.
  gfx::Point offset = bounds.origin();
  view_util::ConvertPointFromScreen(
      browser_view_->root_view()->GetWidget()->GetRootView(), &offset, false);

  return gfx::Point(offset.x() + (bounds.width() - size.width()) / 2,
                    offset.y() + (bounds.height() - size.height()) / 2);
}

gfx::Size HoneycombBrowserPlatformDelegateViews::GetMaximumDialogSize() {
  return browser_view_->root_view()->GetBoundsInScreen().size();
}

HoneycombWindowHandle HoneycombBrowserPlatformDelegateViews::GetParentWindowHandle() const {
  return GetHostWindowHandle();
}

gfx::Point HoneycombBrowserPlatformDelegateViews::GetParentScreenPoint(
    const gfx::Point& view,
    bool want_dip_coords) const {
  return GetScreenPoint(view, want_dip_coords);
}
