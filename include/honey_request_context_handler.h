// Copyright (c) 2013 Marshall A. Greenblatt. All rights reserved.
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

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_REQUEST_CONTEXT_HANDLER_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_REQUEST_CONTEXT_HANDLER_H_
#pragma once

#include "include/honey_base.h"
#include "include/honey_browser.h"
#include "include/honey_frame.h"
#include "include/honey_preference.h"
#include "include/honey_request.h"
#include "include/honey_resource_request_handler.h"

///
/// Implement this interface to provide handler implementations. The handler
/// instance will not be released until all objects related to the context have
/// been destroyed.
///
/*--honey(source=client,no_debugct_check)--*/
class HoneycombRequestContextHandler : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Called on the browser process UI thread immediately after the request
  /// context has been initialized.
  ///
  /*--honey()--*/
  virtual void OnRequestContextInitialized(
      HoneycombRefPtr<HoneycombRequestContext> request_context) {}

  ///
  /// Called on the browser process IO thread before a resource request is
  /// initiated. The |browser| and |frame| values represent the source of the
  /// request, and may be NULL for requests originating from service workers or
  /// HoneycombURLRequest. |request| represents the request contents and cannot be
  /// modified in this callback. |is_navigation| will be true if the resource
  /// request is a navigation. |is_download| will be true if the resource
  /// request is a download. |request_initiator| is the origin (scheme + domain)
  /// of the page that initiated the request. Set |disable_default_handling| to
  /// true to disable default handling of the request, in which case it will
  /// need to be handled via HoneycombResourceRequestHandler::GetResourceHandler or it
  /// will be canceled. To allow the resource load to proceed with default
  /// handling return NULL. To specify a handler for the resource return a
  /// HoneycombResourceRequestHandler object. This method will not be called if the
  /// client associated with |browser| returns a non-NULL value from
  /// HoneycombRequestHandler::GetResourceRequestHandler for the same request
  /// (identified by HoneycombRequest::GetIdentifier).
  ///
  /*--honey(optional_param=browser,optional_param=frame,
          optional_param=request_initiator)--*/
  virtual HoneycombRefPtr<HoneycombResourceRequestHandler> GetResourceRequestHandler(
      HoneycombRefPtr<HoneycombBrowser> browser,
      HoneycombRefPtr<HoneycombFrame> frame,
      HoneycombRefPtr<HoneycombRequest> request,
      bool is_navigation,
      bool is_download,
      const HoneycombString& request_initiator,
      bool& disable_default_handling) {
    return nullptr;
  }
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_REQUEST_CONTEXT_HANDLER_H_
