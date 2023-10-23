// Copyright 2020 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_FEATURES_RUNTIME_H_
#define HONEYCOMB_LIBHONEY_FEATURES_RUNTIME_H_
#pragma once

#include "honey/libhoney/features/features.h"

namespace honey {

#if BUILDFLAG(ENABLE_HONEYCOMB)

inline bool IsHoneycombBuildEnabled() {
  return true;
}

// True if Honeycomb was initialized with the Alloy runtime.
bool IsAlloyRuntimeEnabled();

// True if Honeycomb was initialized with the Chrome runtime.
bool IsChromeRuntimeEnabled();

// True if Honeycomb crash reporting is enabled.
bool IsCrashReportingEnabled();

#else

inline bool IsHoneycombBuildEnabled() {
  return false;
}
inline bool IsAlloyRuntimeEnabled() {
  return false;
}
inline bool IsChromeRuntimeEnabled() {
  return false;
}
inline bool IsCrashReportingEnabled() {
  return false;
}

#endif

}  // namespace honey

#endif  // HONEYCOMB_LIBHONEY_FEATURES_RUNTIME_H_
