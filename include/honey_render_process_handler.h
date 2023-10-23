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

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_RENDER_PROCESS_HANDLER_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_RENDER_PROCESS_HANDLER_H_
#pragma once

#include "include/honey_base.h"
#include "include/honey_browser.h"
#include "include/honey_dom.h"
#include "include/honey_frame.h"
#include "include/honey_load_handler.h"
#include "include/honey_process_message.h"
#include "include/honey_v8.h"
#include "include/honey_values.h"

///
/// Class used to implement render process callbacks. The methods of this class
/// will be called on the render process main thread (TID_RENDERER) unless
/// otherwise indicated.
///
/*--honey(source=client,no_debugct_check)--*/
class HoneycombRenderProcessHandler : public virtual HoneycombBaseRefCounted {
 public:
  typedef honey_navigation_type_t NavigationType;

  ///
  /// Called after WebKit has been initialized.
  ///
  /*--honey()--*/
  virtual void OnWebKitInitialized() {}

  ///
  /// Called after a browser has been created. When browsing cross-origin a new
  /// browser will be created before the old browser with the same identifier is
  /// destroyed. |extra_info| is an optional read-only value originating from
  /// HoneycombBrowserHost::CreateBrowser(), HoneycombBrowserHost::CreateBrowserSync(),
  /// HoneycombLifeSpanHandler::OnBeforePopup() or
  /// HoneycombBrowserView::CreateBrowserView().
  ///
  /*--honey(optional_param=extra_info)--*/
  virtual void OnBrowserCreated(HoneycombRefPtr<HoneycombBrowser> browser,
                                HoneycombRefPtr<HoneycombDictionaryValue> extra_info) {}

  ///
  /// Called before a browser is destroyed.
  ///
  /*--honey()--*/
  virtual void OnBrowserDestroyed(HoneycombRefPtr<HoneycombBrowser> browser) {}

  ///
  /// Return the handler for browser load status events.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombLoadHandler> GetLoadHandler() { return nullptr; }

  ///
  /// Called immediately after the V8 context for a frame has been created. To
  /// retrieve the JavaScript 'window' object use the HoneycombV8Context::GetGlobal()
  /// method. V8 handles can only be accessed from the thread on which they are
  /// created. A task runner for posting tasks on the associated thread can be
  /// retrieved via the HoneycombV8Context::GetTaskRunner() method.
  ///
  /*--honey()--*/
  virtual void OnContextCreated(HoneycombRefPtr<HoneycombBrowser> browser,
                                HoneycombRefPtr<HoneycombFrame> frame,
                                HoneycombRefPtr<HoneycombV8Context> context) {}

  ///
  /// Called immediately before the V8 context for a frame is released. No
  /// references to the context should be kept after this method is called.
  ///
  /*--honey()--*/
  virtual void OnContextReleased(HoneycombRefPtr<HoneycombBrowser> browser,
                                 HoneycombRefPtr<HoneycombFrame> frame,
                                 HoneycombRefPtr<HoneycombV8Context> context) {}

  ///
  /// Called for global uncaught exceptions in a frame. Execution of this
  /// callback is disabled by default. To enable set
  /// honey_settings_t.uncaught_exception_stack_size > 0.
  ///
  /*--honey()--*/
  virtual void OnUncaughtException(HoneycombRefPtr<HoneycombBrowser> browser,
                                   HoneycombRefPtr<HoneycombFrame> frame,
                                   HoneycombRefPtr<HoneycombV8Context> context,
                                   HoneycombRefPtr<HoneycombV8Exception> exception,
                                   HoneycombRefPtr<HoneycombV8StackTrace> stackTrace) {}

  ///
  /// Called when a new node in the the browser gets focus. The |node| value may
  /// be empty if no specific node has gained focus. The node object passed to
  /// this method represents a snapshot of the DOM at the time this method is
  /// executed. DOM objects are only valid for the scope of this method. Do not
  /// keep references to or attempt to access any DOM objects outside the scope
  /// of this method.
  ///
  /*--honey(optional_param=frame,optional_param=node)--*/
  virtual void OnFocusedNodeChanged(HoneycombRefPtr<HoneycombBrowser> browser,
                                    HoneycombRefPtr<HoneycombFrame> frame,
                                    HoneycombRefPtr<HoneycombDOMNode> node) {}

  ///
  /// Called when a new message is received from a different process. Return
  /// true if the message was handled or false otherwise. It is safe to keep a
  /// reference to |message| outside of this callback.
  ///
  /*--honey()--*/
  virtual bool OnProcessMessageReceived(HoneycombRefPtr<HoneycombBrowser> browser,
                                        HoneycombRefPtr<HoneycombFrame> frame,
                                        HoneycombProcessId source_process,
                                        HoneycombRefPtr<HoneycombProcessMessage> message) {
    return false;
  }
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_RENDER_PROCESS_HANDLER_H_
