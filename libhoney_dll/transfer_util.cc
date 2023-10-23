// Copyright (c) 2009 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoney_dll/transfer_util.h"

void transfer_string_list_contents(honey_string_list_t fromList,
                                   StringList& toList) {
  size_t size = honey_string_list_size(fromList);
  HoneycombString value;

  for (size_t i = 0; i < size; i++) {
    honey_string_list_value(fromList, i, value.GetWritableStruct());
    toList.push_back(value);
  }
}

void transfer_string_list_contents(const StringList& fromList,
                                   honey_string_list_t toList) {
  size_t size = fromList.size();
  for (size_t i = 0; i < size; ++i) {
    honey_string_list_append(toList, fromList[i].GetStruct());
  }
}

void transfer_string_map_contents(honey_string_map_t fromMap, StringMap& toMap) {
  size_t size = honey_string_map_size(fromMap);
  HoneycombString key, value;

  for (size_t i = 0; i < size; ++i) {
    honey_string_map_key(fromMap, i, key.GetWritableStruct());
    honey_string_map_value(fromMap, i, value.GetWritableStruct());

    toMap.insert(std::make_pair(key, value));
  }
}

void transfer_string_map_contents(const StringMap& fromMap,
                                  honey_string_map_t toMap) {
  StringMap::const_iterator it = fromMap.begin();
  for (; it != fromMap.end(); ++it) {
    honey_string_map_append(toMap, it->first.GetStruct(), it->second.GetStruct());
  }
}

void transfer_string_multimap_contents(honey_string_multimap_t fromMap,
                                       StringMultimap& toMap) {
  size_t size = honey_string_multimap_size(fromMap);
  HoneycombString key, value;

  for (size_t i = 0; i < size; ++i) {
    honey_string_multimap_key(fromMap, i, key.GetWritableStruct());
    honey_string_multimap_value(fromMap, i, value.GetWritableStruct());

    toMap.insert(std::make_pair(key, value));
  }
}

void transfer_string_multimap_contents(const StringMultimap& fromMap,
                                       honey_string_multimap_t toMap) {
  StringMultimap::const_iterator it = fromMap.begin();
  for (; it != fromMap.end(); ++it) {
    honey_string_multimap_append(toMap, it->first.GetStruct(),
                               it->second.GetStruct());
  }
}
