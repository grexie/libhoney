// Copyright (c) 2023 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the Honeycomb translator tool. If making changes by
// hand only do so within the body of existing method and function
// implementations. See the translator.README.txt file in the tools directory
// for more information.
//
// $hash=22f71b1cb703d48b29d6e4eefb294fd713b5bf0e$
//

#include "libhoney_dll/cpptoc/media_observer_cpptoc.h"
#include "libhoney_dll/ctocpp/media_route_ctocpp.h"
#include "libhoney_dll/ctocpp/media_sink_ctocpp.h"
#include "libhoney_dll/shutdown_checker.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

void HONEYCOMB_CALLBACK
media_observer_on_sinks(struct _honey_media_observer_t* self,
                        size_t sinksCount,
                        struct _honey_media_sink_t* const* sinks) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: sinks; type: refptr_vec_diff_byref_const
  DCHECK(sinksCount == 0 || sinks);
  if (sinksCount > 0 && !sinks) {
    return;
  }

  // Translate param: sinks; type: refptr_vec_diff_byref_const
  std::vector<HoneycombRefPtr<HoneycombMediaSink>> sinksList;
  if (sinksCount > 0) {
    for (size_t i = 0; i < sinksCount; ++i) {
      HoneycombRefPtr<HoneycombMediaSink> sinksVal = HoneycombMediaSinkCToCpp::Wrap(sinks[i]);
      sinksList.push_back(sinksVal);
    }
  }

  // Execute
  HoneycombMediaObserverCppToC::Get(self)->OnSinks(sinksList);
}

void HONEYCOMB_CALLBACK
media_observer_on_routes(struct _honey_media_observer_t* self,
                         size_t routesCount,
                         struct _honey_media_route_t* const* routes) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: routes; type: refptr_vec_diff_byref_const
  DCHECK(routesCount == 0 || routes);
  if (routesCount > 0 && !routes) {
    return;
  }

  // Translate param: routes; type: refptr_vec_diff_byref_const
  std::vector<HoneycombRefPtr<HoneycombMediaRoute>> routesList;
  if (routesCount > 0) {
    for (size_t i = 0; i < routesCount; ++i) {
      HoneycombRefPtr<HoneycombMediaRoute> routesVal = HoneycombMediaRouteCToCpp::Wrap(routes[i]);
      routesList.push_back(routesVal);
    }
  }

  // Execute
  HoneycombMediaObserverCppToC::Get(self)->OnRoutes(routesList);
}

void HONEYCOMB_CALLBACK media_observer_on_route_state_changed(
    struct _honey_media_observer_t* self,
    struct _honey_media_route_t* route,
    honey_media_route_connection_state_t state) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: route; type: refptr_diff
  DCHECK(route);
  if (!route) {
    return;
  }

  // Execute
  HoneycombMediaObserverCppToC::Get(self)->OnRouteStateChanged(
      HoneycombMediaRouteCToCpp::Wrap(route), state);
}

void HONEYCOMB_CALLBACK
media_observer_on_route_message_received(struct _honey_media_observer_t* self,
                                         struct _honey_media_route_t* route,
                                         const void* message,
                                         size_t message_size) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: route; type: refptr_diff
  DCHECK(route);
  if (!route) {
    return;
  }
  // Verify param: message; type: simple_byaddr
  DCHECK(message);
  if (!message) {
    return;
  }

  // Execute
  HoneycombMediaObserverCppToC::Get(self)->OnRouteMessageReceived(
      HoneycombMediaRouteCToCpp::Wrap(route), message, message_size);
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

HoneycombMediaObserverCppToC::HoneycombMediaObserverCppToC() {
  GetStruct()->on_sinks = media_observer_on_sinks;
  GetStruct()->on_routes = media_observer_on_routes;
  GetStruct()->on_route_state_changed = media_observer_on_route_state_changed;
  GetStruct()->on_route_message_received =
      media_observer_on_route_message_received;
}

// DESTRUCTOR - Do not edit by hand.

HoneycombMediaObserverCppToC::~HoneycombMediaObserverCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
HoneycombRefPtr<HoneycombMediaObserver> HoneycombCppToCRefCounted<
    HoneycombMediaObserverCppToC,
    HoneycombMediaObserver,
    honey_media_observer_t>::UnwrapDerived(HoneycombWrapperType type,
                                         honey_media_observer_t* s) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCppToCRefCounted<HoneycombMediaObserverCppToC,
                                   HoneycombMediaObserver,
                                   honey_media_observer_t>::kWrapperType =
    WT_MEDIA_OBSERVER;