// Copyright (c) 2010 Marshall A. Greenblatt. All rights reserved.
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

#ifndef HONEYCOMB_INCLUDE_INTERNAL_HONEYCOMB_STRING_H_
#define HONEYCOMB_INCLUDE_INTERNAL_HONEYCOMB_STRING_H_
#pragma once

// The Honeycomb interface is built with one string type as the default. Comment out
// all but one of the HONEYCOMB_STRING_TYPE_* defines below to specify the default.
// If you change the default you MUST recompile all of Honeycomb.

// Build with the UTF8 string type as default.
// #define HONEYCOMB_STRING_TYPE_UTF8 1

// Build with the UTF16 string type as default.
#define HONEYCOMB_STRING_TYPE_UTF16 1

// Build with the wide string type as default.
// #define HONEYCOMB_STRING_TYPE_WIDE 1

#include "include/internal/honey_string_types.h"

#ifdef __cplusplus
#include "include/internal/honey_string_wrappers.h"
#if defined(HONEYCOMB_STRING_TYPE_UTF16)
typedef HoneycombStringUTF16 HoneycombString;
#elif defined(HONEYCOMB_STRING_TYPE_UTF8)
typedef HoneycombStringUTF8 HoneycombString;
#elif defined(HONEYCOMB_STRING_TYPE_WIDE)
typedef HoneycombStringWide HoneycombString;
#endif
#endif  // __cplusplus

#if defined(HONEYCOMB_STRING_TYPE_UTF8)
typedef char honey_char_t;
typedef honey_string_utf8_t honey_string_t;
typedef honey_string_userfree_utf8_t honey_string_userfree_t;
#define honey_string_set honey_string_utf8_set
#define honey_string_copy honey_string_utf8_copy
#define honey_string_clear honey_string_utf8_clear
#define honey_string_userfree_alloc honey_string_userfree_utf8_alloc
#define honey_string_userfree_free honey_string_userfree_utf8_free
#define honey_string_from_ascii honey_string_utf8_copy
#define honey_string_to_utf8 honey_string_utf8_copy
#define honey_string_from_utf8 honey_string_utf8_copy
#define honey_string_to_utf16 honey_string_utf8_to_utf16
#define honey_string_from_utf16 honey_string_utf16_to_utf8
#define honey_string_to_wide honey_string_utf8_to_wide
#define honey_string_from_wide honey_string_wide_to_utf8
#elif defined(HONEYCOMB_STRING_TYPE_UTF16)
typedef char16_t honey_char_t;
typedef honey_string_userfree_utf16_t honey_string_userfree_t;
typedef honey_string_utf16_t honey_string_t;
#define honey_string_set honey_string_utf16_set
#define honey_string_copy honey_string_utf16_copy
#define honey_string_clear honey_string_utf16_clear
#define honey_string_userfree_alloc honey_string_userfree_utf16_alloc
#define honey_string_userfree_free honey_string_userfree_utf16_free
#define honey_string_from_ascii honey_string_ascii_to_utf16
#define honey_string_to_utf8 honey_string_utf16_to_utf8
#define honey_string_from_utf8 honey_string_utf8_to_utf16
#define honey_string_to_utf16 honey_string_utf16_copy
#define honey_string_from_utf16 honey_string_utf16_copy
#define honey_string_to_wide honey_string_utf16_to_wide
#define honey_string_from_wide honey_string_wide_to_utf16
#elif defined(HONEYCOMB_STRING_TYPE_WIDE)
typedef wchar_t honey_char_t;
typedef honey_string_wide_t honey_string_t;
typedef honey_string_userfree_wide_t honey_string_userfree_t;
#define honey_string_set honey_string_wide_set
#define honey_string_copy honey_string_wide_copy
#define honey_string_clear honey_string_wide_clear
#define honey_string_userfree_alloc honey_string_userfree_wide_alloc
#define honey_string_userfree_free honey_string_userfree_wide_free
#define honey_string_from_ascii honey_string_ascii_to_wide
#define honey_string_to_utf8 honey_string_wide_to_utf8
#define honey_string_from_utf8 honey_string_utf8_to_wide
#define honey_string_to_utf16 honey_string_wide_to_utf16
#define honey_string_from_utf16 honey_string_utf16_to_wide
#define honey_string_to_wide honey_string_wide_copy
#define honey_string_from_wide honey_string_wide_copy
#else
#error Please choose a string type.
#endif

#endif  // HONEYCOMB_INCLUDE_INTERNAL_HONEYCOMB_STRING_H_
