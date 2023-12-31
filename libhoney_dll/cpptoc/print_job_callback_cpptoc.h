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
// $hash=54a355e9511b5d0956f1a7269ee21766fa7f8c87$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CPPTOC_PRINT_JOB_CALLBACK_CPPTOC_H_
#define HONEYCOMB_LIBHONEY_DLL_CPPTOC_PRINT_JOB_CALLBACK_CPPTOC_H_
#pragma once

#if !defined(BUILDING_HONEYCOMB_SHARED)
#error This file can be included DLL-side only
#endif

#include "include/capi/honey_print_handler_capi.h"
#include "include/honey_print_handler.h"
#include "libhoney_dll/cpptoc/cpptoc_ref_counted.h"

// Wrap a C++ class with a C structure.
// This class may be instantiated and accessed DLL-side only.
class HoneycombPrintJobCallbackCppToC
    : public HoneycombCppToCRefCounted<HoneycombPrintJobCallbackCppToC,
                                 HoneycombPrintJobCallback,
                                 honey_print_job_callback_t> {
 public:
  HoneycombPrintJobCallbackCppToC();
  virtual ~HoneycombPrintJobCallbackCppToC();
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CPPTOC_PRINT_JOB_CALLBACK_CPPTOC_H_
