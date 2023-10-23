// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_X509_CERTIFICATE_IMPL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_X509_CERTIFICATE_IMPL_H_
#pragma once

#include "include/honey_x509_certificate.h"

#include <memory>

#include "net/ssl/client_cert_identity.h"

// HoneycombX509Certificate implementation
class HoneycombX509CertificateImpl : public HoneycombX509Certificate {
 public:
  explicit HoneycombX509CertificateImpl(scoped_refptr<net::X509Certificate> cert);

  HoneycombX509CertificateImpl(const HoneycombX509CertificateImpl&) = delete;
  HoneycombX509CertificateImpl& operator=(const HoneycombX509CertificateImpl&) = delete;

  // Used with AlloyContentBrowserClient::SelectClientCertificate only.
  explicit HoneycombX509CertificateImpl(
      std::unique_ptr<net::ClientCertIdentity> identity);

  // HoneycombX509Certificate methods.
  HoneycombRefPtr<HoneycombX509CertPrincipal> GetSubject() override;
  HoneycombRefPtr<HoneycombX509CertPrincipal> GetIssuer() override;
  HoneycombRefPtr<HoneycombBinaryValue> GetSerialNumber() override;
  HoneycombBaseTime GetValidStart() override;
  HoneycombBaseTime GetValidExpiry() override;
  HoneycombRefPtr<HoneycombBinaryValue> GetDEREncoded() override;
  HoneycombRefPtr<HoneycombBinaryValue> GetPEMEncoded() override;
  size_t GetIssuerChainSize() override;
  void GetDEREncodedIssuerChain(IssuerChainBinaryList& chain) override;
  void GetPEMEncodedIssuerChain(IssuerChainBinaryList& chain) override;

  scoped_refptr<net::X509Certificate> GetInternalCertObject() { return cert_; }
  void AcquirePrivateKey(
      base::OnceCallback<void(scoped_refptr<net::SSLPrivateKey>)>
          private_key_callback);

 private:
  void GetEncodedIssuerChain(IssuerChainBinaryList& chain, bool der);

  std::unique_ptr<net::ClientCertIdentity> identity_;
  scoped_refptr<net::X509Certificate> cert_;
  IssuerChainBinaryList pem_encoded_issuer_chain_;
  IssuerChainBinaryList der_encoded_issuer_chain_;

  IMPLEMENT_REFCOUNTING(HoneycombX509CertificateImpl);
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_X509_CERTIFICATE_IMPL_H_
