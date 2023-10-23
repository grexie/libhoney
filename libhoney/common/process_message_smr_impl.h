// Copyright (c) 2022 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_COMMON_PROCESS_MESSAGE_SMR_IMPL_H_
#define HONEYCOMB_LIBHONEY_COMMON_PROCESS_MESSAGE_SMR_IMPL_H_
#pragma once

#include "include/honey_process_message.h"
#include "include/honey_shared_process_message_builder.h"

#include "base/memory/writable_shared_memory_region.h"

class HoneycombProcessMessageSMRImpl final : public HoneycombProcessMessage {
 public:
  HoneycombProcessMessageSMRImpl(const HoneycombString& name,
                           base::WritableSharedMemoryRegion&& region);
  HoneycombProcessMessageSMRImpl(const HoneycombProcessMessageSMRImpl&) = delete;
  HoneycombProcessMessageSMRImpl& operator=(const HoneycombProcessMessageSMRImpl&) = delete;
  ~HoneycombProcessMessageSMRImpl() override;

  // HoneycombProcessMessage methods.
  bool IsValid() override;
  bool IsReadOnly() override { return true; }
  HoneycombRefPtr<HoneycombProcessMessage> Copy() override { return nullptr; }
  HoneycombString GetName() override;
  HoneycombRefPtr<HoneycombListValue> GetArgumentList() override { return nullptr; }
  HoneycombRefPtr<HoneycombSharedMemoryRegion> GetSharedMemoryRegion() override;
  [[nodiscard]] base::WritableSharedMemoryRegion TakeRegion();

 private:
  const HoneycombString name_;
  base::WritableSharedMemoryRegion region_;

  IMPLEMENT_REFCOUNTING(HoneycombProcessMessageSMRImpl);
};

class HoneycombSharedProcessMessageBuilderImpl final
    : public HoneycombSharedProcessMessageBuilder {
 public:
  HoneycombSharedProcessMessageBuilderImpl(const HoneycombString& name, size_t byte_size);
  HoneycombSharedProcessMessageBuilderImpl(const HoneycombProcessMessageSMRImpl&) = delete;
  HoneycombSharedProcessMessageBuilderImpl& operator=(
      const HoneycombSharedProcessMessageBuilderImpl&) = delete;

  bool IsValid() override;
  size_t Size() override;
  void* Memory() override;
  HoneycombRefPtr<HoneycombProcessMessage> Build() override;

 private:
  const HoneycombString name_;
  base::WritableSharedMemoryRegion region_;
  base::WritableSharedMemoryMapping mapping_;

  IMPLEMENT_REFCOUNTING(HoneycombSharedProcessMessageBuilderImpl);
};

#endif  // HONEYCOMB_LIBHONEY_COMMON_PROCESS_MESSAGE_SMR_IMPL_H_
