// Copyright (c) 2023 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the Honeycomb translator tool. If making changes by
// hand only do so within the body of existing method and function
// implementations. See the translator.README.txt file in the tools directory
// for more information.
//
// $hash=39cb5dd7488f7035a1b52e50b48e3bffee27dba6$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CTOCPP_X509CERTIFICATE_CTOCPP_H_
#define HONEYCOMB_LIBHONEY_DLL_CTOCPP_X509CERTIFICATE_CTOCPP_H_
#pragma once

#if !defined(WRAPPING_HONEYCOMB_SHARED)
#error This file can be included wrapper-side only
#endif

#include "include/capi/honey_x509_certificate_capi.h"
#include "include/honey_x509_certificate.h"
#include "libhoney_dll/ctocpp/ctocpp_ref_counted.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class HoneycombX509CertificateCToCpp
    : public HoneycombCToCppRefCounted<HoneycombX509CertificateCToCpp,
                                 HoneycombX509Certificate,
                                 honey_x509certificate_t> {
 public:
  HoneycombX509CertificateCToCpp();
  virtual ~HoneycombX509CertificateCToCpp();

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
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CTOCPP_X509CERTIFICATE_CTOCPP_H_