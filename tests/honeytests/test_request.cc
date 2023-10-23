// Copyright (c) 2020 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeytests/test_request.h"

#include <algorithm>
#include <memory>

#include "include/honey_stream.h"
#include "include/honey_urlrequest.h"
#include "include/wrapper/honey_closure_task.h"
#include "include/wrapper/honey_helpers.h"
#include "include/wrapper/honey_stream_resource_handler.h"

namespace test_request {

namespace {

// Implementation of HoneycombURLRequestClient that stores response information.
class RequestClient : public HoneycombURLRequestClient, public State {
 public:
  RequestClient(const bool has_credentials,
                const std::string& username,
                const std::string& password,
                RequestDoneCallback done_callback)
      : has_credentials_(has_credentials),
        username_(username),
        password_(password),
        done_callback_(std::move(done_callback)) {
    DCHECK(!done_callback_.is_null());
  }

  void OnUploadProgress(HoneycombRefPtr<HoneycombURLRequest> request,
                        int64_t current,
                        int64_t total) override {
    upload_progress_ct_++;
    upload_total_ = total;
  }

  void OnDownloadProgress(HoneycombRefPtr<HoneycombURLRequest> request,
                          int64_t current,
                          int64_t total) override {
    response_ = request->GetResponse();
    DCHECK(response_.get());
    DCHECK(response_->IsReadOnly());
    download_progress_ct_++;
    download_total_ = total;
  }

  void OnDownloadData(HoneycombRefPtr<HoneycombURLRequest> request,
                      const void* data,
                      size_t data_length) override {
    response_ = request->GetResponse();
    DCHECK(response_.get());
    DCHECK(response_->IsReadOnly());
    download_data_ct_++;
    download_data_ += std::string(static_cast<const char*>(data), data_length);
  }

  bool GetAuthCredentials(bool isProxy,
                          const HoneycombString& host,
                          int port,
                          const HoneycombString& realm,
                          const HoneycombString& scheme,
                          HoneycombRefPtr<HoneycombAuthCallback> callback) override {
    auth_credentials_ct_++;
    if (has_credentials_) {
      callback->Continue(username_, password_);
      return true;
    }
    return false;
  }

  void OnRequestComplete(HoneycombRefPtr<HoneycombURLRequest> request) override {
    request_complete_ct_++;

    request_ = request->GetRequest();
    DCHECK(request_->IsReadOnly());
    status_ = request->GetRequestStatus();
    error_code_ = request->GetRequestError();
    response_was_cached_ = request->ResponseWasCached();
    response_ = request->GetResponse();
    if (response_) {
      DCHECK(response_->IsReadOnly());
    }

    std::move(done_callback_).Run(*this);
  }

 private:
  const bool has_credentials_;
  const std::string username_;
  const std::string password_;
  RequestDoneCallback done_callback_;

  IMPLEMENT_REFCOUNTING(RequestClient);
  DISALLOW_COPY_AND_ASSIGN(RequestClient);
};

// Implementation that collects all cookies, and optionally deletes them.
class CookieVisitor : public HoneycombCookieVisitor {
 public:
  CookieVisitor(bool deleteCookies, CookieDoneCallback callback)
      : delete_cookies_(deleteCookies), callback_(std::move(callback)) {
    DCHECK(!callback_.is_null());
  }

  ~CookieVisitor() override {
    HONEYCOMB_REQUIRE_UI_THREAD();
    std::move(callback_).Run(cookies_);
  }

  bool Visit(const HoneycombCookie& cookie,
             int count,
             int total,
             bool& deleteCookie) override {
    HONEYCOMB_REQUIRE_UI_THREAD();
    cookies_.push_back(cookie);
    if (delete_cookies_) {
      deleteCookie = true;
    }
    return true;
  }

 private:
  CookieVector cookies_;
  bool delete_cookies_;
  CookieDoneCallback callback_;

  IMPLEMENT_REFCOUNTING(CookieVisitor);
  DISALLOW_COPY_AND_ASSIGN(CookieVisitor);
};

}  // namespace

void Send(const SendConfig& config, RequestDoneCallback callback) {
  DCHECK(config.request_);
  HoneycombRefPtr<RequestClient> client =
      new RequestClient(config.has_credentials_, config.username_,
                        config.password_, std::move(callback));
  if (config.frame_) {
    config.frame_->CreateURLRequest(config.request_, client.get());
  } else {
    HoneycombURLRequest::Create(config.request_, client.get(),
                          config.request_context_);
  }
}

std::string GetPathURL(const std::string& url) {
  const size_t index1 = url.find('?');
  const size_t index2 = url.find('#');
  size_t index = std::string::npos;
  if (index1 != std::string::npos && index2 != std::string::npos) {
    index = std::min(index1, index2);
  } else if (index1 != std::string::npos) {
    index = index1;
  } else if (index2 != std::string::npos) {
    index = index2;
  }
  if (index != std::string::npos) {
    return url.substr(0, index);
  }
  return url;
}

HoneycombRefPtr<HoneycombResourceHandler> CreateResourceHandler(
    HoneycombRefPtr<HoneycombResponse> response,
    const std::string& response_data) {
  HoneycombRefPtr<HoneycombStreamReader> stream;
  if (!response_data.empty()) {
    stream = HoneycombStreamReader::CreateForData(
        static_cast<void*>(const_cast<char*>(response_data.c_str())),
        response_data.length());
  }

  HoneycombResponse::HeaderMap headerMap;
  response->GetHeaderMap(headerMap);

  return new HoneycombStreamResourceHandler(
      response->GetStatus(), response->GetStatusText(), response->GetMimeType(),
      headerMap, stream);
}

void GetAllCookies(HoneycombRefPtr<HoneycombCookieManager> manager,
                   bool deleteCookies,
                   CookieDoneCallback callback) {
  bool result = manager->VisitAllCookies(
      new CookieVisitor(deleteCookies, std::move(callback)));
  DCHECK(result);
}

// Retrieves URL cookies from |manager| and executes |callback| upon completion.
// If |deleteCookies| is true the cookies will also be deleted.
void GetUrlCookies(HoneycombRefPtr<HoneycombCookieManager> manager,
                   const HoneycombString& url,
                   bool includeHttpOnly,
                   bool deleteCookies,
                   CookieDoneCallback callback) {
  bool result = manager->VisitUrlCookies(
      url, includeHttpOnly,
      new CookieVisitor(deleteCookies, std::move(callback)));
  DCHECK(result);
}

}  // namespace test_request
