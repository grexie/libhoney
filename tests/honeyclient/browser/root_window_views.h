// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_ROOT_WINDOW_VIEWS_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_ROOT_WINDOW_VIEWS_H_
#pragma once

#include <memory>
#include <string>

#include "tests/honeyclient/browser/client_handler.h"
#include "tests/honeyclient/browser/root_window.h"
#include "tests/honeyclient/browser/views_window.h"

namespace client {

// Views framework implementation of a top-level window in the browser process.
// The methods of this class must be called on the main thread unless otherwise
// indicated.
class RootWindowViews : public RootWindow,
                        public ClientHandler::Delegate,
                        public ViewsWindow::Delegate {
 public:
  // Constructor may be called on any thread.
  RootWindowViews();
  ~RootWindowViews();

  void SetTitlebarHeight(const std::optional<float>& height);

  // RootWindow methods:
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
  bool WithWindowlessRendering() const override { return false; }
  bool WithExtension() const override;
  void OnExtensionsChanged(const ExtensionSet& extensions) override;

  // ViewsWindow::Delegate methods:
  bool WithControls() override;
  bool InitiallyHidden() override;
  HoneycombRefPtr<HoneycombWindow> GetParentWindow() override;
  HoneycombRect GetInitialBounds() override;
  honey_show_state_t GetInitialShowState() override;
  scoped_refptr<ImageCache> GetImageCache() override;
  void OnViewsWindowCreated(HoneycombRefPtr<ViewsWindow> window) override;
  void OnViewsWindowClosing(HoneycombRefPtr<ViewsWindow> window) override;
  void OnViewsWindowDestroyed(HoneycombRefPtr<ViewsWindow> window) override;
  void OnViewsWindowActivated(HoneycombRefPtr<ViewsWindow> window) override;
  ViewsWindow::Delegate* GetDelegateForPopup(
      HoneycombRefPtr<HoneycombClient> client) override;
  void CreateExtensionWindow(HoneycombRefPtr<HoneycombExtension> extension,
                             const HoneycombRect& source_bounds,
                             HoneycombRefPtr<HoneycombWindow> parent_window,
                             base::OnceClosure close_callback) override;
  void OnTest(int test_id) override;
  void OnExit() override;

 protected:
  // ClientHandler::Delegate methods:
  void OnBrowserCreated(HoneycombRefPtr<HoneycombBrowser> browser) override;
  void OnBrowserClosing(HoneycombRefPtr<HoneycombBrowser> browser) override;
  void OnBrowserClosed(HoneycombRefPtr<HoneycombBrowser> browser) override;
  void OnSetAddress(const std::string& url) override;
  void OnSetTitle(const std::string& title) override;
  void OnSetFavicon(HoneycombRefPtr<HoneycombImage> image) override;
  void OnSetFullscreen(bool fullscreen) override;
  void OnAutoResize(const HoneycombSize& new_size) override;
  void OnSetLoadingState(bool isLoading,
                         bool canGoBack,
                         bool canGoForward) override;
  void OnSetDraggableRegions(
      const std::vector<HoneycombDraggableRegion>& regions) override;
  void OnTakeFocus(bool next) override;
  void OnBeforeContextMenu(HoneycombRefPtr<HoneycombMenuModel> model) override;

 private:
  void CreateClientHandler(const std::string& url);

  void InitOnUIThread(const HoneycombBrowserSettings& settings,
                      HoneycombRefPtr<HoneycombRequestContext> request_context);
  void CreateViewsWindow(const HoneycombBrowserSettings& settings,
                         HoneycombRefPtr<HoneycombRequestContext> request_context,
                         const ImageCache::ImageSet& images);

  void NotifyViewsWindowDestroyed();
  void NotifyViewsWindowActivated();
  void NotifyDestroyedIfDone();

  // Members set during initialization. Safe to access from any thread.
  std::unique_ptr<RootWindowConfig> config_;
  HoneycombRefPtr<ClientHandler> client_handler_;
  bool initialized_ = false;

  // Only accessed on the main thread.
  HoneycombRefPtr<HoneycombBrowser> browser_;
  bool window_destroyed_ = false;
  bool browser_destroyed_ = false;

  // Only accessed on the browser process UI thread.
  HoneycombRect initial_bounds_;
  honey_show_state_t initial_show_state_ = HONEYCOMB_SHOW_STATE_NORMAL;
  bool position_on_resize_ = false;
  HoneycombRefPtr<ViewsWindow> window_;
  ExtensionSet pending_extensions_;
  scoped_refptr<ImageCache> image_cache_;

  DISALLOW_COPY_AND_ASSIGN(RootWindowViews);
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_ROOT_WINDOW_VIEWS_H_
