// Copyright (c) 2023 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the Honeycomb translator tool. If making changes by
// hand only do so within the body of existing method and function
// implementations. See the translator.README.txt file in the tools directory
// for more information.
//
// $hash=cab5b018f6706a3c8496865e0c9f30fcbc94cdd8$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CTOCPP_V8VALUE_CTOCPP_H_
#define HONEYCOMB_LIBHONEY_DLL_CTOCPP_V8VALUE_CTOCPP_H_
#pragma once

#if !defined(WRAPPING_HONEYCOMB_SHARED)
#error This file can be included wrapper-side only
#endif

#include <vector>
#include "include/capi/honey_v8_capi.h"
#include "include/honey_v8.h"
#include "libhoney_dll/ctocpp/ctocpp_ref_counted.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class HoneycombV8ValueCToCpp
    : public HoneycombCToCppRefCounted<HoneycombV8ValueCToCpp, HoneycombV8Value, honey_v8value_t> {
 public:
  HoneycombV8ValueCToCpp();
  virtual ~HoneycombV8ValueCToCpp();

  // HoneycombV8Value methods.
  bool IsValid() override;
  bool IsUndefined() override;
  bool IsNull() override;
  bool IsBool() override;
  bool IsInt() override;
  bool IsUInt() override;
  bool IsDouble() override;
  bool IsDate() override;
  bool IsString() override;
  bool IsObject() override;
  bool IsArray() override;
  bool IsArrayBuffer() override;
  bool IsFunction() override;
  bool IsPromise() override;
  bool IsSame(HoneycombRefPtr<HoneycombV8Value> that) override;
  bool GetBoolValue() override;
  int32_t GetIntValue() override;
  uint32_t GetUIntValue() override;
  double GetDoubleValue() override;
  HoneycombBaseTime GetDateValue() override;
  HoneycombString GetStringValue() override;
  bool IsUserCreated() override;
  bool HasException() override;
  HoneycombRefPtr<HoneycombV8Exception> GetException() override;
  bool ClearException() override;
  bool WillRethrowExceptions() override;
  bool SetRethrowExceptions(bool rethrow) override;
  bool HasValue(const HoneycombString& key) override;
  bool HasValue(int index) override;
  bool DeleteValue(const HoneycombString& key) override;
  bool DeleteValue(int index) override;
  HoneycombRefPtr<HoneycombV8Value> GetValue(const HoneycombString& key) override;
  HoneycombRefPtr<HoneycombV8Value> GetValue(int index) override;
  bool SetValue(const HoneycombString& key,
                HoneycombRefPtr<HoneycombV8Value> value,
                PropertyAttribute attribute) override;
  bool SetValue(int index, HoneycombRefPtr<HoneycombV8Value> value) override;
  bool SetValue(const HoneycombString& key,
                AccessControl settings,
                PropertyAttribute attribute) override;
  bool GetKeys(std::vector<HoneycombString>& keys) override;
  bool SetUserData(HoneycombRefPtr<HoneycombBaseRefCounted> user_data) override;
  HoneycombRefPtr<HoneycombBaseRefCounted> GetUserData() override;
  int GetExternallyAllocatedMemory() override;
  int AdjustExternallyAllocatedMemory(int change_in_bytes) override;
  int GetArrayLength() override;
  HoneycombRefPtr<HoneycombV8ArrayBufferReleaseCallback> GetArrayBufferReleaseCallback()
      override;
  bool NeuterArrayBuffer() override;
  HoneycombString GetFunctionName() override;
  HoneycombRefPtr<HoneycombV8Handler> GetFunctionHandler() override;
  HoneycombRefPtr<HoneycombV8Value> ExecuteFunction(
      HoneycombRefPtr<HoneycombV8Value> object,
      const HoneycombV8ValueList& arguments) override;
  HoneycombRefPtr<HoneycombV8Value> ExecuteFunctionWithContext(
      HoneycombRefPtr<HoneycombV8Context> context,
      HoneycombRefPtr<HoneycombV8Value> object,
      const HoneycombV8ValueList& arguments) override;
  bool ResolvePromise(HoneycombRefPtr<HoneycombV8Value> arg) override;
  bool RejectPromise(const HoneycombString& errorMsg) override;
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CTOCPP_V8VALUE_CTOCPP_H_
