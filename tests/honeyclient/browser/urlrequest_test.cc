// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeyclient/browser/urlrequest_test.h"

#include <memory>
#include <string>

#include "include/base/honey_callback.h"
#include "include/base/honey_logging.h"
#include "include/honey_urlrequest.h"
#include "include/wrapper/honey_helpers.h"
#include "tests/honeyclient/browser/test_runner.h"

namespace client {
namespace urlrequest_test {

namespace {

const char kTestUrlPath[] = "/urlrequest";
const char kTestMessageName[] = "URLRequestTest";

// Implementation of HoneycombURLRequestClient that stores response information. Only
// accessed on the UI thread.
class RequestClient : public HoneycombURLRequestClient {
 public:
  // Callback to be executed on request completion.
  using Callback =
      base::OnceCallback<void(HoneycombURLRequest::ErrorCode /*error_code*/,
                              const std::string& /*download_data*/)>;

  explicit RequestClient(Callback callback) : callback_(std::move(callback)) {
    HONEYCOMB_REQUIRE_UI_THREAD();
    DCHECK(!callback_.is_null());
  }

  void Detach() {
    HONEYCOMB_REQUIRE_UI_THREAD();
    if (!callback_.is_null()) {
      callback_.Reset();
    }
  }

  void OnRequestComplete(HoneycombRefPtr<HoneycombURLRequest> request) override {
    HONEYCOMB_REQUIRE_UI_THREAD();
    if (!callback_.is_null()) {
      std::move(callback_).Run(request->GetRequestError(), download_data_);
    }
  }

  void OnUploadProgress(HoneycombRefPtr<HoneycombURLRequest> request,
                        int64_t current,
                        int64_t total) override {}

  void OnDownloadProgress(HoneycombRefPtr<HoneycombURLRequest> request,
                          int64_t current,
                          int64_t total) override {}

  void OnDownloadData(HoneycombRefPtr<HoneycombURLRequest> request,
                      const void* data,
                      size_t data_length) override {
    HONEYCOMB_REQUIRE_UI_THREAD();
    download_data_ += std::string(static_cast<const char*>(data), data_length);
  }

  bool GetAuthCredentials(bool isProxy,
                          const HoneycombString& host,
                          int port,
                          const HoneycombString& realm,
                          const HoneycombString& scheme,
                          HoneycombRefPtr<HoneycombAuthCallback> callback) override {
    return false;
  }

 private:
  Callback callback_;
  std::string download_data_;

  IMPLEMENT_REFCOUNTING(RequestClient);
  DISALLOW_COPY_AND_ASSIGN(RequestClient);
};

// Handle messages in the browser process. Only accessed on the UI thread.
class Handler : public HoneycombMessageRouterBrowserSide::Handler {
 public:
  Handler() { HONEYCOMB_REQUIRE_UI_THREAD(); }

  ~Handler() { CancelPendingRequest(); }

  // Called due to honeyQuery execution in urlrequest.html.
  bool OnQuery(HoneycombRefPtr<HoneycombBrowser> browser,
               HoneycombRefPtr<HoneycombFrame> frame,
               int64_t query_id,
               const HoneycombString& request,
               bool persistent,
               HoneycombRefPtr<Callback> callback) override {
    HONEYCOMB_REQUIRE_UI_THREAD();

    // Only handle messages from the test URL.
    const std::string& url = frame->GetURL();
    if (!test_runner::IsTestURL(url, kTestUrlPath)) {
      return false;
    }

    const std::string& message_name = request;
    if (message_name.find(kTestMessageName) == 0) {
      const std::string& load_url =
          message_name.substr(sizeof(kTestMessageName));

      CancelPendingRequest();

      DCHECK(!callback_.get());
      DCHECK(!urlrequest_.get());

      callback_ = callback;

      // Create a HoneycombRequest for the specified URL.
      HoneycombRefPtr<HoneycombRequest> honey_request = HoneycombRequest::Create();
      honey_request->SetURL(load_url);
      honey_request->SetMethod("GET");

      // Callback to be executed on request completion.
      // It's safe to use base::Unretained() here because there is only one
      // RequestClient pending at any given time and we explicitly detach the
      // callback in the Handler destructor.
      auto request_callback =
          base::BindOnce(&Handler::OnRequestComplete, base::Unretained(this));

      // Create and start a new HoneycombURLRequest associated with the frame, so
      // that it shares authentication with ClientHandler::GetAuthCredentials.
      urlrequest_ = frame->CreateURLRequest(
          honey_request, new RequestClient(std::move(request_callback)));

      return true;
    }

    return false;
  }

 private:
  // Cancel the currently pending URL request, if any.
  void CancelPendingRequest() {
    HONEYCOMB_REQUIRE_UI_THREAD();

    if (urlrequest_.get()) {
      // Don't execute the callback when we explicitly cancel the request.
      static_cast<RequestClient*>(urlrequest_->GetClient().get())->Detach();

      urlrequest_->Cancel();
      urlrequest_ = nullptr;
    }

    if (callback_.get()) {
      // Must always execute |callback_| before deleting it.
      callback_->Failure(ERR_ABORTED, test_runner::GetErrorString(ERR_ABORTED));
      callback_ = nullptr;
    }
  }

  void OnRequestComplete(HoneycombURLRequest::ErrorCode error_code,
                         const std::string& download_data) {
    HONEYCOMB_REQUIRE_UI_THREAD();

    if (error_code == ERR_NONE) {
      callback_->Success(download_data);
    } else {
      callback_->Failure(error_code, test_runner::GetErrorString(error_code));
    }

    callback_ = nullptr;
    urlrequest_ = nullptr;
  }

  HoneycombRefPtr<Callback> callback_;
  HoneycombRefPtr<HoneycombURLRequest> urlrequest_;

  DISALLOW_COPY_AND_ASSIGN(Handler);
};

}  // namespace

void CreateMessageHandlers(test_runner::MessageHandlerSet& handlers) {
  handlers.insert(new Handler());
}

}  // namespace urlrequest_test
}  // namespace client
