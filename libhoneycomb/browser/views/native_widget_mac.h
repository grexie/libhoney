// Copyright 2023 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_NATIVE_WIDGET_MAC_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_NATIVE_WIDGET_MAC_H_
#pragma once

#include "include/internal/honey_ptr.h"

#include "ui/views/widget/native_widget_mac.h"

class BrowserView;
class HoneycombWindow;
class HoneycombWindowDelegate;

class HoneycombNativeWidgetMac : public views::NativeWidgetMac {
 public:
  HoneycombNativeWidgetMac(views::internal::NativeWidgetDelegate* delegate,
                     HoneycombRefPtr<HoneycombWindow> window,
                     HoneycombWindowDelegate* window_delegate);
  ~HoneycombNativeWidgetMac() override = default;

  HoneycombNativeWidgetMac(const HoneycombNativeWidgetMac&) = delete;
  HoneycombNativeWidgetMac& operator=(const HoneycombNativeWidgetMac&) = delete;

  void SetBrowserView(BrowserView* browser_view);

  // NativeWidgetMac:
  void ValidateUserInterfaceItem(
      int32_t command,
      remote_cocoa::mojom::ValidateUserInterfaceItemResult* result) override;
  bool WillExecuteCommand(int32_t command,
                          WindowOpenDisposition window_open_disposition,
                          bool is_before_first_responder) override;
  bool ExecuteCommand(int32_t command,
                      WindowOpenDisposition window_open_disposition,
                      bool is_before_first_responder) override;
  NativeWidgetMacNSWindow* CreateNSWindow(
      const remote_cocoa::mojom::CreateWindowParams* params) override;
  void GetWindowFrameTitlebarHeight(bool* override_titlebar_height,
                                    float* titlebar_height) override;
  void OnWindowFullscreenTransitionStart() override;
  void OnWindowFullscreenTransitionComplete() override;
  void OnWindowInitialized() override;

 private:
  const HoneycombRefPtr<HoneycombWindow> window_;
  HoneycombWindowDelegate* const window_delegate_;

  // Returns true if the HoneycombWindow is fully initialized.
  bool IsHoneycombWindowInitialized() const;

  BrowserView* browser_view_ = nullptr;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_NATIVE_WIDGET_MAC_H_
