// Copyright 2020 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_FEATURES_RUNTIME_CHECKS_H_
#define HONEYCOMB_LIBHONEYCOMB_FEATURES_RUNTIME_CHECKS_H_
#pragma once

#include "base/logging.h"
#include "honey/libhoneycomb/features/runtime.h"

#define REQUIRE_ALLOY_RUNTIME() \
  CHECK(honey::IsAlloyRuntimeEnabled()) << "Alloy runtime is required"

#define REQUIRE_CHROME_RUNTIME() \
  CHECK(honey::IsChromeRuntimeEnabled()) << "Chrome runtime is required"

#endif  // HONEYCOMB_LIBHONEYCOMB_FEATURES_RUNTIME_CHECKS_H_
