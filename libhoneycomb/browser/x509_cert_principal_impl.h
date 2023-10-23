// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_X509_CERT_PRINCIPAL_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_X509_CERT_PRINCIPAL_IMPL_H_
#pragma once

#include "include/honey_x509_certificate.h"

#include "net/cert/x509_cert_types.h"

// HoneycombX509CertPrincipal implementation
class HoneycombX509CertPrincipalImpl : public HoneycombX509CertPrincipal {
 public:
  explicit HoneycombX509CertPrincipalImpl(const net::CertPrincipal& value);

  HoneycombX509CertPrincipalImpl(const HoneycombX509CertPrincipalImpl&) = delete;
  HoneycombX509CertPrincipalImpl& operator=(const HoneycombX509CertPrincipalImpl&) = delete;

  // HoneycombX509CertPrincipal methods.
  HoneycombString GetDisplayName() override;
  HoneycombString GetCommonName() override;
  HoneycombString GetLocalityName() override;
  HoneycombString GetStateOrProvinceName() override;
  HoneycombString GetCountryName() override;
  void GetOrganizationNames(std::vector<HoneycombString>& names) override;
  void GetOrganizationUnitNames(std::vector<HoneycombString>& names) override;

 private:
  net::CertPrincipal value_;

  IMPLEMENT_REFCOUNTING(HoneycombX509CertPrincipalImpl);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_X509_CERT_PRINCIPAL_IMPL_H_
