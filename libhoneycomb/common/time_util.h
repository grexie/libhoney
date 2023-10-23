// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_COMMON_TIME_UTIL_H_
#define HONEYCOMB_LIBHONEYCOMB_COMMON_TIME_UTIL_H_
#pragma once

#include "base/time/time.h"
#include "include/internal/honey_time.h"

// Converts honey_time_t to/from a base::Time object.
void honey_time_to_basetime(const honey_time_t& honey_time, base::Time& time);
void honey_time_from_basetime(const base::Time& time, honey_time_t& honey_time);

#endif  // HONEYCOMB_LIBHONEYCOMB_COMMON_TIME_UTIL_H_
