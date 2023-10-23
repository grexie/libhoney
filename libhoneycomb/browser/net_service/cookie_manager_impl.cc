// Copyright (c) 2019 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "libhoneycomb/browser/net_service/cookie_manager_impl.h"

#include "libhoneycomb/common/net_service/net_service_util.h"
#include "libhoneycomb/common/time_util.h"

#include "base/functional/bind.h"
#include "base/logging.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/storage_partition.h"
#include "services/network/public/mojom/cookie_manager.mojom.h"
#include "url/gurl.h"

using network::mojom::CookieManager;

namespace {

// Do not keep a reference to the object returned by this method.
HoneycombBrowserContext* GetBrowserContext(const HoneycombBrowserContext::Getter& getter) {
  HONEYCOMB_REQUIRE_UIT();
  DCHECK(!getter.is_null());

  // Will return nullptr if the BrowserContext has been shut down.
  return getter.Run();
}

// Do not keep a reference to the object returned by this method.
CookieManager* GetCookieManager(HoneycombBrowserContext* browser_context) {
  HONEYCOMB_REQUIRE_UIT();
  return browser_context->AsBrowserContext()
      ->GetDefaultStoragePartition()
      ->GetCookieManagerForBrowserProcess();
}

// Always execute the callback asynchronously.
void RunAsyncCompletionOnUIThread(HoneycombRefPtr<HoneycombCompletionCallback> callback) {
  if (!callback.get()) {
    return;
  }
  HONEYCOMB_POST_TASK(HONEYCOMB_UIT, base::BindOnce(&HoneycombCompletionCallback::OnComplete,
                                        callback.get()));
}

// Always execute the callback asynchronously.
void SetCookieCallbackImpl(HoneycombRefPtr<HoneycombSetCookieCallback> callback,
                           net::CookieAccessResult access_result) {
  if (!callback.get()) {
    return;
  }
  const bool is_include = access_result.status.IsInclude();
  if (!is_include) {
    LOG(WARNING) << "SetCookie failed with reason: "
                 << access_result.status.GetDebugString();
  }
  HONEYCOMB_POST_TASK(HONEYCOMB_UIT, base::BindOnce(&HoneycombSetCookieCallback::OnComplete,
                                        callback.get(), is_include));
}

// Always execute the callback asynchronously.
void DeleteCookiesCallbackImpl(HoneycombRefPtr<HoneycombDeleteCookiesCallback> callback,
                               uint32_t num_deleted) {
  if (!callback.get()) {
    return;
  }
  HONEYCOMB_POST_TASK(HONEYCOMB_UIT, base::BindOnce(&HoneycombDeleteCookiesCallback::OnComplete,
                                        callback.get(), num_deleted));
}

void ExecuteVisitor(HoneycombRefPtr<HoneycombCookieVisitor> visitor,
                    const HoneycombBrowserContext::Getter& browser_context_getter,
                    const std::vector<net::CanonicalCookie>& cookies) {
  HONEYCOMB_REQUIRE_UIT();

  auto browser_context = GetBrowserContext(browser_context_getter);
  if (!browser_context) {
    return;
  }

  auto cookie_manager = GetCookieManager(browser_context);

  int total = cookies.size(), count = 0;
  for (const auto& cc : cookies) {
    HoneycombCookie cookie;
    net_service::MakeHoneycombCookie(cc, cookie);

    bool deleteCookie = false;
    bool keepLooping = visitor->Visit(cookie, count, total, deleteCookie);
    if (deleteCookie) {
      cookie_manager->DeleteCanonicalCookie(
          cc, CookieManager::DeleteCanonicalCookieCallback());
    }
    if (!keepLooping) {
      break;
    }
    count++;
  }
}

// Always execute the callback asynchronously.
void GetAllCookiesCallbackImpl(
    HoneycombRefPtr<HoneycombCookieVisitor> visitor,
    const HoneycombBrowserContext::Getter& browser_context_getter,
    const net::CookieList& cookies) {
  HONEYCOMB_POST_TASK(HONEYCOMB_UIT, base::BindOnce(&ExecuteVisitor, visitor,
                                        browser_context_getter, cookies));
}

void GetCookiesCallbackImpl(
    HoneycombRefPtr<HoneycombCookieVisitor> visitor,
    const HoneycombBrowserContext::Getter& browser_context_getter,
    const net::CookieAccessResultList& include_cookies,
    const net::CookieAccessResultList&) {
  net::CookieList cookies;
  for (const auto& status : include_cookies) {
    cookies.push_back(status.cookie);
  }
  GetAllCookiesCallbackImpl(visitor, browser_context_getter, cookies);
}

}  // namespace

HoneycombCookieManagerImpl::HoneycombCookieManagerImpl() {}

void HoneycombCookieManagerImpl::Initialize(
    HoneycombBrowserContext::Getter browser_context_getter,
    HoneycombRefPtr<HoneycombCompletionCallback> callback) {
  HONEYCOMB_REQUIRE_UIT();
  DCHECK(!initialized_);
  DCHECK(!browser_context_getter.is_null());
  DCHECK(browser_context_getter_.is_null());
  browser_context_getter_ = browser_context_getter;

  initialized_ = true;
  if (!init_callbacks_.empty()) {
    for (auto& init_callback : init_callbacks_) {
      std::move(init_callback).Run();
    }
    init_callbacks_.clear();
  }

  RunAsyncCompletionOnUIThread(callback);
}

bool HoneycombCookieManagerImpl::VisitAllCookies(
    HoneycombRefPtr<HoneycombCookieVisitor> visitor) {
  if (!visitor.get()) {
    return false;
  }

  if (!ValidContext()) {
    StoreOrTriggerInitCallback(base::BindOnce(
        base::IgnoreResult(&HoneycombCookieManagerImpl::VisitAllCookiesInternal),
        this, visitor));
    return true;
  }

  return VisitAllCookiesInternal(visitor);
}

bool HoneycombCookieManagerImpl::VisitUrlCookies(
    const HoneycombString& url,
    bool includeHttpOnly,
    HoneycombRefPtr<HoneycombCookieVisitor> visitor) {
  if (!visitor.get()) {
    return false;
  }

  GURL gurl = GURL(url.ToString());
  if (!gurl.is_valid()) {
    return false;
  }

  if (!ValidContext()) {
    StoreOrTriggerInitCallback(base::BindOnce(
        base::IgnoreResult(&HoneycombCookieManagerImpl::VisitUrlCookiesInternal),
        this, gurl, includeHttpOnly, visitor));
    return true;
  }

  return VisitUrlCookiesInternal(gurl, includeHttpOnly, visitor);
}

bool HoneycombCookieManagerImpl::SetCookie(const HoneycombString& url,
                                     const HoneycombCookie& cookie,
                                     HoneycombRefPtr<HoneycombSetCookieCallback> callback) {
  GURL gurl = GURL(url.ToString());
  if (!gurl.is_valid()) {
    return false;
  }

  if (!ValidContext()) {
    StoreOrTriggerInitCallback(base::BindOnce(
        base::IgnoreResult(&HoneycombCookieManagerImpl::SetCookieInternal), this,
        gurl, cookie, callback));
    return true;
  }

  return SetCookieInternal(gurl, cookie, callback);
}

bool HoneycombCookieManagerImpl::DeleteCookies(
    const HoneycombString& url,
    const HoneycombString& cookie_name,
    HoneycombRefPtr<HoneycombDeleteCookiesCallback> callback) {
  // Empty URLs are allowed but not invalid URLs.
  GURL gurl = GURL(url.ToString());
  if (!gurl.is_empty() && !gurl.is_valid()) {
    return false;
  }

  if (!ValidContext()) {
    StoreOrTriggerInitCallback(base::BindOnce(
        base::IgnoreResult(&HoneycombCookieManagerImpl::DeleteCookiesInternal), this,
        gurl, cookie_name, callback));
    return true;
  }

  return DeleteCookiesInternal(gurl, cookie_name, callback);
}

bool HoneycombCookieManagerImpl::FlushStore(
    HoneycombRefPtr<HoneycombCompletionCallback> callback) {
  if (!ValidContext()) {
    StoreOrTriggerInitCallback(base::BindOnce(
        base::IgnoreResult(&HoneycombCookieManagerImpl::FlushStoreInternal), this,
        callback));
    return true;
  }

  return FlushStoreInternal(callback);
}

bool HoneycombCookieManagerImpl::VisitAllCookiesInternal(
    HoneycombRefPtr<HoneycombCookieVisitor> visitor) {
  DCHECK(ValidContext());
  DCHECK(visitor);

  auto browser_context = GetBrowserContext(browser_context_getter_);
  if (!browser_context) {
    return false;
  }

  GetCookieManager(browser_context)
      ->GetAllCookies(base::BindOnce(&GetAllCookiesCallbackImpl, visitor,
                                     browser_context_getter_));
  return true;
}

bool HoneycombCookieManagerImpl::VisitUrlCookiesInternal(
    const GURL& url,
    bool includeHttpOnly,
    HoneycombRefPtr<HoneycombCookieVisitor> visitor) {
  DCHECK(ValidContext());
  DCHECK(visitor);
  DCHECK(url.is_valid());

  net::CookieOptions options;
  if (includeHttpOnly) {
    options.set_include_httponly();
  }
  options.set_same_site_cookie_context(
      net::CookieOptions::SameSiteCookieContext::MakeInclusive());

  auto browser_context = GetBrowserContext(browser_context_getter_);
  if (!browser_context) {
    return false;
  }

  GetCookieManager(browser_context)
      ->GetCookieList(url, options, net::CookiePartitionKeyCollection(),
                      base::BindOnce(&GetCookiesCallbackImpl, visitor,
                                     browser_context_getter_));
  return true;
}

bool HoneycombCookieManagerImpl::SetCookieInternal(
    const GURL& url,
    const HoneycombCookie& cookie,
    HoneycombRefPtr<HoneycombSetCookieCallback> callback) {
  DCHECK(ValidContext());
  DCHECK(url.is_valid());

  std::string name = HoneycombString(&cookie.name).ToString();
  std::string value = HoneycombString(&cookie.value).ToString();
  std::string domain = HoneycombString(&cookie.domain).ToString();
  std::string path = HoneycombString(&cookie.path).ToString();

  base::Time expiration_time;
  if (cookie.has_expires) {
    expiration_time = HoneycombBaseTime(cookie.expires);
  }

  net::CookieSameSite same_site =
      net_service::MakeCookieSameSite(cookie.same_site);
  net::CookiePriority priority =
      net_service::MakeCookiePriority(cookie.priority);

  auto canonical_cookie = net::CanonicalCookie::CreateSanitizedCookie(
      url, name, value, domain, path,
      base::Time(),  // Creation time.
      expiration_time,
      base::Time(),  // Last access time.
      cookie.secure ? true : false, cookie.httponly ? true : false, same_site,
      priority, /*same_party=*/false, /*partition_key=*/absl::nullopt);

  if (!canonical_cookie) {
    SetCookieCallbackImpl(
        callback, net::CookieAccessResult(net::CookieInclusionStatus(
                      net::CookieInclusionStatus::EXCLUDE_UNKNOWN_ERROR)));
    return true;
  }

  net::CookieOptions options;
  if (cookie.httponly) {
    options.set_include_httponly();
  }
  options.set_same_site_cookie_context(
      net::CookieOptions::SameSiteCookieContext::MakeInclusive());

  auto browser_context = GetBrowserContext(browser_context_getter_);
  if (!browser_context) {
    return false;
  }

  GetCookieManager(browser_context)
      ->SetCanonicalCookie(*canonical_cookie, url, options,
                           base::BindOnce(SetCookieCallbackImpl, callback));
  return true;
}

bool HoneycombCookieManagerImpl::DeleteCookiesInternal(
    const GURL& url,
    const HoneycombString& cookie_name,
    HoneycombRefPtr<HoneycombDeleteCookiesCallback> callback) {
  DCHECK(ValidContext());
  DCHECK(url.is_empty() || url.is_valid());

  network::mojom::CookieDeletionFilterPtr deletion_filter =
      network::mojom::CookieDeletionFilter::New();

  if (url.is_empty()) {
    // Delete all cookies.
  } else if (cookie_name.empty()) {
    // Delete all matching host cookies.
    deletion_filter->host_name = url.host();
  } else {
    // Delete all matching host and domain cookies.
    deletion_filter->url = url;
    deletion_filter->cookie_name = cookie_name;
  }

  auto browser_context = GetBrowserContext(browser_context_getter_);
  if (!browser_context) {
    return false;
  }

  GetCookieManager(browser_context)
      ->DeleteCookies(std::move(deletion_filter),
                      base::BindOnce(DeleteCookiesCallbackImpl, callback));
  return true;
}

bool HoneycombCookieManagerImpl::FlushStoreInternal(
    HoneycombRefPtr<HoneycombCompletionCallback> callback) {
  DCHECK(ValidContext());

  auto browser_context = GetBrowserContext(browser_context_getter_);
  if (!browser_context) {
    return false;
  }

  GetCookieManager(browser_context)
      ->FlushCookieStore(
          base::BindOnce(RunAsyncCompletionOnUIThread, callback));
  return true;
}

void HoneycombCookieManagerImpl::StoreOrTriggerInitCallback(
    base::OnceClosure callback) {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(
        HONEYCOMB_UIT,
        base::BindOnce(&HoneycombCookieManagerImpl::StoreOrTriggerInitCallback, this,
                       std::move(callback)));
    return;
  }

  if (initialized_) {
    std::move(callback).Run();
  } else {
    init_callbacks_.emplace_back(std::move(callback));
  }
}

bool HoneycombCookieManagerImpl::ValidContext() const {
  return HONEYCOMB_CURRENTLY_ON_UIT() && initialized_;
}

// HoneycombCookieManager methods ----------------------------------------------------

// static
HoneycombRefPtr<HoneycombCookieManager> HoneycombCookieManager::GetGlobalManager(
    HoneycombRefPtr<HoneycombCompletionCallback> callback) {
  HoneycombRefPtr<HoneycombRequestContext> context = HoneycombRequestContext::GetGlobalContext();
  return context ? context->GetCookieManager(callback) : nullptr;
}
