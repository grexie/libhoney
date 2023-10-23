// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_SSL_STATUS_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_SSL_STATUS_IMPL_H_
#pragma once

#include "include/honey_ssl_status.h"

#include "content/public/browser/ssl_status.h"

// HoneycombSSLStatus implementation
class HoneycombSSLStatusImpl : public HoneycombSSLStatus {
 public:
  explicit HoneycombSSLStatusImpl(const content::SSLStatus& value);

  HoneycombSSLStatusImpl(const HoneycombSSLStatusImpl&) = delete;
  HoneycombSSLStatusImpl& operator=(const HoneycombSSLStatusImpl&) = delete;

  // HoneycombSSLStatus methods.
  bool IsSecureConnection() override;
  honey_cert_status_t GetCertStatus() override;
  honey_ssl_version_t GetSSLVersion() override;
  honey_ssl_content_status_t GetContentStatus() override;
  HoneycombRefPtr<HoneycombX509Certificate> GetX509Certificate() override;

 private:
  honey_cert_status_t cert_status_;
  honey_ssl_version_t ssl_version_;
  honey_ssl_content_status_t content_status_;

  // Don't create a HoneycombX509Certificate object until requested.
  scoped_refptr<net::X509Certificate> certificate_;
  HoneycombRefPtr<HoneycombX509Certificate> honey_certificate_;

  IMPLEMENT_REFCOUNTING(HoneycombSSLStatusImpl);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_SSL_STATUS_IMPL_H_
