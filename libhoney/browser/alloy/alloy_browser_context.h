// Copyright (c) 2011 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_ALLOY_ALLOY_BROWSER_CONTEXT_H_
#define HONEYCOMB_LIBHONEY_BROWSER_ALLOY_ALLOY_BROWSER_CONTEXT_H_
#pragma once

#include "include/honey_request_context_handler.h"
#include "libhoney/browser/alloy/chrome_profile_alloy.h"
#include "libhoney/browser/browser_context.h"
#include "libhoney/browser/request_context_handler_map.h"

#include "chrome/browser/download/download_prefs.h"
#include "components/proxy_config/pref_proxy_config_tracker.h"
#include "components/visitedlink/browser/visitedlink_delegate.h"

class HoneycombDownloadManagerDelegate;
class HoneycombSSLHostStateDelegate;
class HoneycombVisitedLinkListener;
class PrefService;

namespace extensions {
class HoneycombExtensionSystem;
}

namespace visitedlink {
class VisitedLinkWriter;
}

// See HoneycombBrowserContext documentation for usage. Only accessed on the UI thread
// unless otherwise indicated. ChromeProfileAlloy must be the first listed base
// class to avoid issues when casting between void* and content::BrowserContext*
// in Chromium code.
class AlloyBrowserContext : public ChromeProfileAlloy,
                            public HoneycombBrowserContext,
                            public visitedlink::VisitedLinkDelegate {
 public:
  explicit AlloyBrowserContext(const HoneycombRequestContextSettings& settings);

  AlloyBrowserContext(const AlloyBrowserContext&) = delete;
  AlloyBrowserContext& operator=(const AlloyBrowserContext&) = delete;

  // HoneycombBrowserContext overrides.
  content::BrowserContext* AsBrowserContext() override { return this; }
  Profile* AsProfile() override { return this; }
  bool IsInitialized() const override;
  void StoreOrTriggerInitCallback(base::OnceClosure callback) override;
  void Initialize() override;
  void Shutdown() override;
  void RemoveHoneycombRequestContext(HoneycombRequestContextImpl* context) override;
  void LoadExtension(const HoneycombString& root_directory,
                     HoneycombRefPtr<HoneycombDictionaryValue> manifest,
                     HoneycombRefPtr<HoneycombExtensionHandler> handler,
                     HoneycombRefPtr<HoneycombRequestContext> loader_context) override;
  bool GetExtensions(std::vector<HoneycombString>& extension_ids) override;
  HoneycombRefPtr<HoneycombExtension> GetExtension(const HoneycombString& extension_id) override;
  bool UnloadExtension(const HoneycombString& extension_id) override;
  bool IsPrintPreviewSupported() const override;

  // content::BrowserContext overrides.
  content::ResourceContext* GetResourceContext() override;
  content::ClientHintsControllerDelegate* GetClientHintsControllerDelegate()
      override;
  base::FilePath GetPath() override;
  base::FilePath GetPath() const override;
  std::unique_ptr<content::ZoomLevelDelegate> CreateZoomLevelDelegate(
      const base::FilePath& partition_path) override;
  content::DownloadManagerDelegate* GetDownloadManagerDelegate() override;
  content::BrowserPluginGuestManager* GetGuestManager() override;
  storage::SpecialStoragePolicy* GetSpecialStoragePolicy() override;
  content::PlatformNotificationService* GetPlatformNotificationService()
      override;
  content::PushMessagingService* GetPushMessagingService() override;
  content::StorageNotificationService* GetStorageNotificationService() override;
  content::SSLHostStateDelegate* GetSSLHostStateDelegate() override;
  content::PermissionControllerDelegate* GetPermissionControllerDelegate()
      override;
  content::BackgroundFetchDelegate* GetBackgroundFetchDelegate() override;
  content::BackgroundSyncController* GetBackgroundSyncController() override;
  content::BrowsingDataRemoverDelegate* GetBrowsingDataRemoverDelegate()
      override;
  content::ReduceAcceptLanguageControllerDelegate*
  GetReduceAcceptLanguageControllerDelegate() override;

  // Profile overrides.
  ChromeZoomLevelPrefs* GetZoomLevelPrefs() override;
  scoped_refptr<network::SharedURLLoaderFactory> GetURLLoaderFactory() override;
  PrefService* GetPrefs() override;
  bool AllowsBrowserWindows() const override { return false; }
  const PrefService* GetPrefs() const override;
  ProfileKey* GetProfileKey() const override;
  policy::SchemaRegistryService* GetPolicySchemaRegistryService() override;
  policy::UserCloudPolicyManager* GetUserCloudPolicyManager() override;
  policy::ProfileCloudPolicyManager* GetProfileCloudPolicyManager() override;
  policy::ProfilePolicyConnector* GetProfilePolicyConnector() override;
  const policy::ProfilePolicyConnector* GetProfilePolicyConnector()
      const override;
  bool IsNewProfile() const override;

  // Values checked in ProfileNetworkContextService::CreateNetworkContextParams
  // when creating the NetworkContext.
  bool ShouldRestoreOldSessionCookies() override {
    return ShouldPersistSessionCookies();
  }
  bool ShouldPersistSessionCookies() const override {
    return !!settings_.persist_session_cookies;
  }

  // visitedlink::VisitedLinkDelegate methods.
  void RebuildTable(const scoped_refptr<URLEnumerator>& enumerator) override;

  // Manages extensions.
  extensions::HoneycombExtensionSystem* extension_system() const {
    return extension_system_;
  }

  // Called from AlloyBrowserHostImpl::DidFinishNavigation to update the table
  // of visited links.
  void AddVisitedURLs(const std::vector<GURL>& urls);

  // Called from DownloadPrefs::FromBrowserContext.
  DownloadPrefs* GetDownloadPrefs();

 private:
  ~AlloyBrowserContext() override;

  std::unique_ptr<PrefService> pref_service_;
  std::unique_ptr<PrefProxyConfigTracker> pref_proxy_config_tracker_;

  std::unique_ptr<HoneycombDownloadManagerDelegate> download_manager_delegate_;
  std::unique_ptr<HoneycombSSLHostStateDelegate> ssl_host_state_delegate_;
  std::unique_ptr<visitedlink::VisitedLinkWriter> visitedlink_master_;
  // |visitedlink_listener_| is owned by visitedlink_master_.
  HoneycombVisitedLinkListener* visitedlink_listener_ = nullptr;

  // Owned by the KeyedService system.
  extensions::HoneycombExtensionSystem* extension_system_ = nullptr;

  // The key to index KeyedService instances created by
  // SimpleKeyedServiceFactory.
  std::unique_ptr<ProfileKey> key_;

  std::unique_ptr<DownloadPrefs> download_prefs_;

  std::unique_ptr<content::ResourceContext> resource_context_;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_ALLOY_ALLOY_BROWSER_CONTEXT_H_
