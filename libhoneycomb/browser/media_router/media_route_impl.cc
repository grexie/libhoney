// Copyright (c) 2020 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoneycomb/browser/media_router/media_route_impl.h"

#include "libhoneycomb/browser/media_router/media_router_manager.h"
#include "libhoneycomb/browser/media_router/media_sink_impl.h"
#include "libhoneycomb/browser/media_router/media_source_impl.h"
#include "libhoneycomb/browser/thread_util.h"

namespace {

// Do not keep a reference to the object returned by this method.
HoneycombBrowserContext* GetBrowserContext(const HoneycombBrowserContext::Getter& getter) {
  HONEYCOMB_REQUIRE_UIT();
  DCHECK(!getter.is_null());

  // Will return nullptr if the BrowserContext has been shut down.
  return getter.Run();
}

}  // namespace

HoneycombMediaRouteImpl::HoneycombMediaRouteImpl(
    const media_router::MediaRoute& route,
    const HoneycombBrowserContext::Getter& browser_context_getter)
    : route_(route), browser_context_getter_(browser_context_getter) {
  HONEYCOMB_REQUIRE_UIT();
}

HoneycombString HoneycombMediaRouteImpl::GetId() {
  return route_.media_route_id();
}

HoneycombRefPtr<HoneycombMediaSource> HoneycombMediaRouteImpl::GetSource() {
  return new HoneycombMediaSourceImpl(route_.media_source().id());
}

HoneycombRefPtr<HoneycombMediaSink> HoneycombMediaRouteImpl::GetSink() {
  return new HoneycombMediaSinkImpl(
      route_.media_sink_id(), route_.media_sink_name(),
      route_.media_source().IsDialSource()
          ? media_router::mojom::MediaRouteProviderId::DIAL
          : media_router::mojom::MediaRouteProviderId::CAST);
}

void HoneycombMediaRouteImpl::SendRouteMessage(const void* message,
                                         size_t message_size) {
  std::string message_str(reinterpret_cast<const char*>(message), message_size);

  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(
        HONEYCOMB_UIT,
        base::BindOnce(
            [](HoneycombRefPtr<HoneycombMediaRouteImpl> self, std::string message_str) {
              self->SendRouteMessageInternal(std::move(message_str));
            },
            HoneycombRefPtr<HoneycombMediaRouteImpl>(this), std::move(message_str)));
    return;
  }

  SendRouteMessageInternal(std::move(message_str));
}

void HoneycombMediaRouteImpl::Terminate() {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT, base::BindOnce(&HoneycombMediaRouteImpl::Terminate, this));
    return;
  }

  auto browser_context = GetBrowserContext(browser_context_getter_);
  if (!browser_context) {
    return;
  }

  browser_context->GetMediaRouterManager()->TerminateRoute(
      route_.media_route_id());
}

void HoneycombMediaRouteImpl::SendRouteMessageInternal(std::string message) {
  auto browser_context = GetBrowserContext(browser_context_getter_);
  if (!browser_context) {
    return;
  }

  browser_context->GetMediaRouterManager()->SendRouteMessage(
      route_.media_route_id(), message);
}
