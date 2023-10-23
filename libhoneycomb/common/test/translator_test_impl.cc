// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "include/test/honey_translator_test.h"

class HoneycombTranslatorTestRefPtrLibraryImpl
    : public HoneycombTranslatorTestRefPtrLibrary {
 public:
  explicit HoneycombTranslatorTestRefPtrLibraryImpl(int value) : value_(value) {}

  HoneycombTranslatorTestRefPtrLibraryImpl(
      const HoneycombTranslatorTestRefPtrLibraryImpl&) = delete;
  HoneycombTranslatorTestRefPtrLibraryImpl& operator=(
      const HoneycombTranslatorTestRefPtrLibraryImpl&) = delete;

  int GetValue() override { return value_; }

  void SetValue(int value) override { value_ = value; }

 protected:
  int value_;

 private:
  IMPLEMENT_REFCOUNTING(HoneycombTranslatorTestRefPtrLibraryImpl);
};

// static
HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibrary>
HoneycombTranslatorTestRefPtrLibrary::Create(int value) {
  return new HoneycombTranslatorTestRefPtrLibraryImpl(value);
}

class HoneycombTranslatorTestRefPtrLibraryChildImpl
    : public HoneycombTranslatorTestRefPtrLibraryChild {
 public:
  HoneycombTranslatorTestRefPtrLibraryChildImpl(int value, int other_value)
      : value_(value), other_value_(other_value) {}

  HoneycombTranslatorTestRefPtrLibraryChildImpl(
      const HoneycombTranslatorTestRefPtrLibraryChildImpl&) = delete;
  HoneycombTranslatorTestRefPtrLibraryChildImpl& operator=(
      const HoneycombTranslatorTestRefPtrLibraryChildImpl&) = delete;

  int GetValue() override { return value_; }

  void SetValue(int value) override { value_ = value; }

  int GetOtherValue() override { return other_value_; }

  void SetOtherValue(int value) override { other_value_ = value; }

 protected:
  int value_;
  int other_value_;

 private:
  IMPLEMENT_REFCOUNTING(HoneycombTranslatorTestRefPtrLibraryChildImpl);
};

// static
HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibraryChild>
HoneycombTranslatorTestRefPtrLibraryChild::Create(int value, int other_value) {
  return new HoneycombTranslatorTestRefPtrLibraryChildImpl(value, other_value);
}

class HoneycombTranslatorTestRefPtrLibraryChildChildImpl
    : public HoneycombTranslatorTestRefPtrLibraryChildChild {
 public:
  HoneycombTranslatorTestRefPtrLibraryChildChildImpl(int value,
                                               int other_value,
                                               int other_other_value)
      : value_(value),
        other_value_(other_value),
        other_other_value_(other_other_value) {}

  HoneycombTranslatorTestRefPtrLibraryChildChildImpl(
      const HoneycombTranslatorTestRefPtrLibraryChildChildImpl&) = delete;
  HoneycombTranslatorTestRefPtrLibraryChildChildImpl& operator=(
      const HoneycombTranslatorTestRefPtrLibraryChildChildImpl&) = delete;

  int GetValue() override { return value_; }

  void SetValue(int value) override { value_ = value; }

  int GetOtherValue() override { return other_value_; }

  void SetOtherValue(int value) override { other_value_ = value; }

  int GetOtherOtherValue() override { return other_other_value_; }

  void SetOtherOtherValue(int value) override { other_other_value_ = value; }

 protected:
  int value_;
  int other_value_;
  int other_other_value_;

 private:
  IMPLEMENT_REFCOUNTING(HoneycombTranslatorTestRefPtrLibraryChildChildImpl);
};

// static
HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibraryChildChild>
HoneycombTranslatorTestRefPtrLibraryChildChild::Create(int value,
                                                 int other_value,
                                                 int other_other_value) {
  return new HoneycombTranslatorTestRefPtrLibraryChildChildImpl(value, other_value,
                                                          other_other_value);
}

class HoneycombTranslatorTestScopedLibraryImpl
    : public HoneycombTranslatorTestScopedLibrary {
 public:
  explicit HoneycombTranslatorTestScopedLibraryImpl(int value) : value_(value) {}

  HoneycombTranslatorTestScopedLibraryImpl(
      const HoneycombTranslatorTestScopedLibraryImpl&) = delete;
  HoneycombTranslatorTestScopedLibraryImpl& operator=(
      const HoneycombTranslatorTestScopedLibraryImpl&) = delete;

  int GetValue() override { return value_; }

  void SetValue(int value) override { value_ = value; }

 protected:
  int value_;
};

// static
HoneycombOwnPtr<HoneycombTranslatorTestScopedLibrary>
HoneycombTranslatorTestScopedLibrary::Create(int value) {
  return HoneycombOwnPtr<HoneycombTranslatorTestScopedLibrary>(
      new HoneycombTranslatorTestScopedLibraryImpl(value));
}

class HoneycombTranslatorTestScopedLibraryChildImpl
    : public HoneycombTranslatorTestScopedLibraryChild {
 public:
  HoneycombTranslatorTestScopedLibraryChildImpl(int value, int other_value)
      : value_(value), other_value_(other_value) {}

  HoneycombTranslatorTestScopedLibraryChildImpl(
      const HoneycombTranslatorTestScopedLibraryChildImpl&) = delete;
  HoneycombTranslatorTestScopedLibraryChildImpl& operator=(
      const HoneycombTranslatorTestScopedLibraryChildImpl&) = delete;

  int GetValue() override { return value_; }

  void SetValue(int value) override { value_ = value; }

  int GetOtherValue() override { return other_value_; }

  void SetOtherValue(int value) override { other_value_ = value; }

 protected:
  int value_;
  int other_value_;
};

// static
HoneycombOwnPtr<HoneycombTranslatorTestScopedLibraryChild>
HoneycombTranslatorTestScopedLibraryChild::Create(int value, int other_value) {
  return HoneycombOwnPtr<HoneycombTranslatorTestScopedLibraryChild>(
      new HoneycombTranslatorTestScopedLibraryChildImpl(value, other_value));
}

class HoneycombTranslatorTestScopedLibraryChildChildImpl
    : public HoneycombTranslatorTestScopedLibraryChildChild {
 public:
  HoneycombTranslatorTestScopedLibraryChildChildImpl(int value,
                                               int other_value,
                                               int other_other_value)
      : value_(value),
        other_value_(other_value),
        other_other_value_(other_other_value) {}

  HoneycombTranslatorTestScopedLibraryChildChildImpl(
      const HoneycombTranslatorTestScopedLibraryChildChildImpl&) = delete;
  HoneycombTranslatorTestScopedLibraryChildChildImpl& operator=(
      const HoneycombTranslatorTestScopedLibraryChildChildImpl&) = delete;

  int GetValue() override { return value_; }

  void SetValue(int value) override { value_ = value; }

  int GetOtherValue() override { return other_value_; }

  void SetOtherValue(int value) override { other_value_ = value; }

  int GetOtherOtherValue() override { return other_other_value_; }

  void SetOtherOtherValue(int value) override { other_other_value_ = value; }

 protected:
  int value_;
  int other_value_;
  int other_other_value_;
};

// static
HoneycombOwnPtr<HoneycombTranslatorTestScopedLibraryChildChild>
HoneycombTranslatorTestScopedLibraryChildChild::Create(int value,
                                                 int other_value,
                                                 int other_other_value) {
  return HoneycombOwnPtr<HoneycombTranslatorTestScopedLibraryChildChild>(
      new HoneycombTranslatorTestScopedLibraryChildChildImpl(value, other_value,
                                                       other_other_value));
}

class HoneycombTranslatorTestImpl : public HoneycombTranslatorTest {
 public:
  HoneycombTranslatorTestImpl() = default;

  HoneycombTranslatorTestImpl(const HoneycombTranslatorTestImpl&) = delete;
  HoneycombTranslatorTestImpl& operator=(const HoneycombTranslatorTestImpl&) = delete;

  // PRIMITIVE VALUES

  void GetVoid() override {}

  bool GetBool() override { return TEST_BOOL_VAL; }

  int GetInt() override { return TEST_INT_VAL; }

  double GetDouble() override { return TEST_DOUBLE_VAL; }

  long GetLong() override { return TEST_LONG_VAL; }

  size_t GetSizet() override { return TEST_SIZET_VAL; }

  bool SetVoid() override { return true; }

  bool SetBool(bool val) override { return (val == TEST_BOOL_VAL); }

  bool SetInt(int val) override { return (val == TEST_INT_VAL); }

  bool SetDouble(double val) override { return (val == TEST_DOUBLE_VAL); }

  bool SetLong(long val) override { return (val == TEST_LONG_VAL); }

  bool SetSizet(size_t val) override { return (val == TEST_SIZET_VAL); }

  // PRIMITIVE LIST VALUES

  bool SetIntList(const std::vector<int>& val) override {
    if (val.size() != 2U) {
      return false;
    }
    return val[0] == TEST_INT_VAL && val[1] == TEST_INT_VAL2;
  }

  bool GetIntListByRef(IntList& val) override {
    if (val.size() != GetIntListSize()) {
      return false;
    }
    val.clear();
    val.push_back(TEST_INT_VAL);
    val.push_back(TEST_INT_VAL2);
    return true;
  }

  size_t GetIntListSize() override { return 2U; }

  // STRING VALUES

  HoneycombString GetString() override { return TEST_STRING_VAL; }

  bool SetString(const HoneycombString& val) override {
    return (val.ToString() == TEST_STRING_VAL);
  }

  void GetStringByRef(HoneycombString& val) override { val = TEST_STRING_VAL; }

  // STRING LIST VALUES

  bool SetStringList(const std::vector<HoneycombString>& val) override {
    if (val.size() != 3U) {
      return false;
    }
    return val[0] == TEST_STRING_VAL && val[1] == TEST_STRING_VAL2 &&
           val[2] == TEST_STRING_VAL3;
  }

  bool GetStringListByRef(StringList& val) override {
    if (val.size() != 0U) {
      return false;
    }
    val.push_back(TEST_STRING_VAL);
    val.push_back(TEST_STRING_VAL2);
    val.push_back(TEST_STRING_VAL3);
    return true;
  }

  // STRING MAP VALUES

  bool SetStringMap(const StringMap& val) override {
    if (val.size() != 3U) {
      return false;
    }

    StringMap::const_iterator it;

    it = val.find(TEST_STRING_KEY);
    if (it == val.end() || it->second != TEST_STRING_VAL) {
      return false;
    }
    it = val.find(TEST_STRING_KEY2);
    if (it == val.end() || it->second != TEST_STRING_VAL2) {
      return false;
    }
    it = val.find(TEST_STRING_KEY3);
    if (it == val.end() || it->second != TEST_STRING_VAL3) {
      return false;
    }
    return true;
  }

  bool GetStringMapByRef(std::map<HoneycombString, HoneycombString>& val) override {
    if (val.size() != 0U) {
      return false;
    }

    val.insert(std::make_pair(TEST_STRING_KEY, TEST_STRING_VAL));
    val.insert(std::make_pair(TEST_STRING_KEY2, TEST_STRING_VAL2));
    val.insert(std::make_pair(TEST_STRING_KEY3, TEST_STRING_VAL3));
    return true;
  }

  // STRING MULTIMAP VALUES

  bool SetStringMultimap(
      const std::multimap<HoneycombString, HoneycombString>& val) override {
    if (val.size() != 3U) {
      return false;
    }

    StringMultimap::const_iterator it;

    it = val.find(TEST_STRING_KEY);
    if (it == val.end() || it->second != TEST_STRING_VAL) {
      return false;
    }
    it = val.find(TEST_STRING_KEY2);
    if (it == val.end() || it->second != TEST_STRING_VAL2) {
      return false;
    }
    it = val.find(TEST_STRING_KEY3);
    if (it == val.end() || it->second != TEST_STRING_VAL3) {
      return false;
    }
    return true;
  }

  bool GetStringMultimapByRef(StringMultimap& val) override {
    if (val.size() != 0U) {
      return false;
    }

    val.insert(std::make_pair(TEST_STRING_KEY, TEST_STRING_VAL));
    val.insert(std::make_pair(TEST_STRING_KEY2, TEST_STRING_VAL2));
    val.insert(std::make_pair(TEST_STRING_KEY3, TEST_STRING_VAL3));
    return true;
  }

  // STRUCT VALUES

  HoneycombPoint GetPoint() override { return HoneycombPoint(TEST_X_VAL, TEST_Y_VAL); }

  bool SetPoint(const HoneycombPoint& val) override {
    return val.x == TEST_X_VAL && val.y == TEST_Y_VAL;
  }

  void GetPointByRef(HoneycombPoint& val) override {
    val = HoneycombPoint(TEST_X_VAL, TEST_Y_VAL);
  }

  // STRUCT LIST VALUES

  bool SetPointList(const std::vector<HoneycombPoint>& val) override {
    if (val.size() != 2U) {
      return false;
    }
    return val[0].x == TEST_X_VAL && val[0].y == TEST_Y_VAL &&
           val[1].x == TEST_X_VAL2 && val[1].y == TEST_Y_VAL2;
  }

  bool GetPointListByRef(PointList& val) override {
    if (val.size() != GetPointListSize()) {
      return false;
    }
    val.clear();
    val.push_back(HoneycombPoint(TEST_X_VAL, TEST_Y_VAL));
    val.push_back(HoneycombPoint(TEST_X_VAL2, TEST_Y_VAL2));
    return true;
  }

  size_t GetPointListSize() override { return 2U; }

  // LIBRARY-SIDE REFPTR VALUES

  HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibrary> GetRefPtrLibrary(int val) override {
    return new HoneycombTranslatorTestRefPtrLibraryChildImpl(val, 0);
  }

  int SetRefPtrLibrary(HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibrary> val) override {
    return val->GetValue();
  }

  HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibrary> SetRefPtrLibraryAndReturn(
      HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibrary> val) override {
    return val;
  }

  int SetChildRefPtrLibrary(
      HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibraryChild> val) override {
    return val->GetValue();
  }

  HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibrary>
  SetChildRefPtrLibraryAndReturnParent(
      HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibraryChild> val) override {
    return val;
  }

  // LIBRARY-SIDE REFPTR LIST VALUES

  bool SetRefPtrLibraryList(
      const std::vector<HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibrary>>& val,
      int val1,
      int val2) override {
    if (val.size() != 2U) {
      return false;
    }
    return val[0]->GetValue() == val1 && val[1]->GetValue() == val2;
  }

  bool GetRefPtrLibraryListByRef(RefPtrLibraryList& val,
                                 int val1,
                                 int val2) override {
    if (val.size() != GetRefPtrLibraryListSize()) {
      return false;
    }
    val.clear();
    val.push_back(new HoneycombTranslatorTestRefPtrLibraryChildImpl(val1, 0));
    val.push_back(new HoneycombTranslatorTestRefPtrLibraryImpl(val2));
    return true;
  }

  size_t GetRefPtrLibraryListSize() override { return 2U; }

  // CLIENT-SIDE REFPTR VALUES

  int SetRefPtrClient(HoneycombRefPtr<HoneycombTranslatorTestRefPtrClient> val) override {
    return val->GetValue();
  }

  HoneycombRefPtr<HoneycombTranslatorTestRefPtrClient> SetRefPtrClientAndReturn(
      HoneycombRefPtr<HoneycombTranslatorTestRefPtrClient> val) override {
    return val;
  }

  int SetChildRefPtrClient(
      HoneycombRefPtr<HoneycombTranslatorTestRefPtrClientChild> val) override {
    return val->GetValue();
  }

  HoneycombRefPtr<HoneycombTranslatorTestRefPtrClient> SetChildRefPtrClientAndReturnParent(
      HoneycombRefPtr<HoneycombTranslatorTestRefPtrClientChild> val) override {
    return val;
  }

  // CLIENT-SIDE REFPTR LIST VALUES

  bool SetRefPtrClientList(
      const std::vector<HoneycombRefPtr<HoneycombTranslatorTestRefPtrClient>>& val,
      int val1,
      int val2) override {
    if (val.size() != 2U) {
      return false;
    }
    return val[0]->GetValue() == val1 && val[1]->GetValue() == val2;
  }

  bool GetRefPtrClientListByRef(
      RefPtrClientList& val,
      HoneycombRefPtr<HoneycombTranslatorTestRefPtrClient> val1,
      HoneycombRefPtr<HoneycombTranslatorTestRefPtrClient> val2) override {
    if (val.size() != GetRefPtrClientListSize()) {
      return false;
    }
    val.clear();
    val.push_back(val1);
    val.push_back(val2);
    return true;
  }

  size_t GetRefPtrClientListSize() override { return 2U; }

  // LIBRARY-SIDE OWNPTR VALUES

  HoneycombOwnPtr<HoneycombTranslatorTestScopedLibrary> GetOwnPtrLibrary(int val) override {
    return HoneycombOwnPtr<HoneycombTranslatorTestScopedLibrary>(
        new HoneycombTranslatorTestScopedLibraryChildImpl(val, 0));
  }

  int SetOwnPtrLibrary(HoneycombOwnPtr<HoneycombTranslatorTestScopedLibrary> val) override {
    return val->GetValue();
  }

  HoneycombOwnPtr<HoneycombTranslatorTestScopedLibrary> SetOwnPtrLibraryAndReturn(
      HoneycombOwnPtr<HoneycombTranslatorTestScopedLibrary> val) override {
    return val;
  }

  int SetChildOwnPtrLibrary(
      HoneycombOwnPtr<HoneycombTranslatorTestScopedLibraryChild> val) override {
    return val->GetValue();
  }

  HoneycombOwnPtr<HoneycombTranslatorTestScopedLibrary>
  SetChildOwnPtrLibraryAndReturnParent(
      HoneycombOwnPtr<HoneycombTranslatorTestScopedLibraryChild> val) override {
    return HoneycombOwnPtr<HoneycombTranslatorTestScopedLibrary>(val.release());
  }

  // CLIENT-SIDE OWNPTR VALUES

  int SetOwnPtrClient(HoneycombOwnPtr<HoneycombTranslatorTestScopedClient> val) override {
    return val->GetValue();
  }

  HoneycombOwnPtr<HoneycombTranslatorTestScopedClient> SetOwnPtrClientAndReturn(
      HoneycombOwnPtr<HoneycombTranslatorTestScopedClient> val) override {
    return val;
  }

  int SetChildOwnPtrClient(
      HoneycombOwnPtr<HoneycombTranslatorTestScopedClientChild> val) override {
    return val->GetValue();
  }

  HoneycombOwnPtr<HoneycombTranslatorTestScopedClient> SetChildOwnPtrClientAndReturnParent(
      HoneycombOwnPtr<HoneycombTranslatorTestScopedClientChild> val) override {
    return HoneycombOwnPtr<HoneycombTranslatorTestScopedClient>(val.release());
  }

  // LIBRARY-SIDE RAWPTR VALUES

  int SetRawPtrLibrary(HoneycombRawPtr<HoneycombTranslatorTestScopedLibrary> val) override {
    return val->GetValue();
  }

  int SetChildRawPtrLibrary(
      HoneycombRawPtr<HoneycombTranslatorTestScopedLibraryChild> val) override {
    return val->GetValue();
  }

  // LIBRARY-SIDE RAWPTR LIST VALUES

  bool SetRawPtrLibraryList(
      const std::vector<HoneycombRawPtr<HoneycombTranslatorTestScopedLibrary>>& val,
      int val1,
      int val2) override {
    if (val.size() != 2U) {
      return false;
    }
    return val[0]->GetValue() == val1 && val[1]->GetValue() == val2;
  }

  // CLIENT-SIDE RAWPTR VALUES

  int SetRawPtrClient(HoneycombRawPtr<HoneycombTranslatorTestScopedClient> val) override {
    return val->GetValue();
  }

  int SetChildRawPtrClient(
      HoneycombRawPtr<HoneycombTranslatorTestScopedClientChild> val) override {
    return val->GetValue();
  }

  // CLIENT-SIDE RAWPTR LIST VALUES

  bool SetRawPtrClientList(
      const std::vector<HoneycombRawPtr<HoneycombTranslatorTestScopedClient>>& val,
      int val1,
      int val2) override {
    if (val.size() != 2U) {
      return false;
    }
    return val[0]->GetValue() == val1 && val[1]->GetValue() == val2;
  }

 private:
  IMPLEMENT_REFCOUNTING(HoneycombTranslatorTestImpl);
};

// static
HoneycombRefPtr<HoneycombTranslatorTest> HoneycombTranslatorTest::Create() {
  return new HoneycombTranslatorTestImpl();
}
