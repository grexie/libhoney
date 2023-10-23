// Copyright (c) 2020 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_MEDIA_ROUTER_MEDIA_ROUTE_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_MEDIA_ROUTER_MEDIA_ROUTE_IMPL_H_
#pragma once

#include "include/honey_media_router.h"
#include "libhoneycomb/browser/browser_context.h"

#include "components/media_router/common/media_route.h"

// Implementation of the HoneycombMediaRoute interface. Only created on the UI thread.
class HoneycombMediaRouteImpl : public HoneycombMediaRoute {
 public:
  HoneycombMediaRouteImpl(const media_router::MediaRoute& route,
                    const HoneycombBrowserContext::Getter& browser_context_getter);

  HoneycombMediaRouteImpl(const HoneycombMediaRouteImpl&) = delete;
  HoneycombMediaRouteImpl& operator=(const HoneycombMediaRouteImpl&) = delete;

  // HoneycombMediaRoute methods.
  HoneycombString GetId() override;
  HoneycombRefPtr<HoneycombMediaSource> GetSource() override;
  HoneycombRefPtr<HoneycombMediaSink> GetSink() override;
  void SendRouteMessage(const void* message, size_t message_size) override;
  void Terminate() override;

  const media_router::MediaRoute& route() const { return route_; }

 private:
  void SendRouteMessageInternal(std::string message);

  // Read-only after creation.
  const media_router::MediaRoute route_;
  const HoneycombBrowserContext::Getter browser_context_getter_;

  IMPLEMENT_REFCOUNTING(HoneycombMediaRouteImpl);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_MEDIA_ROUTER_MEDIA_ROUTE_IMPL_H_
