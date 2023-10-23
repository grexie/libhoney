// Copyright (c) 2023 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeyclient/browser/default_client_handler.h"

#include "tests/honeyclient/browser/test_runner.h"

namespace client {

DefaultClientHandler::DefaultClientHandler() {
  resource_manager_ = new HoneycombResourceManager();
  test_runner::SetupResourceManager(resource_manager_, nullptr);
}

HoneycombRefPtr<HoneycombResourceRequestHandler>
DefaultClientHandler::GetResourceRequestHandler(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombFrame> frame,
    HoneycombRefPtr<HoneycombRequest> request,
    bool is_navigation,
    bool is_download,
    const HoneycombString& request_initiator,
    bool& disable_default_handling) {
  HONEYCOMB_REQUIRE_IO_THREAD();
  return this;
}

honey_return_value_t DefaultClientHandler::OnBeforeResourceLoad(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombFrame> frame,
    HoneycombRefPtr<HoneycombRequest> request,
    HoneycombRefPtr<HoneycombCallback> callback) {
  HONEYCOMB_REQUIRE_IO_THREAD();

  return resource_manager_->OnBeforeResourceLoad(browser, frame, request,
                                                 callback);
}

HoneycombRefPtr<HoneycombResourceHandler> DefaultClientHandler::GetResourceHandler(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombFrame> frame,
    HoneycombRefPtr<HoneycombRequest> request) {
  HONEYCOMB_REQUIRE_IO_THREAD();

  return resource_manager_->GetResourceHandler(browser, frame, request);
}

HoneycombRefPtr<HoneycombResponseFilter> DefaultClientHandler::GetResourceResponseFilter(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombFrame> frame,
    HoneycombRefPtr<HoneycombRequest> request,
    HoneycombRefPtr<HoneycombResponse> response) {
  HONEYCOMB_REQUIRE_IO_THREAD();

  return test_runner::GetResourceResponseFilter(browser, frame, request,
                                                response);
}

}  // namespace client
