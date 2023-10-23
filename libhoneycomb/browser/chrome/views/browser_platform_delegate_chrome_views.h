// Copyright 2021 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_CHROME_VIEWS_BROWSER_PLATFORM_DELEGATE_CHROME_VIEWS_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_CHROME_VIEWS_BROWSER_PLATFORM_DELEGATE_CHROME_VIEWS_H_

#include "libhoneycomb/browser/chrome/browser_platform_delegate_chrome.h"
#include "libhoneycomb/browser/views/browser_view_impl.h"

class HoneycombWindowImpl;

// Implementation of Chrome-based browser functionality.
class HoneycombBrowserPlatformDelegateChromeViews
    : public HoneycombBrowserPlatformDelegateChrome {
 public:
  explicit HoneycombBrowserPlatformDelegateChromeViews(
      std::unique_ptr<HoneycombBrowserPlatformDelegateNative> native_delegate,
      HoneycombRefPtr<HoneycombBrowserViewImpl> browser_view);

  // HoneycombBrowserPlatformDelegate overrides.
  void WebContentsCreated(content::WebContents* web_contents,
                          bool owned) override;
  void BrowserCreated(HoneycombBrowserHostBase* browser) override;
  void NotifyBrowserCreated() override;
  void NotifyBrowserDestroyed() override;
  void BrowserDestroyed(HoneycombBrowserHostBase* browser) override;
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
  void UpdateFindBarBoundingBox(gfx::Rect* bounds) const override;
  bool IsViewsHosted() const override;

  HoneycombRefPtr<HoneycombBrowserViewImpl> browser_view() const { return browser_view_; }

 private:
  void SetBrowserView(HoneycombRefPtr<HoneycombBrowserViewImpl> browser_view);

  HoneycombWindowImpl* GetWindowImpl() const;

  HoneycombRefPtr<HoneycombBrowserViewImpl> browser_view_;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_CHROME_VIEWS_BROWSER_PLATFORM_DELEGATE_CHROME_VIEWS_H_
