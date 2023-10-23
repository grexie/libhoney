// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_COMMON_NET_HTTP_HEADER_UTILS_H_
#define HONEYCOMB_LIBHONEYCOMB_COMMON_NET_HTTP_HEADER_UTILS_H_
#pragma once

#include <map>
#include <string>

#include "include/honey_base.h"

namespace HttpHeaderUtils {

using HeaderMap = std::multimap<HoneycombString, HoneycombString>;

std::string GenerateHeaders(const HeaderMap& map);
void ParseHeaders(const std::string& header_str, HeaderMap& map);

// Convert |str| to lower-case.
void MakeASCIILower(std::string* str);

// Finds the first instance of |name| (already lower-case) in |map| with
// case-insensitive comparison.
HeaderMap::iterator FindHeaderInMap(const std::string& name, HeaderMap& map);

}  // namespace HttpHeaderUtils

#endif  // HONEYCOMB_LIBHONEYCOMB_COMMON_NET_HTTP_HEADER_UTILS_H_
