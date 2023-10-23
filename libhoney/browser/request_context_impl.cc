// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoney/browser/request_context_impl.h"
#include "libhoney/browser/browser_context.h"
#include "libhoney/browser/context.h"
#include "libhoney/browser/prefs/pref_helper.h"
#include "libhoney/browser/thread_util.h"
#include "libhoney/common/app_manager.h"
#include "libhoney/common/task_runner_impl.h"
#include "libhoney/common/values_impl.h"

#include "base/atomic_sequence_num.h"
#include "base/logging.h"
#include "base/strings/stringprintf.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "components/prefs/pref_service.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/child_process_host.h"
#include "content/public/browser/ssl_host_state_delegate.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "net/dns/host_resolver.h"
#include "services/network/public/cpp/resolve_host_client_base.h"
#include "services/network/public/mojom/network_context.mojom.h"

using content::BrowserThread;

namespace {

base::AtomicSequenceNumber g_next_id;

class ResolveHostHelper : public network::ResolveHostClientBase {
 public:
  explicit ResolveHostHelper(HoneycombRefPtr<HoneycombResolveCallback> callback)
      : callback_(callback), receiver_(this) {}

  ResolveHostHelper(const ResolveHostHelper&) = delete;
  ResolveHostHelper& operator=(const ResolveHostHelper&) = delete;

  void Start(HoneycombBrowserContext* browser_context, const HoneycombString& origin) {
    HONEYCOMB_REQUIRE_UIT();

    browser_context->GetNetworkContext()->CreateHostResolver(
        net::DnsConfigOverrides(), host_resolver_.BindNewPipeAndPassReceiver());

    host_resolver_.set_disconnect_handler(base::BindOnce(
        &ResolveHostHelper::OnComplete, base::Unretained(this), net::ERR_FAILED,
        net::ResolveErrorInfo(net::ERR_FAILED), absl::nullopt, absl::nullopt));

    host_resolver_->ResolveHost(
        network::mojom::HostResolverHost::NewHostPortPair(
            net::HostPortPair::FromURL(GURL(origin.ToString()))),
        net::NetworkAnonymizationKey::CreateTransient(), nullptr,
        receiver_.BindNewPipeAndPassRemote());
  }

 private:
  void OnComplete(int result,
                  const net::ResolveErrorInfo& resolve_error_info,
                  const absl::optional<net::AddressList>& resolved_addresses,
                  const absl::optional<net::HostResolverEndpointResults>&
                      endpoint_results_with_metadat) override {
    HONEYCOMB_REQUIRE_UIT();

    host_resolver_.reset();
    receiver_.reset();

    std::vector<HoneycombString> resolved_ips;

    if (result == net::OK && resolved_addresses.has_value()) {
      DCHECK(!resolved_addresses->empty());
      for (const auto& value : *resolved_addresses) {
        resolved_ips.push_back(value.ToStringWithoutPort());
      }
    }

    callback_->OnResolveCompleted(static_cast<honey_errorcode_t>(result),
                                  resolved_ips);
    delete this;
  }

  HoneycombRefPtr<HoneycombResolveCallback> callback_;

  mojo::Remote<network::mojom::HostResolver> host_resolver_;
  mojo::Receiver<network::mojom::ResolveHostClient> receiver_{this};
};

}  // namespace

// HoneycombBrowserContext

// static
HoneycombRefPtr<HoneycombRequestContext> HoneycombRequestContext::GetGlobalContext() {
  // Verify that the context is in a valid state.
  if (!CONTEXT_STATE_VALID()) {
    DCHECK(false) << "context not valid";
    return nullptr;
  }

  HoneycombRequestContextImpl::Config config;
  config.is_global = true;
  return HoneycombRequestContextImpl::GetOrCreateRequestContext(config);
}

// static
HoneycombRefPtr<HoneycombRequestContext> HoneycombRequestContext::CreateContext(
    const HoneycombRequestContextSettings& settings,
    HoneycombRefPtr<HoneycombRequestContextHandler> handler) {
  // Verify that the context is in a valid state.
  if (!CONTEXT_STATE_VALID()) {
    DCHECK(false) << "context not valid";
    return nullptr;
  }

  HoneycombRequestContextImpl::Config config;
  config.settings = settings;
  config.handler = handler;
  config.unique_id = g_next_id.GetNext();
  return HoneycombRequestContextImpl::GetOrCreateRequestContext(config);
}

// static
HoneycombRefPtr<HoneycombRequestContext> HoneycombRequestContext::CreateContext(
    HoneycombRefPtr<HoneycombRequestContext> other,
    HoneycombRefPtr<HoneycombRequestContextHandler> handler) {
  // Verify that the context is in a valid state.
  if (!CONTEXT_STATE_VALID()) {
    DCHECK(false) << "context not valid";
    return nullptr;
  }

  if (!other.get()) {
    return nullptr;
  }

  HoneycombRequestContextImpl::Config config;
  config.other = static_cast<HoneycombRequestContextImpl*>(other.get());
  config.handler = handler;
  config.unique_id = g_next_id.GetNext();
  return HoneycombRequestContextImpl::GetOrCreateRequestContext(config);
}

// HoneycombRequestContextImpl

HoneycombRequestContextImpl::~HoneycombRequestContextImpl() {
  HONEYCOMB_REQUIRE_UIT();

  if (browser_context_) {
    // May result in |browser_context_| being deleted if no other
    // HoneycombRequestContextImpl are referencing it.
    browser_context_->RemoveHoneycombRequestContext(this);
  }
}

// static
HoneycombRefPtr<HoneycombRequestContextImpl>
HoneycombRequestContextImpl::CreateGlobalRequestContext(
    const HoneycombRequestContextSettings& settings) {
  // Create and initialize the global context immediately.
  Config config;
  config.is_global = true;
  config.settings = settings;
  HoneycombRefPtr<HoneycombRequestContextImpl> impl = new HoneycombRequestContextImpl(config);
  impl->Initialize();
  return impl;
}

// static
HoneycombRefPtr<HoneycombRequestContextImpl>
HoneycombRequestContextImpl::GetOrCreateForRequestContext(
    HoneycombRefPtr<HoneycombRequestContext> request_context) {
  if (request_context.get()) {
    // Use the context from the provided HoneycombRequestContext.
    return static_cast<HoneycombRequestContextImpl*>(request_context.get());
  }

  // Use the global context.
  Config config;
  config.is_global = true;
  return HoneycombRequestContextImpl::GetOrCreateRequestContext(config);
}

bool HoneycombRequestContextImpl::VerifyBrowserContext() const {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    DCHECK(false) << "called on invalid thread";
    return false;
  }

  if (!browser_context() || !browser_context()->IsInitialized()) {
    DCHECK(false) << "Uninitialized context";
    return false;
  }

  return true;
}

HoneycombBrowserContext* HoneycombRequestContextImpl::GetBrowserContext() {
  CHECK(VerifyBrowserContext());
  return browser_context();
}

void HoneycombRequestContextImpl::ExecuteWhenBrowserContextInitialized(
    base::OnceClosure callback) {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(
        HONEYCOMB_UIT,
        base::BindOnce(
            &HoneycombRequestContextImpl::ExecuteWhenBrowserContextInitialized, this,
            std::move(callback)));
    return;
  }

  EnsureBrowserContext();
  browser_context()->StoreOrTriggerInitCallback(std::move(callback));
}

void HoneycombRequestContextImpl::GetBrowserContext(
    scoped_refptr<base::SingleThreadTaskRunner> task_runner,
    BrowserContextCallback callback) {
  if (!task_runner.get()) {
    task_runner = HoneycombTaskRunnerImpl::GetCurrentTaskRunner();
  }

  ExecuteWhenBrowserContextInitialized(base::BindOnce(
      [](HoneycombRefPtr<HoneycombRequestContextImpl> context,
         scoped_refptr<base::SingleThreadTaskRunner> task_runner,
         BrowserContextCallback callback) {
        HONEYCOMB_REQUIRE_UIT();

        auto browser_context = context->browser_context();
        DCHECK(browser_context->IsInitialized());

        if (task_runner->BelongsToCurrentThread()) {
          // Execute the callback immediately.
          std::move(callback).Run(browser_context->getter());
        } else {
          // Execute the callback on the target thread.
          task_runner->PostTask(
              FROM_HERE,
              base::BindOnce(std::move(callback), browser_context->getter()));
        }
      },
      HoneycombRefPtr<HoneycombRequestContextImpl>(this), task_runner,
      std::move(callback)));
}

bool HoneycombRequestContextImpl::IsSame(HoneycombRefPtr<HoneycombRequestContext> other) {
  HoneycombRequestContextImpl* other_impl =
      static_cast<HoneycombRequestContextImpl*>(other.get());
  if (!other_impl) {
    return false;
  }

  // Compare whether both are the global context.
  if (config_.is_global && other_impl->config_.is_global) {
    return true;
  }

  // Compare HoneycombBrowserContext pointers if one has been associated.
  if (browser_context() && other_impl->browser_context()) {
    return (browser_context() == other_impl->browser_context());
  } else if (browser_context() || other_impl->browser_context()) {
    return false;
  }

  // Otherwise compare unique IDs.
  return (config_.unique_id == other_impl->config_.unique_id);
}

bool HoneycombRequestContextImpl::IsSharingWith(HoneycombRefPtr<HoneycombRequestContext> other) {
  HoneycombRequestContextImpl* other_impl =
      static_cast<HoneycombRequestContextImpl*>(other.get());
  if (!other_impl) {
    return false;
  }

  if (IsSame(other)) {
    return true;
  }

  HoneycombRefPtr<HoneycombRequestContext> pending_other = config_.other;
  if (pending_other.get()) {
    // This object is not initialized but we know what context this object will
    // share with. Compare to that other context instead.
    return pending_other->IsSharingWith(other);
  }

  pending_other = other_impl->config_.other;
  if (pending_other.get()) {
    // The other object is not initialized but we know what context that object
    // will share with. Compare to that other context instead.
    return pending_other->IsSharingWith(this);
  }

  // This or the other object is not initialized. Compare the cache path values.
  // If both are non-empty and the same then they'll share the same storage.
  if (config_.settings.cache_path.length > 0 &&
      other_impl->config_.settings.cache_path.length > 0) {
    return (
        base::FilePath(HoneycombString(&config_.settings.cache_path)) ==
        base::FilePath(HoneycombString(&other_impl->config_.settings.cache_path)));
  }

  return false;
}

bool HoneycombRequestContextImpl::IsGlobal() {
  return config_.is_global;
}

HoneycombRefPtr<HoneycombRequestContextHandler> HoneycombRequestContextImpl::GetHandler() {
  return config_.handler;
}

HoneycombString HoneycombRequestContextImpl::GetCachePath() {
  return HoneycombString(&config_.settings.cache_path);
}

HoneycombRefPtr<HoneycombCookieManager> HoneycombRequestContextImpl::GetCookieManager(
    HoneycombRefPtr<HoneycombCompletionCallback> callback) {
  HoneycombRefPtr<HoneycombCookieManagerImpl> cookie_manager = new HoneycombCookieManagerImpl();
  InitializeCookieManagerInternal(cookie_manager, callback);
  return cookie_manager.get();
}

bool HoneycombRequestContextImpl::RegisterSchemeHandlerFactory(
    const HoneycombString& scheme_name,
    const HoneycombString& domain_name,
    HoneycombRefPtr<HoneycombSchemeHandlerFactory> factory) {
  GetBrowserContext(
      content::GetUIThreadTaskRunner({}),
      base::BindOnce(
          [](const HoneycombString& scheme_name, const HoneycombString& domain_name,
             HoneycombRefPtr<HoneycombSchemeHandlerFactory> factory,
             HoneycombBrowserContext::Getter browser_context_getter) {
            auto browser_context = browser_context_getter.Run();
            if (browser_context) {
              browser_context->RegisterSchemeHandlerFactory(
                  scheme_name, domain_name, factory);
            }
          },
          scheme_name, domain_name, factory));

  return true;
}

bool HoneycombRequestContextImpl::ClearSchemeHandlerFactories() {
  GetBrowserContext(
      content::GetUIThreadTaskRunner({}),
      base::BindOnce([](HoneycombBrowserContext::Getter browser_context_getter) {
        auto browser_context = browser_context_getter.Run();
        if (browser_context) {
          browser_context->ClearSchemeHandlerFactories();
        }
      }));

  return true;
}

bool HoneycombRequestContextImpl::HasPreference(const HoneycombString& name) {
  if (!VerifyBrowserContext()) {
    return false;
  }

  PrefService* pref_service = browser_context()->AsProfile()->GetPrefs();
  return pref_helper::HasPreference(pref_service, name);
}

HoneycombRefPtr<HoneycombValue> HoneycombRequestContextImpl::GetPreference(
    const HoneycombString& name) {
  if (!VerifyBrowserContext()) {
    return nullptr;
  }

  PrefService* pref_service = browser_context()->AsProfile()->GetPrefs();
  return pref_helper::GetPreference(pref_service, name);
}

HoneycombRefPtr<HoneycombDictionaryValue> HoneycombRequestContextImpl::GetAllPreferences(
    bool include_defaults) {
  if (!VerifyBrowserContext()) {
    return nullptr;
  }

  PrefService* pref_service = browser_context()->AsProfile()->GetPrefs();
  return pref_helper::GetAllPreferences(pref_service, include_defaults);
}

bool HoneycombRequestContextImpl::CanSetPreference(const HoneycombString& name) {
  if (!VerifyBrowserContext()) {
    return false;
  }

  PrefService* pref_service = browser_context()->AsProfile()->GetPrefs();
  return pref_helper::CanSetPreference(pref_service, name);
}

bool HoneycombRequestContextImpl::SetPreference(const HoneycombString& name,
                                          HoneycombRefPtr<HoneycombValue> value,
                                          HoneycombString& error) {
  if (!VerifyBrowserContext()) {
    return false;
  }

  PrefService* pref_service = browser_context()->AsProfile()->GetPrefs();
  return pref_helper::SetPreference(pref_service, name, value, error);
}

void HoneycombRequestContextImpl::ClearCertificateExceptions(
    HoneycombRefPtr<HoneycombCompletionCallback> callback) {
  GetBrowserContext(
      content::GetUIThreadTaskRunner({}),
      base::BindOnce(&HoneycombRequestContextImpl::ClearCertificateExceptionsInternal,
                     this, callback));
}

void HoneycombRequestContextImpl::ClearHttpAuthCredentials(
    HoneycombRefPtr<HoneycombCompletionCallback> callback) {
  GetBrowserContext(
      content::GetUIThreadTaskRunner({}),
      base::BindOnce(&HoneycombRequestContextImpl::ClearHttpAuthCredentialsInternal,
                     this, callback));
}

void HoneycombRequestContextImpl::CloseAllConnections(
    HoneycombRefPtr<HoneycombCompletionCallback> callback) {
  GetBrowserContext(
      content::GetUIThreadTaskRunner({}),
      base::BindOnce(&HoneycombRequestContextImpl::CloseAllConnectionsInternal, this,
                     callback));
}

void HoneycombRequestContextImpl::ResolveHost(
    const HoneycombString& origin,
    HoneycombRefPtr<HoneycombResolveCallback> callback) {
  GetBrowserContext(content::GetUIThreadTaskRunner({}),
                    base::BindOnce(&HoneycombRequestContextImpl::ResolveHostInternal,
                                   this, origin, callback));
}

void HoneycombRequestContextImpl::LoadExtension(
    const HoneycombString& root_directory,
    HoneycombRefPtr<HoneycombDictionaryValue> manifest,
    HoneycombRefPtr<HoneycombExtensionHandler> handler) {
  GetBrowserContext(content::GetUIThreadTaskRunner({}),
                    base::BindOnce(
                        [](const HoneycombString& root_directory,
                           HoneycombRefPtr<HoneycombDictionaryValue> manifest,
                           HoneycombRefPtr<HoneycombExtensionHandler> handler,
                           HoneycombRefPtr<HoneycombRequestContextImpl> self,
                           HoneycombBrowserContext::Getter browser_context_getter) {
                          auto browser_context = browser_context_getter.Run();
                          if (browser_context) {
                            browser_context->LoadExtension(
                                root_directory, manifest, handler, self);
                          }
                        },
                        root_directory, manifest, handler,
                        HoneycombRefPtr<HoneycombRequestContextImpl>(this)));
}

bool HoneycombRequestContextImpl::DidLoadExtension(const HoneycombString& extension_id) {
  HoneycombRefPtr<HoneycombExtension> extension = GetExtension(extension_id);
  // GetLoaderContext() will return NULL for internal extensions.
  return extension && IsSame(extension->GetLoaderContext());
}

bool HoneycombRequestContextImpl::HasExtension(const HoneycombString& extension_id) {
  return !!GetExtension(extension_id);
}

bool HoneycombRequestContextImpl::GetExtensions(
    std::vector<HoneycombString>& extension_ids) {
  extension_ids.clear();

  if (!VerifyBrowserContext()) {
    return false;
  }

  return browser_context()->GetExtensions(extension_ids);
}

HoneycombRefPtr<HoneycombExtension> HoneycombRequestContextImpl::GetExtension(
    const HoneycombString& extension_id) {
  if (!VerifyBrowserContext()) {
    return nullptr;
  }

  return browser_context()->GetExtension(extension_id);
}

HoneycombRefPtr<HoneycombMediaRouter> HoneycombRequestContextImpl::GetMediaRouter(
    HoneycombRefPtr<HoneycombCompletionCallback> callback) {
  HoneycombRefPtr<HoneycombMediaRouterImpl> media_router = new HoneycombMediaRouterImpl();
  InitializeMediaRouterInternal(media_router, callback);
  return media_router.get();
}

HoneycombRefPtr<HoneycombValue> HoneycombRequestContextImpl::GetWebsiteSetting(
    const HoneycombString& requesting_url,
    const HoneycombString& top_level_url,
    honey_content_setting_types_t content_type) {
  if (!VerifyBrowserContext()) {
    return nullptr;
  }

  auto* settings_map = HostContentSettingsMapFactory::GetForProfile(
      browser_context()->AsProfile());
  if (!settings_map) {
    return nullptr;
  }

  // Either or both URLs may be invalid.
  GURL requesting_gurl(requesting_url.ToString());
  GURL top_level_gurl(top_level_url.ToString());

  content_settings::SettingInfo info;
  base::Value value = settings_map->GetWebsiteSetting(
      requesting_gurl, top_level_gurl,
      static_cast<ContentSettingsType>(content_type), &info);
  if (value.is_none()) {
    return nullptr;
  }

  return new HoneycombValueImpl(std::move(value));
}

void HoneycombRequestContextImpl::SetWebsiteSetting(
    const HoneycombString& requesting_url,
    const HoneycombString& top_level_url,
    honey_content_setting_types_t content_type,
    HoneycombRefPtr<HoneycombValue> value) {
  GetBrowserContext(
      content::GetUIThreadTaskRunner({}),
      base::BindOnce(&HoneycombRequestContextImpl::SetWebsiteSettingInternal, this,
                     requesting_url, top_level_url, content_type, value));
}

honey_content_setting_values_t HoneycombRequestContextImpl::GetContentSetting(
    const HoneycombString& requesting_url,
    const HoneycombString& top_level_url,
    honey_content_setting_types_t content_type) {
  // Verify that our enums match Chromium's values.
  static_assert(static_cast<int>(HONEYCOMB_CONTENT_SETTING_TYPE_NUM_TYPES) ==
                    static_cast<int>(ContentSettingsType::NUM_TYPES),
                "Mismatched enum found for HONEYCOMB_CONTENT_SETTING_TYPE_NUM_TYPES");
  static_assert(
      static_cast<int>(HONEYCOMB_CONTENT_SETTING_VALUE_NUM_VALUES) ==
          static_cast<int>(CONTENT_SETTING_NUM_SETTINGS),
      "Mismatched enum found for HONEYCOMB_CONTENT_SETTING_VALUE_NUM_VALUES");

  if (!VerifyBrowserContext()) {
    return HONEYCOMB_CONTENT_SETTING_VALUE_DEFAULT;
  }

  auto* settings_map = HostContentSettingsMapFactory::GetForProfile(
      browser_context()->AsProfile());
  if (!settings_map) {
    return HONEYCOMB_CONTENT_SETTING_VALUE_DEFAULT;
  }

  ContentSetting value = ContentSetting::CONTENT_SETTING_DEFAULT;

  if (requesting_url.empty() && top_level_url.empty()) {
    value = settings_map->GetDefaultContentSetting(
        static_cast<ContentSettingsType>(content_type),
        /*provider_id=*/nullptr);
  } else {
    GURL requesting_gurl(requesting_url.ToString());
    GURL top_level_gurl(top_level_url.ToString());
    if (requesting_gurl.is_valid() || top_level_gurl.is_valid()) {
      value = settings_map->GetContentSetting(
          requesting_gurl, top_level_gurl,
          static_cast<ContentSettingsType>(content_type));
    }
  }

  return static_cast<honey_content_setting_values_t>(value);
}

void HoneycombRequestContextImpl::SetContentSetting(
    const HoneycombString& requesting_url,
    const HoneycombString& top_level_url,
    honey_content_setting_types_t content_type,
    honey_content_setting_values_t value) {
  GetBrowserContext(
      content::GetUIThreadTaskRunner({}),
      base::BindOnce(&HoneycombRequestContextImpl::SetContentSettingInternal, this,
                     requesting_url, top_level_url, content_type, value));
}

void HoneycombRequestContextImpl::OnRenderFrameCreated(
    const content::GlobalRenderFrameHostId& global_id,
    bool is_main_frame,
    bool is_guest_view) {
  browser_context_->OnRenderFrameCreated(this, global_id, is_main_frame,
                                         is_guest_view);
}

void HoneycombRequestContextImpl::OnRenderFrameDeleted(
    const content::GlobalRenderFrameHostId& global_id,
    bool is_main_frame,
    bool is_guest_view) {
  browser_context_->OnRenderFrameDeleted(this, global_id, is_main_frame,
                                         is_guest_view);
}

// static
HoneycombRefPtr<HoneycombRequestContextImpl>
HoneycombRequestContextImpl::GetOrCreateRequestContext(const Config& config) {
  if (config.is_global ||
      (config.other && config.other->IsGlobal() && !config.handler)) {
    // Return the singleton global context.
    return static_cast<HoneycombRequestContextImpl*>(
        HoneycombAppManager::Get()->GetGlobalRequestContext().get());
  }

  // The new context will be initialized later by EnsureBrowserContext().
  HoneycombRefPtr<HoneycombRequestContextImpl> context = new HoneycombRequestContextImpl(config);

  // Initialize ASAP so that any tasks blocked on initialization will execute.
  if (HONEYCOMB_CURRENTLY_ON_UIT()) {
    context->Initialize();
  } else {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT,
                  base::BindOnce(&HoneycombRequestContextImpl::Initialize, context));
  }

  return context;
}

HoneycombRequestContextImpl::HoneycombRequestContextImpl(
    const HoneycombRequestContextImpl::Config& config)
    : config_(config) {}

void HoneycombRequestContextImpl::Initialize() {
  HONEYCOMB_REQUIRE_UIT();

  DCHECK(!browser_context_);

  if (config_.other) {
    // Share storage with |config_.other|.
    browser_context_ = config_.other->browser_context();
    CHECK(browser_context_);
  }

  if (!browser_context_) {
    if (!config_.is_global) {
      // User-specified settings need to be normalized.
      HoneycombContext::Get()->NormalizeRequestContextSettings(&config_.settings);
    }

    const base::FilePath& cache_path =
        base::FilePath(HoneycombString(&config_.settings.cache_path));
    if (!cache_path.empty()) {
      // Check if a HoneycombBrowserContext is already globally registered for
      // the specified cache path. If so then use it.
      browser_context_ = HoneycombBrowserContext::FromCachePath(cache_path);
    }
  }

  auto initialized_cb =
      base::BindOnce(&HoneycombRequestContextImpl::BrowserContextInitialized, this);

  if (!browser_context_) {
    // Create a new HoneycombBrowserContext instance. If the cache path is non-
    // empty then this new instance will become the globally registered
    // HoneycombBrowserContext for that path. Otherwise, this new instance will
    // be a completely isolated "incognito mode" context.
    browser_context_ = HoneycombAppManager::Get()->CreateNewBrowserContext(
        config_.settings, std::move(initialized_cb));
  } else {
    // Share the same settings as the existing context.
    config_.settings = browser_context_->settings();
    browser_context_->StoreOrTriggerInitCallback(std::move(initialized_cb));
  }

  // We'll disassociate from |browser_context_| on destruction.
  browser_context_->AddHoneycombRequestContext(this);

  if (config_.other) {
    // Clear the reference to |config_.other| after setting
    // |request_context_getter_|. This is the reverse order of checks in
    // IsSharedWith().
    config_.other = nullptr;
  }
}

void HoneycombRequestContextImpl::BrowserContextInitialized() {
  if (config_.handler) {
    // Always execute asynchronously so the current call stack can unwind.
    HONEYCOMB_POST_TASK(
        HONEYCOMB_UIT,
        base::BindOnce(&HoneycombRequestContextHandler::OnRequestContextInitialized,
                       config_.handler, HoneycombRefPtr<HoneycombRequestContext>(this)));
  }
}

void HoneycombRequestContextImpl::EnsureBrowserContext() {
  HONEYCOMB_REQUIRE_UIT();
  if (!browser_context()) {
    Initialize();
  }
  DCHECK(browser_context());
}

void HoneycombRequestContextImpl::ClearCertificateExceptionsInternal(
    HoneycombRefPtr<HoneycombCompletionCallback> callback,
    HoneycombBrowserContext::Getter browser_context_getter) {
  auto browser_context = browser_context_getter.Run();
  if (!browser_context) {
    return;
  }

  content::SSLHostStateDelegate* ssl_delegate =
      browser_context->AsBrowserContext()->GetSSLHostStateDelegate();
  if (ssl_delegate) {
    ssl_delegate->Clear(base::NullCallback());
  }

  if (callback) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT,
                  base::BindOnce(&HoneycombCompletionCallback::OnComplete, callback));
  }
}

void HoneycombRequestContextImpl::ClearHttpAuthCredentialsInternal(
    HoneycombRefPtr<HoneycombCompletionCallback> callback,
    HoneycombBrowserContext::Getter browser_context_getter) {
  auto browser_context = browser_context_getter.Run();
  if (!browser_context) {
    return;
  }

  browser_context->GetNetworkContext()->ClearHttpAuthCache(
      /*start_time=*/base::Time(), /*end_time=*/base::Time::Max(),
      base::BindOnce(&HoneycombCompletionCallback::OnComplete, callback));
}

void HoneycombRequestContextImpl::CloseAllConnectionsInternal(
    HoneycombRefPtr<HoneycombCompletionCallback> callback,
    HoneycombBrowserContext::Getter browser_context_getter) {
  auto browser_context = browser_context_getter.Run();
  if (!browser_context) {
    return;
  }

  browser_context->GetNetworkContext()->CloseAllConnections(
      base::BindOnce(&HoneycombCompletionCallback::OnComplete, callback));
}

void HoneycombRequestContextImpl::ResolveHostInternal(
    const HoneycombString& origin,
    HoneycombRefPtr<HoneycombResolveCallback> callback,
    HoneycombBrowserContext::Getter browser_context_getter) {
  auto browser_context = browser_context_getter.Run();
  if (!browser_context) {
    return;
  }

  // |helper| will be deleted in ResolveHostHelper::OnComplete().
  ResolveHostHelper* helper = new ResolveHostHelper(callback);
  helper->Start(browser_context, origin);
}

void HoneycombRequestContextImpl::SetWebsiteSettingInternal(
    const HoneycombString& requesting_url,
    const HoneycombString& top_level_url,
    honey_content_setting_types_t content_type,
    HoneycombRefPtr<HoneycombValue> value,
    HoneycombBrowserContext::Getter browser_context_getter) {
  auto browser_context = browser_context_getter.Run();
  if (!browser_context) {
    return;
  }

  auto* settings_map = HostContentSettingsMapFactory::GetForProfile(
      browser_context->AsProfile());
  if (!settings_map) {
    return;
  }

  // Starts as a NONE value.
  base::Value new_value;
  if (value && value->IsValid()) {
    new_value = static_cast<HoneycombValueImpl*>(value.get())->CopyValue();
  }

  if (requesting_url.empty() && top_level_url.empty()) {
    settings_map->SetWebsiteSettingCustomScope(
        ContentSettingsPattern::Wildcard(), ContentSettingsPattern::Wildcard(),
        static_cast<ContentSettingsType>(content_type), std::move(new_value));
  } else {
    GURL requesting_gurl(requesting_url.ToString());
    GURL top_level_gurl(top_level_url.ToString());
    if (requesting_gurl.is_valid() || top_level_gurl.is_valid()) {
      settings_map->SetWebsiteSettingDefaultScope(
          requesting_gurl, top_level_gurl,
          static_cast<ContentSettingsType>(content_type), std::move(new_value));
    }
  }
}

void HoneycombRequestContextImpl::SetContentSettingInternal(
    const HoneycombString& requesting_url,
    const HoneycombString& top_level_url,
    honey_content_setting_types_t content_type,
    honey_content_setting_values_t value,
    HoneycombBrowserContext::Getter browser_context_getter) {
  auto browser_context = browser_context_getter.Run();
  if (!browser_context) {
    return;
  }

  auto* settings_map = HostContentSettingsMapFactory::GetForProfile(
      browser_context->AsProfile());
  if (!settings_map) {
    return;
  }

  if (requesting_url.empty() && top_level_url.empty()) {
    settings_map->SetDefaultContentSetting(
        static_cast<ContentSettingsType>(content_type),
        static_cast<ContentSetting>(value));
  } else {
    GURL requesting_gurl(requesting_url.ToString());
    GURL top_level_gurl(top_level_url.ToString());
    if (requesting_gurl.is_valid() || top_level_gurl.is_valid()) {
      settings_map->SetContentSettingDefaultScope(
          requesting_gurl, top_level_gurl,
          static_cast<ContentSettingsType>(content_type),
          static_cast<ContentSetting>(value));
    }
  }
}

void HoneycombRequestContextImpl::InitializeCookieManagerInternal(
    HoneycombRefPtr<HoneycombCookieManagerImpl> cookie_manager,
    HoneycombRefPtr<HoneycombCompletionCallback> callback) {
  GetBrowserContext(content::GetUIThreadTaskRunner({}),
                    base::BindOnce(
                        [](HoneycombRefPtr<HoneycombCookieManagerImpl> cookie_manager,
                           HoneycombRefPtr<HoneycombCompletionCallback> callback,
                           HoneycombBrowserContext::Getter browser_context_getter) {
                          cookie_manager->Initialize(browser_context_getter,
                                                     callback);
                        },
                        cookie_manager, callback));
}

void HoneycombRequestContextImpl::InitializeMediaRouterInternal(
    HoneycombRefPtr<HoneycombMediaRouterImpl> media_router,
    HoneycombRefPtr<HoneycombCompletionCallback> callback) {
  GetBrowserContext(content::GetUIThreadTaskRunner({}),
                    base::BindOnce(
                        [](HoneycombRefPtr<HoneycombMediaRouterImpl> media_router,
                           HoneycombRefPtr<HoneycombCompletionCallback> callback,
                           HoneycombBrowserContext::Getter browser_context_getter) {
                          media_router->Initialize(browser_context_getter,
                                                   callback);
                        },
                        media_router, callback));
}

HoneycombBrowserContext* HoneycombRequestContextImpl::browser_context() const {
  return browser_context_;
}
