// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_TEST_RUNNER_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_TEST_RUNNER_H_
#pragma once

#include <set>
#include <string>

#include "include/honey_browser.h"
#include "include/honey_request.h"
#include "include/wrapper/honey_message_router.h"
#include "include/wrapper/honey_resource_manager.h"

namespace client {
namespace test_runner {

// Run a test.
void RunTest(HoneycombRefPtr<HoneycombBrowser> browser, int id);

// Returns the contents of the HoneycombRequest as a string.
std::string DumpRequestContents(HoneycombRefPtr<HoneycombRequest> request);

// Returns the dump response as a stream. |request| is the request.
// |response_headers| will be populated with extra response headers, if any.
HoneycombRefPtr<HoneycombStreamReader> GetDumpResponse(
    HoneycombRefPtr<HoneycombRequest> request,
    HoneycombResponse::HeaderMap& response_headers);

// Returns a data: URI with the specified contents.
std::string GetDataURI(const std::string& data, const std::string& mime_type);

// Returns the string representation of the specified error code.
std::string GetErrorString(honey_errorcode_t code);

typedef std::map<std::string, std::string> StringResourceMap;

// Set up the resource manager for tests.
void SetupResourceManager(HoneycombRefPtr<HoneycombResourceManager> resource_manager,
                          StringResourceMap* string_resource_map);

// Show a JS alert message.
void Alert(HoneycombRefPtr<HoneycombBrowser> browser, const std::string& message);

// Returns "https://tests/<path>".
std::string GetTestURL(const std::string& path);

// Returns true if |url| is a test URL with the specified |path|. This matches
// both "https://tests/<path>" and "http://localhost:xxxx/<path>".
bool IsTestURL(const std::string& url, const std::string& path);

// Create all HoneycombMessageRouterBrowserSide::Handler objects. They will be
// deleted when the ClientHandler is destroyed.
typedef std::set<HoneycombMessageRouterBrowserSide::Handler*> MessageHandlerSet;
void CreateMessageHandlers(MessageHandlerSet& handlers);

// Register scheme handlers for tests.
void RegisterSchemeHandlers();

// Create a resource response filter for tests.
HoneycombRefPtr<HoneycombResponseFilter> GetResourceResponseFilter(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombFrame> frame,
    HoneycombRefPtr<HoneycombRequest> request,
    HoneycombRefPtr<HoneycombResponse> response);

}  // namespace test_runner
}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_TEST_RUNNER_H_
