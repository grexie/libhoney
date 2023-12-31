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
// $hash=b9ca51a2ee848580b14c1a06b49b2b9e048ab798$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CTOCPP_RESPONSE_FILTER_CTOCPP_H_
#define HONEYCOMB_LIBHONEY_DLL_CTOCPP_RESPONSE_FILTER_CTOCPP_H_
#pragma once

#if !defined(BUILDING_HONEYCOMB_SHARED)
#error This file can be included DLL-side only
#endif

#include "include/capi/honey_response_filter_capi.h"
#include "include/honey_response_filter.h"
#include "libhoney_dll/ctocpp/ctocpp_ref_counted.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed DLL-side only.
class HoneycombResponseFilterCToCpp
    : public HoneycombCToCppRefCounted<HoneycombResponseFilterCToCpp,
                                 HoneycombResponseFilter,
                                 honey_response_filter_t> {
 public:
  HoneycombResponseFilterCToCpp();
  virtual ~HoneycombResponseFilterCToCpp();

  // HoneycombResponseFilter methods.
  bool InitFilter() override;
  FilterStatus Filter(void* data_in,
                      size_t data_in_size,
                      size_t& data_in_read,
                      void* data_out,
                      size_t data_out_size,
                      size_t& data_out_written) override;
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CTOCPP_RESPONSE_FILTER_CTOCPP_H_
