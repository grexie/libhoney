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
// $hash=46508464579e797d4684f4a7facdb39f9bdb312b$
//

#ifndef HONEYCOMB_INCLUDE_CAPI_HONEYCOMB_PRINT_SETTINGS_CAPI_H_
#define HONEYCOMB_INCLUDE_CAPI_HONEYCOMB_PRINT_SETTINGS_CAPI_H_
#pragma once

#include "include/capi/honey_base_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

///
/// Structure representing print settings.
///
typedef struct _honey_print_settings_t {
  ///
  /// Base structure.
  ///
  honey_base_ref_counted_t base;

  ///
  /// Returns true (1) if this object is valid. Do not call any other functions
  /// if this function returns false (0).
  ///
  int(HONEYCOMB_CALLBACK* is_valid)(struct _honey_print_settings_t* self);

  ///
  /// Returns true (1) if the values of this object are read-only. Some APIs may
  /// expose read-only objects.
  ///
  int(HONEYCOMB_CALLBACK* is_read_only)(struct _honey_print_settings_t* self);

  ///
  /// Set the page orientation.
  ///
  void(HONEYCOMB_CALLBACK* set_orientation)(struct _honey_print_settings_t* self,
                                      int landscape);

  ///
  /// Returns true (1) if the orientation is landscape.
  ///
  int(HONEYCOMB_CALLBACK* is_landscape)(struct _honey_print_settings_t* self);

  ///
  /// Set the printer printable area in device units. Some platforms already
  /// provide flipped area. Set |landscape_needs_flip| to false (0) on those
  /// platforms to avoid double flipping.
  ///
  void(HONEYCOMB_CALLBACK* set_printer_printable_area)(
      struct _honey_print_settings_t* self,
      const honey_size_t* physical_size_device_units,
      const honey_rect_t* printable_area_device_units,
      int landscape_needs_flip);

  ///
  /// Set the device name.
  ///
  void(HONEYCOMB_CALLBACK* set_device_name)(struct _honey_print_settings_t* self,
                                      const honey_string_t* name);

  ///
  /// Get the device name.
  ///
  // The resulting string must be freed by calling honey_string_userfree_free().
  honey_string_userfree_t(HONEYCOMB_CALLBACK* get_device_name)(
      struct _honey_print_settings_t* self);

  ///
  /// Set the DPI (dots per inch).
  ///
  void(HONEYCOMB_CALLBACK* set_dpi)(struct _honey_print_settings_t* self, int dpi);

  ///
  /// Get the DPI (dots per inch).
  ///
  int(HONEYCOMB_CALLBACK* get_dpi)(struct _honey_print_settings_t* self);

  ///
  /// Set the page ranges.
  ///
  void(HONEYCOMB_CALLBACK* set_page_ranges)(struct _honey_print_settings_t* self,
                                      size_t rangesCount,
                                      honey_range_t const* ranges);

  ///
  /// Returns the number of page ranges that currently exist.
  ///
  size_t(HONEYCOMB_CALLBACK* get_page_ranges_count)(
      struct _honey_print_settings_t* self);

  ///
  /// Retrieve the page ranges.
  ///
  void(HONEYCOMB_CALLBACK* get_page_ranges)(struct _honey_print_settings_t* self,
                                      size_t* rangesCount,
                                      honey_range_t* ranges);

  ///
  /// Set whether only the selection will be printed.
  ///
  void(HONEYCOMB_CALLBACK* set_selection_only)(struct _honey_print_settings_t* self,
                                         int selection_only);

  ///
  /// Returns true (1) if only the selection will be printed.
  ///
  int(HONEYCOMB_CALLBACK* is_selection_only)(struct _honey_print_settings_t* self);

  ///
  /// Set whether pages will be collated.
  ///
  void(HONEYCOMB_CALLBACK* set_collate)(struct _honey_print_settings_t* self,
                                  int collate);

  ///
  /// Returns true (1) if pages will be collated.
  ///
  int(HONEYCOMB_CALLBACK* will_collate)(struct _honey_print_settings_t* self);

  ///
  /// Set the color model.
  ///
  void(HONEYCOMB_CALLBACK* set_color_model)(struct _honey_print_settings_t* self,
                                      honey_color_model_t model);

  ///
  /// Get the color model.
  ///
  honey_color_model_t(HONEYCOMB_CALLBACK* get_color_model)(
      struct _honey_print_settings_t* self);

  ///
  /// Set the number of copies.
  ///
  void(HONEYCOMB_CALLBACK* set_copies)(struct _honey_print_settings_t* self,
                                 int copies);

  ///
  /// Get the number of copies.
  ///
  int(HONEYCOMB_CALLBACK* get_copies)(struct _honey_print_settings_t* self);

  ///
  /// Set the duplex mode.
  ///
  void(HONEYCOMB_CALLBACK* set_duplex_mode)(struct _honey_print_settings_t* self,
                                      honey_duplex_mode_t mode);

  ///
  /// Get the duplex mode.
  ///
  honey_duplex_mode_t(HONEYCOMB_CALLBACK* get_duplex_mode)(
      struct _honey_print_settings_t* self);
} honey_print_settings_t;

///
/// Create a new honey_print_settings_t object.
///
HONEYCOMB_EXPORT honey_print_settings_t* honey_print_settings_create(void);

#ifdef __cplusplus
}
#endif

#endif  // HONEYCOMB_INCLUDE_CAPI_HONEYCOMB_PRINT_SETTINGS_CAPI_H_