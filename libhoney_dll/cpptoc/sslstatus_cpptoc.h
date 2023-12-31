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
// $hash=dba266754e189de39172bddaacf0dfa3fdd79351$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CPPTOC_SSLSTATUS_CPPTOC_H_
#define HONEYCOMB_LIBHONEY_DLL_CPPTOC_SSLSTATUS_CPPTOC_H_
#pragma once

#if !defined(BUILDING_HONEYCOMB_SHARED)
#error This file can be included DLL-side only
#endif

#include "include/capi/honey_ssl_status_capi.h"
#include "include/honey_ssl_status.h"
#include "libhoney_dll/cpptoc/cpptoc_ref_counted.h"

// Wrap a C++ class with a C structure.
// This class may be instantiated and accessed DLL-side only.
class HoneycombSSLStatusCppToC : public HoneycombCppToCRefCounted<HoneycombSSLStatusCppToC,
                                                      HoneycombSSLStatus,
                                                      honey_sslstatus_t> {
 public:
  HoneycombSSLStatusCppToC();
  virtual ~HoneycombSSLStatusCppToC();
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CPPTOC_SSLSTATUS_CPPTOC_H_
