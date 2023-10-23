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

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_RESOURCE_HANDLER_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_RESOURCE_HANDLER_H_
#pragma once

#include "include/honey_base.h"
#include "include/honey_browser.h"
#include "include/honey_callback.h"
#include "include/honey_cookie.h"
#include "include/honey_request.h"
#include "include/honey_response.h"

///
/// Callback for asynchronous continuation of HoneycombResourceHandler::Skip().
///
/*--honey(source=library)--*/
class HoneycombResourceSkipCallback : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Callback for asynchronous continuation of Skip(). If |bytes_skipped| > 0
  /// then either Skip() will be called again until the requested number of
  /// bytes have been skipped or the request will proceed. If |bytes_skipped|
  /// <= 0 the request will fail with ERR_REQUEST_RANGE_NOT_SATISFIABLE.
  ///
  /*--honey(capi_name=cont)--*/
  virtual void Continue(int64_t bytes_skipped) = 0;
};

///
/// Callback for asynchronous continuation of HoneycombResourceHandler::Read().
///
/*--honey(source=library)--*/
class HoneycombResourceReadCallback : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Callback for asynchronous continuation of Read(). If |bytes_read| == 0
  /// the response will be considered complete. If |bytes_read| > 0 then Read()
  /// will be called again until the request is complete (based on either the
  /// result or the expected content length). If |bytes_read| < 0 then the
  /// request will fail and the |bytes_read| value will be treated as the error
  /// code.
  ///
  /*--honey(capi_name=cont)--*/
  virtual void Continue(int bytes_read) = 0;
};

///
/// Class used to implement a custom request handler interface. The methods of
/// this class will be called on the IO thread unless otherwise indicated.
///
/*--honey(source=client)--*/
class HoneycombResourceHandler : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Open the response stream. To handle the request immediately set
  /// |handle_request| to true and return true. To decide at a later time set
  /// |handle_request| to false, return true, and execute |callback| to continue
  /// or cancel the request. To cancel the request immediately set
  /// |handle_request| to true and return false. This method will be called in
  /// sequence but not from a dedicated thread. For backwards compatibility set
  /// |handle_request| to false and return false and the ProcessRequest method
  /// will be called.
  ///
  /*--honey()--*/
  virtual bool Open(HoneycombRefPtr<HoneycombRequest> request,
                    bool& handle_request,
                    HoneycombRefPtr<HoneycombCallback> callback) {
    handle_request = false;
    return false;
  }

  ///
  /// Begin processing the request. To handle the request return true and call
  /// HoneycombCallback::Continue() once the response header information is available
  /// (HoneycombCallback::Continue() can also be called from inside this method if
  /// header information is available immediately). To cancel the request return
  /// false.
  ///
  /// WARNING: This method is deprecated. Use Open instead.
  ///
  /*--honey()--*/
  virtual bool ProcessRequest(HoneycombRefPtr<HoneycombRequest> request,
                              HoneycombRefPtr<HoneycombCallback> callback) {
    return false;
  }

  ///
  /// Retrieve response header information. If the response length is not known
  /// set |response_length| to -1 and ReadResponse() will be called until it
  /// returns false. If the response length is known set |response_length|
  /// to a positive value and ReadResponse() will be called until it returns
  /// false or the specified number of bytes have been read. Use the |response|
  /// object to set the mime type, http status code and other optional header
  /// values. To redirect the request to a new URL set |redirectUrl| to the new
  /// URL. |redirectUrl| can be either a relative or fully qualified URL.
  /// It is also possible to set |response| to a redirect http status code
  /// and pass the new URL via a Location header. Likewise with |redirectUrl| it
  /// is valid to set a relative or fully qualified URL as the Location header
  /// value. If an error occured while setting up the request you can call
  /// SetError() on |response| to indicate the error condition.
  ///
  /*--honey()--*/
  virtual void GetResponseHeaders(HoneycombRefPtr<HoneycombResponse> response,
                                  int64_t& response_length,
                                  HoneycombString& redirectUrl) = 0;

  ///
  /// Skip response data when requested by a Range header. Skip over and discard
  /// |bytes_to_skip| bytes of response data. If data is available immediately
  /// set |bytes_skipped| to the number of bytes skipped and return true. To
  /// read the data at a later time set |bytes_skipped| to 0, return true and
  /// execute |callback| when the data is available. To indicate failure set
  /// |bytes_skipped| to < 0 (e.g. -2 for ERR_FAILED) and return false. This
  /// method will be called in sequence but not from a dedicated thread.
  ///
  /*--honey()--*/
  virtual bool Skip(int64_t bytes_to_skip,
                    int64_t& bytes_skipped,
                    HoneycombRefPtr<HoneycombResourceSkipCallback> callback) {
    bytes_skipped = -2;
    return false;
  }

  ///
  /// Read response data. If data is available immediately copy up to
  /// |bytes_to_read| bytes into |data_out|, set |bytes_read| to the number of
  /// bytes copied, and return true. To read the data at a later time keep a
  /// pointer to |data_out|, set |bytes_read| to 0, return true and execute
  /// |callback| when the data is available (|data_out| will remain valid until
  /// the callback is executed). To indicate response completion set
  /// |bytes_read| to 0 and return false. To indicate failure set |bytes_read|
  /// to < 0 (e.g. -2 for ERR_FAILED) and return false. This method will be
  /// called in sequence but not from a dedicated thread. For backwards
  /// compatibility set |bytes_read| to -1 and return false and the ReadResponse
  /// method will be called.
  ///
  /*--honey()--*/
  virtual bool Read(void* data_out,
                    int bytes_to_read,
                    int& bytes_read,
                    HoneycombRefPtr<HoneycombResourceReadCallback> callback) {
    bytes_read = -1;
    return false;
  }

  ///
  /// Read response data. If data is available immediately copy up to
  /// |bytes_to_read| bytes into |data_out|, set |bytes_read| to the number of
  /// bytes copied, and return true. To read the data at a later time set
  /// |bytes_read| to 0, return true and call HoneycombCallback::Continue() when the
  /// data is available. To indicate response completion return false.
  ///
  /// WARNING: This method is deprecated. Use Skip and Read instead.
  ///
  /*--honey()--*/
  virtual bool ReadResponse(void* data_out,
                            int bytes_to_read,
                            int& bytes_read,
                            HoneycombRefPtr<HoneycombCallback> callback) {
    bytes_read = -2;
    return false;
  }

  ///
  /// Request processing has been canceled.
  ///
  /*--honey()--*/
  virtual void Cancel() = 0;
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_RESOURCE_HANDLER_H_
