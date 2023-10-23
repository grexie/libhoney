// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_ROOT_WINDOW_MAC_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_ROOT_WINDOW_MAC_H_
#pragma once

#include <memory>
#include <string>

#include "tests/honeyclient/browser/browser_window.h"
#include "tests/honeyclient/browser/root_window.h"

namespace client {

class RootWindowMacImpl;

// OS X implementation of a top-level native window in the browser process.
// The methods of this class must be called on the main thread unless otherwise
// indicated.
class RootWindowMac : public RootWindow, public BrowserWindow::Delegate {
 public:
  // Constructor may be called on any thread.
  RootWindowMac();
  ~RootWindowMac();

  BrowserWindow* browser_window() const;
  RootWindow::Delegate* delegate() const;

  // RootWindow methods.
  void Init(RootWindow::Delegate* delegate,
            std::unique_ptr<RootWindowConfig> config,
            const HoneycombBrowserSettings& settings) override;
  void InitAsPopup(RootWindow::Delegate* delegate,
                   bool with_controls,
                   bool with_osr,
                   const HoneycombPopupFeatures& popupFeatures,
                   HoneycombWindowInfo& windowInfo,
                   HoneycombRefPtr<HoneycombClient>& client,
                   HoneycombBrowserSettings& settings) override;
  void Show(ShowMode mode) override;
  void Hide() override;
  void SetBounds(int x, int y, size_t width, size_t height) override;
  void Close(bool force) override;
  void SetDeviceScaleFactor(float device_scale_factor) override;
  float GetDeviceScaleFactor() const override;
  HoneycombRefPtr<HoneycombBrowser> GetBrowser() const override;
  ClientWindowHandle GetWindowHandle() const override;
  bool WithWindowlessRendering() const override;
  bool WithExtension() const override;

  // BrowserWindow::Delegate methods.
  void OnBrowserCreated(HoneycombRefPtr<HoneycombBrowser> browser) override;
  void OnBrowserWindowDestroyed() override;
  void OnSetAddress(const std::string& url) override;
  void OnSetTitle(const std::string& title) override;
  void OnSetFullscreen(bool fullscreen) override;
  void OnAutoResize(const HoneycombSize& new_size) override;
  void OnSetLoadingState(bool isLoading,
                         bool canGoBack,
                         bool canGoForward) override;
  void OnSetDraggableRegions(
      const std::vector<HoneycombDraggableRegion>& regions) override;

  void OnNativeWindowClosed();

 private:
  HoneycombRefPtr<RootWindowMacImpl> impl_;

  DISALLOW_COPY_AND_ASSIGN(RootWindowMac);

  friend class RootWindowMacImpl;
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_ROOT_WINDOW_MAC_H_
