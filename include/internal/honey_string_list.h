// Copyright (c) 2009 Marshall A. Greenblatt. All rights reserved.
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

#ifndef HONEYCOMB_INCLUDE_INTERNAL_HONEYCOMB_STRING_LIST_H_
#define HONEYCOMB_INCLUDE_INTERNAL_HONEYCOMB_STRING_LIST_H_
#pragma once

#include "include/internal/honey_export.h"
#include "include/internal/honey_string.h"

#ifdef __cplusplus
extern "C" {
#endif

///
/// Honeycomb string maps are a set of key/value string pairs.
///
typedef struct _honey_string_list_t* honey_string_list_t;

///
/// Allocate a new string map.
///
HONEYCOMB_EXPORT honey_string_list_t honey_string_list_alloc(void);

///
/// Return the number of elements in the string list.
///
HONEYCOMB_EXPORT size_t honey_string_list_size(honey_string_list_t list);

///
/// Retrieve the value at the specified zero-based string list index. Returns
/// true (1) if the value was successfully retrieved.
///
HONEYCOMB_EXPORT int honey_string_list_value(honey_string_list_t list,
                                     size_t index,
                                     honey_string_t* value);

///
/// Append a new value at the end of the string list.
///
HONEYCOMB_EXPORT void honey_string_list_append(honey_string_list_t list,
                                       const honey_string_t* value);

///
/// Clear the string list.
///
HONEYCOMB_EXPORT void honey_string_list_clear(honey_string_list_t list);

///
/// Free the string list.
///
HONEYCOMB_EXPORT void honey_string_list_free(honey_string_list_t list);

///
/// Creates a copy of an existing string list.
///
HONEYCOMB_EXPORT honey_string_list_t honey_string_list_copy(honey_string_list_t list);

#ifdef __cplusplus
}
#endif

#endif  // HONEYCOMB_INCLUDE_INTERNAL_HONEYCOMB_STRING_LIST_H_
