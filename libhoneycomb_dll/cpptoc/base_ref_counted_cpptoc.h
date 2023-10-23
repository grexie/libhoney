// Copyright (c) 2009 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_DLL_CPPTOC_BASE_REF_COUNTED_CPPTOC_H_
#define HONEYCOMB_LIBHONEYCOMB_DLL_CPPTOC_BASE_REF_COUNTED_CPPTOC_H_
#pragma once

#include "include/capi/honey_base_capi.h"
#include "include/honey_base.h"
#include "libhoneycomb_dll/cpptoc/cpptoc_ref_counted.h"

#if !defined(WRAPPING_HONEYCOMB_SHARED)
#error This file can be included wrapper-side only
#endif

// Wrap a C++ class with a C structure.
class HoneycombBaseRefCountedCppToC
    : public HoneycombCppToCRefCounted<HoneycombBaseRefCountedCppToC,
                                 HoneycombBaseRefCounted,
                                 honey_base_ref_counted_t> {
 public:
  HoneycombBaseRefCountedCppToC();
};

#endif  // HONEYCOMB_LIBHONEYCOMB_DLL_CPPTOC_BASE_REF_COUNTED_CPPTOC_H_
