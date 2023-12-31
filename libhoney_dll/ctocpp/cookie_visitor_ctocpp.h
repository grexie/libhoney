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
// $hash=70fdf3c9319fd0ca0c0653ded314d645f8f39dfb$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CTOCPP_COOKIE_VISITOR_CTOCPP_H_
#define HONEYCOMB_LIBHONEY_DLL_CTOCPP_COOKIE_VISITOR_CTOCPP_H_
#pragma once

#if !defined(BUILDING_HONEYCOMB_SHARED)
#error This file can be included DLL-side only
#endif

#include "include/capi/honey_cookie_capi.h"
#include "include/honey_cookie.h"
#include "libhoney_dll/ctocpp/ctocpp_ref_counted.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed DLL-side only.
class HoneycombCookieVisitorCToCpp
    : public HoneycombCToCppRefCounted<HoneycombCookieVisitorCToCpp,
                                 HoneycombCookieVisitor,
                                 honey_cookie_visitor_t> {
 public:
  HoneycombCookieVisitorCToCpp();
  virtual ~HoneycombCookieVisitorCToCpp();

  // HoneycombCookieVisitor methods.
  bool Visit(const HoneycombCookie& cookie,
             int count,
             int total,
             bool& deleteCookie) override;
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CTOCPP_COOKIE_VISITOR_CTOCPP_H_
