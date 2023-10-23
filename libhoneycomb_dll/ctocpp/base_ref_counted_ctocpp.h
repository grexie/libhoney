// Copyright (c) 2009 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_DLL_CTOCPP_BASE_REF_COUNTED_CTOCPP_H_
#define HONEYCOMB_LIBHONEYCOMB_DLL_CTOCPP_BASE_REF_COUNTED_CTOCPP_H_
#pragma once

#include "include/capi/honey_base_capi.h"
#include "include/honey_base.h"
#include "libhoneycomb_dll/ctocpp/ctocpp_ref_counted.h"

#if !defined(BUILDING_HONEYCOMB_SHARED)
#error This file can be included DLL-side only
#endif

// Wrap a C structure with a C++ class.
class HoneycombBaseRefCountedCToCpp
    : public HoneycombCToCppRefCounted<HoneycombBaseRefCountedCToCpp,
                                 HoneycombBaseRefCounted,
                                 honey_base_ref_counted_t> {
 public:
  HoneycombBaseRefCountedCToCpp();
};

#endif  // HONEYCOMB_LIBHONEYCOMB_DLL_CTOCPP_BASE_REF_COUNTED_CTOCPP_H_
