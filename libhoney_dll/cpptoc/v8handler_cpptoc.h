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
// $hash=25ab4ee4f4c72c6be2e4df28dfaa8bbe5ec522d6$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CPPTOC_V8HANDLER_CPPTOC_H_
#define HONEYCOMB_LIBHONEY_DLL_CPPTOC_V8HANDLER_CPPTOC_H_
#pragma once

#if !defined(WRAPPING_HONEYCOMB_SHARED)
#error This file can be included wrapper-side only
#endif

#include "include/capi/honey_v8_capi.h"
#include "include/honey_v8.h"
#include "libhoney_dll/cpptoc/cpptoc_ref_counted.h"

// Wrap a C++ class with a C structure.
// This class may be instantiated and accessed wrapper-side only.
class HoneycombV8HandlerCppToC : public HoneycombCppToCRefCounted<HoneycombV8HandlerCppToC,
                                                      HoneycombV8Handler,
                                                      honey_v8handler_t> {
 public:
  HoneycombV8HandlerCppToC();
  virtual ~HoneycombV8HandlerCppToC();
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CPPTOC_V8HANDLER_CPPTOC_H_
