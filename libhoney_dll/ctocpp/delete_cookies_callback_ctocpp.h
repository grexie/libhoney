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
// $hash=e064baa776ef2fb9b70d51ec556613859a222067$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CTOCPP_DELETE_COOKIES_CALLBACK_CTOCPP_H_
#define HONEYCOMB_LIBHONEY_DLL_CTOCPP_DELETE_COOKIES_CALLBACK_CTOCPP_H_
#pragma once

#if !defined(BUILDING_HONEYCOMB_SHARED)
#error This file can be included DLL-side only
#endif

#include "include/capi/honey_cookie_capi.h"
#include "include/honey_cookie.h"
#include "libhoney_dll/ctocpp/ctocpp_ref_counted.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed DLL-side only.
class HoneycombDeleteCookiesCallbackCToCpp
    : public HoneycombCToCppRefCounted<HoneycombDeleteCookiesCallbackCToCpp,
                                 HoneycombDeleteCookiesCallback,
                                 honey_delete_cookies_callback_t> {
 public:
  HoneycombDeleteCookiesCallbackCToCpp();
  virtual ~HoneycombDeleteCookiesCallbackCToCpp();

  // HoneycombDeleteCookiesCallback methods.
  void OnComplete(int num_deleted) override;
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CTOCPP_DELETE_COOKIES_CALLBACK_CTOCPP_H_
