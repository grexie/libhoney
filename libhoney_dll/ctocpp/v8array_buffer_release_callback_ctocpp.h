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
// $hash=4f9c4bb702d2824ee94dd334244cd9ba14609025$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CTOCPP_V8ARRAY_BUFFER_RELEASE_CALLBACK_CTOCPP_H_
#define HONEYCOMB_LIBHONEY_DLL_CTOCPP_V8ARRAY_BUFFER_RELEASE_CALLBACK_CTOCPP_H_
#pragma once

#if !defined(BUILDING_HONEYCOMB_SHARED)
#error This file can be included DLL-side only
#endif

#include "include/capi/honey_v8_capi.h"
#include "include/honey_v8.h"
#include "libhoney_dll/ctocpp/ctocpp_ref_counted.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed DLL-side only.
class HoneycombV8ArrayBufferReleaseCallbackCToCpp
    : public HoneycombCToCppRefCounted<HoneycombV8ArrayBufferReleaseCallbackCToCpp,
                                 HoneycombV8ArrayBufferReleaseCallback,
                                 honey_v8array_buffer_release_callback_t> {
 public:
  HoneycombV8ArrayBufferReleaseCallbackCToCpp();
  virtual ~HoneycombV8ArrayBufferReleaseCallbackCToCpp();

  // HoneycombV8ArrayBufferReleaseCallback methods.
  void ReleaseBuffer(void* buffer) override;
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CTOCPP_V8ARRAY_BUFFER_RELEASE_CALLBACK_CTOCPP_H_
