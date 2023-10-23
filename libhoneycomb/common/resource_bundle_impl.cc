// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "libhoneycomb/common/resource_bundle_impl.h"

#include "base/memory/ref_counted_memory.h"
#include "ui/base/resource/resource_bundle.h"

HoneycombResourceBundleImpl::HoneycombResourceBundleImpl() {}

HoneycombString HoneycombResourceBundleImpl::GetLocalizedString(int string_id) {
  if (!ui::ResourceBundle::HasSharedInstance()) {
    return HoneycombString();
  }

  return ui::ResourceBundle::GetSharedInstance().GetLocalizedString(string_id);
}

HoneycombRefPtr<HoneycombBinaryValue> HoneycombResourceBundleImpl::GetDataResource(
    int resource_id) {
  return GetDataResourceForScale(resource_id, SCALE_FACTOR_NONE);
}

HoneycombRefPtr<HoneycombBinaryValue> HoneycombResourceBundleImpl::GetDataResourceForScale(
    int resource_id,
    ScaleFactor scale_factor) {
  if (!ui::ResourceBundle::HasSharedInstance()) {
    return nullptr;
  }

  base::RefCountedMemory* result =
      ui::ResourceBundle::GetSharedInstance().LoadDataResourceBytesForScale(
          resource_id, static_cast<ui::ResourceScaleFactor>(scale_factor));
  if (!result) {
    return nullptr;
  }

  return HoneycombBinaryValue::Create(result->data(), result->size());
}

// static
HoneycombRefPtr<HoneycombResourceBundle> HoneycombResourceBundle::GetGlobal() {
  return new HoneycombResourceBundleImpl();
}
