// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_RESPONSE_FILTER_TEST_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_RESPONSE_FILTER_TEST_H_
#pragma once

#include "include/honey_browser.h"
#include "include/honey_request.h"
#include "include/honey_response.h"
#include "include/honey_response_filter.h"

namespace client {
namespace response_filter_test {

// Create a resource response filter. Called from test_runner.cc.
HoneycombRefPtr<HoneycombResponseFilter> GetResourceResponseFilter(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombFrame> frame,
    HoneycombRefPtr<HoneycombRequest> request,
    HoneycombRefPtr<HoneycombResponse> response);

}  // namespace response_filter_test
}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_RESPONSE_FILTER_TEST_H_
