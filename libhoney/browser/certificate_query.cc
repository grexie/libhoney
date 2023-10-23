// Copyright 2022 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoney/browser/certificate_query.h"

#include "include/honey_request_handler.h"
#include "libhoney/browser/browser_host_base.h"
#include "libhoney/browser/ssl_info_impl.h"
#include "libhoney/browser/thread_util.h"

#include "base/functional/callback.h"
#include "base/functional/callback_helpers.h"
#include "base/logging.h"
#include "content/public/browser/web_contents.h"
#include "net/ssl/ssl_info.h"
#include "url/gurl.h"

namespace certificate_query {

namespace {

class HoneycombAllowCertificateErrorCallbackImpl : public HoneycombCallback {
 public:
  using CallbackType = CertificateErrorCallback;

  explicit HoneycombAllowCertificateErrorCallbackImpl(CallbackType callback)
      : callback_(std::move(callback)) {}

  HoneycombAllowCertificateErrorCallbackImpl(
      const HoneycombAllowCertificateErrorCallbackImpl&) = delete;
  HoneycombAllowCertificateErrorCallbackImpl& operator=(
      const HoneycombAllowCertificateErrorCallbackImpl&) = delete;

  ~HoneycombAllowCertificateErrorCallbackImpl() {
    if (!callback_.is_null()) {
      // The callback is still pending. Cancel it now.
      if (HONEYCOMB_CURRENTLY_ON_UIT()) {
        RunNow(std::move(callback_), false);
      } else {
        HONEYCOMB_POST_TASK(
            HONEYCOMB_UIT,
            base::BindOnce(&HoneycombAllowCertificateErrorCallbackImpl::RunNow,
                           std::move(callback_), false));
      }
    }
  }

  void Continue() override { ContinueNow(true); }

  void Cancel() override { ContinueNow(false); }

  [[nodiscard]] CallbackType Disconnect() { return std::move(callback_); }

 private:
  void ContinueNow(bool allow) {
    if (HONEYCOMB_CURRENTLY_ON_UIT()) {
      if (!callback_.is_null()) {
        RunNow(std::move(callback_), allow);
      }
    } else {
      HONEYCOMB_POST_TASK(
          HONEYCOMB_UIT,
          base::BindOnce(&HoneycombAllowCertificateErrorCallbackImpl::ContinueNow,
                         this, allow));
    }
  }

  static void RunNow(CallbackType callback, bool allow) {
    HONEYCOMB_REQUIRE_UIT();
    std::move(callback).Run(
        allow ? content::CERTIFICATE_REQUEST_RESULT_TYPE_CONTINUE
              : content::CERTIFICATE_REQUEST_RESULT_TYPE_DENY);
  }

  CallbackType callback_;

  IMPLEMENT_REFCOUNTING(HoneycombAllowCertificateErrorCallbackImpl);
};

}  // namespace

CertificateErrorCallback AllowCertificateError(
    content::WebContents* web_contents,
    int cert_error,
    const net::SSLInfo& ssl_info,
    const GURL& request_url,
    bool is_main_frame_request,
    bool strict_enforcement,
    CertificateErrorCallback callback,
    bool default_disallow) {
  HONEYCOMB_REQUIRE_UIT();

  if (!is_main_frame_request) {
    // A sub-resource has a certificate error. The user doesn't really
    // have a context for making the right decision, so block the request
    // hard.
    std::move(callback).Run(content::CERTIFICATE_REQUEST_RESULT_TYPE_DENY);
    return base::NullCallback();
  }

  if (auto browser = HoneycombBrowserHostBase::GetBrowserForContents(web_contents)) {
    if (auto client = browser->GetClient()) {
      if (auto handler = client->GetRequestHandler()) {
        HoneycombRefPtr<HoneycombSSLInfo> sslInfo(new HoneycombSSLInfoImpl(ssl_info));
        HoneycombRefPtr<HoneycombAllowCertificateErrorCallbackImpl> callbackImpl(
            new HoneycombAllowCertificateErrorCallbackImpl(std::move(callback)));

        bool proceed = handler->OnCertificateError(
            browser.get(), static_cast<honey_errorcode_t>(cert_error),
            request_url.spec(), sslInfo, callbackImpl.get());
        if (!proceed) {
          callback = callbackImpl->Disconnect();
          LOG_IF(ERROR, callback.is_null())
              << "Should return true from OnCertificateError when executing "
                 "the callback";
        }
      }
    }
  }

  if (!callback.is_null() && default_disallow) {
    std::move(callback).Run(content::CERTIFICATE_REQUEST_RESULT_TYPE_DENY);
    return base::NullCallback();
  }

  return callback;
}

}  // namespace certificate_query
