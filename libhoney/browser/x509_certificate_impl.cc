// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoney/browser/x509_certificate_impl.h"

#include "libhoney/browser/x509_cert_principal_impl.h"
#include "libhoney/common/time_util.h"

#include "net/cert/x509_util.h"
#include "net/ssl/ssl_private_key.h"

namespace {

HoneycombRefPtr<HoneycombBinaryValue> EncodeCertificate(const CRYPTO_BUFFER* cert_buffer,
                                            bool der) {
  std::string encoded;
  if (der) {
    encoded =
        std::string(net::x509_util::CryptoBufferAsStringPiece(cert_buffer));
  } else if (!net::X509Certificate::GetPEMEncoded(cert_buffer, &encoded)) {
    return nullptr;
  }
  if (encoded.empty()) {
    return nullptr;
  }
  return HoneycombBinaryValue::Create(encoded.c_str(), encoded.size());
}

}  // namespace

HoneycombX509CertificateImpl::HoneycombX509CertificateImpl(
    std::unique_ptr<net::ClientCertIdentity> identity)
    : identity_(std::move(identity)), cert_(identity_->certificate()) {}

HoneycombX509CertificateImpl::HoneycombX509CertificateImpl(
    scoped_refptr<net::X509Certificate> cert)
    : cert_(cert) {}

HoneycombRefPtr<HoneycombX509CertPrincipal> HoneycombX509CertificateImpl::GetSubject() {
  if (cert_) {
    return new HoneycombX509CertPrincipalImpl(cert_->subject());
  }
  return nullptr;
}

HoneycombRefPtr<HoneycombX509CertPrincipal> HoneycombX509CertificateImpl::GetIssuer() {
  if (cert_) {
    return new HoneycombX509CertPrincipalImpl(cert_->issuer());
  }
  return nullptr;
}

HoneycombRefPtr<HoneycombBinaryValue> HoneycombX509CertificateImpl::GetSerialNumber() {
  if (cert_) {
    const std::string& serial = cert_->serial_number();
    return HoneycombBinaryValue::Create(serial.c_str(), serial.size());
  }
  return nullptr;
}

HoneycombBaseTime HoneycombX509CertificateImpl::GetValidStart() {
  if (cert_) {
    return cert_->valid_start();
  }
  return HoneycombBaseTime();
}

HoneycombBaseTime HoneycombX509CertificateImpl::GetValidExpiry() {
  if (cert_) {
    return cert_->valid_expiry();
  }
  return HoneycombBaseTime();
}

HoneycombRefPtr<HoneycombBinaryValue> HoneycombX509CertificateImpl::GetDEREncoded() {
  if (cert_) {
    const CRYPTO_BUFFER* cert_buffer = cert_->cert_buffer();
    if (cert_buffer) {
      return EncodeCertificate(cert_buffer, true);
    }
  }
  return nullptr;
}

HoneycombRefPtr<HoneycombBinaryValue> HoneycombX509CertificateImpl::GetPEMEncoded() {
  if (cert_) {
    const CRYPTO_BUFFER* cert_buffer = cert_->cert_buffer();
    if (cert_buffer) {
      return EncodeCertificate(cert_buffer, false);
    }
  }
  return nullptr;
}

size_t HoneycombX509CertificateImpl::GetIssuerChainSize() {
  if (cert_) {
    return cert_->intermediate_buffers().size();
  }
  return 0;
}

void HoneycombX509CertificateImpl::AcquirePrivateKey(
    base::OnceCallback<void(scoped_refptr<net::SSLPrivateKey>)>
        private_key_callback) {
  if (identity_) {
    identity_->AcquirePrivateKey(std::move(private_key_callback));
  } else {
    std::move(private_key_callback).Run(nullptr);
  }
}

void HoneycombX509CertificateImpl::GetEncodedIssuerChain(
    HoneycombX509Certificate::IssuerChainBinaryList& chain,
    bool der) {
  chain.clear();
  if (cert_) {
    for (const auto& it : cert_->intermediate_buffers()) {
      // Add each to the chain, even if one conversion unexpectedly failed.
      // GetIssuerChainSize depends on these being the same length.
      chain.push_back(EncodeCertificate(it.get(), der));
    }
  }
}

void HoneycombX509CertificateImpl::GetDEREncodedIssuerChain(
    HoneycombX509Certificate::IssuerChainBinaryList& chain) {
  if (der_encoded_issuer_chain_.empty()) {
    GetEncodedIssuerChain(der_encoded_issuer_chain_, true);
  }
  chain = der_encoded_issuer_chain_;
}

void HoneycombX509CertificateImpl::GetPEMEncodedIssuerChain(
    HoneycombX509Certificate::IssuerChainBinaryList& chain) {
  if (pem_encoded_issuer_chain_.empty()) {
    GetEncodedIssuerChain(pem_encoded_issuer_chain_, false);
  }
  chain = pem_encoded_issuer_chain_;
}
