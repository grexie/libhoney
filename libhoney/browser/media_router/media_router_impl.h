// Copyright (c) 2020 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_MEDIA_ROUTER_MEDIA_ROUTER_IMPL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_MEDIA_ROUTER_MEDIA_ROUTER_IMPL_H_
#pragma once

#include "include/honey_media_router.h"
#include "libhoney/browser/browser_context.h"

#include "components/media_router/common/mojom/media_router.mojom.h"

class HoneycombRegistrationImpl;

// Implementation of the HoneycombMediaRouter interface. May be created on any thread.
class HoneycombMediaRouterImpl : public HoneycombMediaRouter {
 public:
  HoneycombMediaRouterImpl();

  HoneycombMediaRouterImpl(const HoneycombMediaRouterImpl&) = delete;
  HoneycombMediaRouterImpl& operator=(const HoneycombMediaRouterImpl&) = delete;

  // Called on the UI thread after object creation and before any other object
  // methods are executed on the UI thread.
  void Initialize(const HoneycombBrowserContext::Getter& browser_context_getter,
                  HoneycombRefPtr<HoneycombCompletionCallback> callback);

  // HoneycombMediaRouter methods.
  HoneycombRefPtr<HoneycombRegistration> AddObserver(
      HoneycombRefPtr<HoneycombMediaObserver> observer) override;
  HoneycombRefPtr<HoneycombMediaSource> GetSource(const HoneycombString& urn) override;
  void NotifyCurrentSinks() override;
  void CreateRoute(HoneycombRefPtr<HoneycombMediaSource> source,
                   HoneycombRefPtr<HoneycombMediaSink> sink,
                   HoneycombRefPtr<HoneycombMediaRouteCreateCallback> callback) override;
  void NotifyCurrentRoutes() override;

 private:
  void InitializeRegistrationInternal(
      HoneycombRefPtr<HoneycombRegistrationImpl> registration);
  void NotifyCurrentSinksInternal();
  void CreateRouteInternal(HoneycombRefPtr<HoneycombMediaSource> source,
                           HoneycombRefPtr<HoneycombMediaSink> sink,
                           HoneycombRefPtr<HoneycombMediaRouteCreateCallback> callback);
  void NotifyCurrentRoutesInternal();

  void CreateRouteCallback(HoneycombRefPtr<HoneycombMediaRouteCreateCallback> callback,
                           const media_router::RouteRequestResult& result);

  // If the context is fully initialized execute |callback|, otherwise
  // store it until the context is fully initialized.
  void StoreOrTriggerInitCallback(base::OnceClosure callback);

  bool ValidContext() const;

  // Only accessed on the UI thread. Will be non-null after Initialize().
  HoneycombBrowserContext::Getter browser_context_getter_;

  bool initialized_ = false;
  std::vector<base::OnceClosure> init_callbacks_;

  IMPLEMENT_REFCOUNTING(HoneycombMediaRouterImpl);
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_MEDIA_ROUTER_MEDIA_ROUTER_IMPL_H_
