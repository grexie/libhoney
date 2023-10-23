// Copyright (c) 2019 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_NET_SERVICE_RESOURCE_HANDLER_WRAPPER_H_
#define HONEYCOMB_LIBHONEY_BROWSER_NET_SERVICE_RESOURCE_HANDLER_WRAPPER_H_

#include "include/honey_request.h"
#include "include/honey_resource_handler.h"

namespace net_service {

class ResourceResponse;

// Create a ResourceResponse that delegates to |handler|.
// The resulting object should be passed to
// InterceptedRequestHandler::ShouldInterceptRequestResultCallback.
std::unique_ptr<ResourceResponse> CreateResourceResponse(
    int32_t request_id,
    HoneycombRefPtr<HoneycombResourceHandler> handler);

}  // namespace net_service

#endif  // HONEYCOMB_LIBHONEY_BROWSER_NET_SERVICE_RESOURCE_HANDLER_WRAPPER_H_
