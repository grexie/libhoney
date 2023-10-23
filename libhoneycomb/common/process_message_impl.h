// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_COMMON_PROCESS_MESSAGE_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_COMMON_PROCESS_MESSAGE_IMPL_H_
#pragma once

#include "include/honey_process_message.h"

#include "base/values.h"

// HoneycombProcessMessage implementation.
class HoneycombProcessMessageImpl : public HoneycombProcessMessage {
 public:
  // Constructor for referencing existing |arguments|.
  HoneycombProcessMessageImpl(const HoneycombString& name,
                        HoneycombRefPtr<HoneycombListValue> arguments);

  // Constructor for creating a new HoneycombListValue that takes ownership of
  // |arguments|.
  HoneycombProcessMessageImpl(const HoneycombString& name,
                        base::Value::List arguments,
                        bool read_only);

  HoneycombProcessMessageImpl(const HoneycombProcessMessageImpl&) = delete;
  HoneycombProcessMessageImpl& operator=(const HoneycombProcessMessageImpl&) = delete;

  ~HoneycombProcessMessageImpl() override;

  // Transfer ownership of the underlying argument list to the caller, or create
  // a copy if the argument list is already owned by something else.
  // TODO: Pass by reference instead of ownership if/when Mojo adds support
  // for that.
  [[nodiscard]] base::Value::List TakeArgumentList();

  // HoneycombProcessMessage methods.
  bool IsValid() override;
  bool IsReadOnly() override;
  HoneycombRefPtr<HoneycombProcessMessage> Copy() override;
  HoneycombString GetName() override;
  HoneycombRefPtr<HoneycombListValue> GetArgumentList() override;
  HoneycombRefPtr<HoneycombSharedMemoryRegion> GetSharedMemoryRegion() override {
    return nullptr;
  }

 private:
  const HoneycombString name_;
  HoneycombRefPtr<HoneycombListValue> arguments_;

  IMPLEMENT_REFCOUNTING(HoneycombProcessMessageImpl);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_COMMON_PROCESS_MESSAGE_IMPL_H_
