// Copyright (c) 2011 Marshall A. Greenblatt. All rights reserved.
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

#ifndef HONEYCOMB_INCLUDE_INTERNAL_HONEYCOMB_STRING_MULTIMAP_H_
#define HONEYCOMB_INCLUDE_INTERNAL_HONEYCOMB_STRING_MULTIMAP_H_
#pragma once

#include "include/internal/honey_export.h"
#include "include/internal/honey_string.h"

#ifdef __cplusplus
extern "C" {
#endif

///
/// Honeycomb string multimaps are a set of key/value string pairs.
/// More than one value can be assigned to a single key.
///
typedef struct _honey_string_multimap_t* honey_string_multimap_t;

///
/// Allocate a new string multimap.
///
HONEYCOMB_EXPORT honey_string_multimap_t honey_string_multimap_alloc(void);

///
/// Return the number of elements in the string multimap.
///
HONEYCOMB_EXPORT size_t honey_string_multimap_size(honey_string_multimap_t map);

///
/// Return the number of values with the specified key.
///
HONEYCOMB_EXPORT size_t honey_string_multimap_find_count(honey_string_multimap_t map,
                                                 const honey_string_t* key);

///
/// Return the value_index-th value with the specified key.
///
HONEYCOMB_EXPORT int honey_string_multimap_enumerate(honey_string_multimap_t map,
                                             const honey_string_t* key,
                                             size_t value_index,
                                             honey_string_t* value);

///
/// Return the key at the specified zero-based string multimap index.
///
HONEYCOMB_EXPORT int honey_string_multimap_key(honey_string_multimap_t map,
                                       size_t index,
                                       honey_string_t* key);

///
/// Return the value at the specified zero-based string multimap index.
///
HONEYCOMB_EXPORT int honey_string_multimap_value(honey_string_multimap_t map,
                                         size_t index,
                                         honey_string_t* value);

///
/// Append a new key/value pair at the end of the string multimap.
///
HONEYCOMB_EXPORT int honey_string_multimap_append(honey_string_multimap_t map,
                                          const honey_string_t* key,
                                          const honey_string_t* value);

///
/// Clear the string multimap.
///
HONEYCOMB_EXPORT void honey_string_multimap_clear(honey_string_multimap_t map);

///
/// Free the string multimap.
///
HONEYCOMB_EXPORT void honey_string_multimap_free(honey_string_multimap_t map);

#ifdef __cplusplus
}
#endif

#endif  // HONEYCOMB_INCLUDE_INTERNAL_HONEYCOMB_STRING_MULTIMAP_H_
