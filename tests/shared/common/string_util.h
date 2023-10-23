// Copyright (c) 2022 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_SHARED_COMMON_STRING_UTIL_H_
#define HONEYCOMB_TESTS_SHARED_COMMON_STRING_UTIL_H_
#pragma once

#include <string>

namespace client {

// Convert |str| to lowercase.
std::string AsciiStrToLower(const std::string& str);

// Replace all instances of |from| with |to| in |str|.
std::string AsciiStrReplace(const std::string& str,
                            const std::string& from,
                            const std::string& to);

}  // namespace client

#endif  // HONEYCOMB_TESTS_SHARED_COMMON_STRING_UTIL_H_
