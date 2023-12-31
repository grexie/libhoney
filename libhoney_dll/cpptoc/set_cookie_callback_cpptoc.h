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
// $hash=886b832f912900c89787888566d4d5e803c91ebc$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CPPTOC_SET_COOKIE_CALLBACK_CPPTOC_H_
#define HONEYCOMB_LIBHONEY_DLL_CPPTOC_SET_COOKIE_CALLBACK_CPPTOC_H_
#pragma once

#if !defined(WRAPPING_HONEYCOMB_SHARED)
#error This file can be included wrapper-side only
#endif

#include "include/capi/honey_cookie_capi.h"
#include "include/honey_cookie.h"
#include "libhoney_dll/cpptoc/cpptoc_ref_counted.h"

// Wrap a C++ class with a C structure.
// This class may be instantiated and accessed wrapper-side only.
class HoneycombSetCookieCallbackCppToC
    : public HoneycombCppToCRefCounted<HoneycombSetCookieCallbackCppToC,
                                 HoneycombSetCookieCallback,
                                 honey_set_cookie_callback_t> {
 public:
  HoneycombSetCookieCallbackCppToC();
  virtual ~HoneycombSetCookieCallbackCppToC();
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CPPTOC_SET_COOKIE_CALLBACK_CPPTOC_H_
