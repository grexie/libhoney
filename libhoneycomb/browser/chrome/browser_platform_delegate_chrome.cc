// Copyright 2020 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoneycomb/browser/chrome/browser_platform_delegate_chrome.h"

#include "libhoneycomb/browser/views/view_util.h"

#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_window.h"
#include "chrome/browser/ui/chrome_web_modal_dialog_manager_delegate.h"
#include "chrome/common/pref_names.h"
#include "ui/display/display.h"
#include "ui/display/screen.h"
#include "ui/gfx/geometry/point.h"

HoneycombBrowserPlatformDelegateChrome::HoneycombBrowserPlatformDelegateChrome(
    std::unique_ptr<HoneycombBrowserPlatformDelegateNative> native_delegate)
    : native_delegate_(std::move(native_delegate)) {
  native_delegate_->set_windowless_handler(this);
}

void HoneycombBrowserPlatformDelegateChrome::WebContentsCreated(
    content::WebContents* web_contents,
    bool owned) {
  HoneycombBrowserPlatformDelegate::WebContentsCreated(web_contents, owned);
  native_delegate_->WebContentsCreated(web_contents, /*owned=*/false);
}

void HoneycombBrowserPlatformDelegateChrome::WebContentsDestroyed(
    content::WebContents* web_contents) {
  HoneycombBrowserPlatformDelegate::WebContentsDestroyed(web_contents);
  native_delegate_->WebContentsDestroyed(web_contents);
}

void HoneycombBrowserPlatformDelegateChrome::BrowserCreated(
    HoneycombBrowserHostBase* browser) {
  HoneycombBrowserPlatformDelegate::BrowserCreated(browser);
  native_delegate_->BrowserCreated(browser);
}

void HoneycombBrowserPlatformDelegateChrome::BrowserDestroyed(
    HoneycombBrowserHostBase* browser) {
  HoneycombBrowserPlatformDelegate::BrowserDestroyed(browser);
  native_delegate_->BrowserDestroyed(browser);
}

HoneycombWindowHandle HoneycombBrowserPlatformDelegateChrome::GetHostWindowHandle() const {
  return view_util::GetWindowHandle(GetNativeWindow());
}

web_modal::WebContentsModalDialogHost*
HoneycombBrowserPlatformDelegateChrome::GetWebContentsModalDialogHost() const {
  if (chrome_browser_) {
    ChromeWebModalDialogManagerDelegate* manager = chrome_browser_;
    return manager->GetWebContentsModalDialogHost();
  }
  DCHECK(false);
  return nullptr;
}

SkColor HoneycombBrowserPlatformDelegateChrome::GetBackgroundColor() const {
  return native_delegate_->GetBackgroundColor();
}

void HoneycombBrowserPlatformDelegateChrome::SendKeyEvent(const HoneycombKeyEvent& event) {
  native_delegate_->SendKeyEvent(event);
}

void HoneycombBrowserPlatformDelegateChrome::SendMouseClickEvent(
    const HoneycombMouseEvent& event,
    HoneycombBrowserHost::MouseButtonType type,
    bool mouseUp,
    int clickCount) {
  native_delegate_->SendMouseClickEvent(event, type, mouseUp, clickCount);
}

void HoneycombBrowserPlatformDelegateChrome::SendMouseMoveEvent(
    const HoneycombMouseEvent& event,
    bool mouseLeave) {
  native_delegate_->SendMouseMoveEvent(event, mouseLeave);
}

void HoneycombBrowserPlatformDelegateChrome::SendMouseWheelEvent(
    const HoneycombMouseEvent& event,
    int deltaX,
    int deltaY) {
  native_delegate_->SendMouseWheelEvent(event, deltaX, deltaY);
}

gfx::Point HoneycombBrowserPlatformDelegateChrome::GetScreenPoint(
    const gfx::Point& view,
    bool want_dip_coords) const {
  auto screen = display::Screen::GetScreen();

  // Get device (pixel) coordinates.
  auto screen_rect = screen->DIPToScreenRectInWindow(
      GetNativeWindow(), gfx::Rect(view, gfx::Size(0, 0)));
  auto screen_point = screen_rect.origin();

  if (want_dip_coords) {
    // Convert to DIP coordinates.
    const auto& display = view_util::GetDisplayNearestPoint(
        screen_point, /*input_pixel_coords=*/true);
    view_util::ConvertPointFromPixels(&screen_point,
                                      display.device_scale_factor());
  }

  return screen_point;
}

void HoneycombBrowserPlatformDelegateChrome::ViewText(const std::string& text) {
  native_delegate_->ViewText(text);
}

HoneycombEventHandle HoneycombBrowserPlatformDelegateChrome::GetEventHandle(
    const content::NativeWebKeyboardEvent& event) const {
  return native_delegate_->GetEventHandle(event);
}

bool HoneycombBrowserPlatformDelegateChrome::IsPrintPreviewSupported() const {
  return chrome_browser_ && !chrome_browser_->profile()->GetPrefs()->GetBoolean(
                                prefs::kPrintPreviewDisabled);
}

HoneycombWindowHandle HoneycombBrowserPlatformDelegateChrome::GetParentWindowHandle()
    const {
  return GetHostWindowHandle();
}

gfx::Point HoneycombBrowserPlatformDelegateChrome::GetParentScreenPoint(
    const gfx::Point& view,
    bool want_dip_coords) const {
  return GetScreenPoint(view, want_dip_coords);
}

void HoneycombBrowserPlatformDelegateChrome::set_chrome_browser(Browser* browser) {
  chrome_browser_ = browser;
}

gfx::NativeWindow HoneycombBrowserPlatformDelegateChrome::GetNativeWindow() const {
  if (chrome_browser_ && chrome_browser_->window()) {
    return chrome_browser_->window()->GetNativeWindow();
  }
  DCHECK(false);
  return gfx::NativeWindow();
}
