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
// $hash=e9f8f124502684b0e814730ea52aed7c87fef3b3$
//

#include "libhoney_dll/cpptoc/x509certificate_cpptoc.h"
#include <algorithm>
#include "libhoney_dll/cpptoc/binary_value_cpptoc.h"
#include "libhoney_dll/cpptoc/x509cert_principal_cpptoc.h"
#include "libhoney_dll/shutdown_checker.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

honey_x509cert_principal_t* HONEYCOMB_CALLBACK
x509certificate_get_subject(struct _honey_x509certificate_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombX509CertPrincipal> _retval =
      HoneycombX509CertificateCppToC::Get(self)->GetSubject();

  // Return type: refptr_same
  return HoneycombX509CertPrincipalCppToC::Wrap(_retval);
}

honey_x509cert_principal_t* HONEYCOMB_CALLBACK
x509certificate_get_issuer(struct _honey_x509certificate_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombX509CertPrincipal> _retval =
      HoneycombX509CertificateCppToC::Get(self)->GetIssuer();

  // Return type: refptr_same
  return HoneycombX509CertPrincipalCppToC::Wrap(_retval);
}

honey_binary_value_t* HONEYCOMB_CALLBACK
x509certificate_get_serial_number(struct _honey_x509certificate_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombBinaryValue> _retval =
      HoneycombX509CertificateCppToC::Get(self)->GetSerialNumber();

  // Return type: refptr_same
  return HoneycombBinaryValueCppToC::Wrap(_retval);
}

honey_basetime_t HONEYCOMB_CALLBACK
x509certificate_get_valid_start(struct _honey_x509certificate_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return HoneycombBaseTime();
  }

  // Execute
  honey_basetime_t _retval = HoneycombX509CertificateCppToC::Get(self)->GetValidStart();

  // Return type: simple
  return _retval;
}

honey_basetime_t HONEYCOMB_CALLBACK
x509certificate_get_valid_expiry(struct _honey_x509certificate_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return HoneycombBaseTime();
  }

  // Execute
  honey_basetime_t _retval =
      HoneycombX509CertificateCppToC::Get(self)->GetValidExpiry();

  // Return type: simple
  return _retval;
}

honey_binary_value_t* HONEYCOMB_CALLBACK
x509certificate_get_derencoded(struct _honey_x509certificate_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombBinaryValue> _retval =
      HoneycombX509CertificateCppToC::Get(self)->GetDEREncoded();

  // Return type: refptr_same
  return HoneycombBinaryValueCppToC::Wrap(_retval);
}

honey_binary_value_t* HONEYCOMB_CALLBACK
x509certificate_get_pemencoded(struct _honey_x509certificate_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombBinaryValue> _retval =
      HoneycombX509CertificateCppToC::Get(self)->GetPEMEncoded();

  // Return type: refptr_same
  return HoneycombBinaryValueCppToC::Wrap(_retval);
}

size_t HONEYCOMB_CALLBACK
x509certificate_get_issuer_chain_size(struct _honey_x509certificate_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  size_t _retval = HoneycombX509CertificateCppToC::Get(self)->GetIssuerChainSize();

  // Return type: simple
  return _retval;
}

void HONEYCOMB_CALLBACK
x509certificate_get_derencoded_issuer_chain(struct _honey_x509certificate_t* self,
                                            size_t* chainCount,
                                            honey_binary_value_t** chain) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: chain; type: refptr_vec_same_byref
  DCHECK(chainCount && (*chainCount == 0 || chain));
  if (!chainCount || (*chainCount > 0 && !chain)) {
    return;
  }

  // Translate param: chain; type: refptr_vec_same_byref
  std::vector<HoneycombRefPtr<HoneycombBinaryValue>> chainList;
  if (chainCount && *chainCount > 0 && chain) {
    for (size_t i = 0; i < *chainCount; ++i) {
      chainList.push_back(HoneycombBinaryValueCppToC::Unwrap(chain[i]));
    }
  }

  // Execute
  HoneycombX509CertificateCppToC::Get(self)->GetDEREncodedIssuerChain(chainList);

  // Restore param: chain; type: refptr_vec_same_byref
  if (chainCount && chain) {
    *chainCount = std::min(chainList.size(), *chainCount);
    if (*chainCount > 0) {
      for (size_t i = 0; i < *chainCount; ++i) {
        chain[i] = HoneycombBinaryValueCppToC::Wrap(chainList[i]);
      }
    }
  }
}

void HONEYCOMB_CALLBACK
x509certificate_get_pemencoded_issuer_chain(struct _honey_x509certificate_t* self,
                                            size_t* chainCount,
                                            honey_binary_value_t** chain) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: chain; type: refptr_vec_same_byref
  DCHECK(chainCount && (*chainCount == 0 || chain));
  if (!chainCount || (*chainCount > 0 && !chain)) {
    return;
  }

  // Translate param: chain; type: refptr_vec_same_byref
  std::vector<HoneycombRefPtr<HoneycombBinaryValue>> chainList;
  if (chainCount && *chainCount > 0 && chain) {
    for (size_t i = 0; i < *chainCount; ++i) {
      chainList.push_back(HoneycombBinaryValueCppToC::Unwrap(chain[i]));
    }
  }

  // Execute
  HoneycombX509CertificateCppToC::Get(self)->GetPEMEncodedIssuerChain(chainList);

  // Restore param: chain; type: refptr_vec_same_byref
  if (chainCount && chain) {
    *chainCount = std::min(chainList.size(), *chainCount);
    if (*chainCount > 0) {
      for (size_t i = 0; i < *chainCount; ++i) {
        chain[i] = HoneycombBinaryValueCppToC::Wrap(chainList[i]);
      }
    }
  }
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

HoneycombX509CertificateCppToC::HoneycombX509CertificateCppToC() {
  GetStruct()->get_subject = x509certificate_get_subject;
  GetStruct()->get_issuer = x509certificate_get_issuer;
  GetStruct()->get_serial_number = x509certificate_get_serial_number;
  GetStruct()->get_valid_start = x509certificate_get_valid_start;
  GetStruct()->get_valid_expiry = x509certificate_get_valid_expiry;
  GetStruct()->get_derencoded = x509certificate_get_derencoded;
  GetStruct()->get_pemencoded = x509certificate_get_pemencoded;
  GetStruct()->get_issuer_chain_size = x509certificate_get_issuer_chain_size;
  GetStruct()->get_derencoded_issuer_chain =
      x509certificate_get_derencoded_issuer_chain;
  GetStruct()->get_pemencoded_issuer_chain =
      x509certificate_get_pemencoded_issuer_chain;
}

// DESTRUCTOR - Do not edit by hand.

HoneycombX509CertificateCppToC::~HoneycombX509CertificateCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
HoneycombRefPtr<HoneycombX509Certificate> HoneycombCppToCRefCounted<
    HoneycombX509CertificateCppToC,
    HoneycombX509Certificate,
    honey_x509certificate_t>::UnwrapDerived(HoneycombWrapperType type,
                                          honey_x509certificate_t* s) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCppToCRefCounted<HoneycombX509CertificateCppToC,
                                   HoneycombX509Certificate,
                                   honey_x509certificate_t>::kWrapperType =
    WT_X509CERTIFICATE;
