// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoneycomb/browser/ssl_status_impl.h"

#include "libhoneycomb/browser/x509_certificate_impl.h"

#include "net/ssl/ssl_connection_status_flags.h"

HoneycombSSLStatusImpl::HoneycombSSLStatusImpl(const content::SSLStatus& value) {
  cert_status_ = static_cast<honey_cert_status_t>(value.cert_status);
  content_status_ = static_cast<honey_ssl_content_status_t>(value.content_status);
  ssl_version_ = static_cast<honey_ssl_version_t>(
      net::SSLConnectionStatusToVersion(value.connection_status));
  certificate_ = value.certificate;
}

bool HoneycombSSLStatusImpl::IsSecureConnection() {
  return !!certificate_.get();
}

honey_cert_status_t HoneycombSSLStatusImpl::GetCertStatus() {
  return cert_status_;
}

honey_ssl_version_t HoneycombSSLStatusImpl::GetSSLVersion() {
  return ssl_version_;
}

honey_ssl_content_status_t HoneycombSSLStatusImpl::GetContentStatus() {
  return content_status_;
}

HoneycombRefPtr<HoneycombX509Certificate> HoneycombSSLStatusImpl::GetX509Certificate() {
  if (certificate_ && !honey_certificate_) {
    honey_certificate_ = new HoneycombX509CertificateImpl(certificate_);
  }
  return honey_certificate_;
}
