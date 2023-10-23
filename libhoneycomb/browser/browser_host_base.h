// Copyright 2020 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_BROWSER_HOST_BASE_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_BROWSER_HOST_BASE_H_
#pragma once

#include "include/honey_browser.h"
#include "include/honey_client.h"
#include "include/views/honey_browser_view.h"
#include "libhoneycomb/browser/browser_contents_delegate.h"
#include "libhoneycomb/browser/browser_info.h"
#include "libhoneycomb/browser/browser_platform_delegate.h"
#include "libhoneycomb/browser/devtools/devtools_manager.h"
#include "libhoneycomb/browser/file_dialog_manager.h"
#include "libhoneycomb/browser/frame_host_impl.h"
#include "libhoneycomb/browser/media_stream_registrar.h"
#include "libhoneycomb/browser/request_context_impl.h"

#include "base/observer_list.h"
#include "base/synchronization/lock.h"
#include "extensions/common/mojom/view_type.mojom.h"

namespace extensions {
class Extension;
}

// Parameters that are passed to the runtime-specific Create methods.
struct HoneycombBrowserCreateParams {
  HoneycombBrowserCreateParams() {}

  // Copy constructor used with the chrome runtime only.
  HoneycombBrowserCreateParams(const HoneycombBrowserCreateParams& that) {
    operator=(that);
  }
  HoneycombBrowserCreateParams& operator=(const HoneycombBrowserCreateParams& that) {
    // Not all parameters can be copied.
    client = that.client;
    url = that.url;
    settings = that.settings;
    request_context = that.request_context;
    extra_info = that.extra_info;
    if (that.window_info) {
      MaybeSetWindowInfo(*that.window_info);
    }
    browser_view = that.browser_view;
    return *this;
  }

  // Set |window_info| if appropriate (see below).
  void MaybeSetWindowInfo(const HoneycombWindowInfo& window_info);

  // Platform-specific window creation info. Will be nullptr for Views-hosted
  // browsers except when using the Chrome runtime with a native parent handle.
  std::unique_ptr<HoneycombWindowInfo> window_info;

  // The BrowserView that will own a Views-hosted browser. Will be nullptr for
  // popup browsers.
  HoneycombRefPtr<HoneycombBrowserView> browser_view;

  // True if this browser is a popup and has a Views-hosted opener, in which
  // case the BrowserView for this browser will be created later (from
  // PopupWebContentsCreated).
  bool popup_with_views_hosted_opener = false;

  // Client implementation. May be nullptr.
  HoneycombRefPtr<HoneycombClient> client;

  // Initial URL to load. May be empty. If this is a valid extension URL then
  // the browser will be created as an app view extension host.
  HoneycombString url;

  // Browser settings.
  HoneycombBrowserSettings settings;

  // Other browser that opened this DevTools browser. Will be nullptr for non-
  // DevTools browsers. Currently used with the alloy runtime only.
  HoneycombRefPtr<HoneycombBrowserHostBase> devtools_opener;

  // Request context to use when creating the browser. If nullptr the global
  // request context will be used.
  HoneycombRefPtr<HoneycombRequestContext> request_context;

  // Extra information that will be passed to
  // HoneycombRenderProcessHandler::OnBrowserCreated.
  HoneycombRefPtr<HoneycombDictionaryValue> extra_info;

  // Used when explicitly creating the browser as an extension host via
  // ProcessManager::CreateBackgroundHost. Currently used with the alloy
  // runtime only.
  const extensions::Extension* extension = nullptr;
  extensions::mojom::ViewType extension_host_type =
      extensions::mojom::ViewType::kInvalid;
};

// Base class for HoneycombBrowserHost implementations. Includes functionality that is
// shared by the alloy and chrome runtimes. All methods are thread-safe unless
// otherwise indicated.
class HoneycombBrowserHostBase : public HoneycombBrowserHost,
                           public HoneycombBrowser,
                           public HoneycombBrowserContentsDelegate::Observer {
 public:
  // Interface to implement for observers that wish to be informed of changes
  // to the HoneycombBrowserHostBase. All methods will be called on the UI thread.
  class Observer : public base::CheckedObserver {
   public:
    // Called before |browser| is destroyed. Any references to |browser| should
    // be cleared when this method is called.
    virtual void OnBrowserDestroyed(HoneycombBrowserHostBase* browser) = 0;

   protected:
    virtual ~Observer() {}
  };

  // Create a new HoneycombBrowserHost instance of the current runtime type with
  // owned WebContents.
  static HoneycombRefPtr<HoneycombBrowserHostBase> Create(
      HoneycombBrowserCreateParams& create_params);

  // Returns the browser associated with the specified RenderViewHost.
  static HoneycombRefPtr<HoneycombBrowserHostBase> GetBrowserForHost(
      const content::RenderViewHost* host);
  // Returns the browser associated with the specified RenderFrameHost.
  static HoneycombRefPtr<HoneycombBrowserHostBase> GetBrowserForHost(
      const content::RenderFrameHost* host);
  // Returns the browser associated with the specified WebContents.
  static HoneycombRefPtr<HoneycombBrowserHostBase> GetBrowserForContents(
      const content::WebContents* contents);
  // Returns the browser associated with the specified global ID.
  static HoneycombRefPtr<HoneycombBrowserHostBase> GetBrowserForGlobalId(
      const content::GlobalRenderFrameHostId& global_id);
  // Returns the browser associated with the specified top-level window.
  static HoneycombRefPtr<HoneycombBrowserHostBase> GetBrowserForTopLevelNativeWindow(
      gfx::NativeWindow owning_window);

  // Returns the browser most likely to be focused. This may be somewhat iffy
  // with windowless browsers as there is no guarantee that the client has only
  // one browser focused at a time.
  static HoneycombRefPtr<HoneycombBrowserHostBase> GetLikelyFocusedBrowser();

  HoneycombBrowserHostBase(
      const HoneycombBrowserSettings& settings,
      HoneycombRefPtr<HoneycombClient> client,
      std::unique_ptr<HoneycombBrowserPlatformDelegate> platform_delegate,
      scoped_refptr<HoneycombBrowserInfo> browser_info,
      HoneycombRefPtr<HoneycombRequestContextImpl> request_context);

  HoneycombBrowserHostBase(const HoneycombBrowserHostBase&) = delete;
  HoneycombBrowserHostBase& operator=(const HoneycombBrowserHostBase&) = delete;

  // Called on the UI thread after the associated WebContents is created.
  virtual void InitializeBrowser();

  // Called on the UI thread when the OS window hosting the browser is
  // destroyed.
  virtual void WindowDestroyed() = 0;

  // Returns true if the browser is in the process of being destroyed. Called on
  // the UI thread only.
  virtual bool WillBeDestroyed() const = 0;

  // Called on the UI thread after the associated WebContents is destroyed.
  // Also called from HoneycombBrowserInfoManager::DestroyAllBrowsers if the browser
  // was not properly shut down.
  virtual void DestroyBrowser();

  // HoneycombBrowserHost methods:
  HoneycombRefPtr<HoneycombBrowser> GetBrowser() override;
  HoneycombRefPtr<HoneycombClient> GetClient() override;
  HoneycombRefPtr<HoneycombRequestContext> GetRequestContext() override;
  bool CanZoom(honey_zoom_command_t command) override;
  void Zoom(honey_zoom_command_t command) override;
  double GetDefaultZoomLevel() override;
  double GetZoomLevel() override;
  void SetZoomLevel(double zoomLevel) override;
  bool HasView() override;
  void SetFocus(bool focus) override;
  void RunFileDialog(FileDialogMode mode,
                     const HoneycombString& title,
                     const HoneycombString& default_file_path,
                     const std::vector<HoneycombString>& accept_filters,
                     HoneycombRefPtr<HoneycombRunFileDialogCallback> callback) override;
  void StartDownload(const HoneycombString& url) override;
  void DownloadImage(const HoneycombString& image_url,
                     bool is_favicon,
                     uint32_t max_image_size,
                     bool bypass_cache,
                     HoneycombRefPtr<HoneycombDownloadImageCallback> callback) override;
  void Print() override;
  void PrintToPDF(const HoneycombString& path,
                  const HoneycombPdfPrintSettings& settings,
                  HoneycombRefPtr<HoneycombPdfPrintCallback> callback) override;
  void ReplaceMisspelling(const HoneycombString& word) override;
  void AddWordToDictionary(const HoneycombString& word) override;
  void SendKeyEvent(const HoneycombKeyEvent& event) override;
  void SendMouseClickEvent(const HoneycombMouseEvent& event,
                           MouseButtonType type,
                           bool mouseUp,
                           int clickCount) override;
  void SendMouseMoveEvent(const HoneycombMouseEvent& event, bool mouseLeave) override;
  void SendMouseWheelEvent(const HoneycombMouseEvent& event,
                           int deltaX,
                           int deltaY) override;
  bool SendDevToolsMessage(const void* message, size_t message_size) override;
  int ExecuteDevToolsMethod(int message_id,
                            const HoneycombString& method,
                            HoneycombRefPtr<HoneycombDictionaryValue> params) override;
  HoneycombRefPtr<HoneycombRegistration> AddDevToolsMessageObserver(
      HoneycombRefPtr<HoneycombDevToolsMessageObserver> observer) override;
  void GetNavigationEntries(HoneycombRefPtr<HoneycombNavigationEntryVisitor> visitor,
                            bool current_only) override;
  HoneycombRefPtr<HoneycombNavigationEntry> GetVisibleNavigationEntry() override;
  void NotifyMoveOrResizeStarted() override;

  // HoneycombBrowser methods:
  bool IsValid() override;
  HoneycombRefPtr<HoneycombBrowserHost> GetHost() override;
  bool CanGoBack() override;
  void GoBack() override;
  bool CanGoForward() override;
  void GoForward() override;
  bool IsLoading() override;
  void Reload() override;
  void ReloadIgnoreCache() override;
  void StopLoad() override;
  int GetIdentifier() override;
  bool IsSame(HoneycombRefPtr<HoneycombBrowser> that) override;
  bool HasDocument() override;
  bool IsPopup() override;
  HoneycombRefPtr<HoneycombFrame> GetMainFrame() override;
  HoneycombRefPtr<HoneycombFrame> GetFocusedFrame() override;
  HoneycombRefPtr<HoneycombFrame> GetFrame(int64_t identifier) override;
  HoneycombRefPtr<HoneycombFrame> GetFrame(const HoneycombString& name) override;
  size_t GetFrameCount() override;
  void GetFrameIdentifiers(std::vector<int64_t>& identifiers) override;
  void GetFrameNames(std::vector<HoneycombString>& names) override;

  // HoneycombBrowserContentsDelegate::Observer methods:
  void OnStateChanged(HoneycombBrowserContentsState state_changed) override;
  void OnWebContentsDestroyed(content::WebContents* web_contents) override;

  // Returns the frame associated with the specified RenderFrameHost.
  HoneycombRefPtr<HoneycombFrame> GetFrameForHost(const content::RenderFrameHost* host);

  // Returns the frame associated with the specified global ID. See
  // documentation on RenderFrameHost::GetFrameTreeNodeId() for why the global
  // ID is preferred.
  HoneycombRefPtr<HoneycombFrame> GetFrameForGlobalId(
      const content::GlobalRenderFrameHostId& global_id);

  // Manage observer objects. The observer must either outlive this object or
  // be removed before destruction. Must be called on the UI thread.
  void AddObserver(Observer* observer);
  void RemoveObserver(Observer* observer);
  bool HasObserver(Observer* observer) const;

  // Methods called from HoneycombFrameHostImpl.
  void LoadMainFrameURL(const content::OpenURLParams& params);
  virtual void OnSetFocus(honey_focus_source_t source) = 0;
  void ViewText(const std::string& text);

  // Calls HoneycombFileDialogManager methods.
  void RunFileChooserForBrowser(
      const blink::mojom::FileChooserParams& params,
      HoneycombFileDialogManager::RunFileChooserCallback callback);
  void RunSelectFile(ui::SelectFileDialog::Listener* listener,
                     std::unique_ptr<ui::SelectFilePolicy> policy,
                     ui::SelectFileDialog::Type type,
                     const std::u16string& title,
                     const base::FilePath& default_path,
                     const ui::SelectFileDialog::FileTypeInfo* file_types,
                     int file_type_index,
                     const base::FilePath::StringType& default_extension,
                     gfx::NativeWindow owning_window,
                     void* params);
  void SelectFileListenerDestroyed(ui::SelectFileDialog::Listener* listener);

  // Called from HoneycombBrowserInfoManager::MaybeAllowNavigation.
  virtual bool MaybeAllowNavigation(content::RenderFrameHost* opener,
                                    bool is_guest_view,
                                    const content::OpenURLParams& params);

  // Helpers for executing client callbacks. Must be called on the UI thread.
  void OnAfterCreated();
  void OnBeforeClose();
  void OnBrowserDestroyed();

  // Thread-safe accessors.
  const HoneycombBrowserSettings& settings() const { return settings_; }
  HoneycombRefPtr<HoneycombClient> client() const { return client_; }
  scoped_refptr<HoneycombBrowserInfo> browser_info() const { return browser_info_; }
  int browser_id() const;
  HoneycombRefPtr<HoneycombRequestContextImpl> request_context() const {
    return request_context_;
  }
  bool is_views_hosted() const { return is_views_hosted_; }
  SkColor GetBackgroundColor() const;

  // Returns true if windowless rendering is enabled.
  virtual bool IsWindowless() const;

  // Accessors that must be called on the UI thread.
  content::WebContents* GetWebContents() const;
  content::BrowserContext* GetBrowserContext() const;
  HoneycombBrowserPlatformDelegate* platform_delegate() const {
    return platform_delegate_.get();
  }
  HoneycombBrowserContentsDelegate* contents_delegate() const {
    return contents_delegate_.get();
  }
  HoneycombMediaStreamRegistrar* GetMediaStreamRegistrar();

  // Returns the Widget owner for the browser window. Only used with windowed
  // browsers.
  views::Widget* GetWindowWidget() const;

  // Returns the BrowserView associated with this browser. Only used with Views-
  // based browsers.
  HoneycombRefPtr<HoneycombBrowserView> GetBrowserView() const;

  // Returns the top-level native window for this browser. With windowed
  // browsers this will be an aura::Window* on Aura platforms (Windows/Linux)
  // and an NSWindow wrapper object from native_widget_types.h on MacOS. With
  // windowless browsers this method will always return an empty value.
  gfx::NativeWindow GetTopLevelNativeWindow() const;

  // Returns true if this browser is currently focused. A browser is considered
  // focused when the top-level RenderFrameHost is in the parent chain of the
  // currently focused RFH within the frame tree. In addition, its associated
  // RenderWidgetHost must also be focused. With windowed browsers only one
  // browser should be focused at a time. With windowless browsers this relies
  // on the client to properly configure focus state.
  bool IsFocused() const;

  // Returns true if this browser is currently visible.
  virtual bool IsVisible() const;

 protected:
  bool EnsureDevToolsManager();
  void InitializeDevToolsRegistrationOnUIThread(
      HoneycombRefPtr<HoneycombRegistration> registration);

  // Called from LoadMainFrameURL to perform the actual navigation.
  virtual bool Navigate(const content::OpenURLParams& params);

  // Create the HoneycombFileDialogManager if it doesn't already exist.
  bool EnsureFileDialogManager();

  // Thread-safe members.
  HoneycombBrowserSettings settings_;
  HoneycombRefPtr<HoneycombClient> client_;
  std::unique_ptr<HoneycombBrowserPlatformDelegate> platform_delegate_;
  scoped_refptr<HoneycombBrowserInfo> browser_info_;
  HoneycombRefPtr<HoneycombRequestContextImpl> request_context_;
  const bool is_views_hosted_;

  // Only accessed on the UI thread.
  std::unique_ptr<HoneycombBrowserContentsDelegate> contents_delegate_;

  // Observers that want to be notified of changes to this object.
  // Only accessed on the UI thread.
  base::ObserverList<Observer> observers_;

  // Used for creating and managing file dialogs.
  std::unique_ptr<HoneycombFileDialogManager> file_dialog_manager_;

  // Volatile state accessed from multiple threads. All access must be protected
  // by |state_lock_|.
  base::Lock state_lock_;
  bool is_loading_ = false;
  bool can_go_back_ = false;
  bool can_go_forward_ = false;
  bool has_document_ = false;
  bool is_fullscreen_ = false;
  HoneycombRefPtr<HoneycombFrameHostImpl> focused_frame_;

  // Used for creating and managing DevTools instances.
  std::unique_ptr<HoneycombDevToolsManager> devtools_manager_;

  std::unique_ptr<HoneycombMediaStreamRegistrar> media_stream_registrar_;

 private:
  IMPLEMENT_REFCOUNTING(HoneycombBrowserHostBase);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_BROWSER_HOST_BASE_H_
