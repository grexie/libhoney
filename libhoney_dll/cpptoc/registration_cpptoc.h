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
// $hash=461d6b9297ebd61bf8d2df2e3960458a9a3705f6$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CPPTOC_REGISTRATION_CPPTOC_H_
#define HONEYCOMB_LIBHONEY_DLL_CPPTOC_REGISTRATION_CPPTOC_H_
#pragma once

#if !defined(BUILDING_HONEYCOMB_SHARED)
#error This file can be included DLL-side only
#endif

#include "include/capi/honey_registration_capi.h"
#include "include/honey_registration.h"
#include "libhoney_dll/cpptoc/cpptoc_ref_counted.h"

// Wrap a C++ class with a C structure.
// This class may be instantiated and accessed DLL-side only.
class HoneycombRegistrationCppToC : public HoneycombCppToCRefCounted<HoneycombRegistrationCppToC,
                                                         HoneycombRegistration,
                                                         honey_registration_t> {
 public:
  HoneycombRegistrationCppToC();
  virtual ~HoneycombRegistrationCppToC();
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CPPTOC_REGISTRATION_CPPTOC_H_
