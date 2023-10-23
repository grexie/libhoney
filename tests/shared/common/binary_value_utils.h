// Copyright (c) 2022 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_SHARED_COMMON_BINARY_VALUE_UTILS
#define HONEYCOMB_TESTS_SHARED_COMMON_BINARY_VALUE_UTILS
#pragma once

#include <chrono>
#include <cstdint>
#include <vector>
#include "include/honey_values.h"

namespace bv_utils {

extern const char kTestSendProcessMessage[];
extern const char kTestSendSMRProcessMessage[];

using TimePoint = std::chrono::high_resolution_clock::time_point;
using Duration = std::chrono::high_resolution_clock::duration;

struct RendererMessage {
  int test_id;
  TimePoint start_time;
};

struct BrowserMessage {
  int test_id;
  Duration duration;
  TimePoint start_time;
};

TimePoint Now();

HoneycombRefPtr<HoneycombBinaryValue> CreateHoneycombBinaryValue(
    const std::vector<uint8_t>& data);

void CopyDataIntoMemory(const std::vector<uint8_t>& data, void* dst);

RendererMessage GetRendererMsgFromBinary(
    const HoneycombRefPtr<HoneycombBinaryValue>& value);

BrowserMessage GetBrowserMsgFromBinary(const HoneycombRefPtr<HoneycombBinaryValue>& value);

std::string ToMicroSecString(const Duration& duration);

}  // namespace bv_utils

#endif  // HONEYCOMB_TESTS_SHARED_COMMON_BINARY_VALUE_UTILS
