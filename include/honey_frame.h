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

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_FRAME_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_FRAME_H_
#pragma once

#include "include/honey_base.h"
#include "include/honey_dom.h"
#include "include/honey_process_message.h"
#include "include/honey_request.h"
#include "include/honey_stream.h"
#include "include/honey_string_visitor.h"

class HoneycombBrowser;
class HoneycombURLRequest;
class HoneycombURLRequestClient;
class HoneycombV8Context;

///
/// Class used to represent a frame in the browser window. When used in the
/// browser process the methods of this class may be called on any thread unless
/// otherwise indicated in the comments. When used in the render process the
/// methods of this class may only be called on the main thread.
///
/*--honey(source=library)--*/
class HoneycombFrame : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// True if this object is currently attached to a valid frame.
  ///
  /*--honey()--*/
  virtual bool IsValid() = 0;

  ///
  /// Execute undo in this frame.
  ///
  /*--honey()--*/
  virtual void Undo() = 0;

  ///
  /// Execute redo in this frame.
  ///
  /*--honey()--*/
  virtual void Redo() = 0;

  ///
  /// Execute cut in this frame.
  ///
  /*--honey()--*/
  virtual void Cut() = 0;

  ///
  /// Execute copy in this frame.
  ///
  /*--honey()--*/
  virtual void Copy() = 0;

  ///
  /// Execute paste in this frame.
  ///
  /*--honey()--*/
  virtual void Paste() = 0;

  ///
  /// Execute delete in this frame.
  ///
  /*--honey(capi_name=del)--*/
  virtual void Delete() = 0;

  ///
  /// Execute select all in this frame.
  ///
  /*--honey()--*/
  virtual void SelectAll() = 0;

  ///
  /// Save this frame's HTML source to a temporary file and open it in the
  /// default text viewing application. This method can only be called from the
  /// browser process.
  ///
  /*--honey()--*/
  virtual void ViewSource() = 0;

  ///
  /// Retrieve this frame's HTML source as a string sent to the specified
  /// visitor.
  ///
  /*--honey()--*/
  virtual void GetSource(HoneycombRefPtr<HoneycombStringVisitor> visitor) = 0;

  ///
  /// Retrieve this frame's display text as a string sent to the specified
  /// visitor.
  ///
  /*--honey()--*/
  virtual void GetText(HoneycombRefPtr<HoneycombStringVisitor> visitor) = 0;

  ///
  /// Load the request represented by the |request| object.
  ///
  /// WARNING: This method will fail with "bad IPC message" reason
  /// INVALID_INITIATOR_ORIGIN (213) unless you first navigate to the
  /// request origin using some other mechanism (LoadURL, link click, etc).
  ///
  /*--honey()--*/
  virtual void LoadRequest(HoneycombRefPtr<HoneycombRequest> request) = 0;

  ///
  /// Load the specified |url|.
  ///
  /*--honey()--*/
  virtual void LoadURL(const HoneycombString& url) = 0;

  ///
  /// Execute a string of JavaScript code in this frame. The |script_url|
  /// parameter is the URL where the script in question can be found, if any.
  /// The renderer may request this URL to show the developer the source of the
  /// error.  The |start_line| parameter is the base line number to use for
  /// error reporting.
  ///
  /*--honey(optional_param=script_url)--*/
  virtual void ExecuteJavaScript(const HoneycombString& code,
                                 const HoneycombString& script_url,
                                 int start_line) = 0;

  ///
  /// Returns true if this is the main (top-level) frame.
  ///
  /*--honey()--*/
  virtual bool IsMain() = 0;

  ///
  /// Returns true if this is the focused frame.
  ///
  /*--honey()--*/
  virtual bool IsFocused() = 0;

  ///
  /// Returns the name for this frame. If the frame has an assigned name (for
  /// example, set via the iframe "name" attribute) then that value will be
  /// returned. Otherwise a unique name will be constructed based on the frame
  /// parent hierarchy. The main (top-level) frame will always have an empty
  /// name value.
  ///
  /*--honey()--*/
  virtual HoneycombString GetName() = 0;

  ///
  /// Returns the globally unique identifier for this frame or < 0 if the
  /// underlying frame does not yet exist.
  ///
  /*--honey()--*/
  virtual int64_t GetIdentifier() = 0;

  ///
  /// Returns the parent of this frame or NULL if this is the main (top-level)
  /// frame.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombFrame> GetParent() = 0;

  ///
  /// Returns the URL currently loaded in this frame.
  ///
  /*--honey()--*/
  virtual HoneycombString GetURL() = 0;

  ///
  /// Returns the browser that this frame belongs to.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombBrowser> GetBrowser() = 0;

  ///
  /// Get the V8 context associated with the frame. This method can only be
  /// called from the render process.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombV8Context> GetV8Context() = 0;

  ///
  /// Visit the DOM document. This method can only be called from the render
  /// process.
  ///
  /*--honey()--*/
  virtual void VisitDOM(HoneycombRefPtr<HoneycombDOMVisitor> visitor) = 0;

  ///
  /// Create a new URL request that will be treated as originating from this
  /// frame and the associated browser. Use HoneycombURLRequest::Create instead if you
  /// do not want the request to have this association, in which case it may be
  /// handled differently (see documentation on that method). A request created
  /// with this method may only originate from the browser process, and will
  /// behave as follows:
  ///   - It may be intercepted by the client via HoneycombResourceRequestHandler or
  ///     HoneycombSchemeHandlerFactory.
  ///   - POST data may only contain a single element of type PDE_TYPE_FILE or
  ///     PDE_TYPE_BYTES.
  ///
  /// The |request| object will be marked as read-only after calling this
  /// method.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombURLRequest> CreateURLRequest(
      HoneycombRefPtr<HoneycombRequest> request,
      HoneycombRefPtr<HoneycombURLRequestClient> client) = 0;

  ///
  /// Send a message to the specified |target_process|. Ownership of the message
  /// contents will be transferred and the |message| reference will be
  /// invalidated. Message delivery is not guaranteed in all cases (for example,
  /// if the browser is closing, navigating, or if the target process crashes).
  /// Send an ACK message back from the target process if confirmation is
  /// required.
  ///
  /*--honey()--*/
  virtual void SendProcessMessage(HoneycombProcessId target_process,
                                  HoneycombRefPtr<HoneycombProcessMessage> message) = 0;
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_FRAME_H_
