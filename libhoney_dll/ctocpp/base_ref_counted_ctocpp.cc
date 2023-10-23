// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoney_dll/ctocpp/base_ref_counted_ctocpp.h"

HoneycombBaseRefCountedCToCpp::HoneycombBaseRefCountedCToCpp() {}

template <>
honey_base_ref_counted_t* HoneycombCToCppRefCounted<
    HoneycombBaseRefCountedCToCpp,
    HoneycombBaseRefCounted,
    honey_base_ref_counted_t>::UnwrapDerived(HoneycombWrapperType type,
                                           HoneycombBaseRefCounted* c) {
  DCHECK(false);
  return NULL;
}

template <>
HoneycombWrapperType HoneycombCToCppRefCounted<HoneycombBaseRefCountedCToCpp,
                                   HoneycombBaseRefCounted,
                                   honey_base_ref_counted_t>::kWrapperType =
    WT_BASE_REF_COUNTED;
