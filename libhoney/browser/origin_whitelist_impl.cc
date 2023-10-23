// Copyright (c) 2011 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoney/browser/origin_whitelist_impl.h"

#include <string>
#include <vector>

#include "include/honey_origin_whitelist.h"
#include "libhoney/browser/browser_manager.h"
#include "libhoney/browser/context.h"
#include "libhoney/browser/thread_util.h"

#include "base/functional/bind.h"
#include "base/lazy_instance.h"
#include "base/synchronization/lock.h"
#include "honey/libhoney/common/mojom/honey.mojom.h"
#include "chrome/common/webui_url_constants.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/common/url_constants.h"
#include "extensions/common/constants.h"
#include "url/gurl.h"
#include "url/origin.h"

namespace {

// Class that manages cross-origin whitelist registrations.
class HoneycombOriginWhitelistManager {
 public:
  HoneycombOriginWhitelistManager() = default;

  HoneycombOriginWhitelistManager(const HoneycombOriginWhitelistManager&) = delete;
  HoneycombOriginWhitelistManager& operator=(const HoneycombOriginWhitelistManager&) =
      delete;

  // Retrieve the singleton instance.
  static HoneycombOriginWhitelistManager* GetInstance();

  bool AddOriginEntry(const std::string& source_origin,
                      const std::string& target_protocol,
                      const std::string& target_domain,
                      bool allow_target_subdomains) {
    auto info = honey::mojom::CrossOriginWhiteListEntry::New();
    info->source_origin = source_origin;
    info->target_protocol = target_protocol;
    info->target_domain = target_domain;
    info->allow_target_subdomains = allow_target_subdomains;

    {
      base::AutoLock lock_scope(lock_);

      // Verify that the origin entry doesn't already exist.
      for (const auto& entry : origin_list_) {
        if (entry == info) {
          return false;
        }
      }

      origin_list_.push_back(info->Clone());
    }

    SendModifyCrossOriginWhitelistEntry(true, info);
    return true;
  }

  bool RemoveOriginEntry(const std::string& source_origin,
                         const std::string& target_protocol,
                         const std::string& target_domain,
                         bool allow_target_subdomains) {
    auto info = honey::mojom::CrossOriginWhiteListEntry::New();
    info->source_origin = source_origin;
    info->target_protocol = target_protocol;
    info->target_domain = target_domain;
    info->allow_target_subdomains = allow_target_subdomains;

    bool found = false;

    {
      base::AutoLock lock_scope(lock_);

      CrossOriginWhiteList::iterator it = origin_list_.begin();
      for (; it != origin_list_.end(); ++it) {
        if (*it == info) {
          origin_list_.erase(it);
          found = true;
          break;
        }
      }
    }

    if (!found) {
      return false;
    }

    SendModifyCrossOriginWhitelistEntry(false, info);
    return true;
  }

  void ClearOrigins() {
    {
      base::AutoLock lock_scope(lock_);
      origin_list_.clear();
    }

    SendClearCrossOriginWhitelist();
  }

  void GetCrossOriginWhitelistEntries(
      absl::optional<CrossOriginWhiteList>* entries) const {
    base::AutoLock lock_scope(lock_);

    if (!origin_list_.empty()) {
      CrossOriginWhiteList vec;
      for (const auto& entry : origin_list_) {
        vec.push_back(entry->Clone());
      }
      *entries = std::move(vec);
    }
  }

  bool HasCrossOriginWhitelistEntry(const url::Origin& source,
                                    const url::Origin& target) const {
    base::AutoLock lock_scope(lock_);

    if (!origin_list_.empty()) {
      for (const auto& entry : origin_list_) {
        if (IsMatch(source, target, entry)) {
          return true;
        }
      }
    }

    return false;
  }

 private:
  // Send the modify cross-origin whitelist entry message to all currently
  // existing hosts.
  static void SendModifyCrossOriginWhitelistEntry(
      bool add,
      const honey::mojom::CrossOriginWhiteListEntryPtr& info) {
    HONEYCOMB_REQUIRE_UIT();

    content::RenderProcessHost::iterator i(
        content::RenderProcessHost::AllHostsIterator());
    for (; !i.IsAtEnd(); i.Advance()) {
      auto render_manager =
          HoneycombBrowserManager::GetRenderManagerForProcess(i.GetCurrentValue());
      render_manager->ModifyCrossOriginWhitelistEntry(add, info->Clone());
    }
  }

  // Send the clear cross-origin whitelists message to all currently existing
  // hosts.
  static void SendClearCrossOriginWhitelist() {
    HONEYCOMB_REQUIRE_UIT();

    content::RenderProcessHost::iterator i(
        content::RenderProcessHost::AllHostsIterator());
    for (; !i.IsAtEnd(); i.Advance()) {
      auto render_manager =
          HoneycombBrowserManager::GetRenderManagerForProcess(i.GetCurrentValue());
      render_manager->ClearCrossOriginWhitelist();
    }
  }

  static bool IsMatch(const url::Origin& source_origin,
                      const url::Origin& target_origin,
                      const honey::mojom::CrossOriginWhiteListEntryPtr& param) {
    if (!source_origin.IsSameOriginWith(
            url::Origin::Create(GURL(param->source_origin)))) {
      // Source origin does not match.
      return false;
    }

    if (target_origin.scheme() != param->target_protocol) {
      // Target scheme does not match.
      return false;
    }

    if (param->allow_target_subdomains) {
      if (param->target_domain.empty()) {
        // Any domain will match.
        return true;
      } else {
        // Match sub-domains.
        return target_origin.DomainIs(param->target_domain.c_str());
      }
    } else {
      // Match full domain.
      return (target_origin.host() == param->target_domain);
    }
  }

  mutable base::Lock lock_;

  // List of registered origins. Access must be protected by |lock_|.
  CrossOriginWhiteList origin_list_;
};

base::LazyInstance<HoneycombOriginWhitelistManager>::Leaky g_manager =
    LAZY_INSTANCE_INITIALIZER;

HoneycombOriginWhitelistManager* HoneycombOriginWhitelistManager::GetInstance() {
  return g_manager.Pointer();
}

}  // namespace

bool HoneycombAddCrossOriginWhitelistEntry(const HoneycombString& source_origin,
                                     const HoneycombString& target_protocol,
                                     const HoneycombString& target_domain,
                                     bool allow_target_subdomains) {
  // Verify that the context is in a valid state.
  if (!CONTEXT_STATE_VALID()) {
    DCHECK(false);
    return false;
  }

  std::string source_url = source_origin;
  GURL gurl = GURL(source_url);
  if (gurl.is_empty() || !gurl.is_valid()) {
    DCHECK(false) << "Invalid source_origin URL: " << source_url;
    return false;
  }

  if (HONEYCOMB_CURRENTLY_ON_UIT()) {
    return HoneycombOriginWhitelistManager::GetInstance()->AddOriginEntry(
        source_origin, target_protocol, target_domain, allow_target_subdomains);
  } else {
    HONEYCOMB_POST_TASK(
        HONEYCOMB_UIT,
        base::BindOnce(base::IgnoreResult(&HoneycombAddCrossOriginWhitelistEntry),
                       source_origin, target_protocol, target_domain,
                       allow_target_subdomains));
  }

  return true;
}

bool HoneycombRemoveCrossOriginWhitelistEntry(const HoneycombString& source_origin,
                                        const HoneycombString& target_protocol,
                                        const HoneycombString& target_domain,
                                        bool allow_target_subdomains) {
  // Verify that the context is in a valid state.
  if (!CONTEXT_STATE_VALID()) {
    DCHECK(false);
    return false;
  }

  std::string source_url = source_origin;
  GURL gurl = GURL(source_url);
  if (gurl.is_empty() || !gurl.is_valid()) {
    DCHECK(false) << "Invalid source_origin URL: " << source_url;
    return false;
  }

  if (HONEYCOMB_CURRENTLY_ON_UIT()) {
    return HoneycombOriginWhitelistManager::GetInstance()->RemoveOriginEntry(
        source_origin, target_protocol, target_domain, allow_target_subdomains);
  } else {
    HONEYCOMB_POST_TASK(
        HONEYCOMB_UIT,
        base::BindOnce(base::IgnoreResult(&HoneycombRemoveCrossOriginWhitelistEntry),
                       source_origin, target_protocol, target_domain,
                       allow_target_subdomains));
  }

  return true;
}

bool HoneycombClearCrossOriginWhitelist() {
  // Verify that the context is in a valid state.
  if (!CONTEXT_STATE_VALID()) {
    DCHECK(false);
    return false;
  }

  if (HONEYCOMB_CURRENTLY_ON_UIT()) {
    HoneycombOriginWhitelistManager::GetInstance()->ClearOrigins();
  } else {
    HONEYCOMB_POST_TASK(
        HONEYCOMB_UIT,
        base::BindOnce(base::IgnoreResult(&HoneycombClearCrossOriginWhitelist)));
  }

  return true;
}

void GetCrossOriginWhitelistEntries(
    absl::optional<CrossOriginWhiteList>* entries) {
  HoneycombOriginWhitelistManager::GetInstance()->GetCrossOriginWhitelistEntries(
      entries);
}

bool HasCrossOriginWhitelistEntry(const url::Origin& source,
                                  const url::Origin& target) {
  // Components of chrome that are implemented as extensions or platform apps
  // are allowed to use chrome://resources/ and chrome://theme/ URLs.
  // See also RegisterNonNetworkSubresourceURLLoaderFactories.
  if (source.scheme() == extensions::kExtensionScheme &&
      target.scheme() == content::kChromeUIScheme &&
      (target.host() == chrome::kChromeUIThemeHost ||
       target.host() == content::kChromeUIResourcesHost)) {
    return true;
  }

  return HoneycombOriginWhitelistManager::GetInstance()->HasCrossOriginWhitelistEntry(
      source, target);
}
