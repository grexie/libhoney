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
// $hash=361eefa5a258faf92d09e28787293fa29bbed742$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CTOCPP_V8STACK_TRACE_CTOCPP_H_
#define HONEYCOMB_LIBHONEY_DLL_CTOCPP_V8STACK_TRACE_CTOCPP_H_
#pragma once

#if !defined(WRAPPING_HONEYCOMB_SHARED)
#error This file can be included wrapper-side only
#endif

#include "include/capi/honey_v8_capi.h"
#include "include/honey_v8.h"
#include "libhoney_dll/ctocpp/ctocpp_ref_counted.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class HoneycombV8StackTraceCToCpp : public HoneycombCToCppRefCounted<HoneycombV8StackTraceCToCpp,
                                                         HoneycombV8StackTrace,
                                                         honey_v8stack_trace_t> {
 public:
  HoneycombV8StackTraceCToCpp();
  virtual ~HoneycombV8StackTraceCToCpp();

  // HoneycombV8StackTrace methods.
  bool IsValid() override;
  int GetFrameCount() override;
  HoneycombRefPtr<HoneycombV8StackFrame> GetFrame(int index) override;
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CTOCPP_V8STACK_TRACE_CTOCPP_H_
