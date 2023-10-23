// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoneycomb/browser/ssl_info_impl.h"
#include "libhoneycomb/browser/x509_certificate_impl.h"

#include "net/cert/cert_status_flags.h"

HoneycombSSLInfoImpl::HoneycombSSLInfoImpl(const net::SSLInfo& value)
    : cert_status_(CERT_STATUS_NONE) {
  cert_status_ = static_cast<honey_cert_status_t>(value.cert_status);
  if (value.cert.get()) {
    cert_ = new HoneycombX509CertificateImpl(value.cert);
  }
}

honey_cert_status_t HoneycombSSLInfoImpl::GetCertStatus() {
  return cert_status_;
}

HoneycombRefPtr<HoneycombX509Certificate> HoneycombSSLInfoImpl::GetX509Certificate() {
  return cert_;
}

bool HoneycombIsCertStatusError(honey_cert_status_t status) {
  return net::IsCertStatusError(status);
}
