// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_COMMON_VALUES_IMPL_H_
#define HONEYCOMB_LIBHONEY_COMMON_VALUES_IMPL_H_
#pragma once

#include <memory>
#include <vector>

#include "include/honey_values.h"
#include "libhoney/common/value_base.h"

#include "base/threading/platform_thread.h"
#include "base/values.h"

// HoneycombValue implementation
class HoneycombValueImpl : public HoneycombValue {
 public:
  // Get or create a reference to a complex value or copy a simple value.
  static HoneycombRefPtr<HoneycombValue> GetOrCreateRefOrCopy(
      base::Value* value,
      void* parent_value,
      bool read_only,
      HoneycombValueController* controller);

  HoneycombValueImpl();

  // Take ownership of |value|. Do not pass in a value owned by something else
  // (use GetOrCreateRefOrCopy instead).
  explicit HoneycombValueImpl(base::Value value);

  // Keep a reference to |value|.
  explicit HoneycombValueImpl(HoneycombRefPtr<HoneycombBinaryValue> value);
  explicit HoneycombValueImpl(HoneycombRefPtr<HoneycombDictionaryValue> value);
  explicit HoneycombValueImpl(HoneycombRefPtr<HoneycombListValue> value);

  HoneycombValueImpl(const HoneycombValueImpl&) = delete;
  HoneycombValueImpl& operator=(const HoneycombValueImpl&) = delete;

  ~HoneycombValueImpl() override;

  // Take ownership of |value|. Do not pass in a value owned by something else
  // (use GetOrCreateRefOrCopy or Set*() instead).
  void SetValue(base::Value value);

  // Return a copy of the value.
  [[nodiscard]] base::Value CopyValue();

  // Copy a simple value or transfer ownership of a complex value. If ownership
  // of the value is tranferred then this object's internal reference to the
  // value will be updated and remain valid. base::Value now uses move semantics
  // so we need to perform the copy and swap in two steps.
  [[nodiscard]] std::unique_ptr<base::Value> CopyOrDetachValue(
      HoneycombValueController* new_controller);
  void SwapValue(base::Value* new_value,
                 void* new_parent_value,
                 HoneycombValueController* new_controller);

  // Returns a reference to the underlying data. Access must be protected by
  // calling AcquireLock/ReleaseLock (e.g. use ScopedLockedValue).
  base::Value* GetValueUnsafe() const;

  // HoneycombValue methods.
  bool IsValid() override;
  bool IsOwned() override;
  bool IsReadOnly() override;
  bool IsSame(HoneycombRefPtr<HoneycombValue> that) override;
  bool IsEqual(HoneycombRefPtr<HoneycombValue> that) override;
  HoneycombRefPtr<HoneycombValue> Copy() override;
  HoneycombValueType GetType() override;
  bool GetBool() override;
  int GetInt() override;
  double GetDouble() override;
  HoneycombString GetString() override;
  HoneycombRefPtr<HoneycombBinaryValue> GetBinary() override;
  HoneycombRefPtr<HoneycombDictionaryValue> GetDictionary() override;
  HoneycombRefPtr<HoneycombListValue> GetList() override;
  bool SetNull() override;
  bool SetBool(bool value) override;
  bool SetInt(int value) override;
  bool SetDouble(double value) override;
  bool SetString(const HoneycombString& value) override;
  bool SetBinary(HoneycombRefPtr<HoneycombBinaryValue> value) override;
  bool SetDictionary(HoneycombRefPtr<HoneycombDictionaryValue> value) override;
  bool SetList(HoneycombRefPtr<HoneycombListValue> value) override;

  // Ensures exclusive access to the underlying data for the life of this scoped
  // object.
  class ScopedLockedValue {
   public:
    explicit ScopedLockedValue(HoneycombRefPtr<HoneycombValueImpl> impl) : impl_(impl) {
      impl_->AcquireLock();
    }

    ScopedLockedValue(const ScopedLockedValue&) = delete;
    ScopedLockedValue& operator=(const ScopedLockedValue&) = delete;

    ~ScopedLockedValue() { impl_->ReleaseLock(); }

    base::Value* value() const { return impl_->GetValueUnsafe(); }

   private:
    HoneycombRefPtr<HoneycombValueImpl> impl_;
  };

 private:
  void SetValueInternal(absl::optional<base::Value> value);

  // Returns the controller for the current value, if any.
  HoneycombValueController* GetValueController() const;

  // Explicitly lock/unlock this object and the underlying data.
  void AcquireLock();
  void ReleaseLock();

  // Access to all members must be protected by |lock_|.
  base::Lock lock_;

  // Simple values only.
  std::unique_ptr<base::Value> value_;

  // Complex values.
  HoneycombRefPtr<HoneycombBinaryValue> binary_value_;
  HoneycombRefPtr<HoneycombDictionaryValue> dictionary_value_;
  HoneycombRefPtr<HoneycombListValue> list_value_;

  IMPLEMENT_REFCOUNTING(HoneycombValueImpl);
};

// HoneycombBinaryValue implementation
class HoneycombBinaryValueImpl : public HoneycombValueBase<HoneycombBinaryValue, base::Value> {
 public:
  // Get or create a reference value.
  static HoneycombRefPtr<HoneycombBinaryValue> GetOrCreateRef(
      base::Value* value,
      void* parent_value,
      HoneycombValueController* controller);

  // Take ownership of |value|. Do not pass in a value owned by something else
  // (use GetOrCreateRef or constructor variant with |will_delete| argument).
  explicit HoneycombBinaryValueImpl(base::Value value);

  // Reference an existing value (set |will_delete| to false) or take ownership
  // of an existing value (set |will_delete| to true). When referencing an
  // existing value you must explicitly call Detach(nullptr) when |value| is no
  // longer valid. Use GetOrCreateRef instead of this constructor if |value| is
  // owned by some other object and you do not plan to explicitly call
  // Detach(nullptr).
  HoneycombBinaryValueImpl(base::Value* value, bool will_delete);

  // The data will always be copied.
  explicit HoneycombBinaryValueImpl(base::span<const uint8_t> value);

  HoneycombBinaryValueImpl(const HoneycombBinaryValueImpl&) = delete;
  HoneycombBinaryValueImpl& operator=(const HoneycombBinaryValueImpl&) = delete;

  // Return a copy of the value.
  [[nodiscard]] base::Value CopyValue();

  // If this value is a reference then return a copy. Otherwise, detach and
  // transfer ownership of the value.
  [[nodiscard]] std::unique_ptr<base::Value> CopyOrDetachValue(
      HoneycombValueController* new_controller);

  bool IsSameValue(const base::Value* that);
  bool IsEqualValue(const base::Value* that);

  // Returns the underlying value. Access must be protected by calling
  // lock/unlock on the controller.
  base::Value* GetValueUnsafe() const;

  // HoneycombBinaryValue methods.
  bool IsValid() override;
  bool IsOwned() override;
  bool IsSame(HoneycombRefPtr<HoneycombBinaryValue> that) override;
  bool IsEqual(HoneycombRefPtr<HoneycombBinaryValue> that) override;
  HoneycombRefPtr<HoneycombBinaryValue> Copy() override;
  size_t GetSize() override;
  size_t GetData(void* buffer, size_t buffer_size, size_t data_offset) override;

 private:
  // See the HoneycombValueBase constructor for usage. Binary values are always
  // read-only.
  HoneycombBinaryValueImpl(base::Value* value,
                     void* parent_value,
                     ValueMode value_mode,
                     HoneycombValueController* controller);
};

// HoneycombDictionaryValue implementation
class HoneycombDictionaryValueImpl
    : public HoneycombValueBase<HoneycombDictionaryValue, base::Value> {
 public:
  // Get or create a reference value.
  static HoneycombRefPtr<HoneycombDictionaryValue> GetOrCreateRef(
      base::Value* value,
      void* parent_value,
      bool read_only,
      HoneycombValueController* controller);

  // Take ownership of |value|. Do not pass in a value owned by something else
  // (use GetOrCreateRef or constructor variant with |will_delete| argument).
  HoneycombDictionaryValueImpl(base::Value value, bool read_only);
  HoneycombDictionaryValueImpl(base::Value::Dict value, bool read_only);

  // Reference an existing value (set |will_delete| to false) or take ownership
  // of an existing value (set |will_delete| to true). When referencing an
  // existing value you must explicitly call Detach(nullptr) when |value| is no
  // longer valid. Use GetOrCreateRef instead of this constructor if |value| is
  // owned by some other object and you do not plan to explicitly call
  // Detach(nullptr).
  HoneycombDictionaryValueImpl(base::Value* value, bool will_delete, bool read_only);

  HoneycombDictionaryValueImpl(const HoneycombDictionaryValueImpl&) = delete;
  HoneycombDictionaryValueImpl& operator=(const HoneycombDictionaryValueImpl&) = delete;

  // Return a copy of the value.
  [[nodiscard]] base::Value CopyValue();

  // If this value is a reference then return a copy. Otherwise, detach and
  // transfer ownership of the value.
  [[nodiscard]] std::unique_ptr<base::Value> CopyOrDetachValue(
      HoneycombValueController* new_controller);

  bool IsSameValue(const base::Value* that);
  bool IsEqualValue(const base::Value* that);

  // Returns the underlying value. Access must be protected by calling
  // lock/unlock on the controller.
  base::Value* GetValueUnsafe() const;

  // HoneycombDictionaryValue methods.
  bool IsValid() override;
  bool IsOwned() override;
  bool IsReadOnly() override;
  bool IsSame(HoneycombRefPtr<HoneycombDictionaryValue> that) override;
  bool IsEqual(HoneycombRefPtr<HoneycombDictionaryValue> that) override;
  HoneycombRefPtr<HoneycombDictionaryValue> Copy(bool exclude_empty_children) override;
  size_t GetSize() override;
  bool Clear() override;
  bool HasKey(const HoneycombString& key) override;
  bool GetKeys(KeyList& keys) override;
  bool Remove(const HoneycombString& key) override;
  HoneycombValueType GetType(const HoneycombString& key) override;
  HoneycombRefPtr<HoneycombValue> GetValue(const HoneycombString& key) override;
  bool GetBool(const HoneycombString& key) override;
  int GetInt(const HoneycombString& key) override;
  double GetDouble(const HoneycombString& key) override;
  HoneycombString GetString(const HoneycombString& key) override;
  HoneycombRefPtr<HoneycombBinaryValue> GetBinary(const HoneycombString& key) override;
  HoneycombRefPtr<HoneycombDictionaryValue> GetDictionary(const HoneycombString& key) override;
  HoneycombRefPtr<HoneycombListValue> GetList(const HoneycombString& key) override;
  bool SetValue(const HoneycombString& key, HoneycombRefPtr<HoneycombValue> value) override;
  bool SetNull(const HoneycombString& key) override;
  bool SetBool(const HoneycombString& key, bool value) override;
  bool SetInt(const HoneycombString& key, int value) override;
  bool SetDouble(const HoneycombString& key, double value) override;
  bool SetString(const HoneycombString& key, const HoneycombString& value) override;
  bool SetBinary(const HoneycombString& key,
                 HoneycombRefPtr<HoneycombBinaryValue> value) override;
  bool SetDictionary(const HoneycombString& key,
                     HoneycombRefPtr<HoneycombDictionaryValue> value) override;
  bool SetList(const HoneycombString& key, HoneycombRefPtr<HoneycombListValue> value) override;

 private:
  // See the HoneycombValueBase constructor for usage.
  HoneycombDictionaryValueImpl(base::Value* value,
                         void* parent_value,
                         ValueMode value_mode,
                         bool read_only,
                         HoneycombValueController* controller);

  bool RemoveInternal(const HoneycombString& key);
  base::Value* SetInternal(const HoneycombString& key,
                           std::unique_ptr<base::Value> value);
};

// HoneycombListValue implementation
class HoneycombListValueImpl : public HoneycombValueBase<HoneycombListValue, base::Value> {
 public:
  // Get or create a reference value.
  static HoneycombRefPtr<HoneycombListValue> GetOrCreateRef(base::Value* value,
                                                void* parent_value,
                                                bool read_only,
                                                HoneycombValueController* controller);

  // Take ownership of |value|. Do not pass in a value owned by something else
  // (use GetOrCreateRef or constructor variant with |will_delete| argument).
  HoneycombListValueImpl(base::Value value, bool read_only);
  HoneycombListValueImpl(base::Value::List value, bool read_only);

  // Reference an existing value (set |will_delete| to false) or take ownership
  // of an existing value (set |will_delete| to true). When referencing an
  // existing value you must explicitly call Detach(nullptr) when |value| is no
  // longer valid. Use GetOrCreateRef instead of this constructor if |value| is
  // owned by some other object and you do not plan to explicitly call
  // Detach(nullptr).
  HoneycombListValueImpl(base::Value* value, bool will_delete, bool read_only);

  HoneycombListValueImpl(const HoneycombListValueImpl&) = delete;
  HoneycombListValueImpl& operator=(const HoneycombListValueImpl&) = delete;

  // Return a copy of the value.
  [[nodiscard]] base::Value CopyValue();

  // If this value is a reference then return a copy. Otherwise, detach and
  // transfer ownership of the value.
  [[nodiscard]] std::unique_ptr<base::Value> CopyOrDetachValue(
      HoneycombValueController* new_controller);

  bool IsSameValue(const base::Value* that);
  bool IsEqualValue(const base::Value* that);

  // Returns the underlying value. Access must be protected by calling
  // lock/unlock on the controller.
  base::Value* GetValueUnsafe() const;

  // HoneycombListValue methods.
  bool IsValid() override;
  bool IsOwned() override;
  bool IsReadOnly() override;
  bool IsSame(HoneycombRefPtr<HoneycombListValue> that) override;
  bool IsEqual(HoneycombRefPtr<HoneycombListValue> that) override;
  HoneycombRefPtr<HoneycombListValue> Copy() override;
  bool SetSize(size_t size) override;
  size_t GetSize() override;
  bool Clear() override;
  bool Remove(size_t index) override;
  HoneycombValueType GetType(size_t index) override;
  HoneycombRefPtr<HoneycombValue> GetValue(size_t index) override;
  bool GetBool(size_t index) override;
  int GetInt(size_t index) override;
  double GetDouble(size_t index) override;
  HoneycombString GetString(size_t index) override;
  HoneycombRefPtr<HoneycombBinaryValue> GetBinary(size_t index) override;
  HoneycombRefPtr<HoneycombDictionaryValue> GetDictionary(size_t index) override;
  HoneycombRefPtr<HoneycombListValue> GetList(size_t index) override;
  bool SetValue(size_t index, HoneycombRefPtr<HoneycombValue> value) override;
  bool SetNull(size_t index) override;
  bool SetBool(size_t index, bool value) override;
  bool SetInt(size_t index, int value) override;
  bool SetDouble(size_t index, double value) override;
  bool SetString(size_t index, const HoneycombString& value) override;
  bool SetBinary(size_t index, HoneycombRefPtr<HoneycombBinaryValue> value) override;
  bool SetDictionary(size_t index,
                     HoneycombRefPtr<HoneycombDictionaryValue> value) override;
  bool SetList(size_t index, HoneycombRefPtr<HoneycombListValue> value) override;

 private:
  // See the HoneycombValueBase constructor for usage.
  HoneycombListValueImpl(base::Value* value,
                   void* parent_value,
                   ValueMode value_mode,
                   bool read_only,
                   HoneycombValueController* controller);

  bool RemoveInternal(size_t index);
  base::Value* SetInternal(size_t index, std::unique_ptr<base::Value> value);
};

#endif  // HONEYCOMB_LIBHONEY_COMMON_VALUES_IMPL_H_
