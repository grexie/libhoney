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
// $hash=99dff3042ea437ecf5771eff9b3cab4c22190534$
//

#ifndef HONEYCOMB_INCLUDE_CAPI_HONEYCOMB_SSL_INFO_CAPI_H_
#define HONEYCOMB_INCLUDE_CAPI_HONEYCOMB_SSL_INFO_CAPI_H_
#pragma once

#include "include/capi/honey_base_capi.h"
#include "include/capi/honey_values_capi.h"
#include "include/capi/honey_x509_certificate_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

///
/// Structure representing SSL information.
///
typedef struct _honey_sslinfo_t {
  ///
  /// Base structure.
  ///
  honey_base_ref_counted_t base;

  ///
  /// Returns a bitmask containing any and all problems verifying the server
  /// certificate.
  ///
  honey_cert_status_t(HONEYCOMB_CALLBACK* get_cert_status)(struct _honey_sslinfo_t* self);

  ///
  /// Returns the X.509 certificate.
  ///
  struct _honey_x509certificate_t*(HONEYCOMB_CALLBACK* get_x509certificate)(
      struct _honey_sslinfo_t* self);
} honey_sslinfo_t;

///
/// Returns true (1) if the certificate status represents an error.
///
HONEYCOMB_EXPORT int honey_is_cert_status_error(honey_cert_status_t status);

#ifdef __cplusplus
}
#endif

#endif  // HONEYCOMB_INCLUDE_CAPI_HONEYCOMB_SSL_INFO_CAPI_H_
