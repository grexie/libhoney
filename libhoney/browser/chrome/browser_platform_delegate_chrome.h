// Copyright 2020 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_CHROME_BROWSER_PLATFORM_DELEGATE_CHROME_H_
#define HONEYCOMB_LIBHONEY_BROWSER_CHROME_BROWSER_PLATFORM_DELEGATE_CHROME_H_

#include "libhoney/browser/browser_platform_delegate.h"
#include "libhoney/browser/native/browser_platform_delegate_native.h"

class Browser;

// Implementation of Chrome-based browser functionality.
class HoneycombBrowserPlatformDelegateChrome
    : public HoneycombBrowserPlatformDelegate,
      public HoneycombBrowserPlatformDelegateNative::WindowlessHandler {
 public:
  explicit HoneycombBrowserPlatformDelegateChrome(
      std::unique_ptr<HoneycombBrowserPlatformDelegateNative> native_delegate);

  // HoneycombBrowserPlatformDelegate overrides.
  void WebContentsCreated(content::WebContents* web_contents,
                          bool owned) override;
  void WebContentsDestroyed(content::WebContents* web_contents) override;
  void BrowserCreated(HoneycombBrowserHostBase* browser) override;
  void BrowserDestroyed(HoneycombBrowserHostBase* browser) override;
  HoneycombWindowHandle GetHostWindowHandle() const override;
  web_modal::WebContentsModalDialogHost* GetWebContentsModalDialogHost()
      const override;
  SkColor GetBackgroundColor() const override;
  void SendKeyEvent(const HoneycombKeyEvent& event) override;
  void SendMouseClickEvent(const HoneycombMouseEvent& event,
                           HoneycombBrowserHost::MouseButtonType type,
                           bool mouseUp,
                           int clickCount) override;
  void SendMouseMoveEvent(const HoneycombMouseEvent& event, bool mouseLeave) override;
  void SendMouseWheelEvent(const HoneycombMouseEvent& event,
                           int deltaX,
                           int deltaY) override;
  gfx::Point GetScreenPoint(const gfx::Point& view,
                            bool want_dip_coords) const override;
  void ViewText(const std::string& text) override;
  HoneycombEventHandle GetEventHandle(
      const content::NativeWebKeyboardEvent& event) const override;
  bool IsPrintPreviewSupported() const override;

  // HoneycombBrowserPlatformDelegateNative::WindowlessHandler methods:
  HoneycombWindowHandle GetParentWindowHandle() const override;
  gfx::Point GetParentScreenPoint(const gfx::Point& view,
                                  bool want_dip_coords) const override;

  void set_chrome_browser(Browser* browser);

  HoneycombBrowserPlatformDelegateNative* native_delegate() const {
    return native_delegate_.get();
  }

 protected:
  gfx::NativeWindow GetNativeWindow() const;

  std::unique_ptr<HoneycombBrowserPlatformDelegateNative> native_delegate_;

  Browser* chrome_browser_ = nullptr;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_CHROME_BROWSER_PLATFORM_DELEGATE_CHROME_H_
