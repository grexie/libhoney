// Copyright (c) 2015 Marshall A. Greenblatt. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the name Chromium Embedded
// Framework nor the names of its contributors may be used to endorse
// or promote products derived from this software without specific prior
// written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ---------------------------------------------------------------------------
//
// The contents of this file must follow a specific format in order to
// support the Honeycomb translator tool. See the translator.README.txt file in the
// tools directory for more information.
//
// THIS FILE IS FOR TESTING PURPOSES ONLY.
//
// The APIs defined in this file are for testing purposes only. They should only
// be included from unit test targets.
//

#ifndef HONEYCOMB_INCLUDE_TEST_HONEYCOMB_TEST_H_
#define HONEYCOMB_INCLUDE_TEST_HONEYCOMB_TEST_H_
#pragma once

#if !defined(BUILDING_HONEYCOMB_SHARED) && !defined(WRAPPING_HONEYCOMB_SHARED) && \
    !defined(UNIT_TEST)
#error This file can be included for unit tests only
#endif

#include <map>
#include <vector>

#include "include/honey_base.h"

class HoneycombTranslatorTestRefPtrClient;
class HoneycombTranslatorTestRefPtrClientChild;
class HoneycombTranslatorTestRefPtrLibrary;
class HoneycombTranslatorTestRefPtrLibraryChild;
class HoneycombTranslatorTestScopedClient;
class HoneycombTranslatorTestScopedClientChild;
class HoneycombTranslatorTestScopedLibrary;
class HoneycombTranslatorTestScopedLibraryChild;

// Test values.
#define TEST_INT_VAL 5
#define TEST_INT_VAL2 60
#define TEST_BOOL_VAL true
#define TEST_DOUBLE_VAL 4.543
#define TEST_LONG_VAL -65
#define TEST_SIZET_VAL 3U
#define TEST_STRING_VAL "My test string"
#define TEST_STRING_VAL2 "My 2nd test string"
#define TEST_STRING_VAL3 "My 3rd test string"
#define TEST_STRING_KEY "key0"
#define TEST_STRING_KEY2 "key1"
#define TEST_STRING_KEY3 "key2"
#define TEST_X_VAL 44
#define TEST_Y_VAL 754
#define TEST_X_VAL2 900
#define TEST_Y_VAL2 300

///
/// Class for testing all of the possible data transfer types.
///
/*--honey(source=library)--*/
class HoneycombTranslatorTest : public HoneycombBaseRefCounted {
 public:
  ///
  /// Create the test object.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombTranslatorTest> Create();

  // PRIMITIVE VALUES

  ///
  /// Return a void value.
  ///
  /*--honey()--*/
  virtual void GetVoid() = 0;

  ///
  /// Return a bool value.
  ///
  /*--honey()--*/
  virtual bool GetBool() = 0;

  ///
  /// Return an int value.
  ///
  /*--honey()--*/
  virtual int GetInt() = 0;

  ///
  /// Return a double value.
  ///
  /*--honey()--*/
  virtual double GetDouble() = 0;

  ///
  /// Return a long value.
  ///
  /*--honey()--*/
  virtual long GetLong() = 0;

  ///
  /// Return a size_t value.
  ///
  /*--honey()--*/
  virtual size_t GetSizet() = 0;

  ///
  /// Set a void value.
  ///
  /*--honey()--*/
  virtual bool SetVoid() = 0;

  ///
  /// Set a bool value.
  ///
  /*--honey()--*/
  virtual bool SetBool(bool val) = 0;

  ///
  /// Set an int value.
  ///
  /*--honey()--*/
  virtual bool SetInt(int val) = 0;

  ///
  /// Set a double value.
  ///
  /*--honey()--*/
  virtual bool SetDouble(double val) = 0;

  ///
  /// Set a long value.
  ///
  /*--honey()--*/
  virtual bool SetLong(long val) = 0;

  ///
  /// Set a size_t value.
  ///
  /*--honey()--*/
  virtual bool SetSizet(size_t val) = 0;

  // PRIMITIVE LIST VALUES

  // Test both with and without a typedef.
  typedef std::vector<int> IntList;

  ///
  /// Set a int list value.
  ///
  /*--honey()--*/
  virtual bool SetIntList(const std::vector<int>& val) = 0;

  ///
  /// Return an int list value by out-param.
  ///
  /*--honey(count_func=val:GetIntListSize)--*/
  virtual bool GetIntListByRef(IntList& val) = 0;

  ///
  /// Return the number of points that will be output above.
  ///
  /*--honey()--*/
  virtual size_t GetIntListSize() = 0;

  // STRING VALUES

  ///
  /// Return a string value.
  ///
  /*--honey()--*/
  virtual HoneycombString GetString() = 0;

  ///
  /// Set a string value.
  ///
  /*--honey()--*/
  virtual bool SetString(const HoneycombString& val) = 0;

  ///
  /// Return a string value by out-param.
  ///
  /*--honey()--*/
  virtual void GetStringByRef(HoneycombString& val) = 0;

  // STRING LIST VALUES

  // Test both with and without a typedef.
  typedef std::vector<HoneycombString> StringList;

  ///
  /// Set a string list value.
  ///
  /*--honey()--*/
  virtual bool SetStringList(const std::vector<HoneycombString>& val) = 0;

  ///
  /// Return a string list value by out-param.
  ///
  /*--honey()--*/
  virtual bool GetStringListByRef(StringList& val) = 0;

  // STRING MAP VALUES

  // Test both with and without a typedef.
  typedef std::map<HoneycombString, HoneycombString> StringMap;

  ///
  /// Set a string map value.
  ///
  /*--honey()--*/
  virtual bool SetStringMap(const StringMap& val) = 0;

  ///
  /// Return a string map value by out-param.
  ///
  /*--honey()--*/
  virtual bool GetStringMapByRef(std::map<HoneycombString, HoneycombString>& val) = 0;

  // STRING MULTIMAP VALUES

  // Test both with and without a typedef.
  typedef std::multimap<HoneycombString, HoneycombString> StringMultimap;

  ///
  /// Set a string multimap value.
  ///
  /*--honey()--*/
  virtual bool SetStringMultimap(
      const std::multimap<HoneycombString, HoneycombString>& val) = 0;

  ///
  /// Return a string multimap value by out-param.
  ///
  /*--honey()--*/
  virtual bool GetStringMultimapByRef(StringMultimap& val) = 0;

  // STRUCT VALUES

  ///
  /// Return a point value.
  ///
  /*--honey()--*/
  virtual HoneycombPoint GetPoint() = 0;

  ///
  /// Set a point value.
  ///
  /*--honey()--*/
  virtual bool SetPoint(const HoneycombPoint& val) = 0;

  ///
  /// Return a point value by out-param.
  ///
  /*--honey()--*/
  virtual void GetPointByRef(HoneycombPoint& val) = 0;

  // STRUCT LIST VALUES

  // Test both with and without a typedef.
  typedef std::vector<HoneycombPoint> PointList;

  ///
  /// Set a point list vlaue.
  ///
  /*--honey()--*/
  virtual bool SetPointList(const std::vector<HoneycombPoint>& val) = 0;

  ///
  /// Return a point list value by out-param.
  ///
  /*--honey(count_func=val:GetPointListSize)--*/
  virtual bool GetPointListByRef(PointList& val) = 0;

  ///
  /// Return the number of points that will be output above.
  ///
  /*--honey()--*/
  virtual size_t GetPointListSize() = 0;

  // LIBRARY-SIDE REFPTR VALUES

  ///
  /// Return an new library-side object.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibrary> GetRefPtrLibrary(
      int val) = 0;

  ///
  /// Set an object. Returns the value from
  /// HoneycombTranslatorTestRefPtrLibrary::GetValue().
  /// This tests input and execution of a library-side object type.
  ///
  /*--honey()--*/
  virtual int SetRefPtrLibrary(
      HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibrary> val) = 0;

  ///
  /// Set an object. Returns the object passed in. This tests input and output
  /// of a library-side object type.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibrary> SetRefPtrLibraryAndReturn(
      HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibrary> val) = 0;

  ///
  /// Set a child object. Returns the value from
  /// HoneycombTranslatorTestRefPtrLibrary::GetValue(). This tests input of a library-
  /// side child object type and execution as the parent type.
  ///
  /*--honey()--*/
  virtual int SetChildRefPtrLibrary(
      HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibraryChild> val) = 0;

  ///
  /// Set a child object. Returns the object as the parent type. This tests
  /// input of a library-side child object type and return as the parent type.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibrary>
  SetChildRefPtrLibraryAndReturnParent(
      HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibraryChild> val) = 0;

  // LIBRARY-SIDE REFPTR LIST VALUES

  // Test both with and without a typedef.
  typedef std::vector<HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibrary>>
      RefPtrLibraryList;

  ///
  /// Set an object list vlaue.
  ///
  /*--honey()--*/
  virtual bool SetRefPtrLibraryList(
      const std::vector<HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibrary>>& val,
      int val1,
      int val2) = 0;

  ///
  /// Return an object list value by out-param.
  ///
  /*--honey(count_func=val:GetRefPtrLibraryListSize)--*/
  virtual bool GetRefPtrLibraryListByRef(RefPtrLibraryList& val,
                                         int val1,
                                         int val2) = 0;

  ///
  /// Return the number of object that will be output above.
  ///
  /*--honey()--*/
  virtual size_t GetRefPtrLibraryListSize() = 0;

  // CLIENT-SIDE REFPTR VALUES

  ///
  /// Set an object. Returns the value from
  /// HoneycombTranslatorTestRefPtrClient::GetValue().
  /// This tests input and execution of a client-side object type.
  ///
  /*--honey()--*/
  virtual int SetRefPtrClient(HoneycombRefPtr<HoneycombTranslatorTestRefPtrClient> val) = 0;

  ///
  /// Set an object. Returns the handler passed in. This tests input and output
  /// of a client-side object type.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombTranslatorTestRefPtrClient> SetRefPtrClientAndReturn(
      HoneycombRefPtr<HoneycombTranslatorTestRefPtrClient> val) = 0;

  ///
  /// Set a child object. Returns the value from
  /// HoneycombTranslatorTestRefPtrClient::GetValue(). This tests input of a client-
  /// side child object type and execution as the parent type.
  ///
  /*--honey()--*/
  virtual int SetChildRefPtrClient(
      HoneycombRefPtr<HoneycombTranslatorTestRefPtrClientChild> val) = 0;

  ///
  /// Set a child object. Returns the object as the parent type. This tests
  /// input of a client-side child object type and return as the parent type.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombTranslatorTestRefPtrClient>
  SetChildRefPtrClientAndReturnParent(
      HoneycombRefPtr<HoneycombTranslatorTestRefPtrClientChild> val) = 0;

  // CLIENT-SIDE REFPTR LIST VALUES

  // Test both with and without a typedef.
  typedef std::vector<HoneycombRefPtr<HoneycombTranslatorTestRefPtrClient>>
      RefPtrClientList;

  ///
  /// Set an object list vlaue.
  ///
  /*--honey()--*/
  virtual bool SetRefPtrClientList(
      const std::vector<HoneycombRefPtr<HoneycombTranslatorTestRefPtrClient>>& val,
      int val1,
      int val2) = 0;

  ///
  /// Return an object list value by out-param.
  ///
  /*--honey(count_func=val:GetRefPtrLibraryListSize)--*/
  virtual bool GetRefPtrClientListByRef(
      RefPtrClientList& val,
      HoneycombRefPtr<HoneycombTranslatorTestRefPtrClient> val1,
      HoneycombRefPtr<HoneycombTranslatorTestRefPtrClient> val2) = 0;

  ///
  /// Return the number of object that will be output above.
  ///
  /*--honey()--*/
  virtual size_t GetRefPtrClientListSize() = 0;

  // LIBRARY-SIDE OWNPTR VALUES

  ///
  /// Return an new library-side object.
  ///
  /*--honey()--*/
  virtual HoneycombOwnPtr<HoneycombTranslatorTestScopedLibrary> GetOwnPtrLibrary(
      int val) = 0;

  ///
  /// Set an object. Returns the value from
  /// HoneycombTranslatorTestScopedLibrary::GetValue().
  /// This tests input and execution of a library-side object type.
  ///
  /*--honey()--*/
  virtual int SetOwnPtrLibrary(
      HoneycombOwnPtr<HoneycombTranslatorTestScopedLibrary> val) = 0;

  ///
  /// Set an object. Returns the object passed in. This tests input and output
  /// of a library-side object type.
  ///
  /*--honey()--*/
  virtual HoneycombOwnPtr<HoneycombTranslatorTestScopedLibrary> SetOwnPtrLibraryAndReturn(
      HoneycombOwnPtr<HoneycombTranslatorTestScopedLibrary> val) = 0;

  ///
  /// Set a child object. Returns the value from
  /// HoneycombTranslatorTestScopedLibrary::GetValue(). This tests input of a library-
  /// side child object type and execution as the parent type.
  ///
  /*--honey()--*/
  virtual int SetChildOwnPtrLibrary(
      HoneycombOwnPtr<HoneycombTranslatorTestScopedLibraryChild> val) = 0;

  ///
  /// Set a child object. Returns the object as the parent type. This tests
  /// input of a library-side child object type and return as the parent type.
  ///
  /*--honey()--*/
  virtual HoneycombOwnPtr<HoneycombTranslatorTestScopedLibrary>
  SetChildOwnPtrLibraryAndReturnParent(
      HoneycombOwnPtr<HoneycombTranslatorTestScopedLibraryChild> val) = 0;

  // CLIENT-SIDE OWNPTR VALUES

  ///
  /// Set an object. Returns the value from
  /// HoneycombTranslatorTestScopedClient::GetValue().
  /// This tests input and execution of a client-side object type.
  ///
  /*--honey()--*/
  virtual int SetOwnPtrClient(HoneycombOwnPtr<HoneycombTranslatorTestScopedClient> val) = 0;

  ///
  /// Set an object. Returns the handler passed in. This tests input and output
  /// of a client-side object type.
  ///
  /*--honey()--*/
  virtual HoneycombOwnPtr<HoneycombTranslatorTestScopedClient> SetOwnPtrClientAndReturn(
      HoneycombOwnPtr<HoneycombTranslatorTestScopedClient> val) = 0;

  ///
  /// Set a child object. Returns the value from
  /// HoneycombTranslatorTestScopedClient::GetValue(). This tests input of a client-
  /// side child object type and execution as the parent type.
  ///
  /*--honey()--*/
  virtual int SetChildOwnPtrClient(
      HoneycombOwnPtr<HoneycombTranslatorTestScopedClientChild> val) = 0;

  ///
  /// Set a child object. Returns the object as the parent type. This tests
  /// input of a client-side child object type and return as the parent type.
  ///
  /*--honey()--*/
  virtual HoneycombOwnPtr<HoneycombTranslatorTestScopedClient>
  SetChildOwnPtrClientAndReturnParent(
      HoneycombOwnPtr<HoneycombTranslatorTestScopedClientChild> val) = 0;

  // LIBRARY-SIDE RAWPTR VALUES

  ///
  /// Set an object. Returns the value from
  /// HoneycombTranslatorTestScopedLibrary::GetValue().
  /// This tests input and execution of a library-side object type.
  ///
  /*--honey()--*/
  virtual int SetRawPtrLibrary(
      HoneycombRawPtr<HoneycombTranslatorTestScopedLibrary> val) = 0;

  ///
  /// Set a child object. Returns the value from
  /// HoneycombTranslatorTestScopedLibrary::GetValue(). This tests input of a library-
  /// side child object type and execution as the parent type.
  ///
  /*--honey()--*/
  virtual int SetChildRawPtrLibrary(
      HoneycombRawPtr<HoneycombTranslatorTestScopedLibraryChild> val) = 0;

  // LIBRARY-SIDE RAWPTR LIST VALUES

  // Test both with and without a typedef.
  typedef std::vector<HoneycombRawPtr<HoneycombTranslatorTestScopedLibrary>>
      RawPtrLibraryList;

  ///
  /// Set an object list vlaue.
  ///
  /*--honey()--*/
  virtual bool SetRawPtrLibraryList(
      const std::vector<HoneycombRawPtr<HoneycombTranslatorTestScopedLibrary>>& val,
      int val1,
      int val2) = 0;

  // CLIENT-SIDE RAWPTR VALUES

  ///
  /// Set an object. Returns the value from
  /// HoneycombTranslatorTestScopedClient::GetValue().
  /// This tests input and execution of a client-side object type.
  ///
  /*--honey()--*/
  virtual int SetRawPtrClient(HoneycombRawPtr<HoneycombTranslatorTestScopedClient> val) = 0;

  ///
  /// Set a child object. Returns the value from
  /// HoneycombTranslatorTestScopedClient::GetValue(). This tests input of a client-
  /// side child object type and execution as the parent type.
  ///
  /*--honey()--*/
  virtual int SetChildRawPtrClient(
      HoneycombRawPtr<HoneycombTranslatorTestScopedClientChild> val) = 0;

  // CLIENT-SIDE RAWPTR LIST VALUES

  // Test both with and without a typedef.
  typedef std::vector<HoneycombRawPtr<HoneycombTranslatorTestScopedClient>>
      RawPtrClientList;

  ///
  /// Set an object list vlaue.
  ///
  /*--honey()--*/
  virtual bool SetRawPtrClientList(
      const std::vector<HoneycombRawPtr<HoneycombTranslatorTestScopedClient>>& val,
      int val1,
      int val2) = 0;
};

///
/// Library-side test object for RefPtr.
///
/*--honey(source=library)--*/
class HoneycombTranslatorTestRefPtrLibrary : public HoneycombBaseRefCounted {
 public:
  ///
  /// Create the test object.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibrary> Create(int value);

  ///
  /// Return a value.
  ///
  /*--honey()--*/
  virtual int GetValue() = 0;

  ///
  /// Set a value.
  ///
  /*--honey()--*/
  virtual void SetValue(int value) = 0;
};

///
/// Library-side child test object for RefPtr.
///
/*--honey(source=library)--*/
class HoneycombTranslatorTestRefPtrLibraryChild
    : public HoneycombTranslatorTestRefPtrLibrary {
 public:
  ///
  /// Create the test object.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibraryChild> Create(int value,
                                                               int other_value);

  ///
  /// Return a value.
  ///
  /*--honey()--*/
  virtual int GetOtherValue() = 0;

  ///
  /// Set a value.
  ///
  /*--honey()--*/
  virtual void SetOtherValue(int value) = 0;
};

///
/// Another library-side child test object for RefPtr.
///
/*--honey(source=library)--*/
class HoneycombTranslatorTestRefPtrLibraryChildChild
    : public HoneycombTranslatorTestRefPtrLibraryChild {
 public:
  ///
  /// Create the test object.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombTranslatorTestRefPtrLibraryChildChild>
  Create(int value, int other_value, int other_other_value);

  ///
  /// Return a value.
  ///
  /*--honey()--*/
  virtual int GetOtherOtherValue() = 0;

  ///
  /// Set a value.
  ///
  /*--honey()--*/
  virtual void SetOtherOtherValue(int value) = 0;
};

///
/// Client-side test object for RefPtr.
///
/*--honey(source=client)--*/
class HoneycombTranslatorTestRefPtrClient : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Return a value.
  ///
  /*--honey()--*/
  virtual int GetValue() = 0;
};

///
/// Client-side child test object for RefPtr.
///
/*--honey(source=client)--*/
class HoneycombTranslatorTestRefPtrClientChild
    : public HoneycombTranslatorTestRefPtrClient {
 public:
  ///
  /// Return a value.
  ///
  /*--honey()--*/
  virtual int GetOtherValue() = 0;
};

///
/// Library-side test object for OwnPtr/RawPtr.
///
/*--honey(source=library)--*/
class HoneycombTranslatorTestScopedLibrary : public HoneycombBaseScoped {
 public:
  ///
  /// Create the test object.
  ///
  /*--honey()--*/
  static HoneycombOwnPtr<HoneycombTranslatorTestScopedLibrary> Create(int value);

  ///
  /// Return a value.
  ///
  /*--honey()--*/
  virtual int GetValue() = 0;

  ///
  /// Set a value.
  ///
  /*--honey()--*/
  virtual void SetValue(int value) = 0;
};

///
/// Library-side child test object for OwnPtr/RawPtr.
///
/*--honey(source=library)--*/
class HoneycombTranslatorTestScopedLibraryChild
    : public HoneycombTranslatorTestScopedLibrary {
 public:
  ///
  /// Create the test object.
  ///
  /*--honey()--*/
  static HoneycombOwnPtr<HoneycombTranslatorTestScopedLibraryChild> Create(int value,
                                                               int other_value);

  ///
  /// Return a value.
  ///
  /*--honey()--*/
  virtual int GetOtherValue() = 0;

  ///
  /// Set a value.
  ///
  /*--honey()--*/
  virtual void SetOtherValue(int value) = 0;
};

///
/// Another library-side child test object for OwnPtr/RawPtr.
///
/*--honey(source=library)--*/
class HoneycombTranslatorTestScopedLibraryChildChild
    : public HoneycombTranslatorTestScopedLibraryChild {
 public:
  ///
  /// Create the test object.
  ///
  /*--honey()--*/
  static HoneycombOwnPtr<HoneycombTranslatorTestScopedLibraryChildChild>
  Create(int value, int other_value, int other_other_value);

  ///
  /// Return a value.
  ///
  /*--honey()--*/
  virtual int GetOtherOtherValue() = 0;

  ///
  /// Set a value.
  ///
  /*--honey()--*/
  virtual void SetOtherOtherValue(int value) = 0;
};

///
/// Client-side test object for OwnPtr/RawPtr.
///
/*--honey(source=client)--*/
class HoneycombTranslatorTestScopedClient : public virtual HoneycombBaseScoped {
 public:
  ///
  /// Return a value.
  ///
  /*--honey()--*/
  virtual int GetValue() = 0;
};

///
/// Client-side child test object for OwnPtr/RawPtr.
///
/*--honey(source=client)--*/
class HoneycombTranslatorTestScopedClientChild
    : public HoneycombTranslatorTestScopedClient {
 public:
  ///
  /// Return a value.
  ///
  /*--honey()--*/
  virtual int GetOtherValue() = 0;
};

#endif  // HONEYCOMB_INCLUDE_TEST_HONEYCOMB_TEST_H_
