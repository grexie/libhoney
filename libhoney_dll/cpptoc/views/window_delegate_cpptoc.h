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
// $hash=b4d82958ac79ac843f904c4aa8010a6909ca06fa$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CPPTOC_VIEWS_WINDOW_DELEGATE_CPPTOC_H_
#define HONEYCOMB_LIBHONEY_DLL_CPPTOC_VIEWS_WINDOW_DELEGATE_CPPTOC_H_
#pragma once

#if !defined(WRAPPING_HONEYCOMB_SHARED)
#error This file can be included wrapper-side only
#endif

#include "include/capi/views/honey_window_capi.h"
#include "include/capi/views/honey_window_delegate_capi.h"
#include "include/views/honey_window.h"
#include "include/views/honey_window_delegate.h"
#include "libhoney_dll/cpptoc/cpptoc_ref_counted.h"

// Wrap a C++ class with a C structure.
// This class may be instantiated and accessed wrapper-side only.
class HoneycombWindowDelegateCppToC
    : public HoneycombCppToCRefCounted<HoneycombWindowDelegateCppToC,
                                 HoneycombWindowDelegate,
                                 honey_window_delegate_t> {
 public:
  HoneycombWindowDelegateCppToC();
  virtual ~HoneycombWindowDelegateCppToC();
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CPPTOC_VIEWS_WINDOW_DELEGATE_CPPTOC_H_
