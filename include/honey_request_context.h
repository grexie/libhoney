// Copyright (c) 2013 Marshall A. Greenblatt. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the name Chromium Embedded
// Framework nor the names of its contributors may be used to endorse
// or promote products derived from this software without specific prior
// written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ---------------------------------------------------------------------------
//
// The contents of this file must follow a specific format in order to
// support the Honeycomb translator tool. See the translator.README.txt file in the
// tools directory for more information.
//

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_REQUEST_CONTEXT_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_REQUEST_CONTEXT_H_
#pragma once

#include <vector>

#include "include/honey_callback.h"
#include "include/honey_cookie.h"
#include "include/honey_extension.h"
#include "include/honey_extension_handler.h"
#include "include/honey_media_router.h"
#include "include/honey_preference.h"
#include "include/honey_values.h"

class HoneycombRequestContextHandler;
class HoneycombSchemeHandlerFactory;

///
/// Callback interface for HoneycombRequestContext::ResolveHost.
///
/*--honey(source=client)--*/
class HoneycombResolveCallback : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Called on the UI thread after the ResolveHost request has completed.
  /// |result| will be the result code. |resolved_ips| will be the list of
  /// resolved IP addresses or empty if the resolution failed.
  ///
  /*--honey(optional_param=resolved_ips)--*/
  virtual void OnResolveCompleted(
      honey_errorcode_t result,
      const std::vector<HoneycombString>& resolved_ips) = 0;
};

///
/// A request context provides request handling for a set of related browser
/// or URL request objects. A request context can be specified when creating a
/// new browser via the HoneycombBrowserHost static factory methods or when creating a
/// new URL request via the HoneycombURLRequest static factory methods. Browser
/// objects with different request contexts will never be hosted in the same
/// render process. Browser objects with the same request context may or may not
/// be hosted in the same render process depending on the process model. Browser
/// objects created indirectly via the JavaScript window.open function or
/// targeted links will share the same render process and the same request
/// context as the source browser. When running in single-process mode there is
/// only a single render process (the main process) and so all browsers created
/// in single-process mode will share the same request context. This will be the
/// first request context passed into a HoneycombBrowserHost static factory method and
/// all other request context objects will be ignored.
///
/*--honey(source=library,no_debugct_check)--*/
class HoneycombRequestContext : public HoneycombPreferenceManager {
 public:
  ///
  /// Returns the global context object.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombRequestContext> GetGlobalContext();

  ///
  /// Creates a new context object with the specified |settings| and optional
  /// |handler|.
  ///
  /*--honey(optional_param=handler)--*/
  static HoneycombRefPtr<HoneycombRequestContext> CreateContext(
      const HoneycombRequestContextSettings& settings,
      HoneycombRefPtr<HoneycombRequestContextHandler> handler);

  ///
  /// Creates a new context object that shares storage with |other| and uses an
  /// optional |handler|.
  ///
  /*--honey(capi_name=honey_create_context_shared,optional_param=handler)--*/
  static HoneycombRefPtr<HoneycombRequestContext> CreateContext(
      HoneycombRefPtr<HoneycombRequestContext> other,
      HoneycombRefPtr<HoneycombRequestContextHandler> handler);

  ///
  /// Returns true if this object is pointing to the same context as |that|
  /// object.
  ///
  /*--honey()--*/
  virtual bool IsSame(HoneycombRefPtr<HoneycombRequestContext> other) = 0;

  ///
  /// Returns true if this object is sharing the same storage as |that| object.
  ///
  /*--honey()--*/
  virtual bool IsSharingWith(HoneycombRefPtr<HoneycombRequestContext> other) = 0;

  ///
  /// Returns true if this object is the global context. The global context is
  /// used by default when creating a browser or URL request with a NULL context
  /// argument.
  ///
  /*--honey()--*/
  virtual bool IsGlobal() = 0;

  ///
  /// Returns the handler for this context if any.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombRequestContextHandler> GetHandler() = 0;

  ///
  /// Returns the cache path for this object. If empty an "incognito mode"
  /// in-memory cache is being used.
  ///
  /*--honey()--*/
  virtual HoneycombString GetCachePath() = 0;

  ///
  /// Returns the cookie manager for this object. If |callback| is non-NULL it
  /// will be executed asnychronously on the UI thread after the manager's
  /// storage has been initialized.
  ///
  /*--honey(optional_param=callback)--*/
  virtual HoneycombRefPtr<HoneycombCookieManager> GetCookieManager(
      HoneycombRefPtr<HoneycombCompletionCallback> callback) = 0;

  ///
  /// Register a scheme handler factory for the specified |scheme_name| and
  /// optional |domain_name|. An empty |domain_name| value for a standard scheme
  /// will cause the factory to match all domain names. The |domain_name| value
  /// will be ignored for non-standard schemes. If |scheme_name| is a built-in
  /// scheme and no handler is returned by |factory| then the built-in scheme
  /// handler factory will be called. If |scheme_name| is a custom scheme then
  /// you must also implement the HoneycombApp::OnRegisterCustomSchemes() method in
  /// all processes. This function may be called multiple times to change or
  /// remove the factory that matches the specified |scheme_name| and optional
  /// |domain_name|. Returns false if an error occurs. This function may be
  /// called on any thread in the browser process.
  ///
  /*--honey(optional_param=domain_name,optional_param=factory)--*/
  virtual bool RegisterSchemeHandlerFactory(
      const HoneycombString& scheme_name,
      const HoneycombString& domain_name,
      HoneycombRefPtr<HoneycombSchemeHandlerFactory> factory) = 0;

  ///
  /// Clear all registered scheme handler factories. Returns false on error.
  /// This function may be called on any thread in the browser process.
  ///
  /*--honey()--*/
  virtual bool ClearSchemeHandlerFactories() = 0;

  ///
  /// Clears all certificate exceptions that were added as part of handling
  /// HoneycombRequestHandler::OnCertificateError(). If you call this it is
  /// recommended that you also call CloseAllConnections() or you risk not
  /// being prompted again for server certificates if you reconnect quickly.
  /// If |callback| is non-NULL it will be executed on the UI thread after
  /// completion.
  ///
  /*--honey(optional_param=callback)--*/
  virtual void ClearCertificateExceptions(
      HoneycombRefPtr<HoneycombCompletionCallback> callback) = 0;

  ///
  /// Clears all HTTP authentication credentials that were added as part of
  /// handling GetAuthCredentials. If |callback| is non-NULL it will be executed
  /// on the UI thread after completion.
  ///
  /*--honey(optional_param=callback)--*/
  virtual void ClearHttpAuthCredentials(
      HoneycombRefPtr<HoneycombCompletionCallback> callback) = 0;

  ///
  /// Clears all active and idle connections that Chromium currently has.
  /// This is only recommended if you have released all other Honeycomb objects but
  /// don't yet want to call HoneycombShutdown(). If |callback| is non-NULL it will be
  /// executed on the UI thread after completion.
  ///
  /*--honey(optional_param=callback)--*/
  virtual void CloseAllConnections(
      HoneycombRefPtr<HoneycombCompletionCallback> callback) = 0;

  ///
  /// Attempts to resolve |origin| to a list of associated IP addresses.
  /// |callback| will be executed on the UI thread after completion.
  ///
  /*--honey()--*/
  virtual void ResolveHost(const HoneycombString& origin,
                           HoneycombRefPtr<HoneycombResolveCallback> callback) = 0;

  ///
  /// Load an extension.
  ///
  /// If extension resources will be read from disk using the default load
  /// implementation then |root_directory| should be the absolute path to the
  /// extension resources directory and |manifest| should be NULL. If extension
  /// resources will be provided by the client (e.g. via HoneycombRequestHandler
  /// and/or HoneycombExtensionHandler) then |root_directory| should be a path
  /// component unique to the extension (if not absolute this will be internally
  /// prefixed with the PK_DIR_RESOURCES path) and |manifest| should contain the
  /// contents that would otherwise be read from the "manifest.json" file on
  /// disk.
  ///
  /// The loaded extension will be accessible in all contexts sharing the same
  /// storage (HasExtension returns true). However, only the context on which
  /// this method was called is considered the loader (DidLoadExtension returns
  /// true) and only the loader will receive HoneycombRequestContextHandler callbacks
  /// for the extension.
  ///
  /// HoneycombExtensionHandler::OnExtensionLoaded will be called on load success or
  /// HoneycombExtensionHandler::OnExtensionLoadFailed will be called on load failure.
  ///
  /// If the extension specifies a background script via the "background"
  /// manifest key then HoneycombExtensionHandler::OnBeforeBackgroundBrowser will be
  /// called to create the background browser. See that method for additional
  /// information about background scripts.
  ///
  /// For visible extension views the client application should evaluate the
  /// manifest to determine the correct extension URL to load and then pass that
  /// URL to the HoneycombBrowserHost::CreateBrowser* function after the extension has
  /// loaded. For example, the client can look for the "browser_action" manifest
  /// key as documented at
  /// https://developer.chrome.com/extensions/browserAction. Extension URLs take
  /// the form "chrome-extension://<extension_id>/<path>".
  ///
  /// Browsers that host extensions differ from normal browsers as follows:
  ///  - Can access chrome.* JavaScript APIs if allowed by the manifest. Visit
  ///    chrome://extensions-support for the list of extension APIs currently
  ///    supported by Honeycomb.
  ///  - Main frame navigation to non-extension content is blocked.
  ///  - Pinch-zooming is disabled.
  ///  - HoneycombBrowserHost::GetExtension returns the hosted extension.
  ///  - HoneycombBrowserHost::IsBackgroundHost returns true for background hosts.
  ///
  /// See https://developer.chrome.com/extensions for extension implementation
  /// and usage documentation.
  ///
  /*--honey(optional_param=manifest,optional_param=handler)--*/
  virtual void LoadExtension(const HoneycombString& root_directory,
                             HoneycombRefPtr<HoneycombDictionaryValue> manifest,
                             HoneycombRefPtr<HoneycombExtensionHandler> handler) = 0;

  ///
  /// Returns true if this context was used to load the extension identified by
  /// |extension_id|. Other contexts sharing the same storage will also have
  /// access to the extension (see HasExtension). This method must be called on
  /// the browser process UI thread.
  ///
  /*--honey()--*/
  virtual bool DidLoadExtension(const HoneycombString& extension_id) = 0;

  ///
  /// Returns true if this context has access to the extension identified by
  /// |extension_id|. This may not be the context that was used to load the
  /// extension (see DidLoadExtension). This method must be called on the
  /// browser process UI thread.
  ///
  /*--honey()--*/
  virtual bool HasExtension(const HoneycombString& extension_id) = 0;

  ///
  /// Retrieve the list of all extensions that this context has access to (see
  /// HasExtension). |extension_ids| will be populated with the list of
  /// extension ID values. Returns true on success. This method must be called
  /// on the browser process UI thread.
  ///
  /*--honey()--*/
  virtual bool GetExtensions(std::vector<HoneycombString>& extension_ids) = 0;

  ///
  /// Returns the extension matching |extension_id| or NULL if no matching
  /// extension is accessible in this context (see HasExtension). This method
  /// must be called on the browser process UI thread.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombExtension> GetExtension(
      const HoneycombString& extension_id) = 0;

  ///
  /// Returns the MediaRouter object associated with this context.  If
  /// |callback| is non-NULL it will be executed asnychronously on the UI thread
  /// after the manager's context has been initialized.
  ///
  /*--honey(optional_param=callback)--*/
  virtual HoneycombRefPtr<HoneycombMediaRouter> GetMediaRouter(
      HoneycombRefPtr<HoneycombCompletionCallback> callback) = 0;

  ///
  /// Returns the current value for |content_type| that applies for the
  /// specified URLs. If both URLs are empty the default value will be returned.
  /// Returns nullptr if no value is configured. Must be called on the browser
  /// process UI thread.
  ///
  /*--honey(optional_param=requesting_url,optional_param=top_level_url)--*/
  virtual HoneycombRefPtr<HoneycombValue> GetWebsiteSetting(
      const HoneycombString& requesting_url,
      const HoneycombString& top_level_url,
      honey_content_setting_types_t content_type) = 0;

  ///
  /// Sets the current value for |content_type| for the specified URLs in the
  /// default scope. If both URLs are empty, and the context is not incognito,
  /// the default value will be set. Pass nullptr for |value| to remove the
  /// default value for this content type.
  ///
  /// WARNING: Incorrect usage of this method may cause instability or security
  /// issues in Chromium. Make sure that you first understand the potential
  /// impact of any changes to |content_type| by reviewing the related source
  /// code in Chromium. For example, if you plan to modify
  /// HONEYCOMB_CONTENT_SETTING_TYPE_POPUPS, first review and understand the usage of
  /// ContentSettingsType::POPUPS in Chromium:
  /// https://source.chromium.org/search?q=ContentSettingsType::POPUPS
  ///
  /*--honey(optional_param=requesting_url,optional_param=top_level_url,
          optional_param=value)--*/
  virtual void SetWebsiteSetting(const HoneycombString& requesting_url,
                                 const HoneycombString& top_level_url,
                                 honey_content_setting_types_t content_type,
                                 HoneycombRefPtr<HoneycombValue> value) = 0;

  ///
  /// Returns the current value for |content_type| that applies for the
  /// specified URLs. If both URLs are empty the default value will be returned.
  /// Returns HONEYCOMB_CONTENT_SETTING_VALUE_DEFAULT if no value is configured. Must
  /// be called on the browser process UI thread.
  ///
  /*--honey(optional_param=requesting_url,optional_param=top_level_url,
          default_retval=HONEYCOMB_CONTENT_SETTING_VALUE_DEFAULT)--*/
  virtual honey_content_setting_values_t GetContentSetting(
      const HoneycombString& requesting_url,
      const HoneycombString& top_level_url,
      honey_content_setting_types_t content_type) = 0;

  ///
  /// Sets the current value for |content_type| for the specified URLs in the
  /// default scope. If both URLs are empty, and the context is not incognito,
  /// the default value will be set. Pass HONEYCOMB_CONTENT_SETTING_VALUE_DEFAULT for
  /// |value| to use the default value for this content type.
  ///
  /// WARNING: Incorrect usage of this method may cause instability or security
  /// issues in Chromium. Make sure that you first understand the potential
  /// impact of any changes to |content_type| by reviewing the related source
  /// code in Chromium. For example, if you plan to modify
  /// HONEYCOMB_CONTENT_SETTING_TYPE_POPUPS, first review and understand the usage of
  /// ContentSettingsType::POPUPS in Chromium:
  /// https://source.chromium.org/search?q=ContentSettingsType::POPUPS
  ///
  /*--honey(optional_param=requesting_url,optional_param=top_level_url)--*/
  virtual void SetContentSetting(const HoneycombString& requesting_url,
                                 const HoneycombString& top_level_url,
                                 honey_content_setting_types_t content_type,
                                 honey_content_setting_values_t value) = 0;
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_REQUEST_CONTEXT_H_
