// Copyright (c) 2020 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoneycomb/browser/media_router/media_router_impl.h"

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

class HoneycombRegistrationImpl : public HoneycombRegistration,
                            public HoneycombMediaRouterManager::Observer {
 public:
  explicit HoneycombRegistrationImpl(HoneycombRefPtr<HoneycombMediaObserver> observer)
      : observer_(observer) {
    DCHECK(observer_);
  }

  HoneycombRegistrationImpl(const HoneycombRegistrationImpl&) = delete;
  HoneycombRegistrationImpl& operator=(const HoneycombRegistrationImpl&) = delete;

  ~HoneycombRegistrationImpl() override {
    HONEYCOMB_REQUIRE_UIT();

    // May be null if OnMediaRouterDestroyed was called.
    if (browser_context_getter_.is_null()) {
      return;
    }

    auto browser_context = GetBrowserContext(browser_context_getter_);
    if (!browser_context) {
      return;
    }

    browser_context->GetMediaRouterManager()->RemoveObserver(this);
  }

  void Initialize(const HoneycombBrowserContext::Getter& browser_context_getter) {
    HONEYCOMB_REQUIRE_UIT();
    DCHECK(!browser_context_getter.is_null());
    DCHECK(browser_context_getter_.is_null());
    browser_context_getter_ = browser_context_getter;

    auto browser_context = GetBrowserContext(browser_context_getter_);
    if (!browser_context) {
      return;
    }

    browser_context->GetMediaRouterManager()->AddObserver(this);
  }

 private:
  // HoneycombMediaRouterManager::Observer methods:
  void OnMediaRouterDestroyed() override { browser_context_getter_.Reset(); }

  void OnMediaSinks(
      const HoneycombMediaRouterManager::MediaSinkVector& sinks) override {
    std::vector<HoneycombRefPtr<HoneycombMediaSink>> honey_sinks;
    for (const auto& sink : sinks) {
      honey_sinks.push_back(new HoneycombMediaSinkImpl(sink.sink));
    }
    observer_->OnSinks(honey_sinks);
  }

  void OnMediaRoutes(
      const HoneycombMediaRouterManager::MediaRouteVector& routes) override {
    std::vector<HoneycombRefPtr<HoneycombMediaRoute>> honey_routes;
    for (const auto& route : routes) {
      honey_routes.push_back(MakeHoneycombRoute(route));
    }
    observer_->OnRoutes(honey_routes);
  }

  void OnMediaRouteMessages(
      const media_router::MediaRoute& route,
      const HoneycombMediaRouterManager::MediaMessageVector& messages) override {
    HoneycombRefPtr<HoneycombMediaRoute> honey_route = MakeHoneycombRoute(route);
    for (const auto& message : messages) {
      if (message->type == media_router::mojom::RouteMessage::Type::TEXT) {
        if (message->message.has_value()) {
          const std::string& str = *(message->message);
          observer_->OnRouteMessageReceived(honey_route, str.c_str(), str.size());
        }
      } else if (message->type ==
                 media_router::mojom::RouteMessage::Type::BINARY) {
        if (message->data.has_value()) {
          const std::vector<uint8_t>& data = *(message->data);
          observer_->OnRouteMessageReceived(honey_route, data.data(),
                                            data.size());
        }
      }
    }
  }

  void OnMediaRouteStateChange(
      const media_router::MediaRoute& route,
      const content::PresentationConnectionStateChangeInfo& info) override {
    observer_->OnRouteStateChanged(MakeHoneycombRoute(route),
                                   ToConnectionState(info.state));
  }

  HoneycombRefPtr<HoneycombMediaRoute> MakeHoneycombRoute(const media_router::MediaRoute& route) {
    return new HoneycombMediaRouteImpl(route, browser_context_getter_);
  }

  static HoneycombMediaObserver::ConnectionState ToConnectionState(
      blink::mojom::PresentationConnectionState state) {
    switch (state) {
      case blink::mojom::PresentationConnectionState::CONNECTING:
        return HONEYCOMB_MRCS_CONNECTING;
      case blink::mojom::PresentationConnectionState::CONNECTED:
        return HONEYCOMB_MRCS_CONNECTED;
      case blink::mojom::PresentationConnectionState::CLOSED:
        return HONEYCOMB_MRCS_CLOSED;
      case blink::mojom::PresentationConnectionState::TERMINATED:
        return HONEYCOMB_MRCS_TERMINATED;
    }
    DCHECK(false);
    return HONEYCOMB_MRCS_UNKNOWN;
  }

  HoneycombRefPtr<HoneycombMediaObserver> observer_;
  HoneycombBrowserContext::Getter browser_context_getter_;

  IMPLEMENT_REFCOUNTING_DELETE_ON_UIT(HoneycombRegistrationImpl);
};

HoneycombMediaRouterImpl::HoneycombMediaRouterImpl() = default;

void HoneycombMediaRouterImpl::Initialize(
    const HoneycombBrowserContext::Getter& browser_context_getter,
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

  if (callback) {
    // Execute client callback asynchronously for consistency.
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT, base::BindOnce(&HoneycombCompletionCallback::OnComplete,
                                          callback.get()));
  }
}

HoneycombRefPtr<HoneycombRegistration> HoneycombMediaRouterImpl::AddObserver(
    HoneycombRefPtr<HoneycombMediaObserver> observer) {
  if (!observer) {
    return nullptr;
  }
  HoneycombRefPtr<HoneycombRegistrationImpl> registration =
      new HoneycombRegistrationImpl(observer);
  StoreOrTriggerInitCallback(base::BindOnce(
      &HoneycombMediaRouterImpl::InitializeRegistrationInternal, this, registration));
  return registration.get();
}

HoneycombRefPtr<HoneycombMediaSource> HoneycombMediaRouterImpl::GetSource(const HoneycombString& urn) {
  if (urn.empty()) {
    return nullptr;
  }

  // Check for a valid URL and supported Cast/DIAL schemes.
  GURL presentation_url(urn.ToString());
  if (!media_router::IsValidPresentationUrl(presentation_url)) {
    return nullptr;
  }

  if (presentation_url.SchemeIsHTTPOrHTTPS()) {
    // We don't support tab/desktop mirroring, which is what Cast uses for
    // arbitrary HTTP/HTTPS URLs (see CastMediaSource).
    return nullptr;
  }

  return new HoneycombMediaSourceImpl(presentation_url);
}

void HoneycombMediaRouterImpl::NotifyCurrentSinks() {
  StoreOrTriggerInitCallback(
      base::BindOnce(&HoneycombMediaRouterImpl::NotifyCurrentSinksInternal, this));
}

void HoneycombMediaRouterImpl::CreateRoute(
    HoneycombRefPtr<HoneycombMediaSource> source,
    HoneycombRefPtr<HoneycombMediaSink> sink,
    HoneycombRefPtr<HoneycombMediaRouteCreateCallback> callback) {
  StoreOrTriggerInitCallback(base::BindOnce(
      &HoneycombMediaRouterImpl::CreateRouteInternal, this, source, sink, callback));
}

void HoneycombMediaRouterImpl::NotifyCurrentRoutes() {
  StoreOrTriggerInitCallback(
      base::BindOnce(&HoneycombMediaRouterImpl::NotifyCurrentRoutesInternal, this));
}

void HoneycombMediaRouterImpl::InitializeRegistrationInternal(
    HoneycombRefPtr<HoneycombRegistrationImpl> registration) {
  DCHECK(ValidContext());

  registration->Initialize(browser_context_getter_);
}

void HoneycombMediaRouterImpl::NotifyCurrentSinksInternal() {
  DCHECK(ValidContext());

  auto browser_context = GetBrowserContext(browser_context_getter_);
  if (!browser_context) {
    return;
  }

  browser_context->GetMediaRouterManager()->NotifyCurrentSinks();
}

void HoneycombMediaRouterImpl::CreateRouteInternal(
    HoneycombRefPtr<HoneycombMediaSource> source,
    HoneycombRefPtr<HoneycombMediaSink> sink,
    HoneycombRefPtr<HoneycombMediaRouteCreateCallback> callback) {
  DCHECK(ValidContext());

  std::string error;

  auto browser_context = GetBrowserContext(browser_context_getter_);
  if (!browser_context) {
    error = "Context is not valid";
  } else if (!source) {
    error = "Source is empty or invalid";
  } else if (!sink) {
    error = "Sink is empty or invalid";
  } else if (!sink->IsCompatibleWith(source)) {
    error = "Sink is not compatible with source";
  }

  if (!error.empty()) {
    LOG(WARNING) << "Media route creation failed: " << error;
    if (callback) {
      callback->OnMediaRouteCreateFinished(HONEYCOMB_MRCR_UNKNOWN_ERROR, error,
                                           nullptr);
    }
    return;
  }

  auto source_impl = static_cast<HoneycombMediaSourceImpl*>(source.get());
  auto sink_impl = static_cast<HoneycombMediaSinkImpl*>(sink.get());

  browser_context->GetMediaRouterManager()->CreateRoute(
      source_impl->source().id(), sink_impl->sink().id(), url::Origin(),
      base::BindOnce(&HoneycombMediaRouterImpl::CreateRouteCallback, this, callback));
}

void HoneycombMediaRouterImpl::NotifyCurrentRoutesInternal() {
  DCHECK(ValidContext());

  auto browser_context = GetBrowserContext(browser_context_getter_);
  if (!browser_context) {
    return;
  }

  browser_context->GetMediaRouterManager()->NotifyCurrentRoutes();
}

void HoneycombMediaRouterImpl::CreateRouteCallback(
    HoneycombRefPtr<HoneycombMediaRouteCreateCallback> callback,
    const media_router::RouteRequestResult& result) {
  DCHECK(ValidContext());

  if (result.result_code() != media_router::mojom::RouteRequestResultCode::OK) {
    LOG(WARNING) << "Media route creation failed: " << result.error() << " ("
                 << result.result_code() << ")";
  }

  if (!callback) {
    return;
  }

  HoneycombRefPtr<HoneycombMediaRoute> route;
  if (result.result_code() == media_router::mojom::RouteRequestResultCode::OK &&
      result.route()) {
    route = new HoneycombMediaRouteImpl(*result.route(), browser_context_getter_);
  }

  callback->OnMediaRouteCreateFinished(
      static_cast<honey_media_route_create_result_t>(result.result_code()),
      result.error(), route);
}

void HoneycombMediaRouterImpl::StoreOrTriggerInitCallback(
    base::OnceClosure callback) {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(
        HONEYCOMB_UIT, base::BindOnce(&HoneycombMediaRouterImpl::StoreOrTriggerInitCallback,
                                this, std::move(callback)));
    return;
  }

  if (initialized_) {
    std::move(callback).Run();
  } else {
    init_callbacks_.emplace_back(std::move(callback));
  }
}

bool HoneycombMediaRouterImpl::ValidContext() const {
  return HONEYCOMB_CURRENTLY_ON_UIT() && initialized_;
}

// HoneycombMediaRouter methods ------------------------------------------------------

// static
HoneycombRefPtr<HoneycombMediaRouter> HoneycombMediaRouter::GetGlobalMediaRouter(
    HoneycombRefPtr<HoneycombCompletionCallback> callback) {
  return HoneycombRequestContext::GetGlobalContext()->GetMediaRouter(callback);
}
