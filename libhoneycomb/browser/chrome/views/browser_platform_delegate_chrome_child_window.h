// Copyright 2022 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_CHROME_VIEWS_BROWSER_PLATFORM_DELEGATE_CHROME_CHILD_WINDOW_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_CHROME_VIEWS_BROWSER_PLATFORM_DELEGATE_CHROME_CHILD_WINDOW_H_

#include "libhoneycomb/browser/chrome/views/browser_platform_delegate_chrome_views.h"

// Implementation of Chrome-based browser functionality.
class HoneycombBrowserPlatformDelegateChromeChildWindow
    : public HoneycombBrowserPlatformDelegateChromeViews {
 public:
  HoneycombBrowserPlatformDelegateChromeChildWindow(
      std::unique_ptr<HoneycombBrowserPlatformDelegateNative> native_delegate,
      HoneycombRefPtr<HoneycombBrowserViewImpl> browser_view);

  // HoneycombBrowserPlatformDelegate overrides.
  void CloseHostWindow() override;
  void SetFocus(bool focus) override;

#if BUILDFLAG(IS_WIN) || (BUILDFLAG(IS_POSIX) && !BUILDFLAG(IS_MAC))
  void NotifyMoveOrResizeStarted() override;
#endif

  bool HasExternalParent() const override { return true; }
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_CHROME_VIEWS_BROWSER_PLATFORM_DELEGATE_CHROME_CHILD_WINDOW_H_
