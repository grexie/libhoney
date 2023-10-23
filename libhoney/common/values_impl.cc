// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoney/common/values_impl.h"

#include <algorithm>
#include <vector>

#include "base/memory/ptr_util.h"

namespace {

// Removes empty dictionaries from |dict|, potentially nested.
// Does not modify empty lists.
// From chrome/browser/chromeos/extensions/echo_private/echo_private_api.cc
void RemoveEmptyValueDicts(base::Value::Dict& dict) {
  auto it = dict.begin();
  while (it != dict.end()) {
    base::Value& value = it->second;
    if (value.is_dict()) {
      base::Value::Dict& sub_dict = value.GetDict();
      RemoveEmptyValueDicts(sub_dict);
      if (sub_dict.empty()) {
        it = dict.erase(it);
        continue;
      }
    }
    it++;
  }
}

}  // namespace

// HoneycombValueImpl implementation.

// static
HoneycombRefPtr<HoneycombValue> HoneycombValue::Create() {
  // Start with VTYPE_NULL instead of VTYPE_INVALID for backwards compatibility.
  return new HoneycombValueImpl(base::Value());
}

// static
HoneycombRefPtr<HoneycombValue> HoneycombValueImpl::GetOrCreateRefOrCopy(
    base::Value* value,
    void* parent_value,
    bool read_only,
    HoneycombValueController* controller) {
  DCHECK(value);

  if (value->is_blob()) {
    return new HoneycombValueImpl(
        HoneycombBinaryValueImpl::GetOrCreateRef(value, parent_value, controller));
  }

  if (value->is_dict()) {
    return new HoneycombValueImpl(HoneycombDictionaryValueImpl::GetOrCreateRef(
        value, parent_value, read_only, controller));
  }

  if (value->is_list()) {
    return new HoneycombValueImpl(HoneycombListValueImpl::GetOrCreateRef(
        value, parent_value, read_only, controller));
  }

  return new HoneycombValueImpl(value->Clone());
}

HoneycombValueImpl::HoneycombValueImpl() {}

HoneycombValueImpl::HoneycombValueImpl(base::Value value) {
  SetValue(std::move(value));
}

HoneycombValueImpl::HoneycombValueImpl(HoneycombRefPtr<HoneycombBinaryValue> value)
    : binary_value_(value) {}

HoneycombValueImpl::HoneycombValueImpl(HoneycombRefPtr<HoneycombDictionaryValue> value)
    : dictionary_value_(value) {}

HoneycombValueImpl::HoneycombValueImpl(HoneycombRefPtr<HoneycombListValue> value)
    : list_value_(value) {}

HoneycombValueImpl::~HoneycombValueImpl() {}

void HoneycombValueImpl::SetValue(base::Value value) {
  base::AutoLock lock_scope(lock_);
  SetValueInternal(absl::make_optional(std::move(value)));
}

base::Value HoneycombValueImpl::CopyValue() {
  base::AutoLock lock_scope(lock_);

  if (binary_value_) {
    return static_cast<HoneycombBinaryValueImpl*>(binary_value_.get())->CopyValue();
  }

  if (dictionary_value_) {
    return static_cast<HoneycombDictionaryValueImpl*>(dictionary_value_.get())
        ->CopyValue();
  }

  if (list_value_) {
    return static_cast<HoneycombListValueImpl*>(list_value_.get())->CopyValue();
  }

  return value_->Clone();
}

std::unique_ptr<base::Value> HoneycombValueImpl::CopyOrDetachValue(
    HoneycombValueController* new_controller) {
  base::AutoLock lock_scope(lock_);

  if (binary_value_) {
    return static_cast<HoneycombBinaryValueImpl*>(binary_value_.get())
        ->CopyOrDetachValue(new_controller);
  }

  if (dictionary_value_) {
    return static_cast<HoneycombDictionaryValueImpl*>(dictionary_value_.get())
        ->CopyOrDetachValue(new_controller);
  }

  if (list_value_) {
    return static_cast<HoneycombListValueImpl*>(list_value_.get())
        ->CopyOrDetachValue(new_controller);
  }

  return std::make_unique<base::Value>(value_->Clone());
}

void HoneycombValueImpl::SwapValue(base::Value* new_value,
                             void* new_parent_value,
                             HoneycombValueController* new_controller) {
  base::AutoLock lock_scope(lock_);

  if (binary_value_) {
    binary_value_ = HoneycombBinaryValueImpl::GetOrCreateRef(
        new_value, new_parent_value, new_controller);
  } else if (dictionary_value_) {
    dictionary_value_ = HoneycombDictionaryValueImpl::GetOrCreateRef(
        new_value, new_parent_value, false, new_controller);
  } else if (list_value_) {
    list_value_ = HoneycombListValueImpl::GetOrCreateRef(new_value, new_parent_value,
                                                   false, new_controller);
  }
}

bool HoneycombValueImpl::IsValid() {
  base::AutoLock lock_scope(lock_);

  if (binary_value_) {
    return binary_value_->IsValid();
  }
  if (dictionary_value_) {
    return dictionary_value_->IsValid();
  }
  if (list_value_) {
    return list_value_->IsValid();
  }

  return (value_ != nullptr);
}

bool HoneycombValueImpl::IsOwned() {
  base::AutoLock lock_scope(lock_);

  if (binary_value_) {
    return binary_value_->IsOwned();
  }
  if (dictionary_value_) {
    return dictionary_value_->IsOwned();
  }
  if (list_value_) {
    return list_value_->IsOwned();
  }

  return false;
}

bool HoneycombValueImpl::IsReadOnly() {
  base::AutoLock lock_scope(lock_);

  if (binary_value_) {
    return true;
  }
  if (dictionary_value_) {
    return dictionary_value_->IsReadOnly();
  }
  if (list_value_) {
    return list_value_->IsReadOnly();
  }

  return false;
}

bool HoneycombValueImpl::IsSame(HoneycombRefPtr<HoneycombValue> that) {
  if (that.get() == this) {
    return true;
  }
  if (!that.get() || that->GetType() != GetType()) {
    return false;
  }

  HoneycombValueImpl* impl = static_cast<HoneycombValueImpl*>(that.get());

  base::AutoLock lock_scope(lock_);
  base::AutoLock lock_scope2(impl->lock_);

  if (binary_value_) {
    return binary_value_->IsSame(impl->binary_value_);
  }
  if (dictionary_value_) {
    return dictionary_value_->IsSame(impl->dictionary_value_);
  }
  if (list_value_) {
    return list_value_->IsSame(impl->list_value_);
  }

  // Simple types are never the same.
  return false;
}

bool HoneycombValueImpl::IsEqual(HoneycombRefPtr<HoneycombValue> that) {
  if (that.get() == this) {
    return true;
  }
  if (!that.get() || that->GetType() != GetType()) {
    return false;
  }

  HoneycombValueImpl* impl = static_cast<HoneycombValueImpl*>(that.get());

  base::AutoLock lock_scope(lock_);
  base::AutoLock lock_scope2(impl->lock_);

  if (binary_value_) {
    return binary_value_->IsEqual(impl->binary_value_);
  }
  if (dictionary_value_) {
    return dictionary_value_->IsEqual(impl->dictionary_value_);
  }
  if (list_value_) {
    return list_value_->IsEqual(impl->list_value_);
  }

  if (!value_) {  // Invalid types are equal.
    return true;
  }

  return *value_ == *(impl->value_.get());
}

HoneycombRefPtr<HoneycombValue> HoneycombValueImpl::Copy() {
  base::AutoLock lock_scope(lock_);

  if (binary_value_) {
    return new HoneycombValueImpl(binary_value_->Copy());
  }
  if (dictionary_value_) {
    return new HoneycombValueImpl(dictionary_value_->Copy(false));
  }
  if (list_value_) {
    return new HoneycombValueImpl(list_value_->Copy());
  }
  if (value_) {
    return new HoneycombValueImpl(value_->Clone());
  }

  return new HoneycombValueImpl();
}

HoneycombValueType HoneycombValueImpl::GetType() {
  base::AutoLock lock_scope(lock_);

  if (binary_value_) {
    return VTYPE_BINARY;
  }
  if (dictionary_value_) {
    return VTYPE_DICTIONARY;
  }
  if (list_value_) {
    return VTYPE_LIST;
  }

  if (value_) {
    switch (value_->type()) {
      case base::Value::Type::NONE:
        return VTYPE_NULL;
      case base::Value::Type::BOOLEAN:
        return VTYPE_BOOL;
      case base::Value::Type::INTEGER:
        return VTYPE_INT;
      case base::Value::Type::DOUBLE:
        return VTYPE_DOUBLE;
      case base::Value::Type::STRING:
        return VTYPE_STRING;
      default:
        DCHECK(false);
        break;
    }
  }

  return VTYPE_INVALID;
}

bool HoneycombValueImpl::GetBool() {
  base::AutoLock lock_scope(lock_);

  bool ret_value = false;
  if (value_ && value_->is_bool()) {
    ret_value = value_->GetBool();
  }
  return ret_value;
}

int HoneycombValueImpl::GetInt() {
  base::AutoLock lock_scope(lock_);

  int ret_value = 0;
  if (value_ && value_->is_int()) {
    ret_value = value_->GetInt();
  }
  return ret_value;
}

double HoneycombValueImpl::GetDouble() {
  base::AutoLock lock_scope(lock_);

  double ret_value = 0;
  if (value_ && value_->is_double()) {
    ret_value = value_->GetDouble();
  }
  return ret_value;
}

HoneycombString HoneycombValueImpl::GetString() {
  base::AutoLock lock_scope(lock_);

  std::string ret_value;
  if (value_ && value_->is_string()) {
    ret_value = value_->GetString();
  }
  return ret_value;
}

HoneycombRefPtr<HoneycombBinaryValue> HoneycombValueImpl::GetBinary() {
  base::AutoLock lock_scope(lock_);
  return binary_value_;
}

HoneycombRefPtr<HoneycombDictionaryValue> HoneycombValueImpl::GetDictionary() {
  base::AutoLock lock_scope(lock_);
  return dictionary_value_;
}

HoneycombRefPtr<HoneycombListValue> HoneycombValueImpl::GetList() {
  base::AutoLock lock_scope(lock_);
  return list_value_;
}

bool HoneycombValueImpl::SetNull() {
  SetValue(base::Value());
  return true;
}

bool HoneycombValueImpl::SetBool(bool value) {
  SetValue(base::Value(value));
  return true;
}

bool HoneycombValueImpl::SetInt(int value) {
  SetValue(base::Value(value));
  return true;
}

bool HoneycombValueImpl::SetDouble(double value) {
  SetValue(base::Value(value));
  return true;
}

bool HoneycombValueImpl::SetString(const HoneycombString& value) {
  SetValue(base::Value(value.ToString()));
  return true;
}

bool HoneycombValueImpl::SetBinary(HoneycombRefPtr<HoneycombBinaryValue> value) {
  base::AutoLock lock_scope(lock_);
  SetValueInternal(absl::nullopt);
  binary_value_ = value;
  return true;
}

bool HoneycombValueImpl::SetDictionary(HoneycombRefPtr<HoneycombDictionaryValue> value) {
  base::AutoLock lock_scope(lock_);
  SetValueInternal(absl::nullopt);
  dictionary_value_ = value;
  return true;
}

bool HoneycombValueImpl::SetList(HoneycombRefPtr<HoneycombListValue> value) {
  base::AutoLock lock_scope(lock_);
  SetValueInternal(absl::nullopt);
  list_value_ = value;
  return true;
}

void HoneycombValueImpl::SetValueInternal(absl::optional<base::Value> value) {
  lock_.AssertAcquired();

  value_.reset(nullptr);
  binary_value_ = nullptr;
  dictionary_value_ = nullptr;
  list_value_ = nullptr;

  if (value) {
    switch ((*value).type()) {
      case base::Value::Type::BINARY:
        binary_value_ = new HoneycombBinaryValueImpl(std::move(*value));
        break;
      case base::Value::Type::DICT:
        dictionary_value_ =
            new HoneycombDictionaryValueImpl(std::move(*value), /*read_only=*/false);
        break;
      case base::Value::Type::LIST:
        list_value_ =
            new HoneycombListValueImpl(std::move(*value), /*read_only=*/false);
        break;
      default:
        value_ = std::make_unique<base::Value>(std::move(*value));
        break;
    }
  }
}

HoneycombValueController* HoneycombValueImpl::GetValueController() const {
  lock_.AssertAcquired();

  if (binary_value_) {
    return static_cast<HoneycombBinaryValueImpl*>(binary_value_.get())->controller();
  } else if (dictionary_value_) {
    return static_cast<HoneycombDictionaryValueImpl*>(dictionary_value_.get())
        ->controller();
  } else if (list_value_) {
    return static_cast<HoneycombListValueImpl*>(list_value_.get())->controller();
  }

  return nullptr;
}

void HoneycombValueImpl::AcquireLock() NO_THREAD_SAFETY_ANALYSIS {
  lock_.Acquire();

  HoneycombValueController* controller = GetValueController();
  if (controller) {
    controller->lock();
  }
}

void HoneycombValueImpl::ReleaseLock() NO_THREAD_SAFETY_ANALYSIS {
  HoneycombValueController* controller = GetValueController();
  if (controller) {
    controller->AssertLockAcquired();
    controller->unlock();
  }

  lock_.Release();
}

base::Value* HoneycombValueImpl::GetValueUnsafe() const {
  lock_.AssertAcquired();

  if (binary_value_) {
    return static_cast<HoneycombBinaryValueImpl*>(binary_value_.get())
        ->GetValueUnsafe();
  } else if (dictionary_value_) {
    return static_cast<HoneycombDictionaryValueImpl*>(dictionary_value_.get())
        ->GetValueUnsafe();
  } else if (list_value_) {
    return static_cast<HoneycombListValueImpl*>(list_value_.get())->GetValueUnsafe();
  }

  return value_.get();
}

// HoneycombBinaryValueImpl implementation.

HoneycombRefPtr<HoneycombBinaryValue> HoneycombBinaryValue::Create(const void* data,
                                                 size_t data_size) {
  DCHECK(data);
  DCHECK_GT(data_size, (size_t)0);
  if (!data || data_size == 0) {
    return nullptr;
  }

  const auto ptr = static_cast<const uint8_t*>(data);
  return new HoneycombBinaryValueImpl(base::make_span(ptr, data_size));
}

// static
HoneycombRefPtr<HoneycombBinaryValue> HoneycombBinaryValueImpl::GetOrCreateRef(
    base::Value* value,
    void* parent_value,
    HoneycombValueController* controller) {
  DCHECK(value);
  DCHECK(parent_value);
  DCHECK(controller);

  HoneycombValueController::Object* object = controller->Get(value);
  if (object) {
    return static_cast<HoneycombBinaryValueImpl*>(object);
  }

  return new HoneycombBinaryValueImpl(value, parent_value,
                                HoneycombBinaryValueImpl::kReference, controller);
}

HoneycombBinaryValueImpl::HoneycombBinaryValueImpl(base::Value value)
    : HoneycombBinaryValueImpl(new base::Value(std::move(value)),
                         /*will_delete=*/true) {}

HoneycombBinaryValueImpl::HoneycombBinaryValueImpl(base::Value* value, bool will_delete)
    : HoneycombBinaryValueImpl(value,
                         nullptr,
                         will_delete ? kOwnerWillDelete : kOwnerNoDelete,
                         nullptr) {}

HoneycombBinaryValueImpl::HoneycombBinaryValueImpl(base::span<const uint8_t> value)
    : HoneycombBinaryValueImpl(new base::Value(value),
                         nullptr,
                         kOwnerWillDelete,
                         nullptr) {}

base::Value HoneycombBinaryValueImpl::CopyValue() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, base::Value());
  return const_value().Clone();
}

std::unique_ptr<base::Value> HoneycombBinaryValueImpl::CopyOrDetachValue(
    HoneycombValueController* new_controller) {
  if (!will_delete()) {
    // Copy the value.
    return std::make_unique<base::Value>(CopyValue());
  }

  // Take ownership of the value.
  auto value = base::WrapUnique(Detach(new_controller));
  DCHECK(value.get());
  return value;
}

bool HoneycombBinaryValueImpl::IsSameValue(const base::Value* that) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return (&const_value() == that);
}

bool HoneycombBinaryValueImpl::IsEqualValue(const base::Value* that) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return const_value() == *that;
}

base::Value* HoneycombBinaryValueImpl::GetValueUnsafe() const {
  if (!VerifyAttached()) {
    return nullptr;
  }
  controller()->AssertLockAcquired();
  return mutable_value_unchecked();
}

bool HoneycombBinaryValueImpl::IsValid() {
  return !detached();
}

bool HoneycombBinaryValueImpl::IsOwned() {
  return !will_delete();
}

bool HoneycombBinaryValueImpl::IsSame(HoneycombRefPtr<HoneycombBinaryValue> that) {
  if (!that.get()) {
    return false;
  }
  if (that.get() == this) {
    return true;
  }

  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return static_cast<HoneycombBinaryValueImpl*>(that.get())
      ->IsSameValue(&const_value());
}

bool HoneycombBinaryValueImpl::IsEqual(HoneycombRefPtr<HoneycombBinaryValue> that) {
  if (!that.get()) {
    return false;
  }
  if (that.get() == this) {
    return true;
  }

  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return static_cast<HoneycombBinaryValueImpl*>(that.get())
      ->IsEqualValue(&const_value());
}

HoneycombRefPtr<HoneycombBinaryValue> HoneycombBinaryValueImpl::Copy() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, nullptr);
  return new HoneycombBinaryValueImpl(const_value().Clone());
}

size_t HoneycombBinaryValueImpl::GetSize() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, 0);
  return const_value().GetBlob().size();
}

size_t HoneycombBinaryValueImpl::GetData(void* buffer,
                                   size_t buffer_size,
                                   size_t data_offset) {
  DCHECK(buffer);
  DCHECK_GT(buffer_size, (size_t)0);
  if (!buffer || buffer_size == 0) {
    return 0;
  }

  HONEYCOMB_VALUE_VERIFY_RETURN(false, 0);

  size_t size = const_value().GetBlob().size();
  DCHECK_LT(data_offset, size);
  if (data_offset >= size) {
    return 0;
  }

  size = std::min(buffer_size, size - data_offset);
  auto* data = const_value().GetBlob().data();
  memcpy(buffer, data + data_offset, size);
  return size;
}

HoneycombBinaryValueImpl::HoneycombBinaryValueImpl(base::Value* value,
                                       void* parent_value,
                                       ValueMode value_mode,
                                       HoneycombValueController* controller)
    : HoneycombValueBase<HoneycombBinaryValue, base::Value>(value,
                                                parent_value,
                                                value_mode,
                                                /*read_only=*/true,
                                                controller) {}

// HoneycombDictionaryValueImpl implementation.

// static
HoneycombRefPtr<HoneycombDictionaryValue> HoneycombDictionaryValue::Create() {
  return new HoneycombDictionaryValueImpl(base::Value(base::Value::Type::DICT),
                                    /*read_only=*/false);
}

// static
HoneycombRefPtr<HoneycombDictionaryValue> HoneycombDictionaryValueImpl::GetOrCreateRef(
    base::Value* value,
    void* parent_value,
    bool read_only,
    HoneycombValueController* controller) {
  HoneycombValueController::Object* object = controller->Get(value);
  if (object) {
    return static_cast<HoneycombDictionaryValueImpl*>(object);
  }

  return new HoneycombDictionaryValueImpl(value, parent_value,
                                    HoneycombDictionaryValueImpl::kReference,
                                    read_only, controller);
}

HoneycombDictionaryValueImpl::HoneycombDictionaryValueImpl(base::Value value,
                                               bool read_only)
    : HoneycombDictionaryValueImpl(new base::Value(std::move(value)),
                             /*will_delete=*/true,
                             read_only) {}

HoneycombDictionaryValueImpl::HoneycombDictionaryValueImpl(base::Value::Dict value,
                                               bool read_only)
    : HoneycombDictionaryValueImpl(base::Value(std::move(value)), read_only) {}

HoneycombDictionaryValueImpl::HoneycombDictionaryValueImpl(base::Value* value,
                                               bool will_delete,
                                               bool read_only)
    : HoneycombDictionaryValueImpl(value,
                             nullptr,
                             will_delete ? kOwnerWillDelete : kOwnerNoDelete,
                             read_only,
                             nullptr) {}

base::Value HoneycombDictionaryValueImpl::CopyValue() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, base::Value());
  return const_value().Clone();
}

std::unique_ptr<base::Value> HoneycombDictionaryValueImpl::CopyOrDetachValue(
    HoneycombValueController* new_controller) {
  if (!will_delete()) {
    // Copy the value.
    return std::make_unique<base::Value>(CopyValue());
  }

  // Take ownership of the value.
  auto value = base::WrapUnique(Detach(new_controller));
  DCHECK(value.get());
  return value;
}

bool HoneycombDictionaryValueImpl::IsSameValue(const base::Value* that) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return (&const_value() == that);
}

bool HoneycombDictionaryValueImpl::IsEqualValue(const base::Value* that) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return const_value() == *that;
}

base::Value* HoneycombDictionaryValueImpl::GetValueUnsafe() const {
  if (!VerifyAttached()) {
    return nullptr;
  }
  controller()->AssertLockAcquired();
  return mutable_value_unchecked();
}

bool HoneycombDictionaryValueImpl::IsValid() {
  return !detached();
}

bool HoneycombDictionaryValueImpl::IsOwned() {
  return !will_delete();
}

bool HoneycombDictionaryValueImpl::IsReadOnly() {
  return read_only();
}

bool HoneycombDictionaryValueImpl::IsSame(HoneycombRefPtr<HoneycombDictionaryValue> that) {
  if (!that.get()) {
    return false;
  }
  if (that.get() == this) {
    return true;
  }

  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return static_cast<HoneycombDictionaryValueImpl*>(that.get())
      ->IsSameValue(&const_value());
}

bool HoneycombDictionaryValueImpl::IsEqual(HoneycombRefPtr<HoneycombDictionaryValue> that) {
  if (!that.get()) {
    return false;
  }
  if (that.get() == this) {
    return true;
  }

  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return static_cast<HoneycombDictionaryValueImpl*>(that.get())
      ->IsEqualValue(&const_value());
}

HoneycombRefPtr<HoneycombDictionaryValue> HoneycombDictionaryValueImpl::Copy(
    bool exclude_empty_children) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, nullptr);

  auto value = const_value().Clone();
  if (exclude_empty_children) {
    RemoveEmptyValueDicts(value.GetDict());
  }

  return new HoneycombDictionaryValueImpl(std::move(value), /*read_only=*/false);
}

size_t HoneycombDictionaryValueImpl::GetSize() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, 0);
  return const_value().GetDict().size();
}

bool HoneycombDictionaryValueImpl::Clear() {
  HONEYCOMB_VALUE_VERIFY_RETURN(true, false);

  // Detach any dependent values.
  controller()->RemoveDependencies(mutable_value());

  mutable_value()->GetDict().clear();
  return true;
}

bool HoneycombDictionaryValueImpl::HasKey(const HoneycombString& key) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, 0);
  return const_value().GetDict().contains(base::StringPiece(key.ToString()));
}

bool HoneycombDictionaryValueImpl::GetKeys(KeyList& keys) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, 0);

  for (const auto item : const_value().GetDict()) {
    keys.push_back(item.first);
  }

  return true;
}

bool HoneycombDictionaryValueImpl::Remove(const HoneycombString& key) {
  HONEYCOMB_VALUE_VERIFY_RETURN(true, false);
  return RemoveInternal(key);
}

HoneycombValueType HoneycombDictionaryValueImpl::GetType(const HoneycombString& key) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, VTYPE_INVALID);

  const base::Value* value =
      const_value().GetDict().Find(base::StringPiece(key.ToString()));
  if (value) {
    switch (value->type()) {
      case base::Value::Type::NONE:
        return VTYPE_NULL;
      case base::Value::Type::BOOLEAN:
        return VTYPE_BOOL;
      case base::Value::Type::INTEGER:
        return VTYPE_INT;
      case base::Value::Type::DOUBLE:
        return VTYPE_DOUBLE;
      case base::Value::Type::STRING:
        return VTYPE_STRING;
      case base::Value::Type::BINARY:
        return VTYPE_BINARY;
      case base::Value::Type::DICT:
        return VTYPE_DICTIONARY;
      case base::Value::Type::LIST:
        return VTYPE_LIST;
    }
  }

  return VTYPE_INVALID;
}

HoneycombRefPtr<HoneycombValue> HoneycombDictionaryValueImpl::GetValue(const HoneycombString& key) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, nullptr);

  const base::Value* value =
      const_value().GetDict().Find(base::StringPiece(key.ToString()));
  if (value) {
    return HoneycombValueImpl::GetOrCreateRefOrCopy(const_cast<base::Value*>(value),
                                              mutable_value_unchecked(),
                                              read_only(), controller());
  }

  return nullptr;
}

bool HoneycombDictionaryValueImpl::GetBool(const HoneycombString& key) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);

  bool ret_value = false;

  const base::Value* value =
      const_value().GetDict().Find(base::StringPiece(key.ToString()));
  if (value && value->is_bool()) {
    ret_value = value->GetBool();
  }

  return ret_value;
}

int HoneycombDictionaryValueImpl::GetInt(const HoneycombString& key) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, 0);

  int ret_value = 0;

  const base::Value* value =
      const_value().GetDict().Find(base::StringPiece(key.ToString()));
  if (value && value->is_int()) {
    ret_value = value->GetInt();
  }

  return ret_value;
}

double HoneycombDictionaryValueImpl::GetDouble(const HoneycombString& key) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, 0);

  double ret_value = 0;

  const base::Value* value =
      const_value().GetDict().Find(base::StringPiece(key.ToString()));
  if (value && value->is_double()) {
    ret_value = value->GetDouble();
  }

  return ret_value;
}

HoneycombString HoneycombDictionaryValueImpl::GetString(const HoneycombString& key) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());

  std::string ret_value;

  const base::Value* value =
      const_value().GetDict().Find(base::StringPiece(key.ToString()));
  if (value && value->is_string()) {
    ret_value = value->GetString();
  }

  return ret_value;
}

HoneycombRefPtr<HoneycombBinaryValue> HoneycombDictionaryValueImpl::GetBinary(
    const HoneycombString& key) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, nullptr);

  const base::Value* value =
      const_value().GetDict().Find(base::StringPiece(key.ToString()));
  if (value && value->is_blob()) {
    base::Value* binary_value = const_cast<base::Value*>(value);
    return HoneycombBinaryValueImpl::GetOrCreateRef(
        binary_value, mutable_value_unchecked(), controller());
  }

  return nullptr;
}

HoneycombRefPtr<HoneycombDictionaryValue> HoneycombDictionaryValueImpl::GetDictionary(
    const HoneycombString& key) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, nullptr);

  const base::Value* value =
      const_value().GetDict().Find(base::StringPiece(key.ToString()));
  if (value && value->is_dict()) {
    base::Value* dict_value = const_cast<base::Value*>(value);
    return HoneycombDictionaryValueImpl::GetOrCreateRef(
        dict_value, mutable_value_unchecked(), read_only(), controller());
  }

  return nullptr;
}

HoneycombRefPtr<HoneycombListValue> HoneycombDictionaryValueImpl::GetList(const HoneycombString& key) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, nullptr);

  const base::Value* value =
      const_value().GetDict().Find(base::StringPiece(key.ToString()));
  if (value && value->is_list()) {
    base::Value* list_value = const_cast<base::Value*>(value);
    return HoneycombListValueImpl::GetOrCreateRef(
        list_value, mutable_value_unchecked(), read_only(), controller());
  }

  return nullptr;
}

bool HoneycombDictionaryValueImpl::SetValue(const HoneycombString& key,
                                      HoneycombRefPtr<HoneycombValue> value) {
  HONEYCOMB_VALUE_VERIFY_RETURN(true, false);

  HoneycombValueImpl* impl = static_cast<HoneycombValueImpl*>(value.get());
  DCHECK(impl);

  base::Value* actual_value =
      SetInternal(key, impl->CopyOrDetachValue(controller()));
  impl->SwapValue(actual_value, mutable_value(), controller());
  return true;
}

bool HoneycombDictionaryValueImpl::SetNull(const HoneycombString& key) {
  HONEYCOMB_VALUE_VERIFY_RETURN(true, false);
  SetInternal(key, std::make_unique<base::Value>());
  return true;
}

bool HoneycombDictionaryValueImpl::SetBool(const HoneycombString& key, bool value) {
  HONEYCOMB_VALUE_VERIFY_RETURN(true, false);
  SetInternal(key, std::make_unique<base::Value>(value));
  return true;
}

bool HoneycombDictionaryValueImpl::SetInt(const HoneycombString& key, int value) {
  HONEYCOMB_VALUE_VERIFY_RETURN(true, false);
  SetInternal(key, std::make_unique<base::Value>(value));
  return true;
}

bool HoneycombDictionaryValueImpl::SetDouble(const HoneycombString& key, double value) {
  HONEYCOMB_VALUE_VERIFY_RETURN(true, false);
  SetInternal(key, std::make_unique<base::Value>(value));
  return true;
}

bool HoneycombDictionaryValueImpl::SetString(const HoneycombString& key,
                                       const HoneycombString& value) {
  HONEYCOMB_VALUE_VERIFY_RETURN(true, false);
  SetInternal(key, std::make_unique<base::Value>(value.ToString()));
  return true;
}

bool HoneycombDictionaryValueImpl::SetBinary(const HoneycombString& key,
                                       HoneycombRefPtr<HoneycombBinaryValue> value) {
  HONEYCOMB_VALUE_VERIFY_RETURN(true, false);

  HoneycombBinaryValueImpl* impl = static_cast<HoneycombBinaryValueImpl*>(value.get());
  DCHECK(impl);

  SetInternal(key, impl->CopyOrDetachValue(controller()));
  return true;
}

bool HoneycombDictionaryValueImpl::SetDictionary(
    const HoneycombString& key,
    HoneycombRefPtr<HoneycombDictionaryValue> value) {
  HONEYCOMB_VALUE_VERIFY_RETURN(true, false);

  HoneycombDictionaryValueImpl* impl =
      static_cast<HoneycombDictionaryValueImpl*>(value.get());
  DCHECK(impl);

  SetInternal(key, impl->CopyOrDetachValue(controller()));
  return true;
}

bool HoneycombDictionaryValueImpl::SetList(const HoneycombString& key,
                                     HoneycombRefPtr<HoneycombListValue> value) {
  HONEYCOMB_VALUE_VERIFY_RETURN(true, false);

  HoneycombListValueImpl* impl = static_cast<HoneycombListValueImpl*>(value.get());
  DCHECK(impl);

  SetInternal(key, impl->CopyOrDetachValue(controller()));
  return true;
}

bool HoneycombDictionaryValueImpl::RemoveInternal(const HoneycombString& key) {
  // The ExtractKey() call below which removes the Value from the dictionary
  // will return a new Value object with the moved contents of the Value that
  // exists in the implementation std::map. Consequently we use FindKey() to
  // retrieve the actual Value pointer as it current exists first, for later
  // comparison purposes.
  const base::Value* actual_value =
      const_value().GetDict().Find(base::StringPiece(key.ToString()));
  if (!actual_value) {
    return false;
  }

  // |actual_value| is no longer valid after this call.
  absl::optional<base::Value> out_value =
      mutable_value()->GetDict().Extract(base::StringPiece(key.ToString()));
  if (!out_value.has_value()) {
    return false;
  }

  // Remove the value.
  controller()->Remove(const_cast<base::Value*>(actual_value), true);

  // Only list and dictionary types may have dependencies.
  if (out_value->is_list() || out_value->is_dict()) {
    controller()->RemoveDependencies(const_cast<base::Value*>(actual_value));
  }

  return true;
}

base::Value* HoneycombDictionaryValueImpl::SetInternal(
    const HoneycombString& key,
    std::unique_ptr<base::Value> value) {
  DCHECK(value);

  RemoveInternal(key);

  // base::Value now uses move semantics which means that Set() will move the
  // contents of the passed-in base::Value instead of keeping the same object.
  // Set() then returns the actual Value pointer as it currently exists.
  base::Value* actual_value = mutable_value()->GetDict().Set(
      base::StringPiece(key.ToString()), std::move(*value));
  CHECK(actual_value);

  // |value| will be deleted when this method returns. Update the controller to
  // reference |actual_value| instead.
  controller()->Swap(value.get(), actual_value);

  return actual_value;
}

HoneycombDictionaryValueImpl::HoneycombDictionaryValueImpl(base::Value* value,
                                               void* parent_value,
                                               ValueMode value_mode,
                                               bool read_only,
                                               HoneycombValueController* controller)
    : HoneycombValueBase<HoneycombDictionaryValue, base::Value>(value,
                                                    parent_value,
                                                    value_mode,
                                                    read_only,
                                                    controller) {
  DCHECK(value->is_dict());
}

// HoneycombListValueImpl implementation.

// static
HoneycombRefPtr<HoneycombListValue> HoneycombListValue::Create() {
  return new HoneycombListValueImpl(base::Value(base::Value::Type::LIST),
                              /*read_only=*/false);
}

// static
HoneycombRefPtr<HoneycombListValue> HoneycombListValueImpl::GetOrCreateRef(
    base::Value* value,
    void* parent_value,
    bool read_only,
    HoneycombValueController* controller) {
  HoneycombValueController::Object* object = controller->Get(value);
  if (object) {
    return static_cast<HoneycombListValueImpl*>(object);
  }

  return new HoneycombListValueImpl(value, parent_value, HoneycombListValueImpl::kReference,
                              read_only, controller);
}

HoneycombListValueImpl::HoneycombListValueImpl(base::Value value, bool read_only)
    : HoneycombListValueImpl(new base::Value(std::move(value)),
                       /*will_delete=*/true,
                       read_only) {}

HoneycombListValueImpl::HoneycombListValueImpl(base::Value::List value, bool read_only)
    : HoneycombListValueImpl(base::Value(std::move(value)), read_only) {}

HoneycombListValueImpl::HoneycombListValueImpl(base::Value* value,
                                   bool will_delete,
                                   bool read_only)
    : HoneycombListValueImpl(value,
                       nullptr,
                       will_delete ? kOwnerWillDelete : kOwnerNoDelete,
                       read_only,
                       nullptr) {}

base::Value HoneycombListValueImpl::CopyValue() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, base::Value());
  return const_value().Clone();
}

std::unique_ptr<base::Value> HoneycombListValueImpl::CopyOrDetachValue(
    HoneycombValueController* new_controller) {
  if (!will_delete()) {
    // Copy the value.
    return std::make_unique<base::Value>(CopyValue());
  }

  // Take ownership of the value.
  auto value = base::WrapUnique(Detach(new_controller));
  DCHECK(value.get());
  return value;
}

bool HoneycombListValueImpl::IsSameValue(const base::Value* that) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return (&const_value() == that);
}

bool HoneycombListValueImpl::IsEqualValue(const base::Value* that) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return const_value() == *that;
}

base::Value* HoneycombListValueImpl::GetValueUnsafe() const {
  if (!VerifyAttached()) {
    return nullptr;
  }
  controller()->AssertLockAcquired();
  return mutable_value_unchecked();
}

bool HoneycombListValueImpl::IsValid() {
  return !detached();
}

bool HoneycombListValueImpl::IsOwned() {
  return !will_delete();
}

bool HoneycombListValueImpl::IsReadOnly() {
  return read_only();
}

bool HoneycombListValueImpl::IsSame(HoneycombRefPtr<HoneycombListValue> that) {
  if (!that.get()) {
    return false;
  }
  if (that.get() == this) {
    return true;
  }

  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return static_cast<HoneycombListValueImpl*>(that.get())
      ->IsSameValue(&const_value());
}

bool HoneycombListValueImpl::IsEqual(HoneycombRefPtr<HoneycombListValue> that) {
  if (!that.get()) {
    return false;
  }
  if (that.get() == this) {
    return true;
  }

  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return static_cast<HoneycombListValueImpl*>(that.get())
      ->IsEqualValue(&const_value());
}

HoneycombRefPtr<HoneycombListValue> HoneycombListValueImpl::Copy() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, nullptr);

  return new HoneycombListValueImpl(const_value().Clone(), /*read_only=*/false);
}

bool HoneycombListValueImpl::SetSize(size_t size) {
  HONEYCOMB_VALUE_VERIFY_RETURN(true, false);

  size_t current_size = const_value().GetList().size();
  if (size < current_size) {
    // Clean up any values above the requested size.
    for (size_t i = current_size - 1; i >= size; --i) {
      RemoveInternal(i);
    }
  } else if (size > 0) {
    // Expand the list size.
    // TODO: This approach seems inefficient. See https://crbug.com/1187066#c17
    // for background.
    auto& list = mutable_value()->GetList();
    while (list.size() < size) {
      list.Append(base::Value());
    }
  }
  return true;
}

size_t HoneycombListValueImpl::GetSize() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, 0);
  return const_value().GetList().size();
}

bool HoneycombListValueImpl::Clear() {
  HONEYCOMB_VALUE_VERIFY_RETURN(true, false);

  // Detach any dependent values.
  controller()->RemoveDependencies(mutable_value());

  mutable_value()->GetList().clear();
  return true;
}

bool HoneycombListValueImpl::Remove(size_t index) {
  HONEYCOMB_VALUE_VERIFY_RETURN(true, false);
  return RemoveInternal(index);
}

HoneycombValueType HoneycombListValueImpl::GetType(size_t index) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, VTYPE_INVALID);

  const auto& list = const_value().GetList();
  if (index < list.size()) {
    const base::Value& value = list[index];
    switch (value.type()) {
      case base::Value::Type::NONE:
        return VTYPE_NULL;
      case base::Value::Type::BOOLEAN:
        return VTYPE_BOOL;
      case base::Value::Type::INTEGER:
        return VTYPE_INT;
      case base::Value::Type::DOUBLE:
        return VTYPE_DOUBLE;
      case base::Value::Type::STRING:
        return VTYPE_STRING;
      case base::Value::Type::BINARY:
        return VTYPE_BINARY;
      case base::Value::Type::DICT:
        return VTYPE_DICTIONARY;
      case base::Value::Type::LIST:
        return VTYPE_LIST;
    }
  }

  return VTYPE_INVALID;
}

HoneycombRefPtr<HoneycombValue> HoneycombListValueImpl::GetValue(size_t index) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, nullptr);

  const auto& list = const_value().GetList();
  if (index < list.size()) {
    const base::Value& value = list[index];
    return HoneycombValueImpl::GetOrCreateRefOrCopy(const_cast<base::Value*>(&value),
                                              mutable_value_unchecked(),
                                              read_only(), controller());
  }

  return nullptr;
}

bool HoneycombListValueImpl::GetBool(size_t index) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);

  bool ret_value = false;
  const auto& list = const_value().GetList();
  if (index < list.size()) {
    const base::Value& value = list[index];
    if (value.is_bool()) {
      ret_value = value.GetBool();
    }
  }

  return ret_value;
}

int HoneycombListValueImpl::GetInt(size_t index) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, 0);

  int ret_value = 0;
  const auto& list = const_value().GetList();
  if (index < list.size()) {
    const base::Value& value = list[index];
    if (value.is_int()) {
      ret_value = value.GetInt();
    }
  }

  return ret_value;
}

double HoneycombListValueImpl::GetDouble(size_t index) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, 0);

  double ret_value = 0;
  const auto& list = const_value().GetList();
  if (index < list.size()) {
    const base::Value& value = list[index];
    if (value.is_double()) {
      ret_value = value.GetDouble();
    }
  }

  return ret_value;
}

HoneycombString HoneycombListValueImpl::GetString(size_t index) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());

  std::string ret_value;
  const auto& list = const_value().GetList();
  if (index < list.size()) {
    const base::Value& value = list[index];
    if (value.is_string()) {
      ret_value = value.GetString();
    }
  }

  return ret_value;
}

HoneycombRefPtr<HoneycombBinaryValue> HoneycombListValueImpl::GetBinary(size_t index) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, nullptr);

  const auto& list = const_value().GetList();
  if (index < list.size()) {
    base::Value& value = const_cast<base::Value&>(list[index]);
    if (value.is_blob()) {
      return HoneycombBinaryValueImpl::GetOrCreateRef(
          &value, mutable_value_unchecked(), controller());
    }
  }

  return nullptr;
}

HoneycombRefPtr<HoneycombDictionaryValue> HoneycombListValueImpl::GetDictionary(size_t index) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, nullptr);

  const auto& list = const_value().GetList();
  if (index < list.size()) {
    base::Value& value = const_cast<base::Value&>(list[index]);
    if (value.is_dict()) {
      return HoneycombDictionaryValueImpl::GetOrCreateRef(
          &value, mutable_value_unchecked(), read_only(), controller());
    }
  }

  return nullptr;
}

HoneycombRefPtr<HoneycombListValue> HoneycombListValueImpl::GetList(size_t index) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, nullptr);

  const auto& list = const_value().GetList();
  if (index < list.size()) {
    base::Value& value = const_cast<base::Value&>(list[index]);
    if (value.is_list()) {
      return HoneycombListValueImpl::GetOrCreateRef(&value, mutable_value_unchecked(),
                                              read_only(), controller());
    }
  }

  return nullptr;
}

bool HoneycombListValueImpl::SetValue(size_t index, HoneycombRefPtr<HoneycombValue> value) {
  HONEYCOMB_VALUE_VERIFY_RETURN(true, false);

  HoneycombValueImpl* impl = static_cast<HoneycombValueImpl*>(value.get());
  DCHECK(impl);

  base::Value* actual_value =
      SetInternal(index, impl->CopyOrDetachValue(controller()));
  impl->SwapValue(actual_value, mutable_value(), controller());
  return true;
}

bool HoneycombListValueImpl::SetNull(size_t index) {
  HONEYCOMB_VALUE_VERIFY_RETURN(true, false);
  SetInternal(index, std::make_unique<base::Value>());
  return true;
}

bool HoneycombListValueImpl::SetBool(size_t index, bool value) {
  HONEYCOMB_VALUE_VERIFY_RETURN(true, false);
  SetInternal(index, std::make_unique<base::Value>(value));
  return true;
}

bool HoneycombListValueImpl::SetInt(size_t index, int value) {
  HONEYCOMB_VALUE_VERIFY_RETURN(true, false);
  SetInternal(index, std::make_unique<base::Value>(value));
  return true;
}

bool HoneycombListValueImpl::SetDouble(size_t index, double value) {
  HONEYCOMB_VALUE_VERIFY_RETURN(true, false);
  SetInternal(index, std::make_unique<base::Value>(value));
  return true;
}

bool HoneycombListValueImpl::SetString(size_t index, const HoneycombString& value) {
  HONEYCOMB_VALUE_VERIFY_RETURN(true, false);
  SetInternal(index, std::make_unique<base::Value>(value.ToString()));
  return true;
}

bool HoneycombListValueImpl::SetBinary(size_t index,
                                 HoneycombRefPtr<HoneycombBinaryValue> value) {
  HONEYCOMB_VALUE_VERIFY_RETURN(true, false);

  HoneycombBinaryValueImpl* impl = static_cast<HoneycombBinaryValueImpl*>(value.get());
  DCHECK(impl);

  SetInternal(index, impl->CopyOrDetachValue(controller()));
  return true;
}

bool HoneycombListValueImpl::SetDictionary(size_t index,
                                     HoneycombRefPtr<HoneycombDictionaryValue> value) {
  HONEYCOMB_VALUE_VERIFY_RETURN(true, false);

  HoneycombDictionaryValueImpl* impl =
      static_cast<HoneycombDictionaryValueImpl*>(value.get());
  DCHECK(impl);

  SetInternal(index, impl->CopyOrDetachValue(controller()));
  return true;
}

bool HoneycombListValueImpl::SetList(size_t index, HoneycombRefPtr<HoneycombListValue> value) {
  HONEYCOMB_VALUE_VERIFY_RETURN(true, false);

  HoneycombListValueImpl* impl = static_cast<HoneycombListValueImpl*>(value.get());
  DCHECK(impl);

  SetInternal(index, impl->CopyOrDetachValue(controller()));
  return true;
}

bool HoneycombListValueImpl::RemoveInternal(size_t index) {
  auto& list = mutable_value()->GetList();
  if (index >= list.size()) {
    return false;
  }

  // The std::move() call below which removes the Value from the list will
  // return a new Value object with the moved contents of the Value that exists
  // in the implementation std::vector. Consequently we use operator[] to
  // retrieve the actual Value pointer as it current exists first, for later
  // comparison purposes.
  const base::Value& actual_value = list[index];

  // |actual_value| is no longer valid after this call.
  auto out_value = std::move(list[index]);
  list.erase(list.begin() + index);

  // Remove the value.
  controller()->Remove(const_cast<base::Value*>(&actual_value), true);

  // Only list and dictionary types may have dependencies.
  if (out_value.is_list() || out_value.is_dict()) {
    controller()->RemoveDependencies(const_cast<base::Value*>(&actual_value));
  }

  return true;
}

base::Value* HoneycombListValueImpl::SetInternal(size_t index,
                                           std::unique_ptr<base::Value> value) {
  DCHECK(value);

  auto& list = mutable_value()->GetList();
  if (RemoveInternal(index)) {
    CHECK_LE(index, list.size());
    list.Insert(list.begin() + index, std::move(*value));
  } else {
    if (index >= list.size()) {
      // Expand the list size.
      // TODO: This approach seems inefficient. See
      // https://crbug.com/1187066#c17 for background.
      while (list.size() <= index) {
        list.Append(base::Value());
      }
    }
    list[index] = std::move(*value);
  }

  // base::Value now uses move semantics which means that Insert()/Set() will
  // move the contents of the passed-in base::Value instead of keeping the same
  // object. Consequently we use operator[] to retrieve the actual base::Value
  // pointer as it exists in the std::vector.
  const base::Value& actual_value = list[index];

  // |value| will be deleted when this method returns. Update the controller to
  // reference |actual_value| instead.
  controller()->Swap(value.get(), const_cast<base::Value*>(&actual_value));

  return const_cast<base::Value*>(&actual_value);
}

HoneycombListValueImpl::HoneycombListValueImpl(base::Value* value,
                                   void* parent_value,
                                   ValueMode value_mode,
                                   bool read_only,
                                   HoneycombValueController* controller)
    : HoneycombValueBase<HoneycombListValue, base::Value>(value,
                                              parent_value,
                                              value_mode,
                                              read_only,
                                              controller) {
  DCHECK(value->is_list());
}
