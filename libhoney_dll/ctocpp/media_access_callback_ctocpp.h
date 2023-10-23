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
// $hash=36268cf93134620f9e6e79c4fc96c67cb8fa9570$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CTOCPP_MEDIA_ACCESS_CALLBACK_CTOCPP_H_
#define HONEYCOMB_LIBHONEY_DLL_CTOCPP_MEDIA_ACCESS_CALLBACK_CTOCPP_H_
#pragma once

#if !defined(WRAPPING_HONEYCOMB_SHARED)
#error This file can be included wrapper-side only
#endif

#include "include/capi/honey_permission_handler_capi.h"
#include "include/honey_permission_handler.h"
#include "libhoney_dll/ctocpp/ctocpp_ref_counted.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class HoneycombMediaAccessCallbackCToCpp
    : public HoneycombCToCppRefCounted<HoneycombMediaAccessCallbackCToCpp,
                                 HoneycombMediaAccessCallback,
                                 honey_media_access_callback_t> {
 public:
  HoneycombMediaAccessCallbackCToCpp();
  virtual ~HoneycombMediaAccessCallbackCToCpp();

  // HoneycombMediaAccessCallback methods.
  void Continue(uint32_t allowed_permissions) override;
  void Cancel() override;
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CTOCPP_MEDIA_ACCESS_CALLBACK_CTOCPP_H_