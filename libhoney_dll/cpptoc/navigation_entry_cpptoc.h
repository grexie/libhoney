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
// $hash=213e6404f2260e81c41b20a42ae7788af80710dc$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CPPTOC_NAVIGATION_ENTRY_CPPTOC_H_
#define HONEYCOMB_LIBHONEY_DLL_CPPTOC_NAVIGATION_ENTRY_CPPTOC_H_
#pragma once

#if !defined(BUILDING_HONEYCOMB_SHARED)
#error This file can be included DLL-side only
#endif

#include "include/capi/honey_navigation_entry_capi.h"
#include "include/honey_navigation_entry.h"
#include "libhoney_dll/cpptoc/cpptoc_ref_counted.h"

// Wrap a C++ class with a C structure.
// This class may be instantiated and accessed DLL-side only.
class HoneycombNavigationEntryCppToC
    : public HoneycombCppToCRefCounted<HoneycombNavigationEntryCppToC,
                                 HoneycombNavigationEntry,
                                 honey_navigation_entry_t> {
 public:
  HoneycombNavigationEntryCppToC();
  virtual ~HoneycombNavigationEntryCppToC();
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CPPTOC_NAVIGATION_ENTRY_CPPTOC_H_