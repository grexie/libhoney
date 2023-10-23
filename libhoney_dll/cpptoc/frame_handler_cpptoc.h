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
// $hash=72b035624f1edff425da000635d111f72186fffc$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CPPTOC_FRAME_HANDLER_CPPTOC_H_
#define HONEYCOMB_LIBHONEY_DLL_CPPTOC_FRAME_HANDLER_CPPTOC_H_
#pragma once

#if !defined(WRAPPING_HONEYCOMB_SHARED)
#error This file can be included wrapper-side only
#endif

#include "include/capi/honey_frame_handler_capi.h"
#include "include/honey_frame_handler.h"
#include "libhoney_dll/cpptoc/cpptoc_ref_counted.h"

// Wrap a C++ class with a C structure.
// This class may be instantiated and accessed wrapper-side only.
class HoneycombFrameHandlerCppToC : public HoneycombCppToCRefCounted<HoneycombFrameHandlerCppToC,
                                                         HoneycombFrameHandler,
                                                         honey_frame_handler_t> {
 public:
  HoneycombFrameHandlerCppToC();
  virtual ~HoneycombFrameHandlerCppToC();
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CPPTOC_FRAME_HANDLER_CPPTOC_H_