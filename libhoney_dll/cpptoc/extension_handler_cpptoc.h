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
// $hash=db012b196983395c9684bf1275b638e9ccc57949$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CPPTOC_EXTENSION_HANDLER_CPPTOC_H_
#define HONEYCOMB_LIBHONEY_DLL_CPPTOC_EXTENSION_HANDLER_CPPTOC_H_
#pragma once

#if !defined(WRAPPING_HONEYCOMB_SHARED)
#error This file can be included wrapper-side only
#endif

#include "include/capi/honey_client_capi.h"
#include "include/capi/honey_extension_handler_capi.h"
#include "include/honey_client.h"
#include "include/honey_extension_handler.h"
#include "libhoney_dll/cpptoc/cpptoc_ref_counted.h"

// Wrap a C++ class with a C structure.
// This class may be instantiated and accessed wrapper-side only.
class HoneycombExtensionHandlerCppToC
    : public HoneycombCppToCRefCounted<HoneycombExtensionHandlerCppToC,
                                 HoneycombExtensionHandler,
                                 honey_extension_handler_t> {
 public:
  HoneycombExtensionHandlerCppToC();
  virtual ~HoneycombExtensionHandlerCppToC();
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CPPTOC_EXTENSION_HANDLER_CPPTOC_H_
