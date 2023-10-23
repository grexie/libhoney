// Copyright (c) 2020 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBTESTS_TEST_REQUEST_H_
#define HONEYCOMB_TESTS_HONEYCOMBTESTS_TEST_REQUEST_H_
#pragma once

#include <string>

#include "include/base/honey_callback.h"
#include "include/honey_cookie.h"
#include "include/honey_frame.h"
#include "include/honey_request.h"
#include "include/honey_request_context.h"
#include "include/honey_resource_handler.h"
#include "include/honey_response.h"

namespace test_request {

// Stores all state passed to HoneycombURLRequestClient.
struct State {
 public:
  // Number of times each callback is executed.
  int upload_progress_ct_ = 0;
  int download_progress_ct_ = 0;
  int download_data_ct_ = 0;
  int auth_credentials_ct_ = 0;
  int request_complete_ct_ = 0;

  // From OnUploadProgress.
  int64_t upload_total_ = 0;

  // From OnDownloadProgress.
  int64_t download_total_ = 0;

  // From OnDownloadData.
  std::string download_data_;

  // From OnRequestComplete.
  HoneycombRefPtr<HoneycombRequest> request_;
  honey_urlrequest_status_t status_ = UR_UNKNOWN;
  honey_errorcode_t error_code_ = ERR_NONE;
  HoneycombRefPtr<HoneycombResponse> response_;
  bool response_was_cached_ = false;
};

using RequestDoneCallback = base::OnceCallback<void(const State& state)>;

struct SendConfig {
  // Send using |frame_| or |request_context_| if non-nullptr.
  // Sends using the global request context if both are nullptr.
  HoneycombRefPtr<HoneycombFrame> frame_;
  HoneycombRefPtr<HoneycombRequestContext> request_context_;

  // The request to send.
  HoneycombRefPtr<HoneycombRequest> request_;

  // Returned via GetAuthCredentials if |has_credentials_| is true.
  bool has_credentials_ = false;
  std::string username_;
  std::string password_;
};

// Send a request. |callback| will be executed on the calling thread after the
// request completes.
void Send(const SendConfig& config, RequestDoneCallback callback);

// Removes query and/or fragment components from |url|.
std::string GetPathURL(const std::string& url);

// Creates a new resource handler that returns the specified response.
HoneycombRefPtr<HoneycombResourceHandler> CreateResourceHandler(
    HoneycombRefPtr<HoneycombResponse> response,
    const std::string& response_data);

using CookieVector = std::vector<HoneycombCookie>;
using CookieDoneCallback =
    base::OnceCallback<void(const CookieVector& cookies)>;

// Retrieves all cookies from |manager| and executes |callback| upon completion.
// If |deleteCookies| is true the cookies will also be deleted.
void GetAllCookies(HoneycombRefPtr<HoneycombCookieManager> manager,
                   bool deleteCookies,
                   CookieDoneCallback callback);

// Retrieves URL cookies from |manager| and executes |callback| upon completion.
// If |deleteCookies| is true the cookies will also be deleted.
void GetUrlCookies(HoneycombRefPtr<HoneycombCookieManager> manager,
                   const HoneycombString& url,
                   bool includeHttpOnly,
                   bool deleteCookies,
                   CookieDoneCallback callback);

}  // namespace test_request

#endif  // HONEYCOMB_TESTS_HONEYCOMBTESTS_TEST_REQUEST_H_
