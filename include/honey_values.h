// Copyright (c) 2012 Marshall A. Greenblatt. All rights reserved.
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

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_VALUES_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_VALUES_H_
#pragma once

#include <vector>
#include "include/honey_base.h"

class HoneycombBinaryValue;
class HoneycombDictionaryValue;
class HoneycombListValue;

typedef honey_value_type_t HoneycombValueType;

///
/// Class that wraps other data value types. Complex types (binary, dictionary
/// and list) will be referenced but not owned by this object. Can be used on
/// any process and thread.
///
/*--honey(source=library)--*/
class HoneycombValue : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Creates a new object.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombValue> Create();

  ///
  /// Returns true if the underlying data is valid. This will always be true for
  /// simple types. For complex types (binary, dictionary and list) the
  /// underlying data may become invalid if owned by another object (e.g. list
  /// or dictionary) and that other object is then modified or destroyed. This
  /// value object can be re-used by calling Set*() even if the underlying data
  /// is invalid.
  ///
  /*--honey()--*/
  virtual bool IsValid() = 0;

  ///
  /// Returns true if the underlying data is owned by another object.
  ///
  /*--honey()--*/
  virtual bool IsOwned() = 0;

  ///
  /// Returns true if the underlying data is read-only. Some APIs may expose
  /// read-only objects.
  ///
  /*--honey()--*/
  virtual bool IsReadOnly() = 0;

  ///
  /// Returns true if this object and |that| object have the same underlying
  /// data. If true modifications to this object will also affect |that| object
  /// and vice-versa.
  ///
  /*--honey()--*/
  virtual bool IsSame(HoneycombRefPtr<HoneycombValue> that) = 0;

  ///
  /// Returns true if this object and |that| object have an equivalent
  /// underlying value but are not necessarily the same object.
  ///
  /*--honey()--*/
  virtual bool IsEqual(HoneycombRefPtr<HoneycombValue> that) = 0;

  ///
  /// Returns a copy of this object. The underlying data will also be copied.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombValue> Copy() = 0;

  ///
  /// Returns the underlying value type.
  ///
  /*--honey(default_retval=VTYPE_INVALID)--*/
  virtual HoneycombValueType GetType() = 0;

  ///
  /// Returns the underlying value as type bool.
  ///
  /*--honey()--*/
  virtual bool GetBool() = 0;

  ///
  /// Returns the underlying value as type int.
  ///
  /*--honey()--*/
  virtual int GetInt() = 0;

  ///
  /// Returns the underlying value as type double.
  ///
  /*--honey()--*/
  virtual double GetDouble() = 0;

  ///
  /// Returns the underlying value as type string.
  ///
  /*--honey()--*/
  virtual HoneycombString GetString() = 0;

  ///
  /// Returns the underlying value as type binary. The returned reference may
  /// become invalid if the value is owned by another object or if ownership is
  /// transferred to another object in the future. To maintain a reference to
  /// the value after assigning ownership to a dictionary or list pass this
  /// object to the SetValue() method instead of passing the returned reference
  /// to SetBinary().
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombBinaryValue> GetBinary() = 0;

  ///
  /// Returns the underlying value as type dictionary. The returned reference
  /// may become invalid if the value is owned by another object or if ownership
  /// is transferred to another object in the future. To maintain a reference to
  /// the value after assigning ownership to a dictionary or list pass this
  /// object to the SetValue() method instead of passing the returned reference
  /// to SetDictionary().
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombDictionaryValue> GetDictionary() = 0;

  ///
  /// Returns the underlying value as type list. The returned reference may
  /// become invalid if the value is owned by another object or if ownership is
  /// transferred to another object in the future. To maintain a reference to
  /// the value after assigning ownership to a dictionary or list pass this
  /// object to the SetValue() method instead of passing the returned reference
  /// to SetList().
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombListValue> GetList() = 0;

  ///
  /// Sets the underlying value as type null. Returns true if the value was set
  /// successfully.
  ///
  /*--honey()--*/
  virtual bool SetNull() = 0;

  ///
  /// Sets the underlying value as type bool. Returns true if the value was set
  /// successfully.
  ///
  /*--honey()--*/
  virtual bool SetBool(bool value) = 0;

  ///
  /// Sets the underlying value as type int. Returns true if the value was set
  /// successfully.
  ///
  /*--honey()--*/
  virtual bool SetInt(int value) = 0;

  ///
  /// Sets the underlying value as type double. Returns true if the value was
  /// set successfully.
  ///
  /*--honey()--*/
  virtual bool SetDouble(double value) = 0;

  ///
  /// Sets the underlying value as type string. Returns true if the value was
  /// set successfully.
  ///
  /*--honey(optional_param=value)--*/
  virtual bool SetString(const HoneycombString& value) = 0;

  ///
  /// Sets the underlying value as type binary. Returns true if the value was
  /// set successfully. This object keeps a reference to |value| and ownership
  /// of the underlying data remains unchanged.
  ///
  /*--honey()--*/
  virtual bool SetBinary(HoneycombRefPtr<HoneycombBinaryValue> value) = 0;

  ///
  /// Sets the underlying value as type dict. Returns true if the value was set
  /// successfully. This object keeps a reference to |value| and ownership of
  /// the underlying data remains unchanged.
  ///
  /*--honey()--*/
  virtual bool SetDictionary(HoneycombRefPtr<HoneycombDictionaryValue> value) = 0;

  ///
  /// Sets the underlying value as type list. Returns true if the value was set
  /// successfully. This object keeps a reference to |value| and ownership of
  /// the underlying data remains unchanged.
  ///
  /*--honey()--*/
  virtual bool SetList(HoneycombRefPtr<HoneycombListValue> value) = 0;
};

///
/// Class representing a binary value. Can be used on any process and thread.
///
/*--honey(source=library)--*/
class HoneycombBinaryValue : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Creates a new object that is not owned by any other object. The specified
  /// |data| will be copied.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombBinaryValue> Create(const void* data, size_t data_size);

  ///
  /// Returns true if this object is valid. This object may become invalid if
  /// the underlying data is owned by another object (e.g. list or dictionary)
  /// and that other object is then modified or destroyed. Do not call any other
  /// methods if this method returns false.
  ///
  /*--honey()--*/
  virtual bool IsValid() = 0;

  ///
  /// Returns true if this object is currently owned by another object.
  ///
  /*--honey()--*/
  virtual bool IsOwned() = 0;

  ///
  /// Returns true if this object and |that| object have the same underlying
  /// data.
  ///
  /*--honey()--*/
  virtual bool IsSame(HoneycombRefPtr<HoneycombBinaryValue> that) = 0;

  ///
  /// Returns true if this object and |that| object have an equivalent
  /// underlying value but are not necessarily the same object.
  ///
  /*--honey()--*/
  virtual bool IsEqual(HoneycombRefPtr<HoneycombBinaryValue> that) = 0;

  ///
  /// Returns a copy of this object. The data in this object will also be
  /// copied.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombBinaryValue> Copy() = 0;

  ///
  /// Returns the data size.
  ///
  /*--honey()--*/
  virtual size_t GetSize() = 0;

  ///
  /// Read up to |buffer_size| number of bytes into |buffer|. Reading begins at
  /// the specified byte |data_offset|. Returns the number of bytes read.
  ///
  /*--honey()--*/
  virtual size_t GetData(void* buffer,
                         size_t buffer_size,
                         size_t data_offset) = 0;
};

///
/// Class representing a dictionary value. Can be used on any process and
/// thread.
///
/*--honey(source=library)--*/
class HoneycombDictionaryValue : public virtual HoneycombBaseRefCounted {
 public:
  typedef std::vector<HoneycombString> KeyList;

  ///
  /// Creates a new object that is not owned by any other object.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombDictionaryValue> Create();

  ///
  /// Returns true if this object is valid. This object may become invalid if
  /// the underlying data is owned by another object (e.g. list or dictionary)
  /// and that other object is then modified or destroyed. Do not call any other
  /// methods if this method returns false.
  ///
  /*--honey()--*/
  virtual bool IsValid() = 0;

  ///
  /// Returns true if this object is currently owned by another object.
  ///
  /*--honey()--*/
  virtual bool IsOwned() = 0;

  ///
  /// Returns true if the values of this object are read-only. Some APIs may
  /// expose read-only objects.
  ///
  /*--honey()--*/
  virtual bool IsReadOnly() = 0;

  ///
  /// Returns true if this object and |that| object have the same underlying
  /// data. If true modifications to this object will also affect |that| object
  /// and vice-versa.
  ///
  /*--honey()--*/
  virtual bool IsSame(HoneycombRefPtr<HoneycombDictionaryValue> that) = 0;

  ///
  /// Returns true if this object and |that| object have an equivalent
  /// underlying value but are not necessarily the same object.
  ///
  /*--honey()--*/
  virtual bool IsEqual(HoneycombRefPtr<HoneycombDictionaryValue> that) = 0;

  ///
  /// Returns a writable copy of this object. If |exclude_empty_children| is
  /// true any empty dictionaries or lists will be excluded from the copy.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombDictionaryValue> Copy(bool exclude_empty_children) = 0;

  ///
  /// Returns the number of values.
  ///
  /*--honey()--*/
  virtual size_t GetSize() = 0;

  ///
  /// Removes all values. Returns true on success.
  ///
  /*--honey()--*/
  virtual bool Clear() = 0;

  ///
  /// Returns true if the current dictionary has a value for the given key.
  ///
  /*--honey()--*/
  virtual bool HasKey(const HoneycombString& key) = 0;

  ///
  /// Reads all keys for this dictionary into the specified vector.
  ///
  /*--honey()--*/
  virtual bool GetKeys(KeyList& keys) = 0;

  ///
  /// Removes the value at the specified key. Returns true is the value was
  /// removed successfully.
  ///
  /*--honey()--*/
  virtual bool Remove(const HoneycombString& key) = 0;

  ///
  /// Returns the value type for the specified key.
  ///
  /*--honey(default_retval=VTYPE_INVALID)--*/
  virtual HoneycombValueType GetType(const HoneycombString& key) = 0;

  ///
  /// Returns the value at the specified key. For simple types the returned
  /// value will copy existing data and modifications to the value will not
  /// modify this object. For complex types (binary, dictionary and list) the
  /// returned value will reference existing data and modifications to the value
  /// will modify this object.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombValue> GetValue(const HoneycombString& key) = 0;

  ///
  /// Returns the value at the specified key as type bool.
  ///
  /*--honey()--*/
  virtual bool GetBool(const HoneycombString& key) = 0;

  ///
  /// Returns the value at the specified key as type int.
  ///
  /*--honey()--*/
  virtual int GetInt(const HoneycombString& key) = 0;

  ///
  /// Returns the value at the specified key as type double.
  ///
  /*--honey()--*/
  virtual double GetDouble(const HoneycombString& key) = 0;

  ///
  /// Returns the value at the specified key as type string.
  ///
  /*--honey()--*/
  virtual HoneycombString GetString(const HoneycombString& key) = 0;

  ///
  /// Returns the value at the specified key as type binary. The returned
  /// value will reference existing data.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombBinaryValue> GetBinary(const HoneycombString& key) = 0;

  ///
  /// Returns the value at the specified key as type dictionary. The returned
  /// value will reference existing data and modifications to the value will
  /// modify this object.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombDictionaryValue> GetDictionary(const HoneycombString& key) = 0;

  ///
  /// Returns the value at the specified key as type list. The returned value
  /// will reference existing data and modifications to the value will modify
  /// this object.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombListValue> GetList(const HoneycombString& key) = 0;

  ///
  /// Sets the value at the specified key. Returns true if the value was set
  /// successfully. If |value| represents simple data then the underlying data
  /// will be copied and modifications to |value| will not modify this object.
  /// If |value| represents complex data (binary, dictionary or list) then the
  /// underlying data will be referenced and modifications to |value| will
  /// modify this object.
  ///
  /*--honey()--*/
  virtual bool SetValue(const HoneycombString& key, HoneycombRefPtr<HoneycombValue> value) = 0;

  ///
  /// Sets the value at the specified key as type null. Returns true if the
  /// value was set successfully.
  ///
  /*--honey()--*/
  virtual bool SetNull(const HoneycombString& key) = 0;

  ///
  /// Sets the value at the specified key as type bool. Returns true if the
  /// value was set successfully.
  ///
  /*--honey()--*/
  virtual bool SetBool(const HoneycombString& key, bool value) = 0;

  ///
  /// Sets the value at the specified key as type int. Returns true if the
  /// value was set successfully.
  ///
  /*--honey()--*/
  virtual bool SetInt(const HoneycombString& key, int value) = 0;

  ///
  /// Sets the value at the specified key as type double. Returns true if the
  /// value was set successfully.
  ///
  /*--honey()--*/
  virtual bool SetDouble(const HoneycombString& key, double value) = 0;

  ///
  /// Sets the value at the specified key as type string. Returns true if the
  /// value was set successfully.
  ///
  /*--honey(optional_param=value)--*/
  virtual bool SetString(const HoneycombString& key, const HoneycombString& value) = 0;

  ///
  /// Sets the value at the specified key as type binary. Returns true if the
  /// value was set successfully. If |value| is currently owned by another
  /// object then the value will be copied and the |value| reference will not
  /// change. Otherwise, ownership will be transferred to this object and the
  /// |value| reference will be invalidated.
  ///
  /*--honey()--*/
  virtual bool SetBinary(const HoneycombString& key,
                         HoneycombRefPtr<HoneycombBinaryValue> value) = 0;

  ///
  /// Sets the value at the specified key as type dict. Returns true if the
  /// value was set successfully. If |value| is currently owned by another
  /// object then the value will be copied and the |value| reference will not
  /// change. Otherwise, ownership will be transferred to this object and the
  /// |value| reference will be invalidated.
  ///
  /*--honey()--*/
  virtual bool SetDictionary(const HoneycombString& key,
                             HoneycombRefPtr<HoneycombDictionaryValue> value) = 0;

  ///
  /// Sets the value at the specified key as type list. Returns true if the
  /// value was set successfully. If |value| is currently owned by another
  /// object then the value will be copied and the |value| reference will not
  /// change. Otherwise, ownership will be transferred to this object and the
  /// |value| reference will be invalidated.
  ///
  /*--honey()--*/
  virtual bool SetList(const HoneycombString& key, HoneycombRefPtr<HoneycombListValue> value) = 0;
};

///
/// Class representing a list value. Can be used on any process and thread.
///
/*--honey(source=library)--*/
class HoneycombListValue : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Creates a new object that is not owned by any other object.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombListValue> Create();

  ///
  /// Returns true if this object is valid. This object may become invalid if
  /// the underlying data is owned by another object (e.g. list or dictionary)
  /// and that other object is then modified or destroyed. Do not call any other
  /// methods if this method returns false.
  ///
  /*--honey()--*/
  virtual bool IsValid() = 0;

  ///
  /// Returns true if this object is currently owned by another object.
  ///
  /*--honey()--*/
  virtual bool IsOwned() = 0;

  ///
  /// Returns true if the values of this object are read-only. Some APIs may
  /// expose read-only objects.
  ///
  /*--honey()--*/
  virtual bool IsReadOnly() = 0;

  ///
  /// Returns true if this object and |that| object have the same underlying
  /// data. If true modifications to this object will also affect |that| object
  /// and vice-versa.
  ///
  /*--honey()--*/
  virtual bool IsSame(HoneycombRefPtr<HoneycombListValue> that) = 0;

  ///
  /// Returns true if this object and |that| object have an equivalent
  /// underlying value but are not necessarily the same object.
  ///
  /*--honey()--*/
  virtual bool IsEqual(HoneycombRefPtr<HoneycombListValue> that) = 0;

  ///
  /// Returns a writable copy of this object.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombListValue> Copy() = 0;

  ///
  /// Sets the number of values. If the number of values is expanded all
  /// new value slots will default to type null. Returns true on success.
  ///
  /*--honey()--*/
  virtual bool SetSize(size_t size) = 0;

  ///
  /// Returns the number of values.
  ///
  /*--honey()--*/
  virtual size_t GetSize() = 0;

  ///
  /// Removes all values. Returns true on success.
  ///
  /*--honey()--*/
  virtual bool Clear() = 0;

  ///
  /// Removes the value at the specified index.
  ///
  /*--honey()--*/
  virtual bool Remove(size_t index) = 0;

  ///
  /// Returns the value type at the specified index.
  ///
  /*--honey(default_retval=VTYPE_INVALID)--*/
  virtual HoneycombValueType GetType(size_t index) = 0;

  ///
  /// Returns the value at the specified index. For simple types the returned
  /// value will copy existing data and modifications to the value will not
  /// modify this object. For complex types (binary, dictionary and list) the
  /// returned value will reference existing data and modifications to the value
  /// will modify this object.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombValue> GetValue(size_t index) = 0;

  ///
  /// Returns the value at the specified index as type bool.
  ///
  /*--honey()--*/
  virtual bool GetBool(size_t index) = 0;

  ///
  /// Returns the value at the specified index as type int.
  ///
  /*--honey()--*/
  virtual int GetInt(size_t index) = 0;

  ///
  /// Returns the value at the specified index as type double.
  ///
  /*--honey()--*/
  virtual double GetDouble(size_t index) = 0;

  ///
  /// Returns the value at the specified index as type string.
  ///
  /*--honey()--*/
  virtual HoneycombString GetString(size_t index) = 0;

  ///
  /// Returns the value at the specified index as type binary. The returned
  /// value will reference existing data.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombBinaryValue> GetBinary(size_t index) = 0;

  ///
  /// Returns the value at the specified index as type dictionary. The returned
  /// value will reference existing data and modifications to the value will
  /// modify this object.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombDictionaryValue> GetDictionary(size_t index) = 0;

  ///
  /// Returns the value at the specified index as type list. The returned
  /// value will reference existing data and modifications to the value will
  /// modify this object.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombListValue> GetList(size_t index) = 0;

  ///
  /// Sets the value at the specified index. Returns true if the value was set
  /// successfully. If |value| represents simple data then the underlying data
  /// will be copied and modifications to |value| will not modify this object.
  /// If |value| represents complex data (binary, dictionary or list) then the
  /// underlying data will be referenced and modifications to |value| will
  /// modify this object.
  ///
  /*--honey()--*/
  virtual bool SetValue(size_t index, HoneycombRefPtr<HoneycombValue> value) = 0;

  ///
  /// Sets the value at the specified index as type null. Returns true if the
  /// value was set successfully.
  ///
  /*--honey()--*/
  virtual bool SetNull(size_t index) = 0;

  ///
  /// Sets the value at the specified index as type bool. Returns true if the
  /// value was set successfully.
  ///
  /*--honey()--*/
  virtual bool SetBool(size_t index, bool value) = 0;

  ///
  /// Sets the value at the specified index as type int. Returns true if the
  /// value was set successfully.
  ///
  /*--honey()--*/
  virtual bool SetInt(size_t index, int value) = 0;

  ///
  /// Sets the value at the specified index as type double. Returns true if the
  /// value was set successfully.
  ///
  /*--honey()--*/
  virtual bool SetDouble(size_t index, double value) = 0;

  ///
  /// Sets the value at the specified index as type string. Returns true if the
  /// value was set successfully.
  ///
  /*--honey(optional_param=value)--*/
  virtual bool SetString(size_t index, const HoneycombString& value) = 0;

  ///
  /// Sets the value at the specified index as type binary. Returns true if the
  /// value was set successfully. If |value| is currently owned by another
  /// object then the value will be copied and the |value| reference will not
  /// change. Otherwise, ownership will be transferred to this object and the
  /// |value| reference will be invalidated.
  ///
  /*--honey()--*/
  virtual bool SetBinary(size_t index, HoneycombRefPtr<HoneycombBinaryValue> value) = 0;

  ///
  /// Sets the value at the specified index as type dict. Returns true if the
  /// value was set successfully. If |value| is currently owned by another
  /// object then the value will be copied and the |value| reference will not
  /// change. Otherwise, ownership will be transferred to this object and the
  /// |value| reference will be invalidated.
  ///
  /*--honey()--*/
  virtual bool SetDictionary(size_t index,
                             HoneycombRefPtr<HoneycombDictionaryValue> value) = 0;

  ///
  /// Sets the value at the specified index as type list. Returns true if the
  /// value was set successfully. If |value| is currently owned by another
  /// object then the value will be copied and the |value| reference will not
  /// change. Otherwise, ownership will be transferred to this object and the
  /// |value| reference will be invalidated.
  ///
  /*--honey()--*/
  virtual bool SetList(size_t index, HoneycombRefPtr<HoneycombListValue> value) = 0;
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_VALUES_H_
