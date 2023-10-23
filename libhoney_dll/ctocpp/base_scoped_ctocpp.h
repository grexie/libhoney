// Copyright (c) 2017 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_DLL_CTOCPP_BASE_SCOPED_CTOCPP_H_
#define HONEYCOMB_LIBHONEY_DLL_CTOCPP_BASE_SCOPED_CTOCPP_H_
#pragma once

#include "include/capi/honey_base_capi.h"
#include "include/honey_base.h"
#include "libhoney_dll/ctocpp/ctocpp_scoped.h"

#if !defined(BUILDING_HONEYCOMB_SHARED)
#error This file can be included DLL-side only
#endif

// Wrap a C structure with a C++ class.
class HoneycombBaseScopedCToCpp : public HoneycombCToCppScoped<HoneycombBaseScopedCToCpp,
                                                   HoneycombBaseScoped,
                                                   honey_base_scoped_t> {
 public:
  HoneycombBaseScopedCToCpp();
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CTOCPP_BASE_SCOPED_CTOCPP_H_
