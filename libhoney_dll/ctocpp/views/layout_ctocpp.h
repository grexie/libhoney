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
// $hash=f50cae9c7f44f282497cff43e8b89fc76f60e51b$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CTOCPP_VIEWS_LAYOUT_CTOCPP_H_
#define HONEYCOMB_LIBHONEY_DLL_CTOCPP_VIEWS_LAYOUT_CTOCPP_H_
#pragma once

#if !defined(WRAPPING_HONEYCOMB_SHARED)
#error This file can be included wrapper-side only
#endif

#include "include/capi/views/honey_box_layout_capi.h"
#include "include/capi/views/honey_fill_layout_capi.h"
#include "include/capi/views/honey_layout_capi.h"
#include "include/views/honey_box_layout.h"
#include "include/views/honey_fill_layout.h"
#include "include/views/honey_layout.h"
#include "libhoney_dll/ctocpp/ctocpp_ref_counted.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class HoneycombLayoutCToCpp
    : public HoneycombCToCppRefCounted<HoneycombLayoutCToCpp, HoneycombLayout, honey_layout_t> {
 public:
  HoneycombLayoutCToCpp();
  virtual ~HoneycombLayoutCToCpp();

  // HoneycombLayout methods.
  HoneycombRefPtr<HoneycombBoxLayout> AsBoxLayout() override;
  HoneycombRefPtr<HoneycombFillLayout> AsFillLayout() override;
  bool IsValid() override;
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CTOCPP_VIEWS_LAYOUT_CTOCPP_H_
