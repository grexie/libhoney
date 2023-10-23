// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoney/common/process_message_impl.h"

#include <memory>

#include "libhoney/common/values_impl.h"

#include "base/logging.h"

// static
HoneycombRefPtr<HoneycombProcessMessage> HoneycombProcessMessage::Create(const HoneycombString& name) {
  return new HoneycombProcessMessageImpl(name, HoneycombListValue::Create());
}

HoneycombProcessMessageImpl::HoneycombProcessMessageImpl(const HoneycombString& name,
                                             HoneycombRefPtr<HoneycombListValue> arguments)
    : name_(name), arguments_(arguments) {
  DCHECK(!name_.empty());
  DCHECK(arguments_ && arguments_->IsValid());
}

HoneycombProcessMessageImpl::HoneycombProcessMessageImpl(const HoneycombString& name,
                                             base::Value::List arguments,
                                             bool read_only)
    : name_(name) {
  DCHECK(!name_.empty());

  arguments_ = new HoneycombListValueImpl(std::move(arguments), read_only);
}

HoneycombProcessMessageImpl::~HoneycombProcessMessageImpl() = default;

base::Value::List HoneycombProcessMessageImpl::TakeArgumentList() {
  DCHECK(IsValid());
  HoneycombListValueImpl* value_impl =
      static_cast<HoneycombListValueImpl*>(arguments_.get());
  auto value = value_impl->CopyOrDetachValue(nullptr);
  return std::move(value->GetList());
}

bool HoneycombProcessMessageImpl::IsValid() {
  return arguments_->IsValid();
}

bool HoneycombProcessMessageImpl::IsReadOnly() {
  return arguments_->IsReadOnly();
}

HoneycombRefPtr<HoneycombProcessMessage> HoneycombProcessMessageImpl::Copy() {
  if (!IsValid()) {
    return nullptr;
  }
  return new HoneycombProcessMessageImpl(name_, arguments_->Copy());
}

HoneycombString HoneycombProcessMessageImpl::GetName() {
  return name_;
}

HoneycombRefPtr<HoneycombListValue> HoneycombProcessMessageImpl::GetArgumentList() {
  return arguments_;
}
