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
// $hash=e722a5b9ae2bb6e3d3236a199930600dc3b5e0f8$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CTOCPP_TASK_CTOCPP_H_
#define HONEYCOMB_LIBHONEY_DLL_CTOCPP_TASK_CTOCPP_H_
#pragma once

#if !defined(BUILDING_HONEYCOMB_SHARED)
#error This file can be included DLL-side only
#endif

#include "include/capi/honey_task_capi.h"
#include "include/honey_task.h"
#include "libhoney_dll/ctocpp/ctocpp_ref_counted.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed DLL-side only.
class HoneycombTaskCToCpp
    : public HoneycombCToCppRefCounted<HoneycombTaskCToCpp, HoneycombTask, honey_task_t> {
 public:
  HoneycombTaskCToCpp();
  virtual ~HoneycombTaskCToCpp();

  // HoneycombTask methods.
  void Execute() override;
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CTOCPP_TASK_CTOCPP_H_
