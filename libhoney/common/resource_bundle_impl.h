// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_COMMON_RESOURCE_BUNDLE_IMPL_H_
#define HONEYCOMB_LIBHONEY_COMMON_RESOURCE_BUNDLE_IMPL_H_
#pragma once

#include "include/honey_resource_bundle.h"

class HoneycombResourceBundleImpl : public HoneycombResourceBundle {
 public:
  HoneycombResourceBundleImpl();

  HoneycombResourceBundleImpl(const HoneycombResourceBundleImpl&) = delete;
  HoneycombResourceBundleImpl& operator=(const HoneycombResourceBundleImpl&) = delete;

  // HoneycombResourceBundle methods.
  HoneycombString GetLocalizedString(int string_id) override;
  HoneycombRefPtr<HoneycombBinaryValue> GetDataResource(int resource_id) override;
  HoneycombRefPtr<HoneycombBinaryValue> GetDataResourceForScale(
      int resource_id,
      ScaleFactor scale_factor) override;

 private:
  IMPLEMENT_REFCOUNTING(HoneycombResourceBundleImpl);
};

#endif  // HONEYCOMB_LIBHONEY_COMMON_RESOURCE_BUNDLE_IMPL_H_
