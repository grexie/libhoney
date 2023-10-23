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
// The contents of this file are only available to applications that link
// against the libhoneycomb_dll_wrapper target.
//

#ifndef HONEYCOMB_INCLUDE_WRAPPER_HONEYCOMB_STREAM_RESOURCE_HANDLER_H_
#define HONEYCOMB_INCLUDE_WRAPPER_HONEYCOMB_STREAM_RESOURCE_HANDLER_H_
#pragma once

#include "include/honey_resource_handler.h"
#include "include/honey_response.h"
#include "include/honey_stream.h"

///
/// Implementation of the HoneycombResourceHandler class for reading from a HoneycombStream.
///
class HoneycombStreamResourceHandler : public HoneycombResourceHandler {
 public:
  ///
  /// Create a new object with default response values.
  ///
  HoneycombStreamResourceHandler(const HoneycombString& mime_type,
                           HoneycombRefPtr<HoneycombStreamReader> stream);
  ///
  /// Create a new object with explicit response values.
  ///
  HoneycombStreamResourceHandler(int status_code,
                           const HoneycombString& status_text,
                           const HoneycombString& mime_type,
                           HoneycombResponse::HeaderMap header_map,
                           HoneycombRefPtr<HoneycombStreamReader> stream);

  HoneycombStreamResourceHandler(const HoneycombStreamResourceHandler&) = delete;
  HoneycombStreamResourceHandler& operator=(const HoneycombStreamResourceHandler&) = delete;

  // HoneycombResourceHandler methods.
  bool Open(HoneycombRefPtr<HoneycombRequest> request,
            bool& handle_request,
            HoneycombRefPtr<HoneycombCallback> callback) override;
  void GetResponseHeaders(HoneycombRefPtr<HoneycombResponse> response,
                          int64_t& response_length,
                          HoneycombString& redirectUrl) override;
  bool Read(void* data_out,
            int bytes_to_read,
            int& bytes_read,
            HoneycombRefPtr<HoneycombResourceReadCallback> callback) override;
  void Cancel() override;

 private:
  const int status_code_;
  const HoneycombString status_text_;
  const HoneycombString mime_type_;
  const HoneycombResponse::HeaderMap header_map_;
  const HoneycombRefPtr<HoneycombStreamReader> stream_;

  IMPLEMENT_REFCOUNTING(HoneycombStreamResourceHandler);
};

#endif  // HONEYCOMB_INCLUDE_WRAPPER_HONEYCOMB_STREAM_RESOURCE_HANDLER_H_
