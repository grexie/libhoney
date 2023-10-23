// Copyright (c) 2019 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_NET_SERVICE_COOKIE_MANAGER_IMPL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_NET_SERVICE_COOKIE_MANAGER_IMPL_H_

#include "include/honey_cookie.h"
#include "libhoney/browser/browser_context.h"
#include "libhoney/browser/thread_util.h"

#include "base/files/file_path.h"

// Implementation of the HoneycombCookieManager interface. May be created on any
// thread.
class HoneycombCookieManagerImpl : public HoneycombCookieManager {
 public:
  HoneycombCookieManagerImpl();

  HoneycombCookieManagerImpl(const HoneycombCookieManagerImpl&) = delete;
  HoneycombCookieManagerImpl& operator=(const HoneycombCookieManagerImpl&) = delete;

  // Called on the UI thread after object creation and before any other object
  // methods are executed on the UI thread.
  void Initialize(HoneycombBrowserContext::Getter browser_context_getter,
                  HoneycombRefPtr<HoneycombCompletionCallback> callback);

  // HoneycombCookieManager methods.
  bool VisitAllCookies(HoneycombRefPtr<HoneycombCookieVisitor> visitor) override;
  bool VisitUrlCookies(const HoneycombString& url,
                       bool includeHttpOnly,
                       HoneycombRefPtr<HoneycombCookieVisitor> visitor) override;
  bool SetCookie(const HoneycombString& url,
                 const HoneycombCookie& cookie,
                 HoneycombRefPtr<HoneycombSetCookieCallback> callback) override;
  bool DeleteCookies(const HoneycombString& url,
                     const HoneycombString& cookie_name,
                     HoneycombRefPtr<HoneycombDeleteCookiesCallback> callback) override;
  bool FlushStore(HoneycombRefPtr<HoneycombCompletionCallback> callback) override;

 private:
  bool VisitAllCookiesInternal(HoneycombRefPtr<HoneycombCookieVisitor> visitor);
  bool VisitUrlCookiesInternal(const GURL& url,
                               bool includeHttpOnly,
                               HoneycombRefPtr<HoneycombCookieVisitor> visitor);
  bool SetCookieInternal(const GURL& url,
                         const HoneycombCookie& cookie,
                         HoneycombRefPtr<HoneycombSetCookieCallback> callback);
  bool DeleteCookiesInternal(const GURL& url,
                             const HoneycombString& cookie_name,
                             HoneycombRefPtr<HoneycombDeleteCookiesCallback> callback);
  bool FlushStoreInternal(HoneycombRefPtr<HoneycombCompletionCallback> callback);

  // If the context is fully initialized execute |callback|, otherwise
  // store it until the context is fully initialized.
  void StoreOrTriggerInitCallback(base::OnceClosure callback);

  bool ValidContext() const;

  // Only accessed on the UI thread. Will be non-null after Initialize().
  HoneycombBrowserContext::Getter browser_context_getter_;

  bool initialized_ = false;
  std::vector<base::OnceClosure> init_callbacks_;

  IMPLEMENT_REFCOUNTING(HoneycombCookieManagerImpl);
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_NET_SERVICE_COOKIE_MANAGER_IMPL_H_
