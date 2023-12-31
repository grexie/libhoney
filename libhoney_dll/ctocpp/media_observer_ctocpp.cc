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
// $hash=9b1b93f78443553cdf07bf096add8eda430ad13b$
//

#include "libhoney_dll/ctocpp/media_observer_ctocpp.h"
#include "libhoney_dll/cpptoc/media_route_cpptoc.h"
#include "libhoney_dll/cpptoc/media_sink_cpptoc.h"
#include "libhoney_dll/shutdown_checker.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall")
void HoneycombMediaObserverCToCpp::OnSinks(
    const std::vector<HoneycombRefPtr<HoneycombMediaSink>>& sinks) {
  shutdown_checker::AssertNotShutdown();

  honey_media_observer_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_sinks)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Translate param: sinks; type: refptr_vec_diff_byref_const
  const size_t sinksCount = sinks.size();
  honey_media_sink_t** sinksList = NULL;
  if (sinksCount > 0) {
    sinksList = new honey_media_sink_t*[sinksCount];
    DCHECK(sinksList);
    if (sinksList) {
      for (size_t i = 0; i < sinksCount; ++i) {
        sinksList[i] = HoneycombMediaSinkCppToC::Wrap(sinks[i]);
      }
    }
  }

  // Execute
  _struct->on_sinks(_struct, sinksCount, sinksList);

  // Restore param:sinks; type: refptr_vec_diff_byref_const
  if (sinksList) {
    delete[] sinksList;
  }
}

NO_SANITIZE("cfi-icall")
void HoneycombMediaObserverCToCpp::OnRoutes(
    const std::vector<HoneycombRefPtr<HoneycombMediaRoute>>& routes) {
  shutdown_checker::AssertNotShutdown();

  honey_media_observer_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_routes)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Translate param: routes; type: refptr_vec_diff_byref_const
  const size_t routesCount = routes.size();
  honey_media_route_t** routesList = NULL;
  if (routesCount > 0) {
    routesList = new honey_media_route_t*[routesCount];
    DCHECK(routesList);
    if (routesList) {
      for (size_t i = 0; i < routesCount; ++i) {
        routesList[i] = HoneycombMediaRouteCppToC::Wrap(routes[i]);
      }
    }
  }

  // Execute
  _struct->on_routes(_struct, routesCount, routesList);

  // Restore param:routes; type: refptr_vec_diff_byref_const
  if (routesList) {
    delete[] routesList;
  }
}

NO_SANITIZE("cfi-icall")
void HoneycombMediaObserverCToCpp::OnRouteStateChanged(HoneycombRefPtr<HoneycombMediaRoute> route,
                                                 ConnectionState state) {
  shutdown_checker::AssertNotShutdown();

  honey_media_observer_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_route_state_changed)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: route; type: refptr_diff
  DCHECK(route.get());
  if (!route.get()) {
    return;
  }

  // Execute
  _struct->on_route_state_changed(_struct, HoneycombMediaRouteCppToC::Wrap(route),
                                  state);
}

NO_SANITIZE("cfi-icall")
void HoneycombMediaObserverCToCpp::OnRouteMessageReceived(
    HoneycombRefPtr<HoneycombMediaRoute> route,
    const void* message,
    size_t message_size) {
  shutdown_checker::AssertNotShutdown();

  honey_media_observer_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, on_route_message_received)) {
    return;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: route; type: refptr_diff
  DCHECK(route.get());
  if (!route.get()) {
    return;
  }
  // Verify param: message; type: simple_byaddr
  DCHECK(message);
  if (!message) {
    return;
  }

  // Execute
  _struct->on_route_message_received(_struct, HoneycombMediaRouteCppToC::Wrap(route),
                                     message, message_size);
}

// CONSTRUCTOR - Do not edit by hand.

HoneycombMediaObserverCToCpp::HoneycombMediaObserverCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

HoneycombMediaObserverCToCpp::~HoneycombMediaObserverCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
honey_media_observer_t*
HoneycombCToCppRefCounted<HoneycombMediaObserverCToCpp,
                    HoneycombMediaObserver,
                    honey_media_observer_t>::UnwrapDerived(HoneycombWrapperType type,
                                                         HoneycombMediaObserver* c) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCToCppRefCounted<HoneycombMediaObserverCToCpp,
                                   HoneycombMediaObserver,
                                   honey_media_observer_t>::kWrapperType =
    WT_MEDIA_OBSERVER;
