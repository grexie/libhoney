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

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_DOWNLOAD_HANDLER_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_DOWNLOAD_HANDLER_H_
#pragma once

#include "include/honey_base.h"
#include "include/honey_browser.h"
#include "include/honey_download_item.h"

///
/// Callback interface used to asynchronously continue a download.
///
/*--honey(source=library)--*/
class HoneycombBeforeDownloadCallback : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Call to continue the download. Set |download_path| to the full file path
  /// for the download including the file name or leave blank to use the
  /// suggested name and the default temp directory. Set |show_dialog| to true
  /// if you do wish to show the default "Save As" dialog.
  ///
  /*--honey(capi_name=cont,optional_param=download_path)--*/
  virtual void Continue(const HoneycombString& download_path, bool show_dialog) = 0;
};

///
/// Callback interface used to asynchronously cancel a download.
///
/*--honey(source=library)--*/
class HoneycombDownloadItemCallback : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Call to cancel the download.
  ///
  /*--honey()--*/
  virtual void Cancel() = 0;

  ///
  /// Call to pause the download.
  ///
  /*--honey()--*/
  virtual void Pause() = 0;

  ///
  /// Call to resume the download.
  ///
  /*--honey()--*/
  virtual void Resume() = 0;
};

///
/// Class used to handle file downloads. The methods of this class will called
/// on the browser process UI thread.
///
/*--honey(source=client)--*/
class HoneycombDownloadHandler : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Called before a download begins in response to a user-initiated action
  /// (e.g. alt + link click or link click that returns a `Content-Disposition:
  /// attachment` response from the server). |url| is the target download URL
  /// and |request_method| is the target method (GET, POST, etc). Return true to
  /// proceed with the download or false to cancel the download.
  ///
  /*--honey()--*/
  virtual bool CanDownload(HoneycombRefPtr<HoneycombBrowser> browser,
                           const HoneycombString& url,
                           const HoneycombString& request_method) {
    return true;
  }

  ///
  /// Called before a download begins. |suggested_name| is the suggested name
  /// for the download file. By default the download will be canceled. Execute
  /// |callback| either asynchronously or in this method to continue the
  /// download if desired. Do not keep a reference to |download_item| outside of
  /// this method.
  ///
  /*--honey()--*/
  virtual void OnBeforeDownload(
      HoneycombRefPtr<HoneycombBrowser> browser,
      HoneycombRefPtr<HoneycombDownloadItem> download_item,
      const HoneycombString& suggested_name,
      HoneycombRefPtr<HoneycombBeforeDownloadCallback> callback) = 0;

  ///
  /// Called when a download's status or progress information has been updated.
  /// This may be called multiple times before and after OnBeforeDownload().
  /// Execute |callback| either asynchronously or in this method to cancel the
  /// download if desired. Do not keep a reference to |download_item| outside of
  /// this method.
  ///
  /*--honey()--*/
  virtual void OnDownloadUpdated(HoneycombRefPtr<HoneycombBrowser> browser,
                                 HoneycombRefPtr<HoneycombDownloadItem> download_item,
                                 HoneycombRefPtr<HoneycombDownloadItemCallback> callback) {}
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_DOWNLOAD_HANDLER_H_
