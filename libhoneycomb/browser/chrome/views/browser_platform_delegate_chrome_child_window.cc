// Copyright 2022 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoneycomb/browser/chrome/views/browser_platform_delegate_chrome_child_window.h"

#include "include/views/honey_browser_view.h"
#include "libhoneycomb/browser/chrome/views/chrome_child_window.h"

HoneycombBrowserPlatformDelegateChromeChildWindow::
    HoneycombBrowserPlatformDelegateChromeChildWindow(
        std::unique_ptr<HoneycombBrowserPlatformDelegateNative> native_delegate,
        HoneycombRefPtr<HoneycombBrowserViewImpl> browser_view)
    : HoneycombBrowserPlatformDelegateChromeViews(std::move(native_delegate),
                                            browser_view) {}

void HoneycombBrowserPlatformDelegateChromeChildWindow::CloseHostWindow() {
  native_delegate_->CloseHostWindow();
}

void HoneycombBrowserPlatformDelegateChromeChildWindow::SetFocus(bool focus) {
  native_delegate_->SetFocus(focus);
}

#if BUILDFLAG(IS_WIN) || (BUILDFLAG(IS_POSIX) && !BUILDFLAG(IS_MAC))
void HoneycombBrowserPlatformDelegateChromeChildWindow::NotifyMoveOrResizeStarted() {
  native_delegate_->NotifyMoveOrResizeStarted();
}
#endif
