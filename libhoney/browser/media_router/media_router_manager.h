// Copyright (c) 2020 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_MEDIA_ROUTER_MEDIA_ROUTER_MANAGER_H_
#define HONEYCOMB_LIBHONEY_BROWSER_MEDIA_ROUTER_MEDIA_ROUTER_MANAGER_H_
#pragma once

#include "include/honey_media_router.h"

#include "base/memory/weak_ptr.h"
#include "base/observer_list.h"
#include "chrome/browser/ui/media_router/query_result_manager.h"
#include "components/media_router/browser/media_router.h"
#include "components/media_router/common/mojom/media_router.mojom.h"

namespace content {
class BrowserContext;
}

class HoneycombMediaRoutesObserver;
class HoneycombPresentationConnection;
class HoneycombPresentationConnectionMessageObserver;

// Manages Honeycomb usage of MediaRouter. Owned by HoneycombBrowserContext and only
// accessed on the UI thread.
class HoneycombMediaRouterManager
    : public media_router::MediaSinkWithCastModesObserver {
 public:
  using MediaRouteVector = std::vector<media_router::MediaRoute>;
  using MediaSinkVector = std::vector<media_router::MediaSinkWithCastModes>;
  using MediaMessageVector = std::vector<media_router::mojom::RouteMessagePtr>;

  class Observer : public base::CheckedObserver {
   public:
    virtual void OnMediaRouterDestroyed() = 0;

    virtual void OnMediaSinks(const MediaSinkVector& sinks) = 0;
    virtual void OnMediaRoutes(const MediaRouteVector& routes) = 0;

    virtual void OnMediaRouteMessages(const media_router::MediaRoute& route,
                                      const MediaMessageVector& messages) = 0;
    virtual void OnMediaRouteStateChange(
        const media_router::MediaRoute& route,
        const content::PresentationConnectionStateChangeInfo& info) = 0;

   protected:
    ~Observer() override {}
  };

  explicit HoneycombMediaRouterManager(content::BrowserContext* browser_context);

  HoneycombMediaRouterManager(const HoneycombMediaRouterManager&) = delete;
  HoneycombMediaRouterManager& operator=(const HoneycombMediaRouterManager&) = delete;

  ~HoneycombMediaRouterManager() override;

  // |observer| must outlive this object or be removed.
  void AddObserver(Observer* observer);
  void RemoveObserver(Observer* observer);

  void NotifyCurrentSinks();
  void NotifyCurrentRoutes();

  using CreateRouteResultCallback =
      base::OnceCallback<void(const media_router::RouteRequestResult& result)>;

  void CreateRoute(const media_router::MediaSource::Id& source_id,
                   const media_router::MediaSink::Id& sink_id,
                   const url::Origin& origin,
                   CreateRouteResultCallback callback);

  void SendRouteMessage(const media_router::MediaRoute::Id& route_id,
                        const std::string& message);
  void TerminateRoute(const media_router::MediaRoute::Id& route_id);

  // MediaSinkWithCastModesObserver methods.
  void OnSinksUpdated(const MediaSinkVector& sinks) override;

 private:
  friend class HoneycombMediaRoutesObserver;
  friend class HoneycombPresentationConnection;
  friend class HoneycombPresentationConnectionMessageObserver;

  // Do not keep a reference to the object returned by this method.
  media_router::MediaRouter* GetMediaRouter() const;

  void OnCreateRoute(
      CreateRouteResultCallback callback,
      media_router::mojom::RoutePresentationConnectionPtr connection,
      const media_router::RouteRequestResult& result);
  void OnRouteStateChange(
      const media_router::MediaRoute& route,
      const content::PresentationConnectionStateChangeInfo& info);
  void OnMessagesReceived(const media_router::MediaRoute& route,
                          const MediaMessageVector& messages);

  struct RouteState {
    std::unique_ptr<HoneycombPresentationConnection> presentation_connection_;

    // Used if there is no RoutePresentationConnectionPtr.
    std::unique_ptr<HoneycombPresentationConnectionMessageObserver> message_observer_;
    base::CallbackListSubscription state_subscription_;
  };
  void CreateRouteState(
      const media_router::MediaRoute& route,
      media_router::mojom::RoutePresentationConnectionPtr connection);
  RouteState* GetRouteState(const media_router::MediaRoute::Id& route_id);
  void RemoveRouteState(const media_router::MediaRoute::Id& route_id);

  content::BrowserContext* const browser_context_;

  base::ObserverList<Observer> observers_;

  media_router::QueryResultManager query_result_manager_;
  std::unique_ptr<HoneycombMediaRoutesObserver> routes_observer_;

  MediaRouteVector routes_;
  MediaSinkVector sinks_;

  using RouteStateMap =
      std::map<media_router::MediaRoute::Id, std::unique_ptr<RouteState>>;
  RouteStateMap route_state_map_;

  base::WeakPtrFactory<HoneycombMediaRouterManager> weak_ptr_factory_;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_MEDIA_ROUTER_MEDIA_ROUTER_MANAGER_H_
