// Copyright 2015 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_NATIVE_BROWSER_PLATFORM_DELEGATE_NATIVE_MAC_H_
#define HONEYCOMB_LIBHONEY_BROWSER_NATIVE_BROWSER_PLATFORM_DELEGATE_NATIVE_MAC_H_

#include "libhoney/browser/native/browser_platform_delegate_native.h"

#if defined(__OBJC__)
@class HoneycombWindowDelegate;
#else
class HoneycombWindowDelegate;
#endif

namespace content {
class RenderWidgetHostViewMac;
}

// Windowed browser implementation for Mac OS X.
class HoneycombBrowserPlatformDelegateNativeMac
    : public HoneycombBrowserPlatformDelegateNative {
 public:
  HoneycombBrowserPlatformDelegateNativeMac(const HoneycombWindowInfo& window_info,
                                      SkColor background_color);

  // HoneycombBrowserPlatformDelegate methods:
  void BrowserDestroyed(HoneycombBrowserHostBase* browser) override;
  bool CreateHostWindow() override;
  void CloseHostWindow() override;
  HoneycombWindowHandle GetHostWindowHandle() const override;
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
  std::unique_ptr<HoneycombJavaScriptDialogRunner> CreateJavaScriptDialogRunner()
      override;
  std::unique_ptr<HoneycombMenuRunner> CreateMenuRunner() override;

  // HoneycombBrowserPlatformDelegateNative methods:
  content::NativeWebKeyboardEvent TranslateWebKeyEvent(
      const HoneycombKeyEvent& key_event) const override;
  blink::WebMouseEvent TranslateWebClickEvent(
      const HoneycombMouseEvent& mouse_event,
      HoneycombBrowserHost::MouseButtonType type,
      bool mouseUp,
      int clickCount) const override;
  blink::WebMouseEvent TranslateWebMoveEvent(const HoneycombMouseEvent& mouse_event,
                                             bool mouseLeave) const override;
  blink::WebMouseWheelEvent TranslateWebWheelEvent(
      const HoneycombMouseEvent& mouse_event,
      int deltaX,
      int deltaY) const override;

 private:
  void TranslateWebMouseEvent(blink::WebMouseEvent& result,
                              const HoneycombMouseEvent& mouse_event) const;

  content::RenderWidgetHostViewMac* GetHostView() const;

  // True if the host window has been created.
  bool host_window_created_ = false;

  HoneycombWindowDelegate* __strong window_delegate_;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_NATIVE_BROWSER_PLATFORM_DELEGATE_NATIVE_MAC_H_
