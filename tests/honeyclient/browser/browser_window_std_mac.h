// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_BROWSER_WINDOW_STD_MAC_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_BROWSER_WINDOW_STD_MAC_H_
#pragma once

#include "tests/honeyclient/browser/browser_window.h"

namespace client {

// Represents a native child window hosting a single windowed browser instance.
// The methods of this class must be called on the main thread unless otherwise
// indicated.
class BrowserWindowStdMac : public BrowserWindow {
 public:
  // Constructor may be called on any thread.
  // |delegate| must outlive this object.
  BrowserWindowStdMac(Delegate* delegate,
                      bool with_controls,
                      const std::string& startup_url);

  // BrowserWindow methods.
  void CreateBrowser(ClientWindowHandle parent_handle,
                     const HoneycombRect& rect,
                     const HoneycombBrowserSettings& settings,
                     HoneycombRefPtr<HoneycombDictionaryValue> extra_info,
                     HoneycombRefPtr<HoneycombRequestContext> request_context) override;
  void GetPopupConfig(HoneycombWindowHandle temp_handle,
                      HoneycombWindowInfo& windowInfo,
                      HoneycombRefPtr<HoneycombClient>& client,
                      HoneycombBrowserSettings& settings) override;
  void ShowPopup(ClientWindowHandle parent_handle,
                 int x,
                 int y,
                 size_t width,
                 size_t height) override;
  void Show() override;
  void Hide() override;
  void SetBounds(int x, int y, size_t width, size_t height) override;
  void SetFocus(bool focus) override;
  ClientWindowHandle GetWindowHandle() const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(BrowserWindowStdMac);
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_BROWSER_WINDOW_STD_MAC_H_
