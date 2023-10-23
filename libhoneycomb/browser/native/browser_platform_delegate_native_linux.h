// Copyright 2015 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_NATIVE_BROWSER_PLATFORM_DELEGATE_NATIVE_LINUX_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_NATIVE_BROWSER_PLATFORM_DELEGATE_NATIVE_LINUX_H_

#include "libhoneycomb/browser/native/browser_platform_delegate_native_aura.h"

#include "ui/ozone/buildflags.h"

#if BUILDFLAG(OZONE_PLATFORM_X11)
class HoneycombWindowX11;
#endif

// Windowed browser implementation for Linux.
class HoneycombBrowserPlatformDelegateNativeLinux
    : public HoneycombBrowserPlatformDelegateNativeAura {
 public:
  HoneycombBrowserPlatformDelegateNativeLinux(const HoneycombWindowInfo& window_info,
                                        SkColor background_color);

  // HoneycombBrowserPlatformDelegate methods:
  void BrowserDestroyed(HoneycombBrowserHostBase* browser) override;
  bool CreateHostWindow() override;
  void CloseHostWindow() override;
  HoneycombWindowHandle GetHostWindowHandle() const override;
  views::Widget* GetWindowWidget() const override;
  void SetFocus(bool setFocus) override;
  void NotifyMoveOrResizeStarted() override;
  void SizeTo(int width, int height) override;
  void ViewText(const std::string& text) override;
  bool HandleKeyboardEvent(
      const content::NativeWebKeyboardEvent& event) override;
  HoneycombEventHandle GetEventHandle(
      const content::NativeWebKeyboardEvent& event) const override;

  // HoneycombBrowserPlatformDelegateNativeAura methods:
  ui::KeyEvent TranslateUiKeyEvent(const HoneycombKeyEvent& key_event) const override;
  content::NativeWebKeyboardEvent TranslateWebKeyEvent(
      const HoneycombKeyEvent& key_event) const override;

 private:
  // True if the host window has been created.
  bool host_window_created_ = false;

#if BUILDFLAG(OZONE_PLATFORM_X11)
  HoneycombWindowX11* window_x11_ = nullptr;
#endif
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_NATIVE_BROWSER_PLATFORM_DELEGATE_NATIVE_LINUX_H_
