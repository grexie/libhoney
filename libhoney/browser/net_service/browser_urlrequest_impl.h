// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_NET_SERVICE_BROWSER_URLREQUEST_IMPL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_NET_SERVICE_BROWSER_URLREQUEST_IMPL_H_

#include <memory>

#include "include/honey_urlrequest.h"

#include "third_party/abseil-cpp/absl/types/optional.h"

namespace content {
struct GlobalRequestID;
}

class HoneycombBrowserURLRequest : public HoneycombURLRequest {
 public:
  class Context;

  // TODO(network): After the old network code path is deleted move the
  // HoneycombURLRequestClient::GetAuthCredentials callback to the context thread and
  // return just the HoneycombBrowserURLRequest object here. The *Client object can
  // then be retrieved by calling GetClient() from the required thread.
  using RequestInfo = std::pair<HoneycombRefPtr<HoneycombBrowserURLRequest>,
                                HoneycombRefPtr<HoneycombURLRequestClient>>;

  // Retrieve the request objects, if any, associated with |request_id|.
  static absl::optional<RequestInfo> FromRequestID(int32_t request_id);
  static absl::optional<RequestInfo> FromRequestID(
      const content::GlobalRequestID& request_id);

  // If |frame| is nullptr requests can still be intercepted but no
  // browser/frame will be associated with them.
  HoneycombBrowserURLRequest(HoneycombRefPtr<HoneycombFrame> frame,
                       HoneycombRefPtr<HoneycombRequest> request,
                       HoneycombRefPtr<HoneycombURLRequestClient> client,
                       HoneycombRefPtr<HoneycombRequestContext> request_context);
  ~HoneycombBrowserURLRequest() override;

  bool Start();

  // HoneycombURLRequest methods.
  HoneycombRefPtr<HoneycombRequest> GetRequest() override;
  HoneycombRefPtr<HoneycombURLRequestClient> GetClient() override;
  Status GetRequestStatus() override;
  ErrorCode GetRequestError() override;
  HoneycombRefPtr<HoneycombResponse> GetResponse() override;
  bool ResponseWasCached() override;
  void Cancel() override;

 private:
  bool VerifyContext();

  std::unique_ptr<Context> context_;

  IMPLEMENT_REFCOUNTING(HoneycombBrowserURLRequest);
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_NET_SERVICE_BROWSER_URLREQUEST_IMPL_H_
