// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_REQUEST_CONTEXT_IMPL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_REQUEST_CONTEXT_IMPL_H_
#pragma once

#include "include/honey_request_context.h"
#include "libhoney/browser/browser_context.h"
#include "libhoney/browser/media_router/media_router_impl.h"
#include "libhoney/browser/net_service/cookie_manager_impl.h"
#include "libhoney/browser/thread_util.h"

namespace content {
struct GlobalRenderFrameHostId;
}

class HoneycombBrowserContext;

// Implementation of the HoneycombRequestContext interface. All methods are thread-
// safe unless otherwise indicated. Will be deleted on the UI thread.
class HoneycombRequestContextImpl : public HoneycombRequestContext {
 public:
  HoneycombRequestContextImpl(const HoneycombRequestContextImpl&) = delete;
  HoneycombRequestContextImpl& operator=(const HoneycombRequestContextImpl&) = delete;

  ~HoneycombRequestContextImpl() override;

  // Creates the singleton global RequestContext. Called from
  // AlloyBrowserMainParts::PreMainMessageLoopRun and
  // ChromeBrowserMainExtraPartsHoneycomb::PostProfileInit.
  static HoneycombRefPtr<HoneycombRequestContextImpl> CreateGlobalRequestContext(
      const HoneycombRequestContextSettings& settings);

  // Returns a HoneycombRequestContextImpl for the specified |request_context|.
  // Will return the global context if |request_context| is NULL.
  static HoneycombRefPtr<HoneycombRequestContextImpl> GetOrCreateForRequestContext(
      HoneycombRefPtr<HoneycombRequestContext> request_context);

  // Verify that the browser context can be directly accessed (e.g. on the UI
  // thread and initialized).
  bool VerifyBrowserContext() const;

  // Returns the browser context object. Can only be called on the UI thread
  // after the browser context has been initialized.
  HoneycombBrowserContext* GetBrowserContext();

  // If the context is fully initialized execute |callback|, otherwise
  // store it until the context is fully initialized.
  void ExecuteWhenBrowserContextInitialized(base::OnceClosure callback);

  // Executes |callback| either synchronously or asynchronously after the
  // browser context object has been initialized. If |task_runner| is NULL the
  // callback will be executed on the originating thread. The resulting getter
  // can only be executed on the UI thread.
  using BrowserContextCallback =
      base::OnceCallback<void(HoneycombBrowserContext::Getter)>;
  void GetBrowserContext(
      scoped_refptr<base::SingleThreadTaskRunner> task_runner,
      BrowserContextCallback callback);

  bool IsSame(HoneycombRefPtr<HoneycombRequestContext> other) override;
  bool IsSharingWith(HoneycombRefPtr<HoneycombRequestContext> other) override;
  bool IsGlobal() override;
  HoneycombRefPtr<HoneycombRequestContextHandler> GetHandler() override;
  HoneycombString GetCachePath() override;
  HoneycombRefPtr<HoneycombCookieManager> GetCookieManager(
      HoneycombRefPtr<HoneycombCompletionCallback> callback) override;
  bool RegisterSchemeHandlerFactory(
      const HoneycombString& scheme_name,
      const HoneycombString& domain_name,
      HoneycombRefPtr<HoneycombSchemeHandlerFactory> factory) override;
  bool ClearSchemeHandlerFactories() override;
  bool HasPreference(const HoneycombString& name) override;
  HoneycombRefPtr<HoneycombValue> GetPreference(const HoneycombString& name) override;
  HoneycombRefPtr<HoneycombDictionaryValue> GetAllPreferences(
      bool include_defaults) override;
  bool CanSetPreference(const HoneycombString& name) override;
  bool SetPreference(const HoneycombString& name,
                     HoneycombRefPtr<HoneycombValue> value,
                     HoneycombString& error) override;
  void ClearCertificateExceptions(
      HoneycombRefPtr<HoneycombCompletionCallback> callback) override;
  void ClearHttpAuthCredentials(
      HoneycombRefPtr<HoneycombCompletionCallback> callback) override;
  void CloseAllConnections(HoneycombRefPtr<HoneycombCompletionCallback> callback) override;
  void ResolveHost(const HoneycombString& origin,
                   HoneycombRefPtr<HoneycombResolveCallback> callback) override;
  void LoadExtension(const HoneycombString& root_directory,
                     HoneycombRefPtr<HoneycombDictionaryValue> manifest,
                     HoneycombRefPtr<HoneycombExtensionHandler> handler) override;
  bool DidLoadExtension(const HoneycombString& extension_id) override;
  bool HasExtension(const HoneycombString& extension_id) override;
  bool GetExtensions(std::vector<HoneycombString>& extension_ids) override;
  HoneycombRefPtr<HoneycombExtension> GetExtension(const HoneycombString& extension_id) override;
  HoneycombRefPtr<HoneycombMediaRouter> GetMediaRouter(
      HoneycombRefPtr<HoneycombCompletionCallback> callback) override;
  HoneycombRefPtr<HoneycombValue> GetWebsiteSetting(
      const HoneycombString& requesting_url,
      const HoneycombString& top_level_url,
      honey_content_setting_types_t content_type) override;
  void SetWebsiteSetting(const HoneycombString& requesting_url,
                         const HoneycombString& top_level_url,
                         honey_content_setting_types_t content_type,
                         HoneycombRefPtr<HoneycombValue> value) override;
  honey_content_setting_values_t GetContentSetting(
      const HoneycombString& requesting_url,
      const HoneycombString& top_level_url,
      honey_content_setting_types_t content_type) override;
  void SetContentSetting(const HoneycombString& requesting_url,
                         const HoneycombString& top_level_url,
                         honey_content_setting_types_t content_type,
                         honey_content_setting_values_t value) override;

  const HoneycombRequestContextSettings& settings() const { return config_.settings; }

  // Called from HoneycombBrowserContentsDelegate::RenderFrameCreated or
  // HoneycombMimeHandlerViewGuestDelegate::OnGuestAttached when a render frame is
  // created.
  void OnRenderFrameCreated(const content::GlobalRenderFrameHostId& global_id,
                            bool is_main_frame,
                            bool is_guest_view);

  // Called from HoneycombBrowserContentsDelegate::RenderFrameDeleted or
  // HoneycombMimeHandlerViewGuestDelegate::OnGuestDetached when a render frame is
  // deleted.
  void OnRenderFrameDeleted(const content::GlobalRenderFrameHostId& global_id,
                            bool is_main_frame,
                            bool is_guest_view);

 private:
  friend class HoneycombRequestContext;

  struct Config {
    // True if wrapping the global context.
    bool is_global = false;

    // |settings| or |other| will be set when creating a new HoneycombRequestContext
    // via the API. When wrapping an existing HoneycombBrowserContext* both will be
    // empty and Initialize(HoneycombBrowserContext*) will be called immediately after
    // HoneycombRequestContextImpl construction.
    HoneycombRequestContextSettings settings;
    HoneycombRefPtr<HoneycombRequestContextImpl> other;

    // Optionally use this handler.
    HoneycombRefPtr<HoneycombRequestContextHandler> handler;

    // Used to uniquely identify HoneycombRequestContext objects before an associated
    // HoneycombBrowserContext has been created. Should be set when a new
    // HoneycombRequestContext via the API.
    int unique_id = -1;
  };

  static HoneycombRefPtr<HoneycombRequestContextImpl> GetOrCreateRequestContext(
      const Config& config);

  explicit HoneycombRequestContextImpl(const Config& config);

  void Initialize();
  void BrowserContextInitialized();

  // Make sure the browser context exists. Only called on the UI thread.
  void EnsureBrowserContext();

  void ClearCertificateExceptionsInternal(
      HoneycombRefPtr<HoneycombCompletionCallback> callback,
      HoneycombBrowserContext::Getter browser_context_getter);
  void ClearHttpAuthCredentialsInternal(
      HoneycombRefPtr<HoneycombCompletionCallback> callback,
      HoneycombBrowserContext::Getter browser_context_getter);
  void CloseAllConnectionsInternal(
      HoneycombRefPtr<HoneycombCompletionCallback> callback,
      HoneycombBrowserContext::Getter browser_context_getter);
  void ResolveHostInternal(const HoneycombString& origin,
                           HoneycombRefPtr<HoneycombResolveCallback> callback,
                           HoneycombBrowserContext::Getter browser_context_getter);
  void SetWebsiteSettingInternal(
      const HoneycombString& requesting_url,
      const HoneycombString& top_level_url,
      honey_content_setting_types_t content_type,
      HoneycombRefPtr<HoneycombValue> value,
      HoneycombBrowserContext::Getter browser_context_getter);
  void SetContentSettingInternal(
      const HoneycombString& requesting_url,
      const HoneycombString& top_level_url,
      honey_content_setting_types_t content_type,
      honey_content_setting_values_t value,
      HoneycombBrowserContext::Getter browser_context_getter);

  void InitializeCookieManagerInternal(
      HoneycombRefPtr<HoneycombCookieManagerImpl> cookie_manager,
      HoneycombRefPtr<HoneycombCompletionCallback> callback);
  void InitializeMediaRouterInternal(HoneycombRefPtr<HoneycombMediaRouterImpl> media_router,
                                     HoneycombRefPtr<HoneycombCompletionCallback> callback);

  HoneycombBrowserContext* browser_context() const;

  // We must disassociate from this on destruction.
  HoneycombBrowserContext* browser_context_ = nullptr;

  Config config_;

  IMPLEMENT_REFCOUNTING_DELETE_ON_UIT(HoneycombRequestContextImpl);
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_REQUEST_CONTEXT_IMPL_H_
