// Copyright (c) 2017 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoneycomb_dll/ctocpp/base_scoped_ctocpp.h"

HoneycombBaseScopedCToCpp::HoneycombBaseScopedCToCpp() {}

template <>
honey_base_scoped_t*
HoneycombCToCppScoped<HoneycombBaseScopedCToCpp, HoneycombBaseScoped, honey_base_scoped_t>::
    UnwrapDerivedOwn(HoneycombWrapperType type, HoneycombOwnPtr<HoneycombBaseScoped> c) {
  DCHECK(false);
  return NULL;
}

template <>
honey_base_scoped_t*
HoneycombCToCppScoped<HoneycombBaseScopedCToCpp, HoneycombBaseScoped, honey_base_scoped_t>::
    UnwrapDerivedRaw(HoneycombWrapperType type, HoneycombRawPtr<HoneycombBaseScoped> c) {
  DCHECK(false);
  return NULL;
}

template <>
HoneycombWrapperType HoneycombCToCppScoped<HoneycombBaseScopedCToCpp,
                               HoneycombBaseScoped,
                               honey_base_scoped_t>::kWrapperType =
    WT_BASE_SCOPED;
