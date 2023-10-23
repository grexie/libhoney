// Copyright 2021 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoneycomb/browser/chrome/views/chrome_browser_frame.h"

#include "chrome/browser/themes/theme_service.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_commands.h"
#include "chrome/browser/ui/views/frame/browser_view.h"

#if BUILDFLAG(IS_MAC)
#include "libhoneycomb/browser/views/native_widget_mac.h"
#include "libhoneycomb/browser/views/view_util.h"
#include "ui/views/widget/native_widget_private.h"
#endif

void ChromeBrowserFrame::Init(BrowserView* browser_view,
                              std::unique_ptr<Browser> browser) {
  DCHECK(browser_view);
  DCHECK(browser);

  DCHECK(!browser_view_);
  browser_view_ = browser_view;

  // Initialize BrowserFrame state.
  InitBrowserView(browser_view);

  // Initialize BrowserView state.
  browser_view->InitBrowser(std::move(browser));

#if BUILDFLAG(IS_MAC)
  // Initialize native window state.
  if (auto native_window = view_util::GetNativeWindow(this)) {
    if (auto* native_widget_private = views::internal::NativeWidgetPrivate::
            GetNativeWidgetForNativeWindow(native_window)) {
      auto* native_widget_mac =
          static_cast<HoneycombNativeWidgetMac*>(native_widget_private);
      native_widget_mac->SetBrowserView(browser_view);
      native_widget_mac->OnWindowInitialized();
    }
  }
#endif  // BUILDFLAG(IS_MAC)
}

void ChromeBrowserFrame::ToggleFullscreenMode() {
  chrome::ToggleFullscreenMode(browser_view_->browser());
}

views::internal::RootView* ChromeBrowserFrame::CreateRootView() {
  // Bypass the BrowserFrame implementation.
  return views::Widget::CreateRootView();
}

std::unique_ptr<views::NonClientFrameView>
ChromeBrowserFrame::CreateNonClientFrameView() {
  // Bypass the BrowserFrame implementation.
  return views::Widget::CreateNonClientFrameView();
}
