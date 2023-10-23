// Copyright (c) 2011 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_CLIENT_HANDLER_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_CLIENT_HANDLER_H_
#pragma once

#include <set>
#include <string>

#include "include/honey_client.h"
#include "include/wrapper/honey_helpers.h"
#include "include/wrapper/honey_message_router.h"
#include "include/wrapper/honey_resource_manager.h"
#include "tests/honeyclient/browser/client_types.h"
#include "tests/honeyclient/browser/test_runner.h"

#if defined(OS_LINUX)
#include "tests/honeyclient/browser/dialog_handler_gtk.h"
#include "tests/honeyclient/browser/print_handler_gtk.h"
#endif

namespace client {

class ClientDownloadImageCallback;

// Client handler abstract base class. Provides common functionality shared by
// all concrete client handler implementations.
class ClientHandler : public HoneycombClient,
                      public HoneycombCommandHandler,
                      public HoneycombContextMenuHandler,
                      public HoneycombDisplayHandler,
                      public HoneycombDownloadHandler,
                      public HoneycombDragHandler,
                      public HoneycombFocusHandler,
                      public HoneycombKeyboardHandler,
                      public HoneycombLifeSpanHandler,
                      public HoneycombLoadHandler,
                      public HoneycombPermissionHandler,
                      public HoneycombRequestHandler,
                      public HoneycombResourceRequestHandler {
 public:
  // Implement this interface to receive notification of ClientHandler
  // events. The methods of this class will be called on the main thread unless
  // otherwise indicated.
  class Delegate {
   public:
    // Called when the browser is created.
    virtual void OnBrowserCreated(HoneycombRefPtr<HoneycombBrowser> browser) = 0;

    // Called when the browser is closing.
    virtual void OnBrowserClosing(HoneycombRefPtr<HoneycombBrowser> browser) = 0;

    // Called when the browser has been closed.
    virtual void OnBrowserClosed(HoneycombRefPtr<HoneycombBrowser> browser) = 0;

    // Set the window URL address.
    virtual void OnSetAddress(const std::string& url) = 0;

    // Set the window title.
    virtual void OnSetTitle(const std::string& title) = 0;

    // Set the Favicon image.
    virtual void OnSetFavicon(HoneycombRefPtr<HoneycombImage> image) {}

    // Set fullscreen mode.
    virtual void OnSetFullscreen(bool fullscreen) = 0;

    // Auto-resize contents.
    virtual void OnAutoResize(const HoneycombSize& new_size) = 0;

    // Set the loading state.
    virtual void OnSetLoadingState(bool isLoading,
                                   bool canGoBack,
                                   bool canGoForward) = 0;

    // Set the draggable regions.
    virtual void OnSetDraggableRegions(
        const std::vector<HoneycombDraggableRegion>& regions) = 0;

    // Set focus to the next/previous control.
    virtual void OnTakeFocus(bool next) {}

    // Called on the UI thread before a context menu is displayed.
    virtual void OnBeforeContextMenu(HoneycombRefPtr<HoneycombMenuModel> model) {}

   protected:
    virtual ~Delegate() {}
  };

  typedef std::set<HoneycombMessageRouterBrowserSide::Handler*> MessageHandlerSet;

  // Constructor may be called on any thread.
  // |delegate| must outlive this object or DetachDelegate() must be called.
  ClientHandler(Delegate* delegate,
                bool is_osr,
                bool with_controls,
                const std::string& startup_url);

  // This object may outlive the Delegate object so it's necessary for the
  // Delegate to detach itself before destruction.
  void DetachDelegate();

  // HoneycombClient methods
  HoneycombRefPtr<HoneycombCommandHandler> GetCommandHandler() override { return this; }
  HoneycombRefPtr<HoneycombContextMenuHandler> GetContextMenuHandler() override {
    return this;
  }
  HoneycombRefPtr<HoneycombDisplayHandler> GetDisplayHandler() override { return this; }
  HoneycombRefPtr<HoneycombDownloadHandler> GetDownloadHandler() override { return this; }
  HoneycombRefPtr<HoneycombDragHandler> GetDragHandler() override { return this; }
  HoneycombRefPtr<HoneycombFocusHandler> GetFocusHandler() override { return this; }
  HoneycombRefPtr<HoneycombKeyboardHandler> GetKeyboardHandler() override { return this; }
  HoneycombRefPtr<HoneycombLifeSpanHandler> GetLifeSpanHandler() override { return this; }
  HoneycombRefPtr<HoneycombLoadHandler> GetLoadHandler() override { return this; }
  HoneycombRefPtr<HoneycombRequestHandler> GetRequestHandler() override { return this; }
  HoneycombRefPtr<HoneycombPermissionHandler> GetPermissionHandler() override {
    return this;
  }
  bool OnProcessMessageReceived(HoneycombRefPtr<HoneycombBrowser> browser,
                                HoneycombRefPtr<HoneycombFrame> frame,
                                HoneycombProcessId source_process,
                                HoneycombRefPtr<HoneycombProcessMessage> message) override;

#if defined(OS_LINUX)
  HoneycombRefPtr<HoneycombDialogHandler> GetDialogHandler() override {
    return file_dialog_handler_;
  }
  HoneycombRefPtr<HoneycombJSDialogHandler> GetJSDialogHandler() override {
    return js_dialog_handler_;
  }
  HoneycombRefPtr<HoneycombPrintHandler> GetPrintHandler() override {
    return print_handler_;
  }
#endif

  // HoneycombCommandHandler methods
  bool OnChromeCommand(HoneycombRefPtr<HoneycombBrowser> browser,
                       int command_id,
                       honey_window_open_disposition_t disposition) override;
  bool IsChromeAppMenuItemVisible(HoneycombRefPtr<HoneycombBrowser> browser,
                                  int command_id) override;
  bool IsChromePageActionIconVisible(
      honey_chrome_page_action_icon_type_t icon_type) override;
  bool IsChromeToolbarButtonVisible(
      honey_chrome_toolbar_button_type_t button_type) override;

  // HoneycombContextMenuHandler methods
  void OnBeforeContextMenu(HoneycombRefPtr<HoneycombBrowser> browser,
                           HoneycombRefPtr<HoneycombFrame> frame,
                           HoneycombRefPtr<HoneycombContextMenuParams> params,
                           HoneycombRefPtr<HoneycombMenuModel> model) override;
  bool OnContextMenuCommand(HoneycombRefPtr<HoneycombBrowser> browser,
                            HoneycombRefPtr<HoneycombFrame> frame,
                            HoneycombRefPtr<HoneycombContextMenuParams> params,
                            int command_id,
                            EventFlags event_flags) override;

  // HoneycombDisplayHandler methods
  void OnAddressChange(HoneycombRefPtr<HoneycombBrowser> browser,
                       HoneycombRefPtr<HoneycombFrame> frame,
                       const HoneycombString& url) override;
  void OnTitleChange(HoneycombRefPtr<HoneycombBrowser> browser,
                     const HoneycombString& title) override;
  void OnFaviconURLChange(HoneycombRefPtr<HoneycombBrowser> browser,
                          const std::vector<HoneycombString>& icon_urls) override;
  void OnFullscreenModeChange(HoneycombRefPtr<HoneycombBrowser> browser,
                              bool fullscreen) override;
  bool OnConsoleMessage(HoneycombRefPtr<HoneycombBrowser> browser,
                        honey_log_severity_t level,
                        const HoneycombString& message,
                        const HoneycombString& source,
                        int line) override;
  bool OnAutoResize(HoneycombRefPtr<HoneycombBrowser> browser,
                    const HoneycombSize& new_size) override;
  bool OnCursorChange(HoneycombRefPtr<HoneycombBrowser> browser,
                      HoneycombCursorHandle cursor,
                      honey_cursor_type_t type,
                      const HoneycombCursorInfo& custom_cursor_info) override;

  // HoneycombDownloadHandler methods
  bool CanDownload(HoneycombRefPtr<HoneycombBrowser> browser,
                   const HoneycombString& url,
                   const HoneycombString& request_method) override;
  void OnBeforeDownload(HoneycombRefPtr<HoneycombBrowser> browser,
                        HoneycombRefPtr<HoneycombDownloadItem> download_item,
                        const HoneycombString& suggested_name,
                        HoneycombRefPtr<HoneycombBeforeDownloadCallback> callback) override;
  void OnDownloadUpdated(HoneycombRefPtr<HoneycombBrowser> browser,
                         HoneycombRefPtr<HoneycombDownloadItem> download_item,
                         HoneycombRefPtr<HoneycombDownloadItemCallback> callback) override;

  // HoneycombDragHandler methods
  bool OnDragEnter(HoneycombRefPtr<HoneycombBrowser> browser,
                   HoneycombRefPtr<HoneycombDragData> dragData,
                   HoneycombDragHandler::DragOperationsMask mask) override;
  void OnDraggableRegionsChanged(
      HoneycombRefPtr<HoneycombBrowser> browser,
      HoneycombRefPtr<HoneycombFrame> frame,
      const std::vector<HoneycombDraggableRegion>& regions) override;

  // HoneycombFocusHandler methods
  void OnTakeFocus(HoneycombRefPtr<HoneycombBrowser> browser, bool next) override;
  bool OnSetFocus(HoneycombRefPtr<HoneycombBrowser> browser, FocusSource source) override;

  // HoneycombKeyboardHandler methods
  bool OnPreKeyEvent(HoneycombRefPtr<HoneycombBrowser> browser,
                     const HoneycombKeyEvent& event,
                     HoneycombEventHandle os_event,
                     bool* is_keyboard_shortcut) override;

  // HoneycombLifeSpanHandler methods
  bool OnBeforePopup(
      HoneycombRefPtr<HoneycombBrowser> browser,
      HoneycombRefPtr<HoneycombFrame> frame,
      const HoneycombString& target_url,
      const HoneycombString& target_frame_name,
      HoneycombLifeSpanHandler::WindowOpenDisposition target_disposition,
      bool user_gesture,
      const HoneycombPopupFeatures& popupFeatures,
      HoneycombWindowInfo& windowInfo,
      HoneycombRefPtr<HoneycombClient>& client,
      HoneycombBrowserSettings& settings,
      HoneycombRefPtr<HoneycombDictionaryValue>& extra_info,
      bool* no_javascript_access) override;
  void OnAfterCreated(HoneycombRefPtr<HoneycombBrowser> browser) override;
  bool DoClose(HoneycombRefPtr<HoneycombBrowser> browser) override;
  void OnBeforeClose(HoneycombRefPtr<HoneycombBrowser> browser) override;

  // HoneycombLoadHandler methods
  void OnLoadingStateChange(HoneycombRefPtr<HoneycombBrowser> browser,
                            bool isLoading,
                            bool canGoBack,
                            bool canGoForward) override;
  void OnLoadError(HoneycombRefPtr<HoneycombBrowser> browser,
                   HoneycombRefPtr<HoneycombFrame> frame,
                   ErrorCode errorCode,
                   const HoneycombString& errorText,
                   const HoneycombString& failedUrl) override;

  // HoneycombPermissionHandler methods
  bool OnRequestMediaAccessPermission(
      HoneycombRefPtr<HoneycombBrowser> browser,
      HoneycombRefPtr<HoneycombFrame> frame,
      const HoneycombString& requesting_origin,
      uint32_t requested_permissions,
      HoneycombRefPtr<HoneycombMediaAccessCallback> callback) override;

  // HoneycombRequestHandler methods
  bool OnBeforeBrowse(HoneycombRefPtr<HoneycombBrowser> browser,
                      HoneycombRefPtr<HoneycombFrame> frame,
                      HoneycombRefPtr<HoneycombRequest> request,
                      bool user_gesture,
                      bool is_redirect) override;
  bool OnOpenURLFromTab(
      HoneycombRefPtr<HoneycombBrowser> browser,
      HoneycombRefPtr<HoneycombFrame> frame,
      const HoneycombString& target_url,
      HoneycombRequestHandler::WindowOpenDisposition target_disposition,
      bool user_gesture) override;
  HoneycombRefPtr<HoneycombResourceRequestHandler> GetResourceRequestHandler(
      HoneycombRefPtr<HoneycombBrowser> browser,
      HoneycombRefPtr<HoneycombFrame> frame,
      HoneycombRefPtr<HoneycombRequest> request,
      bool is_navigation,
      bool is_download,
      const HoneycombString& request_initiator,
      bool& disable_default_handling) override;
  bool GetAuthCredentials(HoneycombRefPtr<HoneycombBrowser> browser,
                          const HoneycombString& origin_url,
                          bool isProxy,
                          const HoneycombString& host,
                          int port,
                          const HoneycombString& realm,
                          const HoneycombString& scheme,
                          HoneycombRefPtr<HoneycombAuthCallback> callback) override;
  bool OnCertificateError(HoneycombRefPtr<HoneycombBrowser> browser,
                          ErrorCode cert_error,
                          const HoneycombString& request_url,
                          HoneycombRefPtr<HoneycombSSLInfo> ssl_info,
                          HoneycombRefPtr<HoneycombCallback> callback) override;
  bool OnSelectClientCertificate(
      HoneycombRefPtr<HoneycombBrowser> browser,
      bool isProxy,
      const HoneycombString& host,
      int port,
      const X509CertificateList& certificates,
      HoneycombRefPtr<HoneycombSelectClientCertificateCallback> callback) override;
  void OnRenderProcessTerminated(HoneycombRefPtr<HoneycombBrowser> browser,
                                 TerminationStatus status) override;
  void OnDocumentAvailableInMainFrame(HoneycombRefPtr<HoneycombBrowser> browser) override;

  // HoneycombResourceRequestHandler methods
  honey_return_value_t OnBeforeResourceLoad(
      HoneycombRefPtr<HoneycombBrowser> browser,
      HoneycombRefPtr<HoneycombFrame> frame,
      HoneycombRefPtr<HoneycombRequest> request,
      HoneycombRefPtr<HoneycombCallback> callback) override;
  HoneycombRefPtr<HoneycombResourceHandler> GetResourceHandler(
      HoneycombRefPtr<HoneycombBrowser> browser,
      HoneycombRefPtr<HoneycombFrame> frame,
      HoneycombRefPtr<HoneycombRequest> request) override;
  HoneycombRefPtr<HoneycombResponseFilter> GetResourceResponseFilter(
      HoneycombRefPtr<HoneycombBrowser> browser,
      HoneycombRefPtr<HoneycombFrame> frame,
      HoneycombRefPtr<HoneycombRequest> request,
      HoneycombRefPtr<HoneycombResponse> response) override;
  void OnProtocolExecution(HoneycombRefPtr<HoneycombBrowser> browser,
                           HoneycombRefPtr<HoneycombFrame> frame,
                           HoneycombRefPtr<HoneycombRequest> request,
                           bool& allow_os_execution) override;

  // Returns the number of browsers currently using this handler. Can only be
  // called on the Honeycomb UI thread.
  int GetBrowserCount() const;

  // Show a new DevTools popup window.
  void ShowDevTools(HoneycombRefPtr<HoneycombBrowser> browser,
                    const HoneycombPoint& inspect_element_at);

  // Close the existing DevTools popup window, if any.
  void CloseDevTools(HoneycombRefPtr<HoneycombBrowser> browser);

  // Test if the current site has SSL information available.
  bool HasSSLInformation(HoneycombRefPtr<HoneycombBrowser> browser);

  // Show SSL information for the current site.
  void ShowSSLInformation(HoneycombRefPtr<HoneycombBrowser> browser);

  // Set a string resource for loading via StringResourceProvider.
  void SetStringResource(const std::string& page, const std::string& data);

  // Returns the Delegate.
  Delegate* delegate() const { return delegate_; }

  // Returns the startup URL.
  std::string startup_url() const { return startup_url_; }

  // Set/get whether the client should download favicon images. Only safe to
  // call immediately after client creation or on the browser process UI thread.
  bool download_favicon_images() const { return download_favicon_images_; }
  void set_download_favicon_images(bool allow) {
    download_favicon_images_ = allow;
  }

 private:
  friend class ClientDownloadImageCallback;

  // Create a new popup window using the specified information. |is_devtools|
  // will be true if the window will be used for DevTools. Return true to
  // proceed with popup browser creation or false to cancel the popup browser.
  // May be called on any thead.
  bool CreatePopupWindow(HoneycombRefPtr<HoneycombBrowser> browser,
                         bool is_devtools,
                         const HoneycombPopupFeatures& popupFeatures,
                         HoneycombWindowInfo& windowInfo,
                         HoneycombRefPtr<HoneycombClient>& client,
                         HoneycombBrowserSettings& settings);

  // Execute Delegate notifications on the main thread.
  void NotifyBrowserCreated(HoneycombRefPtr<HoneycombBrowser> browser);
  void NotifyBrowserClosing(HoneycombRefPtr<HoneycombBrowser> browser);
  void NotifyBrowserClosed(HoneycombRefPtr<HoneycombBrowser> browser);
  void NotifyAddress(const HoneycombString& url);
  void NotifyTitle(const HoneycombString& title);
  void NotifyFavicon(HoneycombRefPtr<HoneycombImage> image);
  void NotifyFullscreen(bool fullscreen);
  void NotifyAutoResize(const HoneycombSize& new_size);
  void NotifyLoadingState(bool isLoading, bool canGoBack, bool canGoForward);
  void NotifyDraggableRegions(const std::vector<HoneycombDraggableRegion>& regions);
  void NotifyTakeFocus(bool next);

  // Test context menu creation.
  void BuildTestMenu(HoneycombRefPtr<HoneycombMenuModel> model);
  bool ExecuteTestMenu(int command_id);

  void SetOfflineState(HoneycombRefPtr<HoneycombBrowser> browser, bool offline);

  // THREAD SAFE MEMBERS
  // The following members may be accessed from any thread.

  // True if this handler uses off-screen rendering.
  const bool is_osr_;

  // True if this handler shows controls.
  const bool with_controls_;

  // The startup URL.
  const std::string startup_url_;

  // True if mouse cursor change is disabled.
  bool mouse_cursor_change_disabled_;

  // True if media handling is disabled.
  bool media_handling_disabled_ = true;

  // True if the browser is currently offline.
  bool offline_;

  // True if the Chrome toolbar and menu contents/commands should be filtered.
  bool filter_chrome_commands_;

  // True if Favicon images should be downloaded.
  bool download_favicon_images_ = false;

#if defined(OS_LINUX)
  // Custom dialog handlers for GTK.
  HoneycombRefPtr<ClientDialogHandlerGtk> file_dialog_handler_;
  HoneycombRefPtr<ClientDialogHandlerGtk> js_dialog_handler_;
  HoneycombRefPtr<ClientPrintHandlerGtk> print_handler_;
#endif

  // Handles the browser side of query routing. The renderer side is handled
  // in client_renderer.cc.
  HoneycombRefPtr<HoneycombMessageRouterBrowserSide> message_router_;

  // Manages the registration and delivery of resources.
  HoneycombRefPtr<HoneycombResourceManager> resource_manager_;

  // Used to manage string resources in combination with StringResourceProvider.
  // Only accessed on the IO thread.
  test_runner::StringResourceMap string_resource_map_;

  // MAIN THREAD MEMBERS
  // The following members will only be accessed on the main thread. This will
  // be the same as the Honeycomb UI thread except when using multi-threaded message
  // loop mode on Windows.

  Delegate* delegate_;

  // UI THREAD MEMBERS
  // The following members will only be accessed on the Honeycomb UI thread.

  // Track state information for the text context menu.
  struct TestMenuState {
    TestMenuState() : check_item(true), radio_item(0) {}
    bool check_item;
    int radio_item;
  } test_menu_state_;

  // The current number of browsers using this handler.
  int browser_count_ = 0;

  // Console logging state.
  const std::string console_log_file_;

  // True if an editable field currently has focus.
  bool focus_on_editable_field_ = false;

  // True for the initial navigation after browser creation.
  bool initial_navigation_ = true;

  // Set of Handlers registered with the message router.
  MessageHandlerSet message_handler_set_;

  DISALLOW_COPY_AND_ASSIGN(ClientHandler);
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_CLIENT_HANDLER_H_
