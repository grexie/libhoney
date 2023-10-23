// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_SSL_INFO_IMPL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_SSL_INFO_IMPL_H_
#pragma once

#include "include/honey_ssl_info.h"

#include "net/ssl/ssl_info.h"

// HoneycombSSLInfo implementation
class HoneycombSSLInfoImpl : public HoneycombSSLInfo {
 public:
  explicit HoneycombSSLInfoImpl(const net::SSLInfo& value);

  HoneycombSSLInfoImpl(const HoneycombSSLInfoImpl&) = delete;
  HoneycombSSLInfoImpl& operator=(const HoneycombSSLInfoImpl&) = delete;

  // HoneycombSSLInfo methods.
  honey_cert_status_t GetCertStatus() override;
  HoneycombRefPtr<HoneycombX509Certificate> GetX509Certificate() override;

 private:
  honey_cert_status_t cert_status_;
  HoneycombRefPtr<HoneycombX509Certificate> cert_;

  IMPLEMENT_REFCOUNTING(HoneycombSSLInfoImpl);
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_SSL_INFO_IMPL_H_
