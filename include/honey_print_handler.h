// Copyright (c) 2014 Marshall A. Greenblatt. All rights reserved.
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

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_PRINT_HANDLER_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_PRINT_HANDLER_H_
#pragma once

#include "include/honey_base.h"
#include "include/honey_browser.h"
#include "include/honey_print_settings.h"

///
/// Callback interface for asynchronous continuation of print dialog requests.
///
/*--honey(source=library)--*/
class HoneycombPrintDialogCallback : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Continue printing with the specified |settings|.
  ///
  /*--honey(capi_name=cont)--*/
  virtual void Continue(HoneycombRefPtr<HoneycombPrintSettings> settings) = 0;

  ///
  /// Cancel the printing.
  ///
  /*--honey()--*/
  virtual void Cancel() = 0;
};

///
/// Callback interface for asynchronous continuation of print job requests.
///
/*--honey(source=library)--*/
class HoneycombPrintJobCallback : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Indicate completion of the print job.
  ///
  /*--honey(capi_name=cont)--*/
  virtual void Continue() = 0;
};

///
/// Implement this interface to handle printing on Linux. Each browser will have
/// only one print job in progress at a time. The methods of this class will be
/// called on the browser process UI thread.
///
/*--honey(source=client)--*/
class HoneycombPrintHandler : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Called when printing has started for the specified |browser|. This method
  /// will be called before the other OnPrint*() methods and irrespective of how
  /// printing was initiated (e.g. HoneycombBrowserHost::Print(), JavaScript
  /// window.print() or PDF extension print button).
  ///
  /*--honey()--*/
  virtual void OnPrintStart(HoneycombRefPtr<HoneycombBrowser> browser) = 0;

  ///
  /// Synchronize |settings| with client state. If |get_defaults| is true then
  /// populate |settings| with the default print settings. Do not keep a
  /// reference to |settings| outside of this callback.
  ///
  /*--honey()--*/
  virtual void OnPrintSettings(HoneycombRefPtr<HoneycombBrowser> browser,
                               HoneycombRefPtr<HoneycombPrintSettings> settings,
                               bool get_defaults) = 0;

  ///
  /// Show the print dialog. Execute |callback| once the dialog is dismissed.
  /// Return true if the dialog will be displayed or false to cancel the
  /// printing immediately.
  ///
  /*--honey()--*/
  virtual bool OnPrintDialog(HoneycombRefPtr<HoneycombBrowser> browser,
                             bool has_selection,
                             HoneycombRefPtr<HoneycombPrintDialogCallback> callback) = 0;

  ///
  /// Send the print job to the printer. Execute |callback| once the job is
  /// completed. Return true if the job will proceed or false to cancel the job
  /// immediately.
  ///
  /*--honey()--*/
  virtual bool OnPrintJob(HoneycombRefPtr<HoneycombBrowser> browser,
                          const HoneycombString& document_name,
                          const HoneycombString& pdf_file_path,
                          HoneycombRefPtr<HoneycombPrintJobCallback> callback) = 0;

  ///
  /// Reset client state related to printing.
  ///
  /*--honey()--*/
  virtual void OnPrintReset(HoneycombRefPtr<HoneycombBrowser> browser) = 0;

  ///
  /// Return the PDF paper size in device units. Used in combination with
  /// HoneycombBrowserHost::PrintToPDF().
  ///
  /*--honey()--*/
  virtual HoneycombSize GetPdfPaperSize(HoneycombRefPtr<HoneycombBrowser> browser,
                                  int device_units_per_inch) {
    return HoneycombSize();
  }
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_PRINT_HANDLER_H_
