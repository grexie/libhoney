// Copyright 2017 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_BROWSER_PLATFORM_DELEGATE_BACKGROUND_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_BROWSER_PLATFORM_DELEGATE_BACKGROUND_H_

#include "libhoneycomb/browser/alloy/browser_platform_delegate_alloy.h"
#include "libhoneycomb/browser/native/browser_platform_delegate_native.h"

// Implementation of browser functionality for background script hosts.
class HoneycombBrowserPlatformDelegateBackground
    : public HoneycombBrowserPlatformDelegateAlloy,
      public HoneycombBrowserPlatformDelegateNative::WindowlessHandler {
 public:
  // Platform-specific behaviors will be delegated to |native_delegate|.
  HoneycombBrowserPlatformDelegateBackground(
      std::unique_ptr<HoneycombBrowserPlatformDelegateNative> native_delegate);

  // HoneycombBrowserPlatformDelegate methods:
  bool CreateHostWindow() override;
  void CloseHostWindow() override;
  HoneycombWindowHandle GetHostWindowHandle() const override;
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

  // HoneycombBrowserPlatformDelegateNative::WindowlessHandler methods:
  HoneycombWindowHandle GetParentWindowHandle() const override;
  gfx::Point GetParentScreenPoint(const gfx::Point& view,
                                  bool want_dip_coords) const override;

 private:
  std::unique_ptr<HoneycombBrowserPlatformDelegateNative> native_delegate_;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_BROWSER_PLATFORM_DELEGATE_BACKGROUND_H_
