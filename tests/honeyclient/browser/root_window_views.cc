// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeyclient/browser/root_window_views.h"

#include <memory>

#include "include/base/honey_build.h"
#include "include/base/honey_callback.h"
#include "include/honey_app.h"
#include "include/wrapper/honey_helpers.h"
#include "tests/honeyclient/browser/client_handler_std.h"
#include "tests/honeyclient/browser/client_prefs.h"

namespace client {

namespace {

// Images that are loaded early and cached.
static const char* kDefaultImageCache[] = {"menu_icon", "window_icon"};

}  // namespace

RootWindowViews::RootWindowViews() = default;

RootWindowViews::~RootWindowViews() {
  REQUIRE_MAIN_THREAD();
}

void RootWindowViews::SetTitlebarHeight(const std::optional<float>& height) {
  if (window_) {
    window_->SetTitlebarHeight(height);
  }
}

void RootWindowViews::Init(RootWindow::Delegate* delegate,
                           std::unique_ptr<RootWindowConfig> config,
                           const HoneycombBrowserSettings& settings) {
  DCHECK(delegate);
  DCHECK(!config->with_osr);  // Windowless rendering is not supported.
  DCHECK(!initialized_);

  delegate_ = delegate;
  config_ = std::move(config);

  CreateClientHandler(config_->url);
  initialized_ = true;

  // Continue initialization on the UI thread.
  InitOnUIThread(settings, delegate_->GetRequestContext(this));
}

void RootWindowViews::InitAsPopup(RootWindow::Delegate* delegate,
                                  bool with_controls,
                                  bool with_osr,
                                  const HoneycombPopupFeatures& popupFeatures,
                                  HoneycombWindowInfo& windowInfo,
                                  HoneycombRefPtr<HoneycombClient>& client,
                                  HoneycombBrowserSettings& settings) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  DCHECK(delegate);
  DCHECK(!with_osr);  // Windowless rendering is not supported.
  DCHECK(!initialized_);

  delegate_ = delegate;
  config_ = std::make_unique<RootWindowConfig>();
  config_->with_controls = with_controls;

  if (popupFeatures.xSet) {
    initial_bounds_.x = popupFeatures.x;
  }
  if (popupFeatures.ySet) {
    initial_bounds_.y = popupFeatures.y;
  }
  if (popupFeatures.widthSet) {
    initial_bounds_.width = popupFeatures.width;
  }
  if (popupFeatures.heightSet) {
    initial_bounds_.height = popupFeatures.height;
  }

  CreateClientHandler(std::string());
  initialized_ = true;

  // The Window will be created in ViewsWindow::OnPopupBrowserViewCreated().
  client = client_handler_;
}

void RootWindowViews::Show(ShowMode mode) {
  if (!HoneycombCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
    HoneycombPostTask(TID_UI, base::BindOnce(&RootWindowViews::Show, this, mode));
    return;
  }

  if (!window_) {
    return;
  }

  window_->Show();

  switch (mode) {
    case ShowMinimized:
      window_->Minimize();
      break;
    case ShowMaximized:
      window_->Maximize();
      break;
    default:
      break;
  }
}

void RootWindowViews::Hide() {
  if (!HoneycombCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
    HoneycombPostTask(TID_UI, base::BindOnce(&RootWindowViews::Hide, this));
    return;
  }

  if (window_) {
    window_->Hide();
  }
}

void RootWindowViews::SetBounds(int x, int y, size_t width, size_t height) {
  if (!HoneycombCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
    HoneycombPostTask(TID_UI, base::BindOnce(&RootWindowViews::SetBounds, this, x, y,
                                       width, height));
    return;
  }

  if (window_) {
    window_->SetBounds(
        HoneycombRect(x, y, static_cast<int>(width), static_cast<int>(height)));
  }
}

void RootWindowViews::Close(bool force) {
  if (!HoneycombCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
    HoneycombPostTask(TID_UI, base::BindOnce(&RootWindowViews::Close, this, force));
    return;
  }

  if (window_) {
    window_->Close(force);
  }
}

void RootWindowViews::SetDeviceScaleFactor(float device_scale_factor) {
  REQUIRE_MAIN_THREAD();
  // Windowless rendering is not supported.
  NOTREACHED();
}

float RootWindowViews::GetDeviceScaleFactor() const {
  REQUIRE_MAIN_THREAD();
  // Windowless rendering is not supported.
  NOTREACHED();
  return 0.0;
}

HoneycombRefPtr<HoneycombBrowser> RootWindowViews::GetBrowser() const {
  REQUIRE_MAIN_THREAD();
  return browser_;
}

ClientWindowHandle RootWindowViews::GetWindowHandle() const {
  REQUIRE_MAIN_THREAD();
#if defined(OS_LINUX)
  // ClientWindowHandle is a GtkWidget* on Linux and we don't have one of those.
  return nullptr;
#else
  if (browser_) {
    return browser_->GetHost()->GetWindowHandle();
  }
  return kNullWindowHandle;
#endif
}

bool RootWindowViews::WithExtension() const {
  DCHECK(initialized_);
  return config_->window_type == WindowType::EXTENSION;
}

bool RootWindowViews::WithControls() {
  DCHECK(initialized_);
  return config_->with_controls;
}

void RootWindowViews::OnExtensionsChanged(const ExtensionSet& extensions) {
  if (!HoneycombCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
    HoneycombPostTask(TID_UI, base::BindOnce(&RootWindowViews::OnExtensionsChanged,
                                       this, extensions));
    return;
  }

  if (window_) {
    window_->OnExtensionsChanged(extensions);
  } else {
    // Window may not exist yet for popups.
    pending_extensions_ = extensions;
  }
}

bool RootWindowViews::InitiallyHidden() {
  HONEYCOMB_REQUIRE_UI_THREAD();
  return config_->initially_hidden;
}

HoneycombRefPtr<HoneycombWindow> RootWindowViews::GetParentWindow() {
  HONEYCOMB_REQUIRE_UI_THREAD();
  return config_->parent_window;
}

HoneycombRect RootWindowViews::GetInitialBounds() {
  HONEYCOMB_REQUIRE_UI_THREAD();
  return initial_bounds_;
}

honey_show_state_t RootWindowViews::GetInitialShowState() {
  HONEYCOMB_REQUIRE_UI_THREAD();
  return initial_show_state_;
}

scoped_refptr<ImageCache> RootWindowViews::GetImageCache() {
  HONEYCOMB_REQUIRE_UI_THREAD();
  return image_cache_;
}

void RootWindowViews::OnViewsWindowCreated(HoneycombRefPtr<ViewsWindow> window) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  DCHECK(!window_);
  window_ = window;
  window_->SetAlwaysOnTop(config_->always_on_top);

  if (!pending_extensions_.empty()) {
    window_->OnExtensionsChanged(pending_extensions_);
    pending_extensions_.clear();
  }
}

void RootWindowViews::OnViewsWindowClosing(HoneycombRefPtr<ViewsWindow> window) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  DCHECK(window_);

  if (config_->window_type != WindowType::NORMAL) {
    return;
  }

  honey_show_state_t show_state;
  std::optional<HoneycombRect> dip_bounds;
  if (window_->GetWindowRestorePreferences(show_state, dip_bounds)) {
    prefs::SaveWindowRestorePreferences(show_state, dip_bounds);
  }
}

void RootWindowViews::OnViewsWindowDestroyed(HoneycombRefPtr<ViewsWindow> window) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  window_ = nullptr;

  // Continue on the main thread.
  MAIN_POST_CLOSURE(
      base::BindOnce(&RootWindowViews::NotifyViewsWindowDestroyed, this));
}

void RootWindowViews::OnViewsWindowActivated(HoneycombRefPtr<ViewsWindow> window) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  // Continue on the main thread.
  MAIN_POST_CLOSURE(
      base::BindOnce(&RootWindowViews::NotifyViewsWindowActivated, this));
}

ViewsWindow::Delegate* RootWindowViews::GetDelegateForPopup(
    HoneycombRefPtr<HoneycombClient> client) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  // |handler| was created in RootWindowViews::InitAsPopup().
  ClientHandlerStd* handler = static_cast<ClientHandlerStd*>(client.get());
  RootWindowViews* root_window =
      static_cast<RootWindowViews*>(handler->delegate());

  // May be nullptr when using the default popup behavior.
  if (root_window) {
    // Transfer some state to the child RootWindowViews.
    root_window->image_cache_ = image_cache_;
  }

  return root_window;
}

void RootWindowViews::CreateExtensionWindow(HoneycombRefPtr<HoneycombExtension> extension,
                                            const HoneycombRect& source_bounds,
                                            HoneycombRefPtr<HoneycombWindow> parent_window,
                                            base::OnceClosure close_callback) {
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
    MAIN_POST_CLOSURE(base::BindOnce(&RootWindowViews::CreateExtensionWindow,
                                     this, extension, source_bounds,
                                     parent_window, std::move(close_callback)));
    return;
  }

  delegate_->CreateExtensionWindow(extension, source_bounds, parent_window,
                                   std::move(close_callback), false);
}

void RootWindowViews::OnTest(int test_id) {
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
    MAIN_POST_CLOSURE(base::BindOnce(&RootWindowViews::OnTest, this, test_id));
    return;
  }

  delegate_->OnTest(this, test_id);
}

void RootWindowViews::OnExit() {
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
    MAIN_POST_CLOSURE(base::BindOnce(&RootWindowViews::OnExit, this));
    return;
  }

  delegate_->OnExit(this);
}

void RootWindowViews::OnBrowserCreated(HoneycombRefPtr<HoneycombBrowser> browser) {
  REQUIRE_MAIN_THREAD();
  DCHECK(!browser_);
  browser_ = browser;
  delegate_->OnBrowserCreated(this, browser);
}

void RootWindowViews::OnBrowserClosing(HoneycombRefPtr<HoneycombBrowser> browser) {
  REQUIRE_MAIN_THREAD();
  // Nothing to do here.
}

void RootWindowViews::OnBrowserClosed(HoneycombRefPtr<HoneycombBrowser> browser) {
  REQUIRE_MAIN_THREAD();
  if (browser_) {
    DCHECK_EQ(browser->GetIdentifier(), browser_->GetIdentifier());
    browser_ = nullptr;
  }

  client_handler_->DetachDelegate();
  client_handler_ = nullptr;

  browser_destroyed_ = true;
  NotifyDestroyedIfDone();
}

void RootWindowViews::OnSetAddress(const std::string& url) {
  if (!HoneycombCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
    HoneycombPostTask(TID_UI,
                base::BindOnce(&RootWindowViews::OnSetAddress, this, url));
    return;
  }

  if (window_) {
    window_->SetAddress(url);
  }
}

void RootWindowViews::OnSetTitle(const std::string& title) {
  if (!HoneycombCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
    HoneycombPostTask(TID_UI,
                base::BindOnce(&RootWindowViews::OnSetTitle, this, title));
    return;
  }

  if (window_) {
    window_->SetTitle(title);
  }
}

void RootWindowViews::OnSetFavicon(HoneycombRefPtr<HoneycombImage> image) {
  if (!HoneycombCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
    HoneycombPostTask(TID_UI,
                base::BindOnce(&RootWindowViews::OnSetFavicon, this, image));
    return;
  }

  if (window_) {
    window_->SetFavicon(image);
  }
}

void RootWindowViews::OnSetFullscreen(bool fullscreen) {
  if (!HoneycombCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
    HoneycombPostTask(TID_UI, base::BindOnce(&RootWindowViews::OnSetFullscreen, this,
                                       fullscreen));
    return;
  }

  if (window_) {
    window_->SetFullscreen(fullscreen);
  }
}

void RootWindowViews::OnAutoResize(const HoneycombSize& new_size) {
  if (!HoneycombCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
    HoneycombPostTask(TID_UI,
                base::BindOnce(&RootWindowViews::OnAutoResize, this, new_size));
    return;
  }

  bool has_position = false;
  HoneycombPoint position;
  if (position_on_resize_) {
    // Position the window centered on and immediately below the source.
    const int x_offset = (initial_bounds_.width - new_size.width) / 2;
    position.Set(initial_bounds_.x + x_offset,
                 initial_bounds_.y + initial_bounds_.height);
    has_position = true;

    // Don't change the window position on future resizes.
    position_on_resize_ = false;
  }

  if (window_) {
    window_->SetBrowserSize(new_size, has_position, position);
    window_->Show();
  }
}

void RootWindowViews::OnSetLoadingState(bool isLoading,
                                        bool canGoBack,
                                        bool canGoForward) {
  if (!HoneycombCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
    HoneycombPostTask(TID_UI,
                base::BindOnce(&RootWindowViews::OnSetLoadingState, this,
                               isLoading, canGoBack, canGoForward));
    return;
  }

  if (window_) {
    window_->SetLoadingState(isLoading, canGoBack, canGoForward);

    if (isLoading) {
      // Reset to the default window icon when loading begins.
      window_->SetFavicon(
          delegate_->GetImageCache()->GetCachedImage("window_icon"));
    }
  }
}

void RootWindowViews::OnSetDraggableRegions(
    const std::vector<HoneycombDraggableRegion>& regions) {
  if (!HoneycombCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
    HoneycombPostTask(TID_UI, base::BindOnce(&RootWindowViews::OnSetDraggableRegions,
                                       this, regions));
    return;
  }

  if (window_) {
    window_->SetDraggableRegions(regions);
  }
}

void RootWindowViews::OnTakeFocus(bool next) {
  if (!HoneycombCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
    HoneycombPostTask(TID_UI,
                base::BindOnce(&RootWindowViews::OnTakeFocus, this, next));
    return;
  }

  if (window_) {
    window_->TakeFocus(next);
  }
}

void RootWindowViews::OnBeforeContextMenu(HoneycombRefPtr<HoneycombMenuModel> model) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  if (window_) {
    window_->OnBeforeContextMenu(model);
  }
}

void RootWindowViews::CreateClientHandler(const std::string& url) {
  DCHECK(!client_handler_);

  client_handler_ = new ClientHandlerStd(this, config_->with_controls, url);
  client_handler_->set_download_favicon_images(true);
}

void RootWindowViews::InitOnUIThread(
    const HoneycombBrowserSettings& settings,
    HoneycombRefPtr<HoneycombRequestContext> request_context) {
  if (!HoneycombCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
    HoneycombPostTask(TID_UI, base::BindOnce(&RootWindowViews::InitOnUIThread, this,
                                       settings, request_context));
    return;
  }

  if (config_->initially_hidden && !config_->source_bounds.IsEmpty()) {
    // The window will be sized and positioned in OnAutoResize().
    initial_bounds_ = config_->source_bounds;
    position_on_resize_ = true;
  } else if (!config_->bounds.IsEmpty()) {
    // Initial state was specified via the config object.
    initial_bounds_ = config_->bounds;
    initial_show_state_ = config_->show_state;
  } else if (config_->window_type == WindowType::NORMAL) {
    // Initial state may be specified via the command-line or global
    // preferences.
    std::optional<HoneycombRect> bounds;
    if (prefs::LoadWindowRestorePreferences(initial_show_state_, bounds) &&
        bounds) {
      initial_bounds_ = *bounds;
    }
  }

  image_cache_ = delegate_->GetImageCache();

  // Populate the default image cache.
  ImageCache::ImageInfoSet image_set;
  for (size_t i = 0U; i < std::size(kDefaultImageCache); ++i) {
    image_set.push_back(ImageCache::ImageInfo::Create2x(kDefaultImageCache[i]));
  }

  image_cache_->LoadImages(
      image_set, base::BindOnce(&RootWindowViews::CreateViewsWindow, this,
                                settings, request_context));
}

void RootWindowViews::CreateViewsWindow(
    const HoneycombBrowserSettings& settings,
    HoneycombRefPtr<HoneycombRequestContext> request_context,
    const ImageCache::ImageSet& images) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  DCHECK(!window_);

#ifndef NDEBUG
  // Make sure the default images loaded successfully.
  DCHECK_EQ(images.size(), std::size(kDefaultImageCache));
  for (size_t i = 0U; i < std::size(kDefaultImageCache); ++i) {
    DCHECK(images[i]) << "Default image " << i << " failed to load";
  }
#endif

  // Create the ViewsWindow. It will show itself after creation.
  ViewsWindow::Create(config_->window_type, this, client_handler_, config_->url,
                      settings, request_context);
}

void RootWindowViews::NotifyViewsWindowDestroyed() {
  REQUIRE_MAIN_THREAD();
  window_destroyed_ = true;
  NotifyDestroyedIfDone();
}

void RootWindowViews::NotifyViewsWindowActivated() {
  REQUIRE_MAIN_THREAD();
  delegate_->OnRootWindowActivated(this);
}

void RootWindowViews::NotifyDestroyedIfDone() {
  // Notify once both the window and the browser have been destroyed.
  if (window_destroyed_ && browser_destroyed_) {
    // The delegate may be holding the last reference to |this|, so take a
    // reference here to keep |this| alive until after the method completes.
    scoped_refptr<RootWindow> self = this;

    delegate_->OnRootWindowDestroyed(this);
    if (!config_->close_callback.is_null()) {
      std::move(config_->close_callback).Run();
    }
  }
}

}  // namespace client
