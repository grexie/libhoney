// Copyright (c) 2022 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/shared/common/binary_value_utils.h"

namespace bv_utils {

const char kTestSendProcessMessage[] = "testSendProcessMessage";
const char kTestSendSMRProcessMessage[] = "testSendSMRProcessMessage";

TimePoint Now() {
  return std::chrono::high_resolution_clock::now();
}

HoneycombRefPtr<HoneycombBinaryValue> CreateHoneycombBinaryValue(
    const std::vector<uint8_t>& data) {
  return HoneycombBinaryValue::Create(data.data(), data.size());
}

void CopyDataIntoMemory(const std::vector<uint8_t>& data, void* dst) {
  memcpy(dst, data.data(), data.size());
}

RendererMessage GetRendererMsgFromBinary(
    const HoneycombRefPtr<HoneycombBinaryValue>& value) {
  DCHECK_GE(value->GetSize(), sizeof(RendererMessage));
  std::vector<uint8_t> data(value->GetSize());
  value->GetData(data.data(), data.size(), 0);
  return *reinterpret_cast<const RendererMessage*>(data.data());
}

BrowserMessage GetBrowserMsgFromBinary(const HoneycombRefPtr<HoneycombBinaryValue>& value) {
  DCHECK_GE(value->GetSize(), sizeof(BrowserMessage));
  std::vector<uint8_t> data(value->GetSize());
  value->GetData(data.data(), data.size(), 0);
  return *reinterpret_cast<const BrowserMessage*>(data.data());
}

std::string ToMicroSecString(const Duration& duration) {
  const auto ms =
      std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(
          duration);

  return std::to_string(ms.count());
}

}  // namespace bv_utils
