// Copyright (c) 2023 Marshall A. Greenblatt. All rights reserved.
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
// This file was generated by the Honeycomb translator tool and should not edited
// by hand. See the translator.README.txt file in the tools directory for
// more information.
//
// $hash=1a2d8806256d04362f181350db2835850cb3e0ae$
//

#ifndef HONEYCOMB_INCLUDE_CAPI_HONEYCOMB_SHARED_PROCESS_MESSAGE_BUILDER_CAPI_H_
#define HONEYCOMB_INCLUDE_CAPI_HONEYCOMB_SHARED_PROCESS_MESSAGE_BUILDER_CAPI_H_
#pragma once

#include "include/capi/honey_process_message_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

///
/// Structure that builds a honey_process_message_t containing a shared memory
/// region. This structure is not thread-safe but may be used exclusively on a
/// different thread from the one which constructed it.
///
typedef struct _honey_shared_process_message_builder_t {
  ///
  /// Base structure.
  ///
  honey_base_ref_counted_t base;

  ///
  /// Returns true (1) if the builder is valid.
  ///
  int(HONEYCOMB_CALLBACK* is_valid)(
      struct _honey_shared_process_message_builder_t* self);

  ///
  /// Returns the size of the shared memory region in bytes. Returns 0 for
  /// invalid instances.
  ///
  size_t(HONEYCOMB_CALLBACK* size)(
      struct _honey_shared_process_message_builder_t* self);

  ///
  /// Returns the pointer to the writable memory. Returns nullptr for invalid
  /// instances. The returned pointer is only valid for the life span of this
  /// object.
  ///
  void*(HONEYCOMB_CALLBACK* memory)(
      struct _honey_shared_process_message_builder_t* self);

  ///
  /// Creates a new honey_process_message_t from the data provided to the builder.
  /// Returns nullptr for invalid instances. Invalidates the builder instance.
  ///
  struct _honey_process_message_t*(HONEYCOMB_CALLBACK* build)(
      struct _honey_shared_process_message_builder_t* self);
} honey_shared_process_message_builder_t;

///
/// Creates a new honey_shared_process_message_builder_t with the specified |name|
/// and shared memory region of specified |byte_size|.
///
HONEYCOMB_EXPORT honey_shared_process_message_builder_t*
honey_shared_process_message_builder_create(const honey_string_t* name,
                                          size_t byte_size);

#ifdef __cplusplus
}
#endif

#endif  // HONEYCOMB_INCLUDE_CAPI_HONEYCOMB_SHARED_PROCESS_MESSAGE_BUILDER_CAPI_H_