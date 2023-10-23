// Copyright (c) 2022 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoney/common/process_message_smr_impl.h"

#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/memory/shared_memory_mapping.h"

namespace {

class HoneycombSharedMemoryRegionImpl final : public HoneycombSharedMemoryRegion {
 public:
  HoneycombSharedMemoryRegionImpl(base::WritableSharedMemoryMapping&& mapping)
      : mapping_(std::move(mapping)) {}
  HoneycombSharedMemoryRegionImpl(const HoneycombSharedMemoryRegionImpl&) = delete;
  HoneycombSharedMemoryRegionImpl& operator=(const HoneycombSharedMemoryRegionImpl&) =
      delete;

  // HoneycombSharedMemoryRegion methods
  bool IsValid() override { return mapping_.IsValid(); }
  size_t Size() override { return IsValid() ? mapping_.size() : 0; }
  void* Memory() override { return mapping_.memory(); }

 private:
  base::WritableSharedMemoryMapping mapping_;
  IMPLEMENT_REFCOUNTING(HoneycombSharedMemoryRegionImpl);
};

}  // namespace

HoneycombProcessMessageSMRImpl::HoneycombProcessMessageSMRImpl(
    const HoneycombString& name,
    base::WritableSharedMemoryRegion&& region)
    : name_(name), region_(std::move(region)) {
  DCHECK(!name_.empty());
  DCHECK(region_.IsValid());
}

HoneycombProcessMessageSMRImpl::~HoneycombProcessMessageSMRImpl() = default;

bool HoneycombProcessMessageSMRImpl::IsValid() {
  return region_.IsValid();
}

HoneycombString HoneycombProcessMessageSMRImpl::GetName() {
  return name_;
}

HoneycombRefPtr<HoneycombSharedMemoryRegion>
HoneycombProcessMessageSMRImpl::GetSharedMemoryRegion() {
  return new HoneycombSharedMemoryRegionImpl(region_.Map());
}

base::WritableSharedMemoryRegion HoneycombProcessMessageSMRImpl::TakeRegion() {
  return std::move(region_);
}

// static
HoneycombRefPtr<HoneycombSharedProcessMessageBuilder>
HoneycombSharedProcessMessageBuilder::Create(const HoneycombString& name,
                                       size_t byte_size) {
  return new HoneycombSharedProcessMessageBuilderImpl(name, byte_size);
}

HoneycombSharedProcessMessageBuilderImpl::HoneycombSharedProcessMessageBuilderImpl(
    const HoneycombString& name,
    size_t byte_size)
    : name_(name),
      region_(base::WritableSharedMemoryRegion::Create(byte_size)),
      mapping_(region_.Map()) {}

bool HoneycombSharedProcessMessageBuilderImpl::IsValid() {
  return region_.IsValid() && mapping_.IsValid();
}

size_t HoneycombSharedProcessMessageBuilderImpl::Size() {
  return !IsValid() ? 0 : region_.GetSize();
}

void* HoneycombSharedProcessMessageBuilderImpl::Memory() {
  return !IsValid() ? nullptr : mapping_.memory();
}

HoneycombRefPtr<HoneycombProcessMessage> HoneycombSharedProcessMessageBuilderImpl::Build() {
  if (!IsValid()) {
    return nullptr;
  }

  // Invalidate mappping
  mapping_ = base::WritableSharedMemoryMapping();
  return new HoneycombProcessMessageSMRImpl(name_, std::move(region_));
}