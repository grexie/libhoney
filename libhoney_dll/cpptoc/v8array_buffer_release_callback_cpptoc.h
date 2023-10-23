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
// $hash=f3cb7f220bf24ad178eed9b14d8b6e3d1baed6d5$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CPPTOC_V8ARRAY_BUFFER_RELEASE_CALLBACK_CPPTOC_H_
#define HONEYCOMB_LIBHONEY_DLL_CPPTOC_V8ARRAY_BUFFER_RELEASE_CALLBACK_CPPTOC_H_
#pragma once

#if !defined(WRAPPING_HONEYCOMB_SHARED)
#error This file can be included wrapper-side only
#endif

#include "include/capi/honey_v8_capi.h"
#include "include/honey_v8.h"
#include "libhoney_dll/cpptoc/cpptoc_ref_counted.h"

// Wrap a C++ class with a C structure.
// This class may be instantiated and accessed wrapper-side only.
class HoneycombV8ArrayBufferReleaseCallbackCppToC
    : public HoneycombCppToCRefCounted<HoneycombV8ArrayBufferReleaseCallbackCppToC,
                                 HoneycombV8ArrayBufferReleaseCallback,
                                 honey_v8array_buffer_release_callback_t> {
 public:
  HoneycombV8ArrayBufferReleaseCallbackCppToC();
  virtual ~HoneycombV8ArrayBufferReleaseCallbackCppToC();
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CPPTOC_V8ARRAY_BUFFER_RELEASE_CALLBACK_CPPTOC_H_