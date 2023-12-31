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
// $hash=69545645f079f4593d9cbb6d8a36535c209245f7$
//

#ifndef HONEYCOMB_INCLUDE_CAPI_HONEYCOMB_DIALOG_HANDLER_CAPI_H_
#define HONEYCOMB_INCLUDE_CAPI_HONEYCOMB_DIALOG_HANDLER_CAPI_H_
#pragma once

#include "include/capi/honey_base_capi.h"
#include "include/capi/honey_browser_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

///
/// Callback structure for asynchronous continuation of file dialog requests.
///
typedef struct _honey_file_dialog_callback_t {
  ///
  /// Base structure.
  ///
  honey_base_ref_counted_t base;

  ///
  /// Continue the file selection. |file_paths| should be a single value or a
  /// list of values depending on the dialog mode. An NULL |file_paths| value is
  /// treated the same as calling cancel().
  ///
  void(HONEYCOMB_CALLBACK* cont)(struct _honey_file_dialog_callback_t* self,
                           honey_string_list_t file_paths);

  ///
  /// Cancel the file selection.
  ///
  void(HONEYCOMB_CALLBACK* cancel)(struct _honey_file_dialog_callback_t* self);
} honey_file_dialog_callback_t;

///
/// Implement this structure to handle dialog events. The functions of this
/// structure will be called on the browser process UI thread.
///
typedef struct _honey_dialog_handler_t {
  ///
  /// Base structure.
  ///
  honey_base_ref_counted_t base;

  ///
  /// Called to run a file chooser dialog. |mode| represents the type of dialog
  /// to display. |title| to the title to be used for the dialog and may be NULL
  /// to show the default title ("Open" or "Save" depending on the mode).
  /// |default_file_path| is the path with optional directory and/or file name
  /// component that should be initially selected in the dialog.
  /// |accept_filters| are used to restrict the selectable file types and may
  /// any combination of (a) valid lower-cased MIME types (e.g. "text/*" or
  /// "image/*"), (b) individual file extensions (e.g. ".txt" or ".png"), or (c)
  /// combined description and file extension delimited using "|" and ";" (e.g.
  /// "Image Types|.png;.gif;.jpg"). To display a custom dialog return true (1)
  /// and execute |callback| either inline or at a later time. To display the
  /// default dialog return false (0).
  ///
  int(HONEYCOMB_CALLBACK* on_file_dialog)(
      struct _honey_dialog_handler_t* self,
      struct _honey_browser_t* browser,
      honey_file_dialog_mode_t mode,
      const honey_string_t* title,
      const honey_string_t* default_file_path,
      honey_string_list_t accept_filters,
      struct _honey_file_dialog_callback_t* callback);
} honey_dialog_handler_t;

#ifdef __cplusplus
}
#endif

#endif  // HONEYCOMB_INCLUDE_CAPI_HONEYCOMB_DIALOG_HANDLER_CAPI_H_
