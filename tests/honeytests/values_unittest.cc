// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/honey_task.h"
#include "include/honey_values.h"
#include "tests/honeytests/test_handler.h"
#include "tests/honeytests/test_util.h"
#include "tests/gtest/include/gtest/gtest.h"

namespace {

// Dictionary test keys.
const char* kNullKey = "null_key";
const char* kBoolKey = "bool_key";
const char* kIntKey = "int_key";
const char* kDoubleKey = "double_key";
const char* kStringKey = "string_key";
const char* kBinaryKey = "binary_key";
const char* kDictionaryKey = "dict_key";
const char* kListKey = "list_key";

// List test indexes.
enum {
  kNullIndex = 0,
  kBoolIndex,
  kIntIndex,
  kDoubleIndex,
  kStringIndex,
  kBinaryIndex,
  kDictionaryIndex,
  kListIndex,
};

// Dictionary/list test values.
const bool kBoolValue = true;
const int kIntValue = 12;
const double kDoubleValue = 4.5432;
const char* kStringValue = "My string value";

// BINARY TEST HELPERS

// Test a binary value.
void TestBinary(HoneycombRefPtr<HoneycombBinaryValue> value, char* data, size_t data_size) {
  // Testing requires strings longer than 15 characters.
  EXPECT_GT(data_size, 15U);

  EXPECT_EQ(data_size, value->GetSize());

  char* buff = new char[data_size + 1];
  char old_char;

  // Test full read.
  memset(buff, 0, data_size + 1);
  EXPECT_EQ(data_size, value->GetData(buff, data_size, 0));
  EXPECT_TRUE(!strcmp(buff, data));

  // Test partial read with offset.
  memset(buff, 0, data_size + 1);
  old_char = data[15];
  data[15] = 0;
  EXPECT_EQ(10U, value->GetData(buff, 10, 5));
  EXPECT_TRUE(!strcmp(buff, data + 5));
  data[15] = old_char;

  // Test that changes to the original data have no effect.
  memset(buff, 0, data_size + 1);
  old_char = data[0];
  data[0] = '.';
  EXPECT_EQ(1U, value->GetData(buff, 1, 0));
  EXPECT_EQ(old_char, buff[0]);
  data[0] = old_char;

  // Test copy.
  HoneycombRefPtr<HoneycombBinaryValue> copy = value->Copy();
  TestBinaryEqual(copy, value);

  delete[] buff;
}

// Used to test access of binary data on a different thread.
class BinaryTask : public HoneycombTask {
 public:
  BinaryTask(HoneycombRefPtr<HoneycombBinaryValue> value, char* data, size_t data_size)
      : value_(value), data_(data), data_size_(data_size) {}

  void Execute() override { TestBinary(value_, data_, data_size_); }

 private:
  HoneycombRefPtr<HoneycombBinaryValue> value_;
  char* data_;
  size_t data_size_;

  IMPLEMENT_REFCOUNTING(BinaryTask);
};

// DICTIONARY TEST HELPERS

// Test dictionary null value.
void TestDictionaryNull(HoneycombRefPtr<HoneycombDictionaryValue> value) {
  EXPECT_FALSE(value->HasKey(kNullKey));
  EXPECT_TRUE(value->SetNull(kNullKey));
  EXPECT_TRUE(value->HasKey(kNullKey));
  EXPECT_EQ(VTYPE_NULL, value->GetType(kNullKey));
}

// Test dictionary bool value.
void TestDictionaryBool(HoneycombRefPtr<HoneycombDictionaryValue> value) {
  EXPECT_FALSE(value->HasKey(kBoolKey));
  EXPECT_TRUE(value->SetBool(kBoolKey, kBoolValue));
  EXPECT_TRUE(value->HasKey(kBoolKey));
  EXPECT_EQ(VTYPE_BOOL, value->GetType(kBoolKey));
  EXPECT_EQ(kBoolValue, value->GetBool(kBoolKey));
}

// Test dictionary int value.
void TestDictionaryInt(HoneycombRefPtr<HoneycombDictionaryValue> value) {
  EXPECT_FALSE(value->HasKey(kIntKey));
  EXPECT_TRUE(value->SetInt(kIntKey, kIntValue));
  EXPECT_TRUE(value->HasKey(kIntKey));
  EXPECT_EQ(VTYPE_INT, value->GetType(kIntKey));
  EXPECT_EQ(kIntValue, value->GetInt(kIntKey));
}

// Test dictionary double value.
void TestDictionaryDouble(HoneycombRefPtr<HoneycombDictionaryValue> value) {
  EXPECT_FALSE(value->HasKey(kDoubleKey));
  EXPECT_TRUE(value->SetDouble(kDoubleKey, kDoubleValue));
  EXPECT_TRUE(value->HasKey(kDoubleKey));
  EXPECT_EQ(VTYPE_DOUBLE, value->GetType(kDoubleKey));
  EXPECT_EQ(kDoubleValue, value->GetDouble(kDoubleKey));
}

// Test dictionary string value.
void TestDictionaryString(HoneycombRefPtr<HoneycombDictionaryValue> value) {
  EXPECT_FALSE(value->HasKey(kStringKey));
  EXPECT_TRUE(value->SetString(kStringKey, kStringValue));
  EXPECT_TRUE(value->HasKey(kStringKey));
  EXPECT_EQ(VTYPE_STRING, value->GetType(kStringKey));
  EXPECT_EQ(kStringValue, value->GetString(kStringKey).ToString());
}

// Test dictionary binary value.
void TestDictionaryBinary(HoneycombRefPtr<HoneycombDictionaryValue> value,
                          char* binary_data,
                          size_t binary_data_size,
                          HoneycombRefPtr<HoneycombBinaryValue>& binary_value) {
  binary_value = HoneycombBinaryValue::Create(binary_data, binary_data_size);
  EXPECT_TRUE(binary_value.get());
  EXPECT_TRUE(binary_value->IsValid());
  EXPECT_FALSE(binary_value->IsOwned());
  EXPECT_FALSE(value->HasKey(kBinaryKey));
  EXPECT_TRUE(value->SetBinary(kBinaryKey, binary_value));
  EXPECT_FALSE(binary_value->IsValid());  // Value should be detached
  EXPECT_TRUE(value->HasKey(kBinaryKey));
  EXPECT_EQ(VTYPE_BINARY, value->GetType(kBinaryKey));
  binary_value = value->GetBinary(kBinaryKey);
  EXPECT_TRUE(binary_value.get());
  EXPECT_TRUE(binary_value->IsValid());
  EXPECT_TRUE(binary_value->IsOwned());
  TestBinary(binary_value, binary_data, binary_data_size);
}

// Test dictionary dictionary value.
void TestDictionaryDictionary(HoneycombRefPtr<HoneycombDictionaryValue> value,
                              HoneycombRefPtr<HoneycombDictionaryValue>& dictionary_value) {
  dictionary_value = HoneycombDictionaryValue::Create();
  EXPECT_TRUE(dictionary_value.get());
  EXPECT_TRUE(dictionary_value->IsValid());
  EXPECT_FALSE(dictionary_value->IsOwned());
  EXPECT_FALSE(dictionary_value->IsReadOnly());
  EXPECT_TRUE(dictionary_value->SetInt(kIntKey, kIntValue));
  EXPECT_EQ(1U, dictionary_value->GetSize());
  EXPECT_FALSE(value->HasKey(kDictionaryKey));
  EXPECT_TRUE(value->SetDictionary(kDictionaryKey, dictionary_value));
  EXPECT_FALSE(dictionary_value->IsValid());  // Value should be detached
  EXPECT_TRUE(value->HasKey(kDictionaryKey));
  EXPECT_EQ(VTYPE_DICTIONARY, value->GetType(kDictionaryKey));
  dictionary_value = value->GetDictionary(kDictionaryKey);
  EXPECT_TRUE(dictionary_value.get());
  EXPECT_TRUE(dictionary_value->IsValid());
  EXPECT_TRUE(dictionary_value->IsOwned());
  EXPECT_FALSE(dictionary_value->IsReadOnly());
  EXPECT_EQ(1U, dictionary_value->GetSize());
  EXPECT_EQ(kIntValue, dictionary_value->GetInt(kIntKey));
}

// Test dictionary list value.
void TestDictionaryList(HoneycombRefPtr<HoneycombDictionaryValue> value,
                        HoneycombRefPtr<HoneycombListValue>& list_value) {
  list_value = HoneycombListValue::Create();
  EXPECT_TRUE(list_value.get());
  EXPECT_TRUE(list_value->IsValid());
  EXPECT_FALSE(list_value->IsOwned());
  EXPECT_FALSE(list_value->IsReadOnly());
  EXPECT_TRUE(list_value->SetInt(0, kIntValue));
  EXPECT_EQ(1U, list_value->GetSize());
  EXPECT_FALSE(value->HasKey(kListKey));
  EXPECT_TRUE(value->SetList(kListKey, list_value));
  EXPECT_FALSE(list_value->IsValid());  // Value should be detached
  EXPECT_TRUE(value->HasKey(kListKey));
  EXPECT_EQ(VTYPE_LIST, value->GetType(kListKey));
  list_value = value->GetList(kListKey);
  EXPECT_TRUE(list_value.get());
  EXPECT_TRUE(list_value->IsValid());
  EXPECT_TRUE(list_value->IsOwned());
  EXPECT_FALSE(list_value->IsReadOnly());
  EXPECT_EQ(1U, list_value->GetSize());
  EXPECT_EQ(kIntValue, list_value->GetInt(0));
}

// Test dictionary value.
void TestDictionary(HoneycombRefPtr<HoneycombDictionaryValue> value,
                    char* binary_data,
                    size_t binary_data_size) {
  HoneycombRefPtr<HoneycombBinaryValue> binary_value;
  HoneycombRefPtr<HoneycombDictionaryValue> dictionary_value;
  HoneycombRefPtr<HoneycombListValue> list_value;

  // Test the size.
  EXPECT_EQ(0U, value->GetSize());

  TestDictionaryNull(value);
  TestDictionaryBool(value);
  TestDictionaryInt(value);
  TestDictionaryDouble(value);
  TestDictionaryString(value);
  TestDictionaryBinary(value, binary_data, binary_data_size, binary_value);
  TestDictionaryDictionary(value, dictionary_value);
  TestDictionaryList(value, list_value);

  // Test the size.
  EXPECT_EQ(8U, value->GetSize());

  // Test copy.
  HoneycombRefPtr<HoneycombDictionaryValue> copy = value->Copy(false);
  TestDictionaryEqual(value, copy);

  // Test removal.
  EXPECT_TRUE(value->Remove(kNullKey));
  EXPECT_FALSE(value->HasKey(kNullKey));

  EXPECT_TRUE(value->Remove(kBoolKey));
  EXPECT_FALSE(value->HasKey(kBoolKey));

  EXPECT_TRUE(value->Remove(kIntKey));
  EXPECT_FALSE(value->HasKey(kIntKey));

  EXPECT_TRUE(value->Remove(kDoubleKey));
  EXPECT_FALSE(value->HasKey(kDoubleKey));

  EXPECT_TRUE(value->Remove(kStringKey));
  EXPECT_FALSE(value->HasKey(kStringKey));

  EXPECT_TRUE(value->Remove(kBinaryKey));
  EXPECT_FALSE(value->HasKey(kBinaryKey));
  EXPECT_FALSE(binary_value->IsValid());  // Value should be detached

  EXPECT_TRUE(value->Remove(kDictionaryKey));
  EXPECT_FALSE(value->HasKey(kDictionaryKey));
  EXPECT_FALSE(dictionary_value->IsValid());  // Value should be detached

  EXPECT_TRUE(value->Remove(kListKey));
  EXPECT_FALSE(value->HasKey(kListKey));
  EXPECT_FALSE(list_value->IsValid());  // Value should be detached

  // Test the size.
  EXPECT_EQ(0U, value->GetSize());

  // Re-add some values.
  TestDictionaryNull(value);
  TestDictionaryBool(value);
  TestDictionaryDictionary(value, dictionary_value);

  // Test the size.
  EXPECT_EQ(3U, value->GetSize());

  // Clear the values.
  EXPECT_TRUE(value->Clear());
  EXPECT_EQ(0U, value->GetSize());
  EXPECT_FALSE(dictionary_value->IsValid());  // Value should be detached
}

// Used to test access of dictionary data on a different thread.
class DictionaryTask : public HoneycombTask {
 public:
  DictionaryTask(HoneycombRefPtr<HoneycombDictionaryValue> value,
                 char* binary_data,
                 size_t binary_data_size)
      : value_(value),
        binary_data_(binary_data),
        binary_data_size_(binary_data_size) {}

  void Execute() override {
    TestDictionary(value_, binary_data_, binary_data_size_);
  }

 private:
  HoneycombRefPtr<HoneycombDictionaryValue> value_;
  char* binary_data_;
  size_t binary_data_size_;

  IMPLEMENT_REFCOUNTING(DictionaryTask);
};

// LIST TEST HELPERS

// Test list null value.
void TestListNull(HoneycombRefPtr<HoneycombListValue> value, size_t index) {
  HoneycombValueType type = value->GetType(index);
  EXPECT_TRUE(type == VTYPE_INVALID || type == VTYPE_NULL);

  EXPECT_TRUE(value->SetNull(index));
  EXPECT_EQ(VTYPE_NULL, value->GetType(index));
}

// Test list bool value.
void TestListBool(HoneycombRefPtr<HoneycombListValue> value, size_t index) {
  HoneycombValueType type = value->GetType(index);
  EXPECT_TRUE(type == VTYPE_INVALID || type == VTYPE_NULL);

  EXPECT_TRUE(value->SetBool(index, kBoolValue));
  EXPECT_EQ(VTYPE_BOOL, value->GetType(index));
  EXPECT_EQ(kBoolValue, value->GetBool(index));
}

// Test list int value.
void TestListInt(HoneycombRefPtr<HoneycombListValue> value, size_t index) {
  HoneycombValueType type = value->GetType(index);
  EXPECT_TRUE(type == VTYPE_INVALID || type == VTYPE_NULL);

  EXPECT_TRUE(value->SetInt(index, kIntValue));
  EXPECT_EQ(VTYPE_INT, value->GetType(index));
  EXPECT_EQ(kIntValue, value->GetInt(index));
}

// Test list double value.
void TestListDouble(HoneycombRefPtr<HoneycombListValue> value, size_t index) {
  HoneycombValueType type = value->GetType(index);
  EXPECT_TRUE(type == VTYPE_INVALID || type == VTYPE_NULL);

  EXPECT_TRUE(value->SetDouble(index, kDoubleValue));
  EXPECT_EQ(VTYPE_DOUBLE, value->GetType(index));
  EXPECT_EQ(kDoubleValue, value->GetDouble(index));
}

// Test list string value.
void TestListString(HoneycombRefPtr<HoneycombListValue> value, size_t index) {
  HoneycombValueType type = value->GetType(index);
  EXPECT_TRUE(type == VTYPE_INVALID || type == VTYPE_NULL);

  EXPECT_TRUE(value->SetString(index, kStringValue));
  EXPECT_EQ(VTYPE_STRING, value->GetType(index));
  EXPECT_EQ(kStringValue, value->GetString(index).ToString());
}

// Test list binary value.
void TestListBinary(HoneycombRefPtr<HoneycombListValue> value,
                    size_t index,
                    char* binary_data,
                    size_t binary_data_size,
                    HoneycombRefPtr<HoneycombBinaryValue>& binary_value) {
  binary_value = HoneycombBinaryValue::Create(binary_data, binary_data_size);
  EXPECT_TRUE(binary_value.get());
  EXPECT_TRUE(binary_value->IsValid());
  EXPECT_FALSE(binary_value->IsOwned());

  HoneycombValueType type = value->GetType(index);
  EXPECT_TRUE(type == VTYPE_INVALID || type == VTYPE_NULL);

  EXPECT_TRUE(value->SetBinary(index, binary_value));
  EXPECT_FALSE(binary_value->IsValid());  // Value should be detached
  EXPECT_EQ(VTYPE_BINARY, value->GetType(index));
  binary_value = value->GetBinary(index);
  EXPECT_TRUE(binary_value.get());
  EXPECT_TRUE(binary_value->IsValid());
  EXPECT_TRUE(binary_value->IsOwned());
  TestBinary(binary_value, binary_data, binary_data_size);
}

// Test list dictionary value.
void TestListDictionary(HoneycombRefPtr<HoneycombListValue> value,
                        size_t index,
                        HoneycombRefPtr<HoneycombDictionaryValue>& dictionary_value) {
  dictionary_value = HoneycombDictionaryValue::Create();
  EXPECT_TRUE(dictionary_value.get());
  EXPECT_TRUE(dictionary_value->IsValid());
  EXPECT_FALSE(dictionary_value->IsOwned());
  EXPECT_FALSE(dictionary_value->IsReadOnly());
  EXPECT_TRUE(dictionary_value->SetInt(kIntKey, kIntValue));
  EXPECT_EQ(1U, dictionary_value->GetSize());

  HoneycombValueType type = value->GetType(index);
  EXPECT_TRUE(type == VTYPE_INVALID || type == VTYPE_NULL);

  EXPECT_TRUE(value->SetDictionary(index, dictionary_value));
  EXPECT_FALSE(dictionary_value->IsValid());  // Value should be detached
  EXPECT_EQ(VTYPE_DICTIONARY, value->GetType(index));
  dictionary_value = value->GetDictionary(index);
  EXPECT_TRUE(dictionary_value.get());
  EXPECT_TRUE(dictionary_value->IsValid());
  EXPECT_TRUE(dictionary_value->IsOwned());
  EXPECT_FALSE(dictionary_value->IsReadOnly());
  EXPECT_EQ(1U, dictionary_value->GetSize());
  EXPECT_EQ(kIntValue, dictionary_value->GetInt(kIntKey));
}

// Test list list value.
void TestListList(HoneycombRefPtr<HoneycombListValue> value,
                  size_t index,
                  HoneycombRefPtr<HoneycombListValue>& list_value) {
  list_value = HoneycombListValue::Create();
  EXPECT_TRUE(list_value.get());
  EXPECT_TRUE(list_value->IsValid());
  EXPECT_FALSE(list_value->IsOwned());
  EXPECT_FALSE(list_value->IsReadOnly());
  EXPECT_TRUE(list_value->SetInt(0, kIntValue));
  EXPECT_EQ(1U, list_value->GetSize());

  HoneycombValueType type = value->GetType(index);
  EXPECT_TRUE(type == VTYPE_INVALID || type == VTYPE_NULL);

  EXPECT_TRUE(value->SetList(index, list_value));
  EXPECT_FALSE(list_value->IsValid());  // Value should be detached
  EXPECT_EQ(VTYPE_LIST, value->GetType(index));
  list_value = value->GetList(index);
  EXPECT_TRUE(list_value.get());
  EXPECT_TRUE(list_value->IsValid());
  EXPECT_TRUE(list_value->IsOwned());
  EXPECT_FALSE(list_value->IsReadOnly());
  EXPECT_EQ(1U, list_value->GetSize());
  EXPECT_EQ(kIntValue, list_value->GetInt(0));
}

// Test list value.
void TestList(HoneycombRefPtr<HoneycombListValue> value,
              char* binary_data,
              size_t binary_data_size) {
  HoneycombRefPtr<HoneycombBinaryValue> binary_value;
  HoneycombRefPtr<HoneycombDictionaryValue> dictionary_value;
  HoneycombRefPtr<HoneycombListValue> list_value;

  // Test the size.
  EXPECT_EQ(0U, value->GetSize());

  // Set the size.
  EXPECT_TRUE(value->SetSize(8));
  EXPECT_EQ(8U, value->GetSize());

  EXPECT_EQ(VTYPE_NULL, value->GetType(kNullIndex));
  TestListNull(value, kNullIndex);
  EXPECT_EQ(VTYPE_NULL, value->GetType(kBoolIndex));
  TestListBool(value, kBoolIndex);
  EXPECT_EQ(VTYPE_NULL, value->GetType(kIntIndex));
  TestListInt(value, kIntIndex);
  EXPECT_EQ(VTYPE_NULL, value->GetType(kDoubleIndex));
  TestListDouble(value, kDoubleIndex);
  EXPECT_EQ(VTYPE_NULL, value->GetType(kStringIndex));
  TestListString(value, kStringIndex);
  EXPECT_EQ(VTYPE_NULL, value->GetType(kBinaryIndex));
  TestListBinary(value, kBinaryIndex, binary_data, binary_data_size,
                 binary_value);
  EXPECT_EQ(VTYPE_NULL, value->GetType(kDictionaryIndex));
  TestListDictionary(value, kDictionaryIndex, dictionary_value);
  EXPECT_EQ(VTYPE_NULL, value->GetType(kListIndex));
  TestListList(value, kListIndex, list_value);

  // Test the size.
  EXPECT_EQ(8U, value->GetSize());

  // Test various operations with invalid index.
  EXPECT_FALSE(list_value->Remove(9U));
  EXPECT_TRUE(list_value->GetType(10U) == VTYPE_INVALID);
  EXPECT_FALSE(list_value->GetValue(11U).get() != nullptr &&
               list_value->GetValue(11U)->IsValid());

  // Test copy.
  HoneycombRefPtr<HoneycombListValue> copy = value->Copy();
  TestListEqual(value, copy);

  // Test removal (in reverse order so indexes stay valid).
  EXPECT_TRUE(value->Remove(kListIndex));
  EXPECT_EQ(7U, value->GetSize());
  EXPECT_FALSE(list_value->IsValid());  // Value should be detached

  EXPECT_TRUE(value->Remove(kDictionaryIndex));
  EXPECT_EQ(6U, value->GetSize());
  EXPECT_FALSE(dictionary_value->IsValid());  // Value should be detached

  EXPECT_TRUE(value->Remove(kBinaryIndex));
  EXPECT_EQ(5U, value->GetSize());
  EXPECT_FALSE(binary_value->IsValid());  // Value should be detached

  EXPECT_TRUE(value->Remove(kStringIndex));
  EXPECT_EQ(4U, value->GetSize());

  EXPECT_TRUE(value->Remove(kDoubleIndex));
  EXPECT_EQ(3U, value->GetSize());

  EXPECT_TRUE(value->Remove(kIntIndex));
  EXPECT_EQ(2U, value->GetSize());

  EXPECT_TRUE(value->Remove(kBoolIndex));
  EXPECT_EQ(1U, value->GetSize());

  EXPECT_TRUE(value->Remove(kNullIndex));
  EXPECT_EQ(0U, value->GetSize());

  // Re-add some values.
  EXPECT_EQ(VTYPE_INVALID, value->GetType(0));
  TestListNull(value, 0);
  EXPECT_EQ(VTYPE_INVALID, value->GetType(1));
  TestListBool(value, 1);
  EXPECT_EQ(VTYPE_INVALID, value->GetType(2));
  TestListList(value, 2, list_value);

  // Test the size.
  EXPECT_EQ(3U, value->GetSize());

  // Clear the values.
  EXPECT_TRUE(value->Clear());
  EXPECT_EQ(0U, value->GetSize());
  EXPECT_FALSE(list_value->IsValid());  // Value should be detached

  // Add some values in random order.
  EXPECT_EQ(VTYPE_INVALID, value->GetType(2));
  TestListInt(value, 2);
  EXPECT_EQ(VTYPE_NULL, value->GetType(0));
  TestListBool(value, 0);
  EXPECT_EQ(VTYPE_NULL, value->GetType(1));
  TestListList(value, 1, list_value);

  EXPECT_EQ(VTYPE_BOOL, value->GetType(0));
  EXPECT_EQ(VTYPE_LIST, value->GetType(1));
  EXPECT_EQ(VTYPE_INT, value->GetType(2));

  // Test the size.
  EXPECT_EQ(3U, value->GetSize());

  // Clear some values.
  EXPECT_TRUE(value->SetSize(1));
  EXPECT_EQ(1U, value->GetSize());
  EXPECT_FALSE(list_value->IsValid());  // Value should be detached

  EXPECT_EQ(VTYPE_BOOL, value->GetType(0));
  EXPECT_EQ(VTYPE_INVALID, value->GetType(1));
  EXPECT_EQ(VTYPE_INVALID, value->GetType(2));

  // Clear all values.
  EXPECT_TRUE(value->Clear());
  EXPECT_EQ(0U, value->GetSize());
}

// Used to test access of list data on a different thread.
class ListTask : public HoneycombTask {
 public:
  ListTask(HoneycombRefPtr<HoneycombListValue> value,
           char* binary_data,
           size_t binary_data_size)
      : value_(value),
        binary_data_(binary_data),
        binary_data_size_(binary_data_size) {}

  void Execute() override { TestList(value_, binary_data_, binary_data_size_); }

 private:
  HoneycombRefPtr<HoneycombListValue> value_;
  char* binary_data_;
  size_t binary_data_size_;

  IMPLEMENT_REFCOUNTING(ListTask);
};

void CreateAndCompareCopy(HoneycombRefPtr<HoneycombValue> value) {
  HoneycombRefPtr<HoneycombValue> value2 = value->Copy();
  EXPECT_TRUE(value->IsEqual(value));
  EXPECT_TRUE(value->IsSame(value));
  EXPECT_TRUE(value2->IsEqual(value2));
  EXPECT_TRUE(value2->IsSame(value2));
  EXPECT_TRUE(value->IsEqual(value2));
  EXPECT_FALSE(value->IsSame(value2));
  EXPECT_TRUE(value2->IsEqual(value));
  EXPECT_FALSE(value2->IsSame(value));
}

HoneycombRefPtr<HoneycombBinaryValue> CreateBinaryValue() {
  char binary_data[] = "This is my test data";
  const size_t binary_data_size = sizeof(binary_data) - 1;

  HoneycombRefPtr<HoneycombBinaryValue> binary_value =
      HoneycombBinaryValue::Create(binary_data, binary_data_size);
  EXPECT_TRUE(binary_value.get());
  EXPECT_TRUE(binary_value->IsValid());
  EXPECT_FALSE(binary_value->IsOwned());
  TestBinary(binary_value, binary_data, binary_data_size);
  return binary_value;
}

HoneycombRefPtr<HoneycombListValue> CreateListValue() {
  HoneycombRefPtr<HoneycombListValue> list_value = HoneycombListValue::Create();
  EXPECT_TRUE(list_value.get());
  EXPECT_TRUE(list_value->IsValid());
  EXPECT_FALSE(list_value->IsOwned());
  EXPECT_FALSE(list_value->IsReadOnly());
  EXPECT_TRUE(list_value->SetInt(0, kIntValue));
  EXPECT_TRUE(list_value->SetInt(1, kDoubleValue));
  return list_value;
}

const char* kKey1 = "key1";
const char* kKey2 = "key2";

HoneycombRefPtr<HoneycombDictionaryValue> CreateDictionaryValue() {
  // Create the dictionary.
  HoneycombRefPtr<HoneycombDictionaryValue> dict_value = HoneycombDictionaryValue::Create();
  EXPECT_TRUE(dict_value.get());
  EXPECT_TRUE(dict_value->IsValid());
  EXPECT_FALSE(dict_value->IsOwned());
  EXPECT_FALSE(dict_value->IsReadOnly());
  EXPECT_TRUE(dict_value->SetInt(kKey1, kIntValue));
  EXPECT_TRUE(dict_value->SetInt(kKey2, kDoubleValue));
  return dict_value;
}

}  // namespace

// Test binary value access.
TEST(ValuesTest, BinaryAccess) {
  char data[] = "This is my test data";

  HoneycombRefPtr<HoneycombBinaryValue> value =
      HoneycombBinaryValue::Create(data, sizeof(data) - 1);
  EXPECT_TRUE(value.get());
  EXPECT_TRUE(value->IsValid());
  EXPECT_FALSE(value->IsOwned());

  // Test on this thread.
  TestBinary(value, data, sizeof(data) - 1);
}

// Test binary value access on a different thread.
TEST(ValuesTest, BinaryAccessOtherThread) {
  char data[] = "This is my test data";

  HoneycombRefPtr<HoneycombBinaryValue> value =
      HoneycombBinaryValue::Create(data, sizeof(data) - 1);
  EXPECT_TRUE(value.get());
  EXPECT_TRUE(value->IsValid());
  EXPECT_FALSE(value->IsOwned());

  // Test on a different thread.
  HoneycombPostTask(TID_UI, new BinaryTask(value, data, sizeof(data) - 1));
  WaitForUIThread();
}

// Test dictionary value access.
TEST(ValuesTest, DictionaryAccess) {
  HoneycombRefPtr<HoneycombDictionaryValue> value = HoneycombDictionaryValue::Create();
  EXPECT_TRUE(value.get());
  EXPECT_TRUE(value->IsValid());
  EXPECT_FALSE(value->IsOwned());
  EXPECT_FALSE(value->IsReadOnly());

  char binary_data[] = "This is my test data";

  // Test on this thread.
  TestDictionary(value, binary_data, sizeof(binary_data) - 1);
}

// Test dictionary value access on a different thread.
TEST(ValuesTest, DictionaryAccessOtherThread) {
  HoneycombRefPtr<HoneycombDictionaryValue> value = HoneycombDictionaryValue::Create();
  EXPECT_TRUE(value.get());
  EXPECT_TRUE(value->IsValid());
  EXPECT_FALSE(value->IsOwned());
  EXPECT_FALSE(value->IsReadOnly());

  char binary_data[] = "This is my test data";

  // Test on a different thread.
  HoneycombPostTask(TID_UI,
              new DictionaryTask(value, binary_data, sizeof(binary_data) - 1));
  WaitForUIThread();
}

// Test dictionary value nested detachment
TEST(ValuesTest, DictionaryDetachment) {
  HoneycombRefPtr<HoneycombDictionaryValue> value = HoneycombDictionaryValue::Create();
  EXPECT_TRUE(value.get());
  EXPECT_TRUE(value->IsValid());
  EXPECT_FALSE(value->IsOwned());
  EXPECT_FALSE(value->IsReadOnly());

  HoneycombRefPtr<HoneycombDictionaryValue> dictionary_value = HoneycombDictionaryValue::Create();
  HoneycombRefPtr<HoneycombDictionaryValue> dictionary_value2 =
      HoneycombDictionaryValue::Create();
  HoneycombRefPtr<HoneycombDictionaryValue> dictionary_value3 =
      HoneycombDictionaryValue::Create();

  dictionary_value2->SetDictionary(kDictionaryKey, dictionary_value3);
  EXPECT_FALSE(dictionary_value3->IsValid());
  dictionary_value->SetDictionary(kDictionaryKey, dictionary_value2);
  EXPECT_FALSE(dictionary_value2->IsValid());
  value->SetDictionary(kDictionaryKey, dictionary_value);
  EXPECT_FALSE(dictionary_value->IsValid());

  dictionary_value = value->GetDictionary(kDictionaryKey);
  EXPECT_TRUE(dictionary_value.get());
  EXPECT_TRUE(dictionary_value->IsValid());

  dictionary_value2 = dictionary_value->GetDictionary(kDictionaryKey);
  EXPECT_TRUE(dictionary_value2.get());
  EXPECT_TRUE(dictionary_value2->IsValid());

  dictionary_value3 = dictionary_value2->GetDictionary(kDictionaryKey);
  EXPECT_TRUE(dictionary_value3.get());
  EXPECT_TRUE(dictionary_value3->IsValid());

  EXPECT_TRUE(value->Remove(kDictionaryKey));
  EXPECT_FALSE(dictionary_value->IsValid());
  EXPECT_FALSE(dictionary_value2->IsValid());
  EXPECT_FALSE(dictionary_value3->IsValid());
}

// Test list value access.
TEST(ValuesTest, ListAccess) {
  HoneycombRefPtr<HoneycombListValue> value = HoneycombListValue::Create();
  EXPECT_TRUE(value.get());
  EXPECT_TRUE(value->IsValid());
  EXPECT_FALSE(value->IsOwned());
  EXPECT_FALSE(value->IsReadOnly());

  char binary_data[] = "This is my test data";

  // Test on this thread.
  TestList(value, binary_data, sizeof(binary_data) - 1);
}

// Test list value access on a different thread.
TEST(ValuesTest, ListAccessOtherThread) {
  HoneycombRefPtr<HoneycombListValue> value = HoneycombListValue::Create();
  EXPECT_TRUE(value.get());
  EXPECT_TRUE(value->IsValid());
  EXPECT_FALSE(value->IsOwned());
  EXPECT_FALSE(value->IsReadOnly());

  char binary_data[] = "This is my test data";

  // Test on a different thread.
  HoneycombPostTask(TID_UI,
              new ListTask(value, binary_data, sizeof(binary_data) - 1));
  WaitForUIThread();
}

// Test list value nested detachment
TEST(ValuesTest, ListDetachment) {
  HoneycombRefPtr<HoneycombListValue> value = HoneycombListValue::Create();
  EXPECT_TRUE(value.get());
  EXPECT_TRUE(value->IsValid());
  EXPECT_FALSE(value->IsOwned());
  EXPECT_FALSE(value->IsReadOnly());

  HoneycombRefPtr<HoneycombListValue> list_value = HoneycombListValue::Create();
  HoneycombRefPtr<HoneycombListValue> list_value2 = HoneycombListValue::Create();
  HoneycombRefPtr<HoneycombListValue> list_value3 = HoneycombListValue::Create();

  list_value2->SetList(0, list_value3);
  EXPECT_FALSE(list_value3->IsValid());
  list_value->SetList(0, list_value2);
  EXPECT_FALSE(list_value2->IsValid());
  value->SetList(0, list_value);
  EXPECT_FALSE(list_value->IsValid());

  list_value = value->GetList(0);
  EXPECT_TRUE(list_value.get());
  EXPECT_TRUE(list_value->IsValid());

  list_value2 = list_value->GetList(0);
  EXPECT_TRUE(list_value2.get());
  EXPECT_TRUE(list_value2->IsValid());

  list_value3 = list_value2->GetList(0);
  EXPECT_TRUE(list_value3.get());
  EXPECT_TRUE(list_value3->IsValid());

  EXPECT_TRUE(value->Remove(0));
  EXPECT_FALSE(list_value->IsValid());
  EXPECT_FALSE(list_value2->IsValid());
  EXPECT_FALSE(list_value3->IsValid());
}

// Test get/set of a HoneycombValue simple types.
TEST(ValuesTest, ValueSimple) {
  HoneycombRefPtr<HoneycombValue> value = HoneycombValue::Create();
  EXPECT_TRUE(value.get());
  EXPECT_TRUE(value->IsValid());
  EXPECT_FALSE(value->IsReadOnly());
  EXPECT_FALSE(value->IsOwned());
  EXPECT_EQ(VTYPE_NULL, value->GetType());
  CreateAndCompareCopy(value);

  EXPECT_TRUE(value->SetBool(true));
  EXPECT_EQ(VTYPE_BOOL, value->GetType());
  EXPECT_TRUE(value->GetBool());
  EXPECT_TRUE(value->IsValid());
  EXPECT_FALSE(value->IsReadOnly());
  EXPECT_FALSE(value->IsOwned());
  CreateAndCompareCopy(value);

  EXPECT_TRUE(value->SetBool(false));
  EXPECT_EQ(VTYPE_BOOL, value->GetType());
  EXPECT_FALSE(value->GetBool());
  EXPECT_TRUE(value->IsValid());
  EXPECT_FALSE(value->IsReadOnly());
  EXPECT_FALSE(value->IsOwned());
  CreateAndCompareCopy(value);

  EXPECT_TRUE(value->SetInt(3));
  EXPECT_EQ(VTYPE_INT, value->GetType());
  EXPECT_EQ(3, value->GetInt());
  EXPECT_TRUE(value->IsValid());
  EXPECT_FALSE(value->IsReadOnly());
  EXPECT_FALSE(value->IsOwned());
  CreateAndCompareCopy(value);

  EXPECT_TRUE(value->SetDouble(5.665));
  EXPECT_EQ(VTYPE_DOUBLE, value->GetType());
  EXPECT_EQ(5.665, value->GetDouble());
  EXPECT_TRUE(value->IsValid());
  EXPECT_FALSE(value->IsReadOnly());
  EXPECT_FALSE(value->IsOwned());
  CreateAndCompareCopy(value);

  const char* str = "Test string";
  EXPECT_TRUE(value->SetString(str));
  EXPECT_EQ(VTYPE_STRING, value->GetType());
  EXPECT_STREQ(str, value->GetString().ToString().data());
  EXPECT_TRUE(value->IsValid());
  EXPECT_FALSE(value->IsReadOnly());
  EXPECT_FALSE(value->IsOwned());
  CreateAndCompareCopy(value);

  EXPECT_TRUE(value->SetNull());
  EXPECT_EQ(VTYPE_NULL, value->GetType());
  EXPECT_TRUE(value->IsValid());
  EXPECT_FALSE(value->IsReadOnly());
  EXPECT_FALSE(value->IsOwned());
  CreateAndCompareCopy(value);
}

// Test association of a HoneycombValue simple type with a HoneycombListValue.
TEST(ValuesTest, ValueSimpleToList) {
  const double double_value = 5.665;

  HoneycombRefPtr<HoneycombValue> value = HoneycombValue::Create();
  EXPECT_TRUE(value->SetDouble(double_value));

  // Add the value to the target list.
  HoneycombRefPtr<HoneycombListValue> target_list = HoneycombListValue::Create();
  EXPECT_TRUE(target_list->SetValue(0, value));

  // Test the value in the target list.
  EXPECT_EQ(VTYPE_DOUBLE, target_list->GetType(0));
  EXPECT_EQ(double_value, target_list->GetDouble(0));

  // Get the value from the target list.
  HoneycombRefPtr<HoneycombValue> value2 = target_list->GetValue(0);
  EXPECT_TRUE(value2.get());
  EXPECT_FALSE(value2->IsOwned());
  EXPECT_FALSE(value2->IsReadOnly());
  EXPECT_EQ(VTYPE_DOUBLE, value2->GetType());
  EXPECT_EQ(double_value, value2->GetDouble());

  // Values are equal but not the same.
  EXPECT_TRUE(value->IsEqual(value2));
  EXPECT_TRUE(value2->IsEqual(value));
  EXPECT_FALSE(value->IsSame(value2));
  EXPECT_FALSE(value2->IsSame(value));

  // Change the value in the target list.
  EXPECT_TRUE(target_list->SetInt(0, 5));
  EXPECT_EQ(VTYPE_INT, target_list->GetType(0));
  EXPECT_EQ(5, target_list->GetInt(0));

  // The other values are still valid.
  EXPECT_TRUE(value->IsValid());
  EXPECT_TRUE(value2->IsValid());
}

// Test association of a HoneycombValue simple type with a HoneycombDictionaryValue.
TEST(ValuesTest, ValueSimpleToDictionary) {
  const double double_value = 5.665;
  const char* key = "key";

  HoneycombRefPtr<HoneycombValue> value = HoneycombValue::Create();
  EXPECT_TRUE(value->SetDouble(double_value));

  // Add the value to the target dictionary.
  HoneycombRefPtr<HoneycombDictionaryValue> target_dict = HoneycombDictionaryValue::Create();
  EXPECT_TRUE(target_dict->SetValue(key, value));

  // Test the value in the target dictionary.
  EXPECT_EQ(VTYPE_DOUBLE, target_dict->GetType(key));
  EXPECT_EQ(double_value, target_dict->GetDouble(key));

  // Get the value from the target dictionary.
  HoneycombRefPtr<HoneycombValue> value2 = target_dict->GetValue(key);
  EXPECT_TRUE(value2.get());
  EXPECT_FALSE(value2->IsOwned());
  EXPECT_FALSE(value2->IsReadOnly());
  EXPECT_EQ(VTYPE_DOUBLE, value2->GetType());
  EXPECT_EQ(double_value, value2->GetDouble());

  // Values are equal but not the same.
  EXPECT_TRUE(value->IsEqual(value2));
  EXPECT_TRUE(value2->IsEqual(value));
  EXPECT_FALSE(value->IsSame(value2));
  EXPECT_FALSE(value2->IsSame(value));

  // Change the value in the target dictionary.
  EXPECT_TRUE(target_dict->SetInt(key, 5));
  EXPECT_EQ(VTYPE_INT, target_dict->GetType(key));
  EXPECT_EQ(5, target_dict->GetInt(key));

  // The other values are still valid.
  EXPECT_TRUE(value->IsValid());
  EXPECT_TRUE(value2->IsValid());
}

// Test get/set of a HoneycombValue binary type.
TEST(ValuesTest, ValueBinary) {
  // Create the binary.
  HoneycombRefPtr<HoneycombBinaryValue> binary_value = CreateBinaryValue();

  // Create the value.
  HoneycombRefPtr<HoneycombValue> value = HoneycombValue::Create();
  EXPECT_TRUE(value->SetBinary(binary_value));
  EXPECT_EQ(VTYPE_BINARY, value->GetType());
  EXPECT_TRUE(value->IsValid());
  EXPECT_TRUE(value->IsReadOnly());  // Binary values are always read-only.
  EXPECT_FALSE(value->IsOwned());
  CreateAndCompareCopy(value);

  // Get the binary reference from the value.
  HoneycombRefPtr<HoneycombBinaryValue> binary_value2 = value->GetBinary();
  EXPECT_TRUE(binary_value2.get());
  EXPECT_TRUE(binary_value2->IsValid());
  EXPECT_FALSE(binary_value2->IsOwned());

  // The binaries are the same and equal.
  TestBinaryEqual(binary_value, binary_value2);
  EXPECT_TRUE(binary_value->IsSame(binary_value2));
  EXPECT_TRUE(binary_value2->IsSame(binary_value));
}

// Test association of a HoneycombValue binary with a HoneycombListValue.
TEST(ValuesTest, ValueBinaryToList) {
  // Create the binary.
  HoneycombRefPtr<HoneycombBinaryValue> binary_value = CreateBinaryValue();

  // Add the binary to a value.
  HoneycombRefPtr<HoneycombValue> value = HoneycombValue::Create();
  EXPECT_TRUE(value->SetBinary(binary_value));

  // Add the value to the target list.
  HoneycombRefPtr<HoneycombListValue> target_list = HoneycombListValue::Create();
  EXPECT_TRUE(target_list->SetValue(0, value));

  // The binary value is now owned by the target list.
  EXPECT_FALSE(binary_value->IsValid());

  // The value is still valid and points to the new reference list.
  EXPECT_TRUE(value->IsValid());

  HoneycombRefPtr<HoneycombBinaryValue> binary_value2 = value->GetBinary();
  HoneycombRefPtr<HoneycombBinaryValue> binary_value3 = target_list->GetBinary(0);
  HoneycombRefPtr<HoneycombValue> value2 = target_list->GetValue(0);
  HoneycombRefPtr<HoneycombBinaryValue> binary_value4 = value2->GetBinary();

  // All values are owned by the target list.
  EXPECT_TRUE(value->IsOwned());
  EXPECT_TRUE(value2->IsOwned());
  EXPECT_TRUE(binary_value2->IsOwned());
  EXPECT_TRUE(binary_value3->IsOwned());
  EXPECT_TRUE(binary_value4->IsOwned());

  // All values are the same.
  EXPECT_TRUE(binary_value2->IsSame(binary_value3));
  TestBinaryEqual(binary_value2, binary_value3);
  EXPECT_TRUE(binary_value2->IsSame(binary_value4));
  TestBinaryEqual(binary_value2, binary_value4);

  // Change the value to something else.
  EXPECT_TRUE(target_list->SetInt(0, kIntValue));
  EXPECT_EQ(VTYPE_INT, target_list->GetType(0));
  EXPECT_EQ(kIntValue, target_list->GetInt(0));

  // Now the references are invalid.
  EXPECT_FALSE(value->IsValid());
  EXPECT_FALSE(value2->IsValid());
  EXPECT_FALSE(binary_value2->IsValid());
  EXPECT_FALSE(binary_value3->IsValid());
  EXPECT_FALSE(binary_value4->IsValid());

  // Verify that adding a binary to a list directly invalidates both the binary
  // and the value that references it.
  binary_value = CreateBinaryValue();
  value = HoneycombValue::Create();
  EXPECT_TRUE(value->SetBinary(binary_value));
  target_list->SetBinary(0, binary_value);
  EXPECT_FALSE(binary_value->IsValid());
  EXPECT_FALSE(value->IsValid());
}

// Test association of a HoneycombValue binary with a HoneycombDictionaryValue.
TEST(ValuesTest, ValueBinaryToDictionary) {
  const char* key = "key";

  // Create the binary.
  HoneycombRefPtr<HoneycombBinaryValue> binary_value = CreateBinaryValue();

  // Add the binary to a value.
  HoneycombRefPtr<HoneycombValue> value = HoneycombValue::Create();
  EXPECT_TRUE(value->SetBinary(binary_value));

  // Add the value to the target dictionary.
  HoneycombRefPtr<HoneycombDictionaryValue> target_dict = HoneycombDictionaryValue::Create();
  EXPECT_TRUE(target_dict->SetValue(key, value));

  // The list value is now owned by the target dictionary.
  EXPECT_FALSE(binary_value->IsValid());

  // The value is still valid and points to the new reference list.
  EXPECT_TRUE(value->IsValid());

  HoneycombRefPtr<HoneycombBinaryValue> binary_value2 = value->GetBinary();
  HoneycombRefPtr<HoneycombBinaryValue> binary_value3 = target_dict->GetBinary(key);
  HoneycombRefPtr<HoneycombValue> value2 = target_dict->GetValue(key);
  HoneycombRefPtr<HoneycombBinaryValue> binary_value4 = value2->GetBinary();

  // All values are owned by the target dictionary.
  EXPECT_TRUE(value->IsOwned());
  EXPECT_TRUE(value2->IsOwned());
  EXPECT_TRUE(binary_value2->IsOwned());
  EXPECT_TRUE(binary_value3->IsOwned());
  EXPECT_TRUE(binary_value4->IsOwned());

  // All values are the same.
  EXPECT_TRUE(binary_value2->IsSame(binary_value3));
  TestBinaryEqual(binary_value2, binary_value3);
  EXPECT_TRUE(binary_value2->IsSame(binary_value4));
  TestBinaryEqual(binary_value2, binary_value4);

  // Change the value to something else.
  EXPECT_TRUE(target_dict->SetInt(key, kIntValue));
  EXPECT_EQ(VTYPE_INT, target_dict->GetType(key));
  EXPECT_EQ(kIntValue, target_dict->GetInt(key));

  // Now the references are invalid.
  EXPECT_FALSE(value->IsValid());
  EXPECT_FALSE(value2->IsValid());
  EXPECT_FALSE(binary_value2->IsValid());
  EXPECT_FALSE(binary_value3->IsValid());
  EXPECT_FALSE(binary_value4->IsValid());

  // Verify that adding a binary to a dictionary directly invalidates both the
  // binary and the value that references it.
  binary_value = CreateBinaryValue();
  value = HoneycombValue::Create();
  EXPECT_TRUE(value->SetBinary(binary_value));
  target_dict->SetBinary(key, binary_value);
  EXPECT_FALSE(binary_value->IsValid());
  EXPECT_FALSE(value->IsValid());
}

// Test get/set of a HoneycombValue list type.
TEST(ValuesTest, ValueList) {
  // Create the list.
  HoneycombRefPtr<HoneycombListValue> list_value = CreateListValue();

  // Create the value.
  HoneycombRefPtr<HoneycombValue> value = HoneycombValue::Create();
  EXPECT_TRUE(value->SetList(list_value));
  EXPECT_EQ(VTYPE_LIST, value->GetType());
  EXPECT_TRUE(value->IsValid());
  EXPECT_FALSE(value->IsReadOnly());
  EXPECT_FALSE(value->IsOwned());
  CreateAndCompareCopy(value);

  // Get the list reference from the value.
  HoneycombRefPtr<HoneycombListValue> list_value2 = value->GetList();
  EXPECT_TRUE(list_value2.get());
  EXPECT_TRUE(list_value2->IsValid());
  EXPECT_FALSE(list_value2->IsOwned());
  EXPECT_FALSE(list_value2->IsReadOnly());

  // The lists are the same and equal.
  TestListEqual(list_value, list_value2);
  EXPECT_TRUE(list_value->IsSame(list_value2));
  EXPECT_TRUE(list_value2->IsSame(list_value));

  // Change a value in one list and verify that it's changed in the other list.
  EXPECT_TRUE(list_value->SetString(0, kStringValue));
  EXPECT_EQ(VTYPE_STRING, list_value->GetType(0));
  EXPECT_STREQ(kStringValue, list_value->GetString(0).ToString().data());
}

// Test association of a HoneycombValue list with a HoneycombListValue.
TEST(ValuesTest, ValueListToList) {
  // Create the list.
  HoneycombRefPtr<HoneycombListValue> list_value = CreateListValue();

  // Add the list to a value.
  HoneycombRefPtr<HoneycombValue> value = HoneycombValue::Create();
  EXPECT_TRUE(value->SetList(list_value));

  // Add the value to the target list.
  HoneycombRefPtr<HoneycombListValue> target_list = HoneycombListValue::Create();
  EXPECT_TRUE(target_list->SetValue(0, value));

  // The list value is now owned by the target list.
  EXPECT_FALSE(list_value->IsValid());

  // The value is still valid and points to the new reference list.
  EXPECT_TRUE(value->IsValid());

  HoneycombRefPtr<HoneycombListValue> list_value2 = value->GetList();
  HoneycombRefPtr<HoneycombListValue> list_value3 = target_list->GetList(0);
  HoneycombRefPtr<HoneycombValue> value2 = target_list->GetValue(0);
  HoneycombRefPtr<HoneycombListValue> list_value4 = value2->GetList();

  // All values are owned by the target list.
  EXPECT_TRUE(value->IsOwned());
  EXPECT_TRUE(value2->IsOwned());
  EXPECT_TRUE(list_value2->IsOwned());
  EXPECT_TRUE(list_value3->IsOwned());
  EXPECT_TRUE(list_value4->IsOwned());

  // All values are the same.
  EXPECT_TRUE(list_value2->IsSame(list_value3));
  TestListEqual(list_value2, list_value3);
  EXPECT_TRUE(list_value2->IsSame(list_value4));
  TestListEqual(list_value2, list_value4);

  // Change the value to something else.
  EXPECT_TRUE(target_list->SetInt(0, kIntValue));
  EXPECT_EQ(VTYPE_INT, target_list->GetType(0));
  EXPECT_EQ(kIntValue, target_list->GetInt(0));

  // Now the references are invalid.
  EXPECT_FALSE(value->IsValid());
  EXPECT_FALSE(value2->IsValid());
  EXPECT_FALSE(list_value2->IsValid());
  EXPECT_FALSE(list_value3->IsValid());
  EXPECT_FALSE(list_value4->IsValid());

  // Verify that adding a list to a list directly invalidates both the list
  // and the value that references it.
  list_value = CreateListValue();
  value = HoneycombValue::Create();
  EXPECT_TRUE(value->SetList(list_value));
  target_list->SetList(0, list_value);
  EXPECT_FALSE(list_value->IsValid());
  EXPECT_FALSE(value->IsValid());
}

// Test association of a HoneycombValue list with a HoneycombDictionaryValue.
TEST(ValuesTest, ValueListToDictionary) {
  const char* key = "key";

  // Create the list.
  HoneycombRefPtr<HoneycombListValue> list_value = CreateListValue();

  // Add the list to a value.
  HoneycombRefPtr<HoneycombValue> value = HoneycombValue::Create();
  EXPECT_TRUE(value->SetList(list_value));

  // Add the value to the target dictionary.
  HoneycombRefPtr<HoneycombDictionaryValue> target_dict = HoneycombDictionaryValue::Create();
  EXPECT_TRUE(target_dict->SetValue(key, value));

  // The list value is now owned by the target dictionary.
  EXPECT_FALSE(list_value->IsValid());

  // The value is still valid and points to the new reference list.
  EXPECT_TRUE(value->IsValid());

  HoneycombRefPtr<HoneycombListValue> list_value2 = value->GetList();
  HoneycombRefPtr<HoneycombListValue> list_value3 = target_dict->GetList(key);
  HoneycombRefPtr<HoneycombValue> value2 = target_dict->GetValue(key);
  HoneycombRefPtr<HoneycombListValue> list_value4 = value2->GetList();

  // All values are owned by the target dictionary.
  EXPECT_TRUE(value->IsOwned());
  EXPECT_TRUE(value2->IsOwned());
  EXPECT_TRUE(list_value2->IsOwned());
  EXPECT_TRUE(list_value3->IsOwned());
  EXPECT_TRUE(list_value4->IsOwned());

  // All values are the same.
  EXPECT_TRUE(list_value2->IsSame(list_value3));
  TestListEqual(list_value2, list_value3);
  EXPECT_TRUE(list_value2->IsSame(list_value4));
  TestListEqual(list_value2, list_value4);

  // Change the value to something else.
  EXPECT_TRUE(target_dict->SetInt(key, kIntValue));
  EXPECT_EQ(VTYPE_INT, target_dict->GetType(key));
  EXPECT_EQ(kIntValue, target_dict->GetInt(key));

  // Now the references are invalid.
  EXPECT_FALSE(value->IsValid());
  EXPECT_FALSE(value2->IsValid());
  EXPECT_FALSE(list_value2->IsValid());
  EXPECT_FALSE(list_value3->IsValid());
  EXPECT_FALSE(list_value4->IsValid());

  // Verify that adding a list to a dictionary directly invalidates both the
  // list and the value that references it.
  list_value = CreateListValue();
  value = HoneycombValue::Create();
  EXPECT_TRUE(value->SetList(list_value));
  target_dict->SetList(key, list_value);
  EXPECT_FALSE(list_value->IsValid());
  EXPECT_FALSE(value->IsValid());
}

// Test get/set of a HoneycombValue dictionary type.
TEST(ValuesTest, ValueDictionary) {
  // Create the dictionary.
  HoneycombRefPtr<HoneycombDictionaryValue> dict_value = CreateDictionaryValue();

  // Create the value.
  HoneycombRefPtr<HoneycombValue> value = HoneycombValue::Create();
  EXPECT_TRUE(value->SetDictionary(dict_value));
  EXPECT_EQ(VTYPE_DICTIONARY, value->GetType());
  EXPECT_TRUE(value->IsValid());
  EXPECT_FALSE(value->IsReadOnly());
  EXPECT_FALSE(value->IsOwned());
  CreateAndCompareCopy(value);

  // Get the dictionary reference from the value.
  HoneycombRefPtr<HoneycombDictionaryValue> dict_value2 = value->GetDictionary();
  EXPECT_TRUE(dict_value2.get());
  EXPECT_TRUE(dict_value2->IsValid());
  EXPECT_FALSE(dict_value2->IsOwned());
  EXPECT_FALSE(dict_value2->IsReadOnly());

  // The dictionaries are the same and equal.
  TestDictionaryEqual(dict_value, dict_value2);
  EXPECT_TRUE(dict_value->IsSame(dict_value2));
  EXPECT_TRUE(dict_value2->IsSame(dict_value));

  // Change a value in one dictionary and verify that it's changed in the other
  // dictionary.
  EXPECT_TRUE(dict_value->SetString(kKey1, kStringValue));
  EXPECT_EQ(VTYPE_STRING, dict_value->GetType(kKey1));
  EXPECT_STREQ(kStringValue, dict_value->GetString(kKey1).ToString().data());
}

// Test association of a HoneycombValue dictionary with a HoneycombListValue.
TEST(ValuesTest, ValueDictionaryToList) {
  // Create the dictionary.
  HoneycombRefPtr<HoneycombDictionaryValue> dict_value = CreateDictionaryValue();

  // Add the list to a value.
  HoneycombRefPtr<HoneycombValue> value = HoneycombValue::Create();
  EXPECT_TRUE(value->SetDictionary(dict_value));

  // Add the value to the target list.
  HoneycombRefPtr<HoneycombListValue> target_list = HoneycombListValue::Create();
  EXPECT_TRUE(target_list->SetValue(0, value));

  // The list value is now owned by the target list.
  EXPECT_FALSE(dict_value->IsValid());

  // The value is still valid and points to the new reference list.
  EXPECT_TRUE(value->IsValid());

  HoneycombRefPtr<HoneycombDictionaryValue> dict_value2 = value->GetDictionary();
  HoneycombRefPtr<HoneycombDictionaryValue> dict_value3 = target_list->GetDictionary(0);
  HoneycombRefPtr<HoneycombValue> value2 = target_list->GetValue(0);
  HoneycombRefPtr<HoneycombDictionaryValue> dict_value4 = value2->GetDictionary();

  // All values are owned by the target list.
  EXPECT_TRUE(value->IsOwned());
  EXPECT_TRUE(value2->IsOwned());
  EXPECT_TRUE(dict_value2->IsOwned());
  EXPECT_TRUE(dict_value3->IsOwned());
  EXPECT_TRUE(dict_value4->IsOwned());

  // All values are the same.
  EXPECT_TRUE(dict_value2->IsSame(dict_value3));
  TestDictionaryEqual(dict_value2, dict_value3);
  EXPECT_TRUE(dict_value2->IsSame(dict_value4));
  TestDictionaryEqual(dict_value2, dict_value4);

  // Change the value to something else.
  EXPECT_TRUE(target_list->SetInt(0, kIntValue));
  EXPECT_EQ(VTYPE_INT, target_list->GetType(0));
  EXPECT_EQ(kIntValue, target_list->GetInt(0));

  // Now the references are invalid.
  EXPECT_FALSE(value->IsValid());
  EXPECT_FALSE(value2->IsValid());
  EXPECT_FALSE(dict_value2->IsValid());
  EXPECT_FALSE(dict_value3->IsValid());
  EXPECT_FALSE(dict_value4->IsValid());

  // Verify that adding a dictionary to a list directly invalidates both the
  // dictionary and the value that references it.
  dict_value = CreateDictionaryValue();
  value = HoneycombValue::Create();
  EXPECT_TRUE(value->SetDictionary(dict_value));
  target_list->SetDictionary(0, dict_value);
  EXPECT_FALSE(dict_value->IsValid());
  EXPECT_FALSE(value->IsValid());
}

// Test association of a HoneycombValue dictionary with a HoneycombDictionaryValue.
TEST(ValuesTest, ValueDictionaryToDictionary) {
  const char* key = "key";

  // Create the dictionary.
  HoneycombRefPtr<HoneycombDictionaryValue> dict_value = CreateDictionaryValue();

  // Add the list to a value.
  HoneycombRefPtr<HoneycombValue> value = HoneycombValue::Create();
  EXPECT_TRUE(value->SetDictionary(dict_value));

  // Add the value to the target dictionary.
  HoneycombRefPtr<HoneycombDictionaryValue> target_dict = HoneycombDictionaryValue::Create();
  EXPECT_TRUE(target_dict->SetValue(key, value));

  // The list value is now owned by the target dictionary.
  EXPECT_FALSE(dict_value->IsValid());

  // The value is still valid and points to the new reference list.
  EXPECT_TRUE(value->IsValid());

  HoneycombRefPtr<HoneycombDictionaryValue> dict_value2 = value->GetDictionary();
  HoneycombRefPtr<HoneycombDictionaryValue> dict_value3 = target_dict->GetDictionary(key);
  HoneycombRefPtr<HoneycombValue> value2 = target_dict->GetValue(key);
  HoneycombRefPtr<HoneycombDictionaryValue> dict_value4 = value2->GetDictionary();

  // All values are owned by the target dictionary.
  EXPECT_TRUE(value->IsOwned());
  EXPECT_TRUE(value2->IsOwned());
  EXPECT_TRUE(dict_value2->IsOwned());
  EXPECT_TRUE(dict_value3->IsOwned());
  EXPECT_TRUE(dict_value4->IsOwned());

  // All values are the same.
  EXPECT_TRUE(dict_value2->IsSame(dict_value3));
  TestDictionaryEqual(dict_value2, dict_value3);
  EXPECT_TRUE(dict_value2->IsSame(dict_value4));
  TestDictionaryEqual(dict_value2, dict_value4);

  // Change the value to something else.
  EXPECT_TRUE(target_dict->SetInt(key, kIntValue));
  EXPECT_EQ(VTYPE_INT, target_dict->GetType(key));
  EXPECT_EQ(kIntValue, target_dict->GetInt(key));

  // Now the references are invalid.
  EXPECT_FALSE(value->IsValid());
  EXPECT_FALSE(value2->IsValid());
  EXPECT_FALSE(dict_value2->IsValid());
  EXPECT_FALSE(dict_value3->IsValid());
  EXPECT_FALSE(dict_value4->IsValid());

  // Verify that adding a dictionary to a dictionary directly invalidates both
  // the dictionary and the value that references it.
  dict_value = CreateDictionaryValue();
  value = HoneycombValue::Create();
  EXPECT_TRUE(value->SetDictionary(dict_value));
  target_dict->SetDictionary(key, dict_value);
  EXPECT_FALSE(dict_value->IsValid());
  EXPECT_FALSE(value->IsValid());
}
