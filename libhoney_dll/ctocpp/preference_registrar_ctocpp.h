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
// $hash=bcd7981d2a8feae116d3c658f430f9753c11612d$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CTOCPP_PREFERENCE_REGISTRAR_CTOCPP_H_
#define HONEYCOMB_LIBHONEY_DLL_CTOCPP_PREFERENCE_REGISTRAR_CTOCPP_H_
#pragma once

#if !defined(WRAPPING_HONEYCOMB_SHARED)
#error This file can be included wrapper-side only
#endif

#include "include/capi/honey_preference_capi.h"
#include "include/honey_preference.h"
#include "libhoney_dll/ctocpp/ctocpp_scoped.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class HoneycombPreferenceRegistrarCToCpp
    : public HoneycombCToCppScoped<HoneycombPreferenceRegistrarCToCpp,
                             HoneycombPreferenceRegistrar,
                             honey_preference_registrar_t> {
 public:
  HoneycombPreferenceRegistrarCToCpp();
  virtual ~HoneycombPreferenceRegistrarCToCpp();

  // HoneycombPreferenceRegistrar methods.
  bool AddPreference(const HoneycombString& name,
                     HoneycombRefPtr<HoneycombValue> default_value) override;
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CTOCPP_PREFERENCE_REGISTRAR_CTOCPP_H_
