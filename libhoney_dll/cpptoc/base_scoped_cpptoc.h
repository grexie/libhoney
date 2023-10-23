// Copyright (c) 2009 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_DLL_CPPTOC_BASE_CPPTOC_H_
#define HONEYCOMB_LIBHONEY_DLL_CPPTOC_BASE_CPPTOC_H_
#pragma once

#include "include/capi/honey_base_capi.h"
#include "include/honey_base.h"
#include "libhoney_dll/cpptoc/cpptoc_scoped.h"

#if !defined(WRAPPING_HONEYCOMB_SHARED)
#error This file can be included wrapper-side only
#endif

// Wrap a C++ class with a C structure.
class HoneycombBaseScopedCppToC : public HoneycombCppToCScoped<HoneycombBaseScopedCppToC,
                                                   HoneycombBaseScoped,
                                                   honey_base_scoped_t> {
 public:
  HoneycombBaseScopedCppToC();
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CPPTOC_BASE_CPPTOC_H_
