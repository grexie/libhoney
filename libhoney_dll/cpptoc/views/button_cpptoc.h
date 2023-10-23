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
// $hash=3fc906cb8937c58418501c33ba81462806b26860$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CPPTOC_VIEWS_BUTTON_CPPTOC_H_
#define HONEYCOMB_LIBHONEY_DLL_CPPTOC_VIEWS_BUTTON_CPPTOC_H_
#pragma once

#if !defined(BUILDING_HONEYCOMB_SHARED)
#error This file can be included DLL-side only
#endif

#include "include/capi/views/honey_button_capi.h"
#include "include/capi/views/honey_label_button_capi.h"
#include "include/views/honey_button.h"
#include "include/views/honey_label_button.h"
#include "libhoney_dll/cpptoc/cpptoc_ref_counted.h"

// Wrap a C++ class with a C structure.
// This class may be instantiated and accessed DLL-side only.
class HoneycombButtonCppToC
    : public HoneycombCppToCRefCounted<HoneycombButtonCppToC, HoneycombButton, honey_button_t> {
 public:
  HoneycombButtonCppToC();
  virtual ~HoneycombButtonCppToC();
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CPPTOC_VIEWS_BUTTON_CPPTOC_H_