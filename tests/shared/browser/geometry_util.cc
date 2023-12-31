// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/shared/browser/geometry_util.h"

#include <cmath>

namespace client {

int LogicalToDevice(int value, float device_scale_factor) {
  float scaled_val = static_cast<float>(value) * device_scale_factor;
  return static_cast<int>(std::floor(scaled_val));
}

HoneycombRect LogicalToDevice(const HoneycombRect& value, float device_scale_factor) {
  return HoneycombRect(LogicalToDevice(value.x, device_scale_factor),
                 LogicalToDevice(value.y, device_scale_factor),
                 LogicalToDevice(value.width, device_scale_factor),
                 LogicalToDevice(value.height, device_scale_factor));
}

int DeviceToLogical(int value, float device_scale_factor) {
  float scaled_val = static_cast<float>(value) / device_scale_factor;
  return static_cast<int>(std::floor(scaled_val));
}

HoneycombRect DeviceToLogical(const HoneycombRect& value, float device_scale_factor) {
  return HoneycombRect(DeviceToLogical(value.x, device_scale_factor),
                 DeviceToLogical(value.y, device_scale_factor),
                 DeviceToLogical(value.width, device_scale_factor),
                 DeviceToLogical(value.height, device_scale_factor));
}

void DeviceToLogical(HoneycombMouseEvent& value, float device_scale_factor) {
  value.x = DeviceToLogical(value.x, device_scale_factor);
  value.y = DeviceToLogical(value.y, device_scale_factor);
}

void DeviceToLogical(HoneycombTouchEvent& value, float device_scale_factor) {
  value.x = DeviceToLogical(value.x, device_scale_factor);
  value.y = DeviceToLogical(value.y, device_scale_factor);
}

}  // namespace client
