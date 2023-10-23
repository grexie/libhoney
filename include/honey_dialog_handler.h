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
// The contents of this file must follow a specific format in order to
// support the Honeycomb translator tool. See the translator.README.txt file in the
// tools directory for more information.
//

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_DIALOG_HANDLER_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_DIALOG_HANDLER_H_
#pragma once

#include "include/honey_base.h"
#include "include/honey_browser.h"

///
/// Callback interface for asynchronous continuation of file dialog requests.
///
/*--honey(source=library)--*/
class HoneycombFileDialogCallback : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Continue the file selection. |file_paths| should be a single value or a
  /// list of values depending on the dialog mode. An empty |file_paths| value
  /// is treated the same as calling Cancel().
  ///
  /*--honey(capi_name=cont,optional_param=file_paths)--*/
  virtual void Continue(const std::vector<HoneycombString>& file_paths) = 0;

  ///
  /// Cancel the file selection.
  ///
  /*--honey()--*/
  virtual void Cancel() = 0;
};

///
/// Implement this interface to handle dialog events. The methods of this class
/// will be called on the browser process UI thread.
///
/*--honey(source=client)--*/
class HoneycombDialogHandler : public virtual HoneycombBaseRefCounted {
 public:
  typedef honey_file_dialog_mode_t FileDialogMode;

  ///
  /// Called to run a file chooser dialog. |mode| represents the type of dialog
  /// to display. |title| to the title to be used for the dialog and may be
  /// empty to show the default title ("Open" or "Save" depending on the mode).
  /// |default_file_path| is the path with optional directory and/or file name
  /// component that should be initially selected in the dialog.
  /// |accept_filters| are used to restrict the selectable file types and may
  /// any combination of (a) valid lower-cased MIME types (e.g. "text/*" or
  /// "image/*"), (b) individual file extensions (e.g. ".txt" or ".png"), or (c)
  /// combined description and file extension delimited using "|" and ";" (e.g.
  /// "Image Types|.png;.gif;.jpg"). To display a custom dialog return true and
  /// execute |callback| either inline or at a later time. To display the
  /// default dialog return false.
  ///
  /*--honey(optional_param=title,optional_param=default_file_path,
          optional_param=accept_filters)--*/
  virtual bool OnFileDialog(HoneycombRefPtr<HoneycombBrowser> browser,
                            FileDialogMode mode,
                            const HoneycombString& title,
                            const HoneycombString& default_file_path,
                            const std::vector<HoneycombString>& accept_filters,
                            HoneycombRefPtr<HoneycombFileDialogCallback> callback) {
    return false;
  }
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_DIALOG_HANDLER_H_
