// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/test/honey_translator_test.h"
#include "tests/honeytests/test_handler.h"
#include "tests/gtest/include/gtest/gtest.h"

// Test getting/setting primitive types.
TEST(TranslatorTest, Primitive) {
  HoneycombRefPtr<HoneycombTranslatorTest> obj = HoneycombTranslatorTest::Create();
  obj->GetVoid();  // Does nothing, but shouldn't crash.
  EXPECT_EQ(TEST_BOOL_VAL, obj->GetBool());
  EXPECT_EQ(TEST_INT_VAL, obj->GetInt());
  EXPECT_EQ(TEST_DOUBLE_VAL, obj->GetDouble());
  EXPECT_EQ(TEST_LONG_VAL, obj->GetLong());
  EXPECT_EQ(TEST_SIZET_VAL, obj->GetSizet());
  EXPECT_TRUE(obj->SetVoid());  // Does nothing, but shouldn't crash.
  EXPECT_TRUE(obj->SetBool(TEST_BOOL_VAL));
  EXPECT_TRUE(obj->SetInt(TEST_INT_VAL));
  EXPECT_TRUE(obj->SetDouble(TEST_DOUBLE_VAL));
  EXPECT_TRUE(obj->SetLong(TEST_LONG_VAL));
  EXPECT_TRUE(obj->SetSizet(TEST_SIZET_VAL));

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

// Test getting/setting primitive list types.
TEST(TranslatorTest, PrimitiveList) {
  HoneycombRefPtr<HoneycombTranslatorTest> obj = HoneycombTranslatorTest::Create();

  std::vector<int> list;
  list.push_back(TEST_INT_VAL);
  list.push_back(TEST_INT_VAL2);
  EXPECT_TRUE(obj->SetIntList(list));

  list.clear();
  EXPECT_TRUE(obj->GetIntListByRef(list));
  EXPECT_EQ(2U, list.size());
  EXPECT_EQ(TEST_INT_VAL, list[0]);
  EXPECT_EQ(TEST_INT_VAL2, list[1]);

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

// Test getting/setting string types.
TEST(TranslatorTest, String) {
  HoneycombRefPtr<HoneycombTranslatorTest> obj = HoneycombTranslatorTest::Create();
  EXPECT_STREQ(TEST_STRING_VAL, obj->GetString().ToString().c_str());
  EXPECT_TRUE(obj->SetString(TEST_STRING_VAL));

  HoneycombString str;
  obj->GetStringByRef(str);
  EXPECT_STREQ(TEST_STRING_VAL, str.ToString().c_str());

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

// Test getting/setting string list types.
TEST(TranslatorTest, StringList) {
  HoneycombRefPtr<HoneycombTranslatorTest> obj = HoneycombTranslatorTest::Create();

  std::vector<HoneycombString> list;
  list.push_back(TEST_STRING_VAL);
  list.push_back(TEST_STRING_VAL2);
  list.push_back(TEST_STRING_VAL3);
  EXPECT_TRUE(obj->SetStringList(list));

  list.clear();
  EXPECT_TRUE(obj->GetStringListByRef(list));
  EXPECT_EQ(3U, list.size());
  EXPECT_STREQ(TEST_STRING_VAL, list[0].ToString().c_str());
  EXPECT_STREQ(TEST_STRING_VAL2, list[1].ToString().c_str());
  EXPECT_STREQ(TEST_STRING_VAL3, list[2].ToString().c_str());

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

// Test getting/setting string map types.
TEST(TranslatorTest, StringMap) {
  HoneycombRefPtr<HoneycombTranslatorTest> obj = HoneycombTranslatorTest::Create();

  HoneycombTranslatorTest::StringMap map;
  map.insert(std::make_pair(TEST_STRING_KEY, TEST_STRING_VAL));
  map.insert(std::make_pair(TEST_STRING_KEY2, TEST_STRING_VAL2));
  map.insert(std::make_pair(TEST_STRING_KEY3, TEST_STRING_VAL3));
  EXPECT_TRUE(obj->SetStringMap(map));

  map.clear();
  EXPECT_TRUE(obj->GetStringMapByRef(map));
  EXPECT_EQ(3U, map.size());

  HoneycombTranslatorTest::StringMap::const_iterator it;

  it = map.find(TEST_STRING_KEY);
  EXPECT_TRUE(it != map.end() && it->second == TEST_STRING_VAL);
  it = map.find(TEST_STRING_KEY2);
  EXPECT_TRUE(it != map.end() && it->second == TEST_STRING_VAL2);
  it = map.find(TEST_STRING_KEY3);
  EXPECT_TRUE(it != map.end() && it->second == TEST_STRING_VAL3);

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

// Test getting/setting string multimap types.
TEST(TranslatorTest, StringMultimap) {
  HoneycombRefPtr<HoneycombTranslatorTest> obj = HoneycombTranslatorTest::Create();

  HoneycombTranslatorTest::StringMultimap map;
  map.insert(std::make_pair(TEST_STRING_KEY, TEST_STRING_VAL));
  map.insert(std::make_pair(TEST_STRING_KEY2, TEST_STRING_VAL2));
  map.insert(std::make_pair(TEST_STRING_KEY3, TEST_STRING_VAL3));
  EXPECT_TRUE(obj->SetStringMultimap(map));

  map.clear();
  EXPECT_TRUE(obj->GetStringMultimapByRef(map));
  EXPECT_EQ(3U, map.size());

  HoneycombTranslatorTest::StringMultimap::const_iterator it;

  it = map.find(TEST_STRING_KEY);
  EXPECT_TRUE(it != map.end() && it->second == TEST_STRING_VAL);
  it = map.find(TEST_STRING_KEY2);
  EXPECT_TRUE(it != map.end() && it->second == TEST_STRING_VAL2);
  it = map.find(TEST_STRING_KEY3);
  EXPECT_TRUE(it != map.end() && it->second == TEST_STRING_VAL3);

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

// Test getting/setting struct types.
TEST(TranslatorTest, Struct) {
  HoneycombRefPtr<HoneycombTranslatorTest> obj = HoneycombTranslatorTest::Create();

  HoneycombPoint point(TEST_X_VAL, TEST_Y_VAL);
  EXPECT_EQ(point, obj->GetPoint());
  EXPECT_TRUE(obj->SetPoint(point));

  HoneycombPoint point2;
  obj->GetPointByRef(point2);
  EXPECT_EQ(point, point2);

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

// Test getting/setting struct list types.
TEST(TranslatorTest, StructList) {
  HoneycombRefPtr<HoneycombTranslatorTest> obj = HoneycombTranslatorTest::Create();

  std::vector<HoneycombPoint> list;
  list.push_back(HoneycombPoint(TEST_X_VAL, TEST_Y_VAL));
  list.push_back(HoneycombPoint(TEST_X_VAL2, TEST_Y_VAL2));
  EXPECT_TRUE(obj->SetPointList(list));

  list.clear();
  EXPECT_TRUE(obj->GetPointListByRef(list));
  EXPECT_EQ(2U, list.size());
  EXPECT_EQ(HoneycombPoint(TEST_X_VAL, TEST_Y_VAL), list[0]);
  EXPECT_EQ(HoneycombPoint(TEST_X_VAL2, TEST_Y_VAL2), list[1]);

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

// Test getting/setting library-side RefPtr types.
TEST(TranslatorTest, RefPtrLibrary) {
  HoneycombRefPtr<HoneycombTranslatorTest> obj = HoneycombTranslatorTest::Create();

  const int kTestVal = 12;
  HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibrary> test_obj =
      HoneycombTranslatorTestRefPtrLibrary::Create(kTestVal);
  EXPECT_EQ(kTestVal, test_obj->GetValue());
  int retval = obj->SetRefPtrLibrary(test_obj);
  EXPECT_EQ(kTestVal, retval);
  EXPECT_EQ(kTestVal, test_obj->GetValue());

  const int kTestVal2 = 30;
  HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibrary> test_obj2 =
      obj->GetRefPtrLibrary(kTestVal2);
  EXPECT_EQ(kTestVal2, test_obj2->GetValue());
  int retval2 = obj->SetRefPtrLibrary(test_obj2);
  EXPECT_EQ(kTestVal2, retval2);
  EXPECT_EQ(kTestVal2, test_obj2->GetValue());

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
  EXPECT_TRUE(test_obj->HasOneRef());
  EXPECT_TRUE(test_obj2->HasOneRef());
}

// Test getting/setting inherited library-side RefPtr types.
TEST(TranslatorTest, RefPtrLibraryInherit) {
  HoneycombRefPtr<HoneycombTranslatorTest> obj = HoneycombTranslatorTest::Create();

  const int kTestVal = 12;
  const int kTestVal2 = 40;
  HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibraryChild> test_obj =
      HoneycombTranslatorTestRefPtrLibraryChild::Create(kTestVal, kTestVal2);
  EXPECT_EQ(kTestVal, test_obj->GetValue());
  EXPECT_EQ(kTestVal2, test_obj->GetOtherValue());
  int retval = obj->SetRefPtrLibrary(test_obj);
  EXPECT_EQ(kTestVal, retval);
  EXPECT_EQ(kTestVal, test_obj->GetValue());
  EXPECT_EQ(kTestVal2, test_obj->GetOtherValue());

  EXPECT_EQ(kTestVal, obj->SetChildRefPtrLibrary(test_obj));
  EXPECT_EQ(kTestVal,
            obj->SetChildRefPtrLibraryAndReturnParent(test_obj)->GetValue());

  const int kTestVal3 = 100;
  HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibraryChildChild> test_obj2 =
      HoneycombTranslatorTestRefPtrLibraryChildChild::Create(kTestVal, kTestVal2,
                                                       kTestVal3);
  EXPECT_EQ(kTestVal, test_obj2->GetValue());
  EXPECT_EQ(kTestVal2, test_obj2->GetOtherValue());
  EXPECT_EQ(kTestVal3, test_obj2->GetOtherOtherValue());
  int retval2 = obj->SetRefPtrLibrary(test_obj2);
  EXPECT_EQ(kTestVal, retval2);
  EXPECT_EQ(kTestVal, test_obj2->GetValue());
  EXPECT_EQ(kTestVal2, test_obj2->GetOtherValue());
  EXPECT_EQ(kTestVal3, test_obj2->GetOtherOtherValue());

  EXPECT_EQ(kTestVal, obj->SetChildRefPtrLibrary(test_obj2));
  EXPECT_EQ(kTestVal,
            obj->SetChildRefPtrLibraryAndReturnParent(test_obj2)->GetValue());

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
  EXPECT_TRUE(test_obj->HasOneRef());
  EXPECT_TRUE(test_obj2->HasOneRef());
}

// Test getting/setting library-side RefPtr list types.
TEST(TranslatorTest, RefPtrLibraryList) {
  HoneycombRefPtr<HoneycombTranslatorTest> obj = HoneycombTranslatorTest::Create();

  const int kVal1 = 34;
  const int kVal2 = 10;

  HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibrary> val1 =
      HoneycombTranslatorTestRefPtrLibrary::Create(kVal1);
  HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibrary> val2 =
      HoneycombTranslatorTestRefPtrLibraryChild::Create(kVal2, 0);

  std::vector<HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibrary>> list;
  list.push_back(val1);
  list.push_back(val2);
  EXPECT_TRUE(obj->SetRefPtrLibraryList(list, kVal1, kVal2));

  list.clear();
  EXPECT_TRUE(obj->GetRefPtrLibraryListByRef(list, kVal1, kVal2));
  EXPECT_EQ(2U, list.size());
  EXPECT_EQ(kVal1, list[0]->GetValue());
  EXPECT_EQ(kVal2, list[1]->GetValue());

  list.clear();

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
  EXPECT_TRUE(val1->HasOneRef());
  EXPECT_TRUE(val2->HasOneRef());
}

namespace {

class TranslatorTestRefPtrClient : public HoneycombTranslatorTestRefPtrClient {
 public:
  explicit TranslatorTestRefPtrClient(const int val) : val_(val) {}

  virtual int GetValue() override { return val_; }

 private:
  const int val_;

  IMPLEMENT_REFCOUNTING(TranslatorTestRefPtrClient);
  DISALLOW_COPY_AND_ASSIGN(TranslatorTestRefPtrClient);
};

class TranslatorTestRefPtrClientChild
    : public HoneycombTranslatorTestRefPtrClientChild {
 public:
  TranslatorTestRefPtrClientChild(const int val, const int other_val)
      : val_(val), other_val_(other_val) {}

  virtual int GetValue() override { return val_; }

  virtual int GetOtherValue() override { return other_val_; }

 private:
  const int val_;
  const int other_val_;

  IMPLEMENT_REFCOUNTING(TranslatorTestRefPtrClientChild);
  DISALLOW_COPY_AND_ASSIGN(TranslatorTestRefPtrClientChild);
};

}  // namespace

// Test getting/setting client-side RefPtr types.
TEST(TranslatorTest, RefPtrClient) {
  HoneycombRefPtr<HoneycombTranslatorTest> obj = HoneycombTranslatorTest::Create();

  const int kTestVal = 12;

  HoneycombRefPtr<TranslatorTestRefPtrClient> test_obj =
      new TranslatorTestRefPtrClient(kTestVal);
  EXPECT_EQ(kTestVal, test_obj->GetValue());
  EXPECT_EQ(kTestVal, obj->SetRefPtrClient(test_obj.get()));
  HoneycombRefPtr<HoneycombTranslatorTestRefPtrClient> handler =
      obj->SetRefPtrClientAndReturn(test_obj.get());
  EXPECT_EQ(test_obj.get(), handler.get());
  EXPECT_EQ(kTestVal, handler->GetValue());
  handler = nullptr;

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
  EXPECT_TRUE(test_obj->HasOneRef());
}

// Test getting/setting inherited client-side RefPtr types.
TEST(TranslatorTest, RefPtrClientInherit) {
  HoneycombRefPtr<HoneycombTranslatorTest> obj = HoneycombTranslatorTest::Create();

  const int kTestVal = 12;
  const int kTestVal2 = 86;

  HoneycombRefPtr<TranslatorTestRefPtrClientChild> test_obj =
      new TranslatorTestRefPtrClientChild(kTestVal, kTestVal2);
  EXPECT_EQ(kTestVal, test_obj->GetValue());
  EXPECT_EQ(kTestVal2, test_obj->GetOtherValue());
  int retval = obj->SetRefPtrClient(test_obj);
  EXPECT_EQ(kTestVal, retval);
  EXPECT_EQ(kTestVal, test_obj->GetValue());
  EXPECT_EQ(kTestVal2, test_obj->GetOtherValue());

  EXPECT_EQ(kTestVal, obj->SetChildRefPtrClient(test_obj));
  HoneycombRefPtr<HoneycombTranslatorTestRefPtrClient> handler =
      obj->SetChildRefPtrClientAndReturnParent(test_obj);
  EXPECT_EQ(kTestVal, handler->GetValue());
  EXPECT_EQ(test_obj.get(), handler.get());
  handler = nullptr;

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
  EXPECT_TRUE(test_obj->HasOneRef());
}

// Test getting/setting client-side RefPtr list types.
TEST(TranslatorTest, RefPtrClientList) {
  HoneycombRefPtr<HoneycombTranslatorTest> obj = HoneycombTranslatorTest::Create();

  const int kVal1 = 34;
  const int kVal2 = 10;

  HoneycombRefPtr<HoneycombTranslatorTestRefPtrClient> val1 =
      new TranslatorTestRefPtrClient(kVal1);
  HoneycombRefPtr<HoneycombTranslatorTestRefPtrClient> val2 =
      new TranslatorTestRefPtrClientChild(kVal2, 0);

  std::vector<HoneycombRefPtr<HoneycombTranslatorTestRefPtrClient>> list;
  list.push_back(val1);
  list.push_back(val2);
  EXPECT_TRUE(obj->SetRefPtrClientList(list, kVal1, kVal2));

  list.clear();
  EXPECT_TRUE(obj->GetRefPtrClientListByRef(list, val1, val2));
  EXPECT_EQ(2U, list.size());
  EXPECT_EQ(kVal1, list[0]->GetValue());
  EXPECT_EQ(val1.get(), list[0].get());
  EXPECT_EQ(kVal2, list[1]->GetValue());
  EXPECT_EQ(val2.get(), list[1].get());

  list.clear();

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
  EXPECT_TRUE(val1->HasOneRef());
  EXPECT_TRUE(val2->HasOneRef());
}

// Test getting/setting library-side OwnPtr types.
TEST(TranslatorTest, OwnPtrLibrary) {
  HoneycombRefPtr<HoneycombTranslatorTest> obj = HoneycombTranslatorTest::Create();

  const int kTestVal = 12;
  HoneycombOwnPtr<HoneycombTranslatorTestScopedLibrary> test_obj =
      HoneycombTranslatorTestScopedLibrary::Create(kTestVal);
  EXPECT_TRUE(test_obj.get());
  EXPECT_EQ(kTestVal, test_obj->GetValue());
  int retval = obj->SetOwnPtrLibrary(std::move(test_obj));
  EXPECT_EQ(kTestVal, retval);
  EXPECT_FALSE(test_obj.get());

  const int kTestVal2 = 30;
  HoneycombOwnPtr<HoneycombTranslatorTestScopedLibrary> test_obj2 =
      obj->GetOwnPtrLibrary(kTestVal2);
  EXPECT_TRUE(test_obj2.get());
  EXPECT_EQ(kTestVal2, test_obj2->GetValue());
  int retval2 = obj->SetOwnPtrLibrary(std::move(test_obj2));
  EXPECT_EQ(kTestVal2, retval2);
  EXPECT_FALSE(test_obj2.get());

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

// Test getting/setting inherited library-side OwnPtr types.
TEST(TranslatorTest, OwnPtrLibraryInherit) {
  HoneycombRefPtr<HoneycombTranslatorTest> obj = HoneycombTranslatorTest::Create();

  const int kTestVal = 12;
  const int kTestVal2 = 40;
  HoneycombOwnPtr<HoneycombTranslatorTestScopedLibraryChild> test_obj =
      HoneycombTranslatorTestScopedLibraryChild::Create(kTestVal, kTestVal2);
  EXPECT_TRUE(test_obj.get());
  EXPECT_EQ(kTestVal, test_obj->GetValue());
  EXPECT_EQ(kTestVal2, test_obj->GetOtherValue());
  int retval = obj->SetOwnPtrLibrary(std::move(test_obj));
  EXPECT_EQ(kTestVal, retval);
  EXPECT_FALSE(test_obj.get());

  test_obj = HoneycombTranslatorTestScopedLibraryChild::Create(kTestVal, kTestVal2);
  EXPECT_TRUE(test_obj.get());
  EXPECT_EQ(kTestVal, obj->SetChildOwnPtrLibrary(std::move(test_obj)));
  EXPECT_FALSE(test_obj.get());

  test_obj = HoneycombTranslatorTestScopedLibraryChild::Create(kTestVal, kTestVal2);
  EXPECT_TRUE(test_obj.get());
  HoneycombOwnPtr<HoneycombTranslatorTestScopedLibrary> test_obj_parent =
      obj->SetChildOwnPtrLibraryAndReturnParent(std::move(test_obj));
  EXPECT_FALSE(test_obj.get());
  EXPECT_TRUE(test_obj_parent.get());
  EXPECT_EQ(kTestVal, test_obj_parent->GetValue());
  test_obj_parent.reset(nullptr);

  const int kTestVal3 = 100;
  HoneycombOwnPtr<HoneycombTranslatorTestScopedLibraryChildChild> test_obj2 =
      HoneycombTranslatorTestScopedLibraryChildChild::Create(kTestVal, kTestVal2,
                                                       kTestVal3);
  EXPECT_EQ(kTestVal, test_obj2->GetValue());
  EXPECT_EQ(kTestVal2, test_obj2->GetOtherValue());
  EXPECT_EQ(kTestVal3, test_obj2->GetOtherOtherValue());
  int retval2 = obj->SetOwnPtrLibrary(std::move(test_obj2));
  EXPECT_EQ(kTestVal, retval2);
  EXPECT_FALSE(test_obj2.get());

  test_obj2 = HoneycombTranslatorTestScopedLibraryChildChild::Create(
      kTestVal, kTestVal2, kTestVal3);
  EXPECT_EQ(kTestVal, obj->SetChildOwnPtrLibrary(std::move(test_obj2)));
  EXPECT_FALSE(test_obj2.get());

  test_obj2 = HoneycombTranslatorTestScopedLibraryChildChild::Create(
      kTestVal, kTestVal2, kTestVal3);
  test_obj_parent =
      obj->SetChildOwnPtrLibraryAndReturnParent(std::move(test_obj2));
  EXPECT_FALSE(test_obj2.get());
  EXPECT_TRUE(test_obj_parent.get());
  EXPECT_EQ(kTestVal, test_obj_parent->GetValue());
  test_obj_parent.reset(nullptr);

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

namespace {

class TranslatorTestScopedClient : public HoneycombTranslatorTestScopedClient {
 public:
  TranslatorTestScopedClient(const int val, TrackCallback* got_delete)
      : val_(val), got_delete_(got_delete) {}
  ~TranslatorTestScopedClient() override { got_delete_->yes(); }

  virtual int GetValue() override { return val_; }

 private:
  const int val_;
  TrackCallback* got_delete_;

  DISALLOW_COPY_AND_ASSIGN(TranslatorTestScopedClient);
};

class TranslatorTestScopedClientChild
    : public HoneycombTranslatorTestScopedClientChild {
 public:
  TranslatorTestScopedClientChild(const int val,
                                  const int other_val,
                                  TrackCallback* got_delete)
      : val_(val), other_val_(other_val), got_delete_(got_delete) {}
  ~TranslatorTestScopedClientChild() override { got_delete_->yes(); }

  virtual int GetValue() override { return val_; }

  virtual int GetOtherValue() override { return other_val_; }

 private:
  const int val_;
  const int other_val_;
  TrackCallback* got_delete_;

  DISALLOW_COPY_AND_ASSIGN(TranslatorTestScopedClientChild);
};

}  // namespace

// Test getting/setting client-side OwnPtr types.
TEST(TranslatorTest, OwnPtrClient) {
  HoneycombRefPtr<HoneycombTranslatorTest> obj = HoneycombTranslatorTest::Create();

  const int kTestVal = 12;
  TrackCallback got_delete;

  HoneycombOwnPtr<HoneycombTranslatorTestScopedClient> test_obj(
      new TranslatorTestScopedClient(kTestVal, &got_delete));
  EXPECT_EQ(kTestVal, test_obj->GetValue());
  EXPECT_EQ(kTestVal, obj->SetOwnPtrClient(std::move(test_obj)));
  EXPECT_FALSE(test_obj.get());
  EXPECT_TRUE(got_delete);

  got_delete.reset();
  test_obj.reset(new TranslatorTestScopedClient(kTestVal, &got_delete));
  HoneycombOwnPtr<HoneycombTranslatorTestScopedClient> handler =
      obj->SetOwnPtrClientAndReturn(std::move(test_obj));
  EXPECT_FALSE(test_obj.get());
  EXPECT_TRUE(handler.get());
  EXPECT_FALSE(got_delete);
  EXPECT_EQ(kTestVal, handler->GetValue());
  handler.reset(nullptr);
  EXPECT_TRUE(got_delete);

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

// Test getting/setting inherited client-side OwnPtr types.
TEST(TranslatorTest, OwnPtrClientInherit) {
  HoneycombRefPtr<HoneycombTranslatorTest> obj = HoneycombTranslatorTest::Create();

  const int kTestVal = 12;
  const int kTestVal2 = 86;
  TrackCallback got_delete;

  HoneycombOwnPtr<HoneycombTranslatorTestScopedClientChild> test_obj(
      new TranslatorTestScopedClientChild(kTestVal, kTestVal2, &got_delete));
  EXPECT_EQ(kTestVal, test_obj->GetValue());
  EXPECT_EQ(kTestVal2, test_obj->GetOtherValue());
  EXPECT_EQ(kTestVal, obj->SetOwnPtrClient(std::move(test_obj)));
  EXPECT_FALSE(test_obj.get());
  EXPECT_TRUE(got_delete);

  got_delete.reset();
  test_obj.reset(
      new TranslatorTestScopedClientChild(kTestVal, kTestVal2, &got_delete));
  EXPECT_EQ(kTestVal, obj->SetChildOwnPtrClient(std::move(test_obj)));
  EXPECT_FALSE(test_obj.get());
  EXPECT_TRUE(got_delete);

  got_delete.reset();
  test_obj.reset(
      new TranslatorTestScopedClientChild(kTestVal, kTestVal2, &got_delete));
  HoneycombOwnPtr<HoneycombTranslatorTestScopedClient> handler(
      obj->SetChildOwnPtrClientAndReturnParent(std::move(test_obj)));
  EXPECT_EQ(kTestVal, handler->GetValue());
  EXPECT_FALSE(test_obj.get());
  EXPECT_FALSE(got_delete);
  handler.reset(nullptr);
  EXPECT_TRUE(got_delete);

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

// Test getting/setting library-side RawPtr types.
TEST(TranslatorTest, RawPtrLibrary) {
  HoneycombRefPtr<HoneycombTranslatorTest> obj = HoneycombTranslatorTest::Create();

  const int kTestVal = 12;
  HoneycombOwnPtr<HoneycombTranslatorTestScopedLibrary> test_obj(
      HoneycombTranslatorTestScopedLibrary::Create(kTestVal));
  EXPECT_EQ(kTestVal, test_obj->GetValue());
  int retval = obj->SetRawPtrLibrary(test_obj.get());
  EXPECT_EQ(kTestVal, retval);
  EXPECT_EQ(kTestVal, test_obj->GetValue());

  const int kTestVal2 = 30;
  HoneycombOwnPtr<HoneycombTranslatorTestScopedLibrary> test_obj2(
      obj->GetOwnPtrLibrary(kTestVal2));
  EXPECT_EQ(kTestVal2, test_obj2->GetValue());
  int retval2 = obj->SetRawPtrLibrary(test_obj2.get());
  EXPECT_EQ(kTestVal2, retval2);
  EXPECT_EQ(kTestVal2, test_obj2->GetValue());

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

// Test getting/setting inherited library-side RawPtr types.
TEST(TranslatorTest, RawPtrLibraryInherit) {
  HoneycombRefPtr<HoneycombTranslatorTest> obj = HoneycombTranslatorTest::Create();

  const int kTestVal = 12;
  const int kTestVal2 = 40;
  HoneycombOwnPtr<HoneycombTranslatorTestScopedLibraryChild> test_obj(
      HoneycombTranslatorTestScopedLibraryChild::Create(kTestVal, kTestVal2));
  EXPECT_EQ(kTestVal, test_obj->GetValue());
  EXPECT_EQ(kTestVal2, test_obj->GetOtherValue());
  int retval = obj->SetRawPtrLibrary(test_obj.get());
  EXPECT_EQ(kTestVal, retval);
  EXPECT_EQ(kTestVal, test_obj->GetValue());
  EXPECT_EQ(kTestVal2, test_obj->GetOtherValue());

  EXPECT_EQ(kTestVal, obj->SetChildRawPtrLibrary(test_obj.get()));

  const int kTestVal3 = 100;
  HoneycombOwnPtr<HoneycombTranslatorTestScopedLibraryChildChild> test_obj2(
      HoneycombTranslatorTestScopedLibraryChildChild::Create(kTestVal, kTestVal2,
                                                       kTestVal3));
  EXPECT_EQ(kTestVal, test_obj2->GetValue());
  EXPECT_EQ(kTestVal2, test_obj2->GetOtherValue());
  EXPECT_EQ(kTestVal3, test_obj2->GetOtherOtherValue());
  int retval2 = obj->SetRawPtrLibrary(test_obj2.get());
  EXPECT_EQ(kTestVal, retval2);
  EXPECT_EQ(kTestVal, test_obj2->GetValue());
  EXPECT_EQ(kTestVal2, test_obj2->GetOtherValue());
  EXPECT_EQ(kTestVal3, test_obj2->GetOtherOtherValue());

  EXPECT_EQ(kTestVal, obj->SetChildRawPtrLibrary(test_obj2.get()));

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

// Test getting/setting library-side RawPtr list types.
TEST(TranslatorTest, RawPtrLibraryList) {
  HoneycombRefPtr<HoneycombTranslatorTest> obj = HoneycombTranslatorTest::Create();

  const int kVal1 = 34;
  const int kVal2 = 10;

  HoneycombOwnPtr<HoneycombTranslatorTestScopedLibrary> val1(
      HoneycombTranslatorTestScopedLibrary::Create(kVal1));
  HoneycombOwnPtr<HoneycombTranslatorTestScopedLibraryChild> val2(
      HoneycombTranslatorTestScopedLibraryChild::Create(kVal2, 0));

  std::vector<HoneycombRawPtr<HoneycombTranslatorTestScopedLibrary>> list;
  list.push_back(val1.get());
  list.push_back(val2.get());
  EXPECT_TRUE(obj->SetRawPtrLibraryList(list, kVal1, kVal2));
  list.clear();

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

// Test getting/setting client-side RawPtr types.
TEST(TranslatorTest, RawPtrClient) {
  HoneycombRefPtr<HoneycombTranslatorTest> obj = HoneycombTranslatorTest::Create();

  const int kTestVal = 12;
  TrackCallback got_delete;

  HoneycombOwnPtr<TranslatorTestScopedClient> test_obj(
      new TranslatorTestScopedClient(kTestVal, &got_delete));
  EXPECT_EQ(kTestVal, test_obj->GetValue());
  EXPECT_EQ(kTestVal, obj->SetRawPtrClient(test_obj.get()));
  EXPECT_FALSE(got_delete);
  test_obj.reset(nullptr);
  EXPECT_TRUE(got_delete);

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

// Test getting/setting inherited client-side RawPtr types.
TEST(TranslatorTest, RawPtrClientInherit) {
  HoneycombRefPtr<HoneycombTranslatorTest> obj = HoneycombTranslatorTest::Create();

  const int kTestVal = 12;
  const int kTestVal2 = 86;
  TrackCallback got_delete;

  HoneycombOwnPtr<TranslatorTestScopedClientChild> test_obj(
      new TranslatorTestScopedClientChild(kTestVal, kTestVal2, &got_delete));
  EXPECT_EQ(kTestVal, test_obj->GetValue());
  EXPECT_EQ(kTestVal2, test_obj->GetOtherValue());
  int retval = obj->SetRawPtrClient(test_obj.get());
  EXPECT_EQ(kTestVal, retval);
  EXPECT_EQ(kTestVal, test_obj->GetValue());
  EXPECT_EQ(kTestVal2, test_obj->GetOtherValue());
  EXPECT_FALSE(got_delete);

  EXPECT_EQ(kTestVal, obj->SetChildRawPtrClient(test_obj.get()));
  EXPECT_FALSE(got_delete);
  test_obj.reset(nullptr);
  EXPECT_TRUE(got_delete);

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

// Test getting/setting client-side RawPtr list types.
TEST(TranslatorTest, RawPtrClientList) {
  HoneycombRefPtr<HoneycombTranslatorTest> obj = HoneycombTranslatorTest::Create();

  const int kVal1 = 34;
  const int kVal2 = 10;
  TrackCallback got_delete1, got_delete2;

  HoneycombOwnPtr<HoneycombTranslatorTestScopedClient> val1(
      new TranslatorTestScopedClient(kVal1, &got_delete1));
  HoneycombOwnPtr<HoneycombTranslatorTestScopedClient> val2(
      new TranslatorTestScopedClientChild(kVal2, 0, &got_delete2));

  std::vector<HoneycombRawPtr<HoneycombTranslatorTestScopedClient>> list;
  list.push_back(val1.get());
  list.push_back(val2.get());
  EXPECT_TRUE(obj->SetRawPtrClientList(list, kVal1, kVal2));
  list.clear();

  EXPECT_FALSE(got_delete1);
  val1.reset(nullptr);
  EXPECT_TRUE(got_delete1);

  EXPECT_FALSE(got_delete2);
  val2.reset(nullptr);
  EXPECT_TRUE(got_delete2);

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}
