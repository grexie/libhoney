// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoney_dll/cpptoc/base_ref_counted_cpptoc.h"

HoneycombBaseRefCountedCppToC::HoneycombBaseRefCountedCppToC() {}

template <>
HoneycombRefPtr<HoneycombBaseRefCounted> HoneycombCppToCRefCounted<
    HoneycombBaseRefCountedCppToC,
    HoneycombBaseRefCounted,
    honey_base_ref_counted_t>::UnwrapDerived(HoneycombWrapperType type,
                                           honey_base_ref_counted_t* s) {
  DCHECK(false);
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCppToCRefCounted<HoneycombBaseRefCountedCppToC,
                                   HoneycombBaseRefCounted,
                                   honey_base_ref_counted_t>::kWrapperType =
    WT_BASE_REF_COUNTED;
