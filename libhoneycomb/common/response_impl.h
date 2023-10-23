// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_COMMON_RESPONSE_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_COMMON_RESPONSE_IMPL_H_
#pragma once

#include "include/honey_response.h"

#include "base/synchronization/lock.h"

namespace net {
class HttpResponseHeaders;
}  // namespace net

namespace blink {
class WebURLResponse;
}

// Implementation of HoneycombResponse.
class HoneycombResponseImpl : public HoneycombResponse {
 public:
  HoneycombResponseImpl();

  // HoneycombResponse methods.
  bool IsReadOnly() override;
  honey_errorcode_t GetError() override;
  void SetError(honey_errorcode_t error) override;
  int GetStatus() override;
  void SetStatus(int status) override;
  HoneycombString GetStatusText() override;
  void SetStatusText(const HoneycombString& statusText) override;
  HoneycombString GetMimeType() override;
  void SetMimeType(const HoneycombString& mimeType) override;
  HoneycombString GetCharset() override;
  void SetCharset(const HoneycombString& charset) override;
  HoneycombString GetHeaderByName(const HoneycombString& name) override;
  void SetHeaderByName(const HoneycombString& name,
                       const HoneycombString& value,
                       bool overwrite) override;
  void GetHeaderMap(HeaderMap& headerMap) override;
  void SetHeaderMap(const HeaderMap& headerMap) override;
  HoneycombString GetURL() override;
  void SetURL(const HoneycombString& url) override;

  scoped_refptr<net::HttpResponseHeaders> GetResponseHeaders();
  void SetResponseHeaders(const net::HttpResponseHeaders& headers);

  void Set(const blink::WebURLResponse& response);

  void SetReadOnly(bool read_only);

 protected:
  honey_errorcode_t error_code_;
  int status_code_;
  HoneycombString status_text_;
  HoneycombString mime_type_;
  HoneycombString charset_;
  HoneycombString url_;
  HeaderMap header_map_;
  bool read_only_;

  base::Lock lock_;

  IMPLEMENT_REFCOUNTING(HoneycombResponseImpl);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_COMMON_RESPONSE_IMPL_H_
