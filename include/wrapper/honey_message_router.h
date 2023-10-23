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
// The contents of this file are only available to applications that link
// against the libhoneycomb_dll_wrapper target.
//

#ifndef HONEYCOMB_INCLUDE_WRAPPER_HONEYCOMB_MESSAGE_ROUTER_H_
#define HONEYCOMB_INCLUDE_WRAPPER_HONEYCOMB_MESSAGE_ROUTER_H_
#pragma once

#include "include/base/honey_ref_counted.h"
#include "include/honey_base.h"
#include "include/honey_browser.h"
#include "include/honey_process_message.h"
#include "include/honey_v8.h"

// The below classes implement support for routing aynchronous messages between
// JavaScript running in the renderer process and C++ running in the browser
// process. An application interacts with the router by passing it data from
// standard Honeycomb C++ callbacks (OnBeforeBrowse, OnProcessMessageReceived,
// OnContextCreated, etc). The renderer-side router supports generic JavaScript
// callback registration and execution while the browser-side router supports
// application-specific logic via one or more application-provided Handler
// instances.
//
// The renderer-side router implementation exposes a query function and a cancel
// function via the JavaScript 'window' object:
//
//    // Create and send a new query.
//    var request_id = window.honeyQuery({
//        request: 'my_request',
//        persistent: false,
//        onSuccess: function(response) {},
//        onFailure: function(error_code, error_message) {}
//    });
//
//    // Optionally cancel the query.
//    window.honeyQueryCancel(request_id);
//
// When |window.honeyQuery| is executed the request is sent asynchronously to one
// or more C++ Handler objects registered in the browser process. Each C++
// Handler can choose to either handle or ignore the query in the
// Handler::OnQuery callback. If a Handler chooses to handle the query then it
// should execute Callback::Success when a response is available or
// Callback::Failure if an error occurs. This will result in asynchronous
// execution of the associated JavaScript callback in the renderer process. Any
// queries unhandled by C++ code in the browser process will be automatically
// canceled and the associated JavaScript onFailure callback will be executed
// with an error code of -1.
//
// Queries can be either persistent or non-persistent. If the query is
// persistent then the callbacks will remain registered until one of the
// following conditions are met:
//
// A. The query is canceled in JavaScript using the |window.honeyQueryCancel|
//    function.
// B. The query is canceled in C++ code using the Callback::Failure function.
// C. The context associated with the query is released due to browser
//    destruction, navigation or renderer process termination.
//
// If the query is non-persistent then the registration will be removed after
// the JavaScript callback is executed a single time. If a query is canceled for
// a reason other than Callback::Failure being executed then the associated
// Handler's OnQueryCanceled method will be called.
//
// Some possible usage patterns include:
//
// One-time Request. Use a non-persistent query to send a JavaScript request.
//    The Handler evaluates the request and returns the response. The query is
//    then discarded.
//
// Broadcast. Use a persistent query to register as a JavaScript broadcast
//    receiver. The Handler keeps track of all registered Callbacks and executes
//    them sequentially to deliver the broadcast message.
//
// Subscription. Use a persistent query to register as a JavaScript subscription
//    receiver. The Handler initiates the subscription feed on the first request
//    and delivers responses to all registered subscribers as they become
//    available. The Handler cancels the subscription feed when there are no
//    longer any registered JavaScript receivers.
//
// Message routing occurs on a per-browser and per-context basis. Consequently,
// additional application logic can be applied by restricting which browser or
// context instances are passed into the router. If you choose to use this
// approach do so cautiously. In order for the router to function correctly any
// browser or context instance passed into a single router callback must then
// be passed into all router callbacks.
//
// There is generally no need to have multiple renderer-side routers unless you
// wish to have multiple bindings with different JavaScript function names. It
// can be useful to have multiple browser-side routers with different client-
// provided Handler instances when implementing different behaviors on a per-
// browser basis.
//
// This implementation places no formatting restrictions on payload content.
// An application may choose to exchange anything from simple formatted
// strings to serialized XML or JSON data.
//
//
// EXAMPLE USAGE
//
// 1. Define the router configuration. You can optionally specify settings
//    like the JavaScript function names. The configuration must be the same in
//    both the browser and renderer processes. If using multiple routers in the
//    same application make sure to specify unique function names for each
//    router configuration.
//
//    // Example config object showing the default values.
//    HoneycombMessageRouterConfig config;
//    config.js_query_function = "honeyQuery";
//    config.js_cancel_function = "honeyQueryCancel";
//
// 2. Create an instance of HoneycombMessageRouterBrowserSide in the browser process.
//    You might choose to make it a member of your HoneycombClient implementation,
//    for example.
//
//    browser_side_router_ = HoneycombMessageRouterBrowserSide::Create(config);
//
// 3. Register one or more Handlers. The Handler instances must either outlive
//    the router or be removed from the router before they're deleted.
//
//    browser_side_router_->AddHandler(my_handler);
//
// 4. Call all required HoneycombMessageRouterBrowserSide methods from other callbacks
//    in your HoneycombClient implementation (OnBeforeClose, etc). See the
//    HoneycombMessageRouterBrowserSide class documentation for the complete list of
//    methods.
//
// 5. Create an instance of HoneycombMessageRouterRendererSide in the renderer
// process.
//    You might choose to make it a member of your HoneycombApp implementation, for
//    example.
//
//    renderer_side_router_ = HoneycombMessageRouterRendererSide::Create(config);
//
// 6. Call all required HoneycombMessageRouterRendererSide methods from other
//    callbacks in your HoneycombRenderProcessHandler implementation
//    (OnContextCreated, etc). See the HoneycombMessageRouterRendererSide class
//    documentation for the complete list of methods.
//
// 7. Execute the query function from JavaScript code.
//
//    window.honeyQuery({request: 'my_request',
//                     persistent: false,
//                     onSuccess: function(response) { print(response); },
//                     onFailure: function(error_code, error_message) {} });
//
// 8. Handle the query in your Handler::OnQuery implementation and execute the
//    appropriate callback either immediately or asynchronously.
//
//    void MyHandler::OnQuery(int64_t query_id,
//                            HoneycombRefPtr<HoneycombBrowser> browser,
//                            HoneycombRefPtr<HoneycombFrame> frame,
//                            const HoneycombString& request,
//                            bool persistent,
//                            HoneycombRefPtr<Callback> callback) {
//      if (request == "my_request") {
//        callback->Continue("my_response");
//        return true;
//      }
//      return false;  // Not handled.
//    }
//
// 9. Notice that the onSuccess callback is executed in JavaScript.

///
/// Used to configure the query router. The same values must be passed to both
/// HoneycombMessageRouterBrowserSide and HoneycombMessageRouterRendererSide. If using
/// multiple router pairs make sure to choose values that do not conflict.
///
struct HoneycombMessageRouterConfig {
  HoneycombMessageRouterConfig();

  ///
  /// Name of the JavaScript function that will be added to the 'window' object
  /// for sending a query. The default value is "honeyQuery".
  ///
  HoneycombString js_query_function;

  ///
  /// Name of the JavaScript function that will be added to the 'window' object
  /// for canceling a pending query. The default value is "honeyQueryCancel".
  ///
  HoneycombString js_cancel_function;

  ///
  /// Messages of size (in bytes) larger than this threshold will be sent via
  /// shared memory region.
  ///
  size_t message_size_threshold;
};

///
/// Implements the browser side of query routing. The methods of this class may
/// be called on any browser process thread unless otherwise indicated.
///
class HoneycombMessageRouterBrowserSide
    : public base::RefCountedThreadSafe<HoneycombMessageRouterBrowserSide> {
 public:
  ///
  /// Callback associated with a single pending asynchronous query. Execute the
  /// Success or Failure method to send an asynchronous response to the
  /// associated JavaScript handler. It is a runtime error to destroy a Callback
  /// object associated with an uncanceled query without first executing one of
  /// the callback methods. The methods of this class may be called on any
  /// browser process thread.
  ///
  class Callback : public HoneycombBaseRefCounted {
   public:
    ///
    /// Notify the associated JavaScript onSuccess callback that the query has
    /// completed successfully with the specified |response|.
    ///
    virtual void Success(const HoneycombString& response) = 0;

    ///
    /// Notify the associated JavaScript onFailure callback that the query has
    /// failed with the specified |error_code| and |error_message|.
    ///
    virtual void Failure(int error_code, const HoneycombString& error_message) = 0;
  };

  ///
  /// Implement this interface to handle queries. All methods will be executed
  /// on the browser process UI thread.
  ///
  class Handler {
   public:
    using Callback = HoneycombMessageRouterBrowserSide::Callback;

    ///
    /// Executed when a new query is received. |query_id| uniquely identifies
    /// the query for the life span of the router. Return true to handle the
    /// query or false to propagate the query to other registered handlers, if
    /// any. If no handlers return true from this method then the query will be
    /// automatically canceled with an error code of -1 delivered to the
    /// JavaScript onFailure callback. If this method returns true then a
    /// Callback method must be executed either in this method or asynchronously
    /// to complete the query.
    ///
    virtual bool OnQuery(HoneycombRefPtr<HoneycombBrowser> browser,
                         HoneycombRefPtr<HoneycombFrame> frame,
                         int64_t query_id,
                         const HoneycombString& request,
                         bool persistent,
                         HoneycombRefPtr<Callback> callback) {
      return false;
    }

    ///
    /// Executed when a query has been canceled either explicitly using the
    /// JavaScript cancel function or implicitly due to browser destruction,
    /// navigation or renderer process termination. It will only be called for
    /// the single handler that returned true from OnQuery for the same
    /// |query_id|. No references to the associated Callback object should be
    /// kept after this method is called, nor should any Callback methods be
    /// executed.
    ///
    virtual void OnQueryCanceled(HoneycombRefPtr<HoneycombBrowser> browser,
                                 HoneycombRefPtr<HoneycombFrame> frame,
                                 int64_t query_id) {}

    virtual ~Handler() {}
  };

  ///
  /// Create a new router with the specified configuration.
  ///
  static HoneycombRefPtr<HoneycombMessageRouterBrowserSide> Create(
      const HoneycombMessageRouterConfig& config);

  ///
  /// Add a new query handler. If |first| is true it will be added as the first
  /// handler, otherwise it will be added as the last handler. Returns true if
  /// the handler is added successfully or false if the handler has already been
  /// added. Must be called on the browser process UI thread. The Handler object
  /// must either outlive the router or be removed before deletion.
  ///
  virtual bool AddHandler(Handler* handler, bool first) = 0;

  ///
  /// Remove an existing query handler. Any pending queries associated with the
  /// handler will be canceled. Handler::OnQueryCanceled will be called and the
  /// associated JavaScript onFailure callback will be executed with an error
  /// code of -1. Returns true if the handler is removed successfully or false
  /// if the handler is not found. Must be called on the browser process UI
  /// thread.
  ///
  virtual bool RemoveHandler(Handler* handler) = 0;

  ///
  /// Cancel all pending queries associated with either |browser| or |handler|.
  /// If both |browser| and |handler| are NULL all pending queries will be
  /// canceled. Handler::OnQueryCanceled will be called and the associated
  /// JavaScript onFailure callback will be executed in all cases with an error
  /// code of -1.
  ///
  virtual void CancelPending(HoneycombRefPtr<HoneycombBrowser> browser,
                             Handler* handler) = 0;

  ///
  /// Returns the number of queries currently pending for the specified
  /// |browser| and/or |handler|. Either or both values may be empty. Must be
  /// called on the browser process UI thread.
  ///
  virtual int GetPendingCount(HoneycombRefPtr<HoneycombBrowser> browser,
                              Handler* handler) = 0;

  /// The below methods should be called from other Honeycomb handlers. They must be
  /// called exactly as documented for the router to function correctly.

  ///
  /// Call from HoneycombLifeSpanHandler::OnBeforeClose. Any pending queries
  /// associated with |browser| will be canceled and Handler::OnQueryCanceled
  /// will be called. No JavaScript callbacks will be executed since this
  /// indicates destruction of the browser.
  ///
  virtual void OnBeforeClose(HoneycombRefPtr<HoneycombBrowser> browser) = 0;

  ///
  /// Call from HoneycombRequestHandler::OnRenderProcessTerminated. Any pending
  /// queries associated with |browser| will be canceled and
  /// Handler::OnQueryCanceled will be called. No JavaScript callbacks will be
  /// executed since this indicates destruction of the context.
  ///
  virtual void OnRenderProcessTerminated(HoneycombRefPtr<HoneycombBrowser> browser) = 0;

  ///
  /// Call from HoneycombRequestHandler::OnBeforeBrowse only if the navigation is
  /// allowed to proceed. If |frame| is the main frame then any pending queries
  /// associated with |browser| will be canceled and Handler::OnQueryCanceled
  /// will be called. No JavaScript callbacks will be executed since this
  /// indicates destruction of the context.
  ///
  virtual void OnBeforeBrowse(HoneycombRefPtr<HoneycombBrowser> browser,
                              HoneycombRefPtr<HoneycombFrame> frame) = 0;

  ///
  /// Call from HoneycombClient::OnProcessMessageReceived. Returns true if the message
  /// is handled by this router or false otherwise.
  ///
  virtual bool OnProcessMessageReceived(
      HoneycombRefPtr<HoneycombBrowser> browser,
      HoneycombRefPtr<HoneycombFrame> frame,
      HoneycombProcessId source_process,
      HoneycombRefPtr<HoneycombProcessMessage> message) = 0;

 protected:
  // Protect against accidental deletion of this object.
  friend class base::RefCountedThreadSafe<HoneycombMessageRouterBrowserSide>;
  virtual ~HoneycombMessageRouterBrowserSide() {}
};

///
/// Implements the renderer side of query routing. The methods of this class
/// must be called on the render process main thread.
///
class HoneycombMessageRouterRendererSide
    : public base::RefCountedThreadSafe<HoneycombMessageRouterRendererSide> {
 public:
  ///
  /// Create a new router with the specified configuration.
  ///
  static HoneycombRefPtr<HoneycombMessageRouterRendererSide> Create(
      const HoneycombMessageRouterConfig& config);

  ///
  /// Returns the number of queries currently pending for the specified
  /// |browser| and/or |context|. Either or both values may be empty.
  ///
  virtual int GetPendingCount(HoneycombRefPtr<HoneycombBrowser> browser,
                              HoneycombRefPtr<HoneycombV8Context> context) = 0;

  // The below methods should be called from other Honeycomb handlers. They must be
  // called exactly as documented for the router to function correctly.

  ///
  /// Call from HoneycombRenderProcessHandler::OnContextCreated. Registers the
  /// JavaScripts functions with the new context.
  ///
  virtual void OnContextCreated(HoneycombRefPtr<HoneycombBrowser> browser,
                                HoneycombRefPtr<HoneycombFrame> frame,
                                HoneycombRefPtr<HoneycombV8Context> context) = 0;

  ///
  /// Call from HoneycombRenderProcessHandler::OnContextReleased. Any pending queries
  /// associated with the released context will be canceled and
  /// Handler::OnQueryCanceled will be called in the browser process.
  ///
  virtual void OnContextReleased(HoneycombRefPtr<HoneycombBrowser> browser,
                                 HoneycombRefPtr<HoneycombFrame> frame,
                                 HoneycombRefPtr<HoneycombV8Context> context) = 0;

  ///
  /// Call from HoneycombRenderProcessHandler::OnProcessMessageReceived. Returns true
  /// if the message is handled by this router or false otherwise.
  ///
  virtual bool OnProcessMessageReceived(
      HoneycombRefPtr<HoneycombBrowser> browser,
      HoneycombRefPtr<HoneycombFrame> frame,
      HoneycombProcessId source_process,
      HoneycombRefPtr<HoneycombProcessMessage> message) = 0;

 protected:
  // Protect against accidental deletion of this object.
  friend class base::RefCountedThreadSafe<HoneycombMessageRouterRendererSide>;
  virtual ~HoneycombMessageRouterRendererSide() {}
};

#endif  // HONEYCOMB_INCLUDE_WRAPPER_HONEYCOMB_MESSAGE_ROUTER_H_
