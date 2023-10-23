// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_VIEWS_BROWSER_PLATFORM_DELEGATE_VIEWS_H_
#define HONEYCOMB_LIBHONEY_BROWSER_VIEWS_BROWSER_PLATFORM_DELEGATE_VIEWS_H_

#include "libhoney/browser/alloy/browser_platform_delegate_alloy.h"
#include "libhoney/browser/native/browser_platform_delegate_native.h"
#include "libhoney/browser/views/browser_view_impl.h"

// Implementation of Views-based browser functionality.
class HoneycombBrowserPlatformDelegateViews
    : public HoneycombBrowserPlatformDelegateAlloy,
      public HoneycombBrowserPlatformDelegateNative::WindowlessHandler {
 public:
  // Platform-specific behaviors will be delegated to |native_delegate|.
  // |browser_view_getter| may be initially empty for popup browsers.
  HoneycombBrowserPlatformDelegateViews(
      std::unique_ptr<HoneycombBrowserPlatformDelegateNative> native_delegate,
      HoneycombRefPtr<HoneycombBrowserViewImpl> browser_view);

  // HoneycombBrowserPlatformDelegate methods:
  void WebContentsCreated(content::WebContents* web_contents,
                          bool owned) override;
  void WebContentsDestroyed(content::WebContents* web_contents) override;
  void BrowserCreated(HoneycombBrowserHostBase* browser) override;
  void NotifyBrowserCreated() override;
  void NotifyBrowserDestroyed() override;
  void BrowserDestroyed(HoneycombBrowserHostBase* browser) override;
  bool CreateHostWindow() override;
  void CloseHostWindow() override;
  HoneycombWindowHandle GetHostWindowHandle() const override;
  views::Widget* GetWindowWidget() const override;
  HoneycombRefPtr<HoneycombBrowserView> GetBrowserView() const override;
  void PopupWebContentsCreated(
      const HoneycombBrowserSettings& settings,
      HoneycombRefPtr<HoneycombClient> client,
      content::WebContents* new_web_contents,
      HoneycombBrowserPlatformDelegate* new_platform_delegate,
      bool is_devtools) override;
  void PopupBrowserCreated(HoneycombBrowserHostBase* new_browser,
                           bool is_devtools) override;
  SkColor GetBackgroundColor() const override;
  void WasResized() override;
  void SendKeyEvent(const HoneycombKeyEvent& event) override;
  void SendMouseClickEvent(const HoneycombMouseEvent& event,
                           HoneycombBrowserHost::MouseButtonType type,
                           bool mouseUp,
                           int clickCount) override;
  void SendMouseMoveEvent(const HoneycombMouseEvent& event, bool mouseLeave) override;
  void SendMouseWheelEvent(const HoneycombMouseEvent& event,
                           int deltaX,
                           int deltaY) override;
  void SendTouchEvent(const HoneycombTouchEvent& event) override;
  void SetFocus(bool setFocus) override;
  gfx::Point GetScreenPoint(const gfx::Point& view,
                            bool want_dip_coords) const override;
  void ViewText(const std::string& text) override;
  bool HandleKeyboardEvent(
      const content::NativeWebKeyboardEvent& event) override;
  HoneycombEventHandle GetEventHandle(
      const content::NativeWebKeyboardEvent& event) const override;
  std::unique_ptr<HoneycombMenuRunner> CreateMenuRunner() override;
  bool IsViewsHosted() const override;
  gfx::Point GetDialogPosition(const gfx::Size& size) override;
  gfx::Size GetMaximumDialogSize() override;

  // HoneycombBrowserPlatformDelegateNative::WindowlessHandler methods:
  HoneycombWindowHandle GetParentWindowHandle() const override;
  gfx::Point GetParentScreenPoint(const gfx::Point& view,
                                  bool want_dip_coords) const override;

 private:
  void SetBrowserView(HoneycombRefPtr<HoneycombBrowserViewImpl> browser_view);

  std::unique_ptr<HoneycombBrowserPlatformDelegateNative> native_delegate_;
  HoneycombRefPtr<HoneycombBrowserViewImpl> browser_view_;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_VIEWS_BROWSER_PLATFORM_DELEGATE_VIEWS_H_
