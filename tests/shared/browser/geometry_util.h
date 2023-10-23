// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_SHARED_BROWSER_GEOMETRY_UTIL_H_
#define HONEYCOMB_TESTS_SHARED_BROWSER_GEOMETRY_UTIL_H_
#pragma once

#include "include/internal/honey_types_wrappers.h"

namespace client {

// Convert |value| from logical coordinates to device coordinates.
int LogicalToDevice(int value, float device_scale_factor);
HoneycombRect LogicalToDevice(const HoneycombRect& value, float device_scale_factor);

// Convert |value| from device coordinates to logical coordinates.
int DeviceToLogical(int value, float device_scale_factor);
HoneycombRect DeviceToLogical(const HoneycombRect& value, float device_scale_factor);
void DeviceToLogical(HoneycombMouseEvent& value, float device_scale_factor);
void DeviceToLogical(HoneycombTouchEvent& value, float device_scale_factor);

}  // namespace client

#endif  // HONEYCOMB_TESTS_SHARED_BROWSER_GEOMETRY_UTIL_H_
