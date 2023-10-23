// Copyright (c) 2017 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoneycomb_dll/cpptoc/base_scoped_cpptoc.h"

HoneycombBaseScopedCppToC::HoneycombBaseScopedCppToC() {}

template <>
HoneycombOwnPtr<HoneycombBaseScoped>
HoneycombCppToCScoped<HoneycombBaseScopedCppToC, HoneycombBaseScoped, honey_base_scoped_t>::
    UnwrapDerivedOwn(HoneycombWrapperType type, honey_base_scoped_t* s) {
  DCHECK(false);
  return HoneycombOwnPtr<HoneycombBaseScoped>();
}

template <>
HoneycombRawPtr<HoneycombBaseScoped>
HoneycombCppToCScoped<HoneycombBaseScopedCppToC, HoneycombBaseScoped, honey_base_scoped_t>::
    UnwrapDerivedRaw(HoneycombWrapperType type, honey_base_scoped_t* s) {
  DCHECK(false);
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCppToCScoped<HoneycombBaseScopedCppToC,
                               HoneycombBaseScoped,
                               honey_base_scoped_t>::kWrapperType =
    WT_BASE_SCOPED;
