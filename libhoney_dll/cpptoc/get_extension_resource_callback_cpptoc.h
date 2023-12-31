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
// $hash=76b58a0d3f719bb4899c87ec701d89a96a45ae31$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CPPTOC_GET_EXTENSION_RESOURCE_CALLBACK_CPPTOC_H_
#define HONEYCOMB_LIBHONEY_DLL_CPPTOC_GET_EXTENSION_RESOURCE_CALLBACK_CPPTOC_H_
#pragma once

#if !defined(BUILDING_HONEYCOMB_SHARED)
#error This file can be included DLL-side only
#endif

#include "include/capi/honey_client_capi.h"
#include "include/capi/honey_extension_handler_capi.h"
#include "include/honey_client.h"
#include "include/honey_extension_handler.h"
#include "libhoney_dll/cpptoc/cpptoc_ref_counted.h"

// Wrap a C++ class with a C structure.
// This class may be instantiated and accessed DLL-side only.
class HoneycombGetExtensionResourceCallbackCppToC
    : public HoneycombCppToCRefCounted<HoneycombGetExtensionResourceCallbackCppToC,
                                 HoneycombGetExtensionResourceCallback,
                                 honey_get_extension_resource_callback_t> {
 public:
  HoneycombGetExtensionResourceCallbackCppToC();
  virtual ~HoneycombGetExtensionResourceCallbackCppToC();
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CPPTOC_GET_EXTENSION_RESOURCE_CALLBACK_CPPTOC_H_
