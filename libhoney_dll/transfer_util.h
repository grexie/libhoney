// Copyright (c) 2009 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_DLL_TRANSFER_UTIL_H_
#define HONEYCOMB_LIBHONEY_DLL_TRANSFER_UTIL_H_
#pragma once

#include <map>
#include <vector>

#include "include/internal/honey_string_list.h"
#include "include/internal/honey_string_map.h"
#include "include/internal/honey_string_multimap.h"

// Copy contents from one list type to another.
using StringList = std::vector<HoneycombString>;
void transfer_string_list_contents(honey_string_list_t fromList,
                                   StringList& toList);
void transfer_string_list_contents(const StringList& fromList,
                                   honey_string_list_t toList);

// Copy contents from one map type to another.
using StringMap = std::map<HoneycombString, HoneycombString>;
void transfer_string_map_contents(honey_string_map_t fromMap, StringMap& toMap);
void transfer_string_map_contents(const StringMap& fromMap,
                                  honey_string_map_t toMap);

// Copy contents from one map type to another.
using StringMultimap = std::multimap<HoneycombString, HoneycombString>;
void transfer_string_multimap_contents(honey_string_multimap_t fromMap,
                                       StringMultimap& toMap);
void transfer_string_multimap_contents(const StringMultimap& fromMap,
                                       honey_string_multimap_t toMap);

#endif  // HONEYCOMB_LIBHONEY_DLL_TRANSFER_UTIL_H_
