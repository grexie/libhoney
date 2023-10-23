// Copyright (c) 2020 Marshall A. Greenblatt. All rights reserved.
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

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_MEDIA_ROUTER_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_MEDIA_ROUTER_H_
#pragma once

#include <vector>
#include "include/honey_base.h"
#include "include/honey_callback.h"
#include "include/honey_registration.h"

class HoneycombMediaObserver;
class HoneycombMediaRoute;
class HoneycombMediaRouteCreateCallback;
class HoneycombMediaSink;
class HoneycombMediaSinkDeviceInfoCallback;
class HoneycombMediaSource;

///
/// Supports discovery of and communication with media devices on the local
/// network via the Cast and DIAL protocols. The methods of this class may be
/// called on any browser process thread unless otherwise indicated.
///
/*--honey(source=library)--*/
class HoneycombMediaRouter : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Returns the MediaRouter object associated with the global request context.
  /// If |callback| is non-NULL it will be executed asnychronously on the UI
  /// thread after the manager's storage has been initialized. Equivalent to
  /// calling HoneycombRequestContext::GetGlobalContext()->GetMediaRouter().
  ///
  /*--honey(optional_param=callback)--*/
  static HoneycombRefPtr<HoneycombMediaRouter> GetGlobalMediaRouter(
      HoneycombRefPtr<HoneycombCompletionCallback> callback);

  ///
  /// Add an observer for MediaRouter events. The observer will remain
  /// registered until the returned Registration object is destroyed.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombRegistration> AddObserver(
      HoneycombRefPtr<HoneycombMediaObserver> observer) = 0;

  ///
  /// Returns a MediaSource object for the specified media source URN. Supported
  /// URN schemes include "cast:" and "dial:", and will be already known by the
  /// client application (e.g. "cast:<appId>?clientId=<clientId>").
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombMediaSource> GetSource(const HoneycombString& urn) = 0;

  ///
  /// Trigger an asynchronous call to HoneycombMediaObserver::OnSinks on all
  /// registered observers.
  ///
  /*--honey()--*/
  virtual void NotifyCurrentSinks() = 0;

  ///
  /// Create a new route between |source| and |sink|. Source and sink must be
  /// valid, compatible (as reported by HoneycombMediaSink::IsCompatibleWith), and a
  /// route between them must not already exist. |callback| will be executed
  /// on success or failure. If route creation succeeds it will also trigger an
  /// asynchronous call to HoneycombMediaObserver::OnRoutes on all registered
  /// observers.
  ///
  /*--honey()--*/
  virtual void CreateRoute(HoneycombRefPtr<HoneycombMediaSource> source,
                           HoneycombRefPtr<HoneycombMediaSink> sink,
                           HoneycombRefPtr<HoneycombMediaRouteCreateCallback> callback) = 0;

  ///
  /// Trigger an asynchronous call to HoneycombMediaObserver::OnRoutes on all
  /// registered observers.
  ///
  /*--honey()--*/
  virtual void NotifyCurrentRoutes() = 0;
};

///
/// Implemented by the client to observe MediaRouter events and registered via
/// HoneycombMediaRouter::AddObserver. The methods of this class will be called on the
/// browser process UI thread.
///
/*--honey(source=client)--*/
class HoneycombMediaObserver : public virtual HoneycombBaseRefCounted {
 public:
  typedef honey_media_route_connection_state_t ConnectionState;

  ///
  /// The list of available media sinks has changed or
  /// HoneycombMediaRouter::NotifyCurrentSinks was called.
  ///
  /*--honey()--*/
  virtual void OnSinks(const std::vector<HoneycombRefPtr<HoneycombMediaSink>>& sinks) = 0;

  ///
  /// The list of available media routes has changed or
  /// HoneycombMediaRouter::NotifyCurrentRoutes was called.
  ///
  /*--honey()--*/
  virtual void OnRoutes(
      const std::vector<HoneycombRefPtr<HoneycombMediaRoute>>& routes) = 0;

  ///
  /// The connection state of |route| has changed.
  ///
  /*--honey()--*/
  virtual void OnRouteStateChanged(HoneycombRefPtr<HoneycombMediaRoute> route,
                                   ConnectionState state) = 0;

  ///
  /// A message was recieved over |route|. |message| is only valid for
  /// the scope of this callback and should be copied if necessary.
  ///
  /*--honey()--*/
  virtual void OnRouteMessageReceived(HoneycombRefPtr<HoneycombMediaRoute> route,
                                      const void* message,
                                      size_t message_size) = 0;
};

///
/// Represents the route between a media source and sink. Instances of this
/// object are created via HoneycombMediaRouter::CreateRoute and retrieved via
/// HoneycombMediaObserver::OnRoutes. Contains the status and metadata of a
/// routing operation. The methods of this class may be called on any browser
/// process thread unless otherwise indicated.
///
/*--honey(source=library)--*/
class HoneycombMediaRoute : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Returns the ID for this route.
  ///
  /*--honey()--*/
  virtual HoneycombString GetId() = 0;

  ///
  /// Returns the source associated with this route.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombMediaSource> GetSource() = 0;

  ///
  /// Returns the sink associated with this route.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombMediaSink> GetSink() = 0;

  ///
  /// Send a message over this route. |message| will be copied if necessary.
  ///
  /*--honey()--*/
  virtual void SendRouteMessage(const void* message, size_t message_size) = 0;

  ///
  /// Terminate this route. Will result in an asynchronous call to
  /// HoneycombMediaObserver::OnRoutes on all registered observers.
  ///
  /*--honey()--*/
  virtual void Terminate() = 0;
};

///
/// Callback interface for HoneycombMediaRouter::CreateRoute. The methods of this
/// class will be called on the browser process UI thread.
///
/*--honey(source=client)--*/
class HoneycombMediaRouteCreateCallback : public virtual HoneycombBaseRefCounted {
 public:
  typedef honey_media_route_create_result_t RouteCreateResult;

  ///
  /// Method that will be executed when the route creation has finished.
  /// |result| will be HONEYCOMB_MRCR_OK if the route creation succeeded. |error| will
  /// be a description of the error if the route creation failed. |route| is the
  /// resulting route, or empty if the route creation failed.
  ///
  /*--honey(optional_param=error,optional_param=route)--*/
  virtual void OnMediaRouteCreateFinished(RouteCreateResult result,
                                          const HoneycombString& error,
                                          HoneycombRefPtr<HoneycombMediaRoute> route) = 0;
};

///
/// Represents a sink to which media can be routed. Instances of this object are
/// retrieved via HoneycombMediaObserver::OnSinks. The methods of this class may
/// be called on any browser process thread unless otherwise indicated.
///
/*--honey(source=library)--*/
class HoneycombMediaSink : public virtual HoneycombBaseRefCounted {
 public:
  typedef honey_media_sink_icon_type_t IconType;

  ///
  /// Returns the ID for this sink.
  ///
  /*--honey()--*/
  virtual HoneycombString GetId() = 0;

  ///
  /// Returns the name of this sink.
  ///
  /*--honey()--*/
  virtual HoneycombString GetName() = 0;

  ///
  /// Returns the icon type for this sink.
  ///
  /*--honey(default_retval=HONEYCOMB_MSIT_GENERIC)--*/
  virtual IconType GetIconType() = 0;

  ///
  /// Asynchronously retrieves device info.
  ///
  /*--honey()--*/
  virtual void GetDeviceInfo(
      HoneycombRefPtr<HoneycombMediaSinkDeviceInfoCallback> callback) = 0;

  ///
  /// Returns true if this sink accepts content via Cast.
  ///
  /*--honey()--*/
  virtual bool IsCastSink() = 0;

  ///
  /// Returns true if this sink accepts content via DIAL.
  ///
  /*--honey()--*/
  virtual bool IsDialSink() = 0;

  ///
  /// Returns true if this sink is compatible with |source|.
  ///
  /*--honey()--*/
  virtual bool IsCompatibleWith(HoneycombRefPtr<HoneycombMediaSource> source) = 0;
};

///
/// Callback interface for HoneycombMediaSink::GetDeviceInfo. The methods of this
/// class will be called on the browser process UI thread.
///
/*--honey(source=client)--*/
class HoneycombMediaSinkDeviceInfoCallback : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Method that will be executed asyncronously once device information has
  /// been retrieved.
  ///
  /*--honey()--*/
  virtual void OnMediaSinkDeviceInfo(
      const HoneycombMediaSinkDeviceInfo& device_info) = 0;
};

///
/// Represents a source from which media can be routed. Instances of this object
/// are retrieved via HoneycombMediaRouter::GetSource. The methods of this class may
/// be called on any browser process thread unless otherwise indicated.
///
/*--honey(source=library)--*/
class HoneycombMediaSource : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Returns the ID (media source URN or URL) for this source.
  ///
  /*--honey()--*/
  virtual HoneycombString GetId() = 0;

  ///
  /// Returns true if this source outputs its content via Cast.
  ///
  /*--honey()--*/
  virtual bool IsCastSource() = 0;

  ///
  /// Returns true if this source outputs its content via DIAL.
  ///
  /*--honey()--*/
  virtual bool IsDialSource() = 0;
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_MEDIA_ROUTER_H_
