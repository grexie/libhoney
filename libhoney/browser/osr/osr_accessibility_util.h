// Copyright 2017 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_OSR_ACCESSIBILITY_UTIL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_OSR_ACCESSIBILITY_UTIL_H_
#pragma once

#include <vector>
#include "include/honey_values.h"

namespace content {
struct AXEventNotificationDetails;
struct AXLocationChangeNotificationDetails;
}  // namespace content

namespace osr_accessibility_util {

// Convert Accessibility Event and location updates to HoneycombValue, which may be
// consumed or serialized with HoneycombJSONWrite.
HoneycombRefPtr<HoneycombValue> ParseAccessibilityEventData(
    const content::AXEventNotificationDetails& data);

HoneycombRefPtr<HoneycombValue> ParseAccessibilityLocationData(
    const std::vector<content::AXLocationChangeNotificationDetails>& data);

}  // namespace osr_accessibility_util

#endif  // HONEYCOMB_LIBHONEY_BROWSER_ACCESSIBILITY_UTIL_H_
