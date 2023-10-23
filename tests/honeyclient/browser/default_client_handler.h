// Copyright (c) 2023 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_DEFAULT_CLIENT_HANDLER_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_DEFAULT_CLIENT_HANDLER_H_
#pragma once

#include "include/honey_client.h"
#include "include/wrapper/honey_resource_manager.h"

namespace client {

// Default client handler for unmanaged browser windows. Used with the Chrome
// runtime only.
class DefaultClientHandler : public HoneycombClient,
                             public HoneycombRequestHandler,
                             public HoneycombResourceRequestHandler {
 public:
  DefaultClientHandler();

  // HoneycombClient methods
  HoneycombRefPtr<HoneycombRequestHandler> GetRequestHandler() override { return this; }

  // HoneycombRequestHandler methods
  HoneycombRefPtr<HoneycombResourceRequestHandler> GetResourceRequestHandler(
      HoneycombRefPtr<HoneycombBrowser> browser,
      HoneycombRefPtr<HoneycombFrame> frame,
      HoneycombRefPtr<HoneycombRequest> request,
      bool is_navigation,
      bool is_download,
      const HoneycombString& request_initiator,
      bool& disable_default_handling) override;

  // HoneycombResourceRequestHandler methods
  honey_return_value_t OnBeforeResourceLoad(
      HoneycombRefPtr<HoneycombBrowser> browser,
      HoneycombRefPtr<HoneycombFrame> frame,
      HoneycombRefPtr<HoneycombRequest> request,
      HoneycombRefPtr<HoneycombCallback> callback) override;
  HoneycombRefPtr<HoneycombResourceHandler> GetResourceHandler(
      HoneycombRefPtr<HoneycombBrowser> browser,
      HoneycombRefPtr<HoneycombFrame> frame,
      HoneycombRefPtr<HoneycombRequest> request) override;
  HoneycombRefPtr<HoneycombResponseFilter> GetResourceResponseFilter(
      HoneycombRefPtr<HoneycombBrowser> browser,
      HoneycombRefPtr<HoneycombFrame> frame,
      HoneycombRefPtr<HoneycombRequest> request,
      HoneycombRefPtr<HoneycombResponse> response) override;

 private:
  // Manages the registration and delivery of resources.
  HoneycombRefPtr<HoneycombResourceManager> resource_manager_;

  IMPLEMENT_REFCOUNTING(DefaultClientHandler);
  DISALLOW_COPY_AND_ASSIGN(DefaultClientHandler);
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_DEFAULT_CLIENT_HANDLER_H_
