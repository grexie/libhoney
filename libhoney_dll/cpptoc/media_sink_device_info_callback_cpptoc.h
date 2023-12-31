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
// $hash=66fb13a617c9405f7829e797b21a663e5275e61d$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CPPTOC_MEDIA_SINK_DEVICE_INFO_CALLBACK_CPPTOC_H_
#define HONEYCOMB_LIBHONEY_DLL_CPPTOC_MEDIA_SINK_DEVICE_INFO_CALLBACK_CPPTOC_H_
#pragma once

#if !defined(WRAPPING_HONEYCOMB_SHARED)
#error This file can be included wrapper-side only
#endif

#include "include/capi/honey_media_router_capi.h"
#include "include/honey_media_router.h"
#include "libhoney_dll/cpptoc/cpptoc_ref_counted.h"

// Wrap a C++ class with a C structure.
// This class may be instantiated and accessed wrapper-side only.
class HoneycombMediaSinkDeviceInfoCallbackCppToC
    : public HoneycombCppToCRefCounted<HoneycombMediaSinkDeviceInfoCallbackCppToC,
                                 HoneycombMediaSinkDeviceInfoCallback,
                                 honey_media_sink_device_info_callback_t> {
 public:
  HoneycombMediaSinkDeviceInfoCallbackCppToC();
  virtual ~HoneycombMediaSinkDeviceInfoCallbackCppToC();
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CPPTOC_MEDIA_SINK_DEVICE_INFO_CALLBACK_CPPTOC_H_
