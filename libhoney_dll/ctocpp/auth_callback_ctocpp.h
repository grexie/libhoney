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
// $hash=60f444d40338eb245860b389d087e12af15cbf20$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CTOCPP_AUTH_CALLBACK_CTOCPP_H_
#define HONEYCOMB_LIBHONEY_DLL_CTOCPP_AUTH_CALLBACK_CTOCPP_H_
#pragma once

#if !defined(WRAPPING_HONEYCOMB_SHARED)
#error This file can be included wrapper-side only
#endif

#include "include/capi/honey_auth_callback_capi.h"
#include "include/honey_auth_callback.h"
#include "libhoney_dll/ctocpp/ctocpp_ref_counted.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class HoneycombAuthCallbackCToCpp : public HoneycombCToCppRefCounted<HoneycombAuthCallbackCToCpp,
                                                         HoneycombAuthCallback,
                                                         honey_auth_callback_t> {
 public:
  HoneycombAuthCallbackCToCpp();
  virtual ~HoneycombAuthCallbackCToCpp();

  // HoneycombAuthCallback methods.
  void Continue(const HoneycombString& username, const HoneycombString& password) override;
  void Cancel() override;
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CTOCPP_AUTH_CALLBACK_CTOCPP_H_
