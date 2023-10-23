// Copyright 2015 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_NATIVE_BROWSER_PLATFORM_DELEGATE_NATIVE_WIN_H_
#define HONEYCOMB_LIBHONEY_BROWSER_NATIVE_BROWSER_PLATFORM_DELEGATE_NATIVE_WIN_H_

#include <windows.h>

#include "libhoney/browser/native/browser_platform_delegate_native_aura.h"

// Windowed browser implementation for Windows.
class HoneycombBrowserPlatformDelegateNativeWin
    : public HoneycombBrowserPlatformDelegateNativeAura {
 public:
  HoneycombBrowserPlatformDelegateNativeWin(const HoneycombWindowInfo& window_info,
                                      SkColor background_color);

  // Called from chrome_child_window.cc after |widget| is created.
  void set_widget(views::Widget* widget, HoneycombWindowHandle widget_handle);

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
  gfx::Vector2d GetUiWheelEventOffset(int deltaX, int deltaY) const override;

 private:
  static void RegisterWindowClass();
  static LPCTSTR GetWndClass();
  static LRESULT CALLBACK WndProc(HWND hwnd,
                                  UINT message,
                                  WPARAM wParam,
                                  LPARAM lParam);

  // True if the host window has been created.
  bool host_window_created_ = false;

  bool has_frame_ = false;
  bool called_enable_non_client_dpi_scaling_ = false;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_NATIVE_BROWSER_PLATFORM_DELEGATE_NATIVE_WIN_H_
