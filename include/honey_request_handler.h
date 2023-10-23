// Copyright (c) 2011 Marshall A. Greenblatt. All rights reserved.
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

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_REQUEST_HANDLER_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_REQUEST_HANDLER_H_
#pragma once

#include <vector>

#include "include/honey_auth_callback.h"
#include "include/honey_base.h"
#include "include/honey_browser.h"
#include "include/honey_callback.h"
#include "include/honey_frame.h"
#include "include/honey_request.h"
#include "include/honey_resource_request_handler.h"
#include "include/honey_ssl_info.h"
#include "include/honey_x509_certificate.h"

///
/// Callback interface used to select a client certificate for authentication.
///
/*--honey(source=library)--*/
class HoneycombSelectClientCertificateCallback : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Chooses the specified certificate for client certificate authentication.
  /// NULL value means that no client certificate should be used.
  ///
  /*--honey(optional_param=cert)--*/
  virtual void Select(HoneycombRefPtr<HoneycombX509Certificate> cert) = 0;
};

///
/// Implement this interface to handle events related to browser requests. The
/// methods of this class will be called on the thread indicated.
///
/*--honey(source=client)--*/
class HoneycombRequestHandler : public virtual HoneycombBaseRefCounted {
 public:
  typedef honey_termination_status_t TerminationStatus;
  typedef honey_window_open_disposition_t WindowOpenDisposition;
  typedef std::vector<HoneycombRefPtr<HoneycombX509Certificate>> X509CertificateList;

  ///
  /// Called on the UI thread before browser navigation. Return true to cancel
  /// the navigation or false to allow the navigation to proceed. The |request|
  /// object cannot be modified in this callback.
  /// HoneycombLoadHandler::OnLoadingStateChange will be called twice in all cases.
  /// If the navigation is allowed HoneycombLoadHandler::OnLoadStart and
  /// HoneycombLoadHandler::OnLoadEnd will be called. If the navigation is canceled
  /// HoneycombLoadHandler::OnLoadError will be called with an |errorCode| value of
  /// ERR_ABORTED. The |user_gesture| value will be true if the browser
  /// navigated via explicit user gesture (e.g. clicking a link) or false if it
  /// navigated automatically (e.g. via the DomContentLoaded event).
  ///
  /*--honey()--*/
  virtual bool OnBeforeBrowse(HoneycombRefPtr<HoneycombBrowser> browser,
                              HoneycombRefPtr<HoneycombFrame> frame,
                              HoneycombRefPtr<HoneycombRequest> request,
                              bool user_gesture,
                              bool is_redirect) {
    return false;
  }

  ///
  /// Called on the UI thread before OnBeforeBrowse in certain limited cases
  /// where navigating a new or different browser might be desirable. This
  /// includes user-initiated navigation that might open in a special way (e.g.
  /// links clicked via middle-click or ctrl + left-click) and certain types of
  /// cross-origin navigation initiated from the renderer process (e.g.
  /// navigating the top-level frame to/from a file URL). The |browser| and
  /// |frame| values represent the source of the navigation. The
  /// |target_disposition| value indicates where the user intended to navigate
  /// the browser based on standard Chromium behaviors (e.g. current tab,
  /// new tab, etc). The |user_gesture| value will be true if the browser
  /// navigated via explicit user gesture (e.g. clicking a link) or false if it
  /// navigated automatically (e.g. via the DomContentLoaded event). Return true
  /// to cancel the navigation or false to allow the navigation to proceed in
  /// the source browser's top-level frame.
  ///
  /*--honey()--*/
  virtual bool OnOpenURLFromTab(HoneycombRefPtr<HoneycombBrowser> browser,
                                HoneycombRefPtr<HoneycombFrame> frame,
                                const HoneycombString& target_url,
                                WindowOpenDisposition target_disposition,
                                bool user_gesture) {
    return false;
  }

  ///
  /// Called on the browser process IO thread before a resource request is
  /// initiated. The |browser| and |frame| values represent the source of the
  /// request. |request| represents the request contents and cannot be modified
  /// in this callback. |is_navigation| will be true if the resource request is
  /// a navigation. |is_download| will be true if the resource request is a
  /// download. |request_initiator| is the origin (scheme + domain) of the page
  /// that initiated the request. Set |disable_default_handling| to true to
  /// disable default handling of the request, in which case it will need to be
  /// handled via HoneycombResourceRequestHandler::GetResourceHandler or it will be
  /// canceled. To allow the resource load to proceed with default handling
  /// return NULL. To specify a handler for the resource return a
  /// HoneycombResourceRequestHandler object. If this callback returns NULL the same
  /// method will be called on the associated HoneycombRequestContextHandler, if any.
  ///
  /*--honey(optional_param=request_initiator)--*/
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

  ///
  /// Called on the IO thread when the browser needs credentials from the user.
  /// |origin_url| is the origin making this authentication request. |isProxy|
  /// indicates whether the host is a proxy server. |host| contains the hostname
  /// and |port| contains the port number. |realm| is the realm of the challenge
  /// and may be empty. |scheme| is the authentication scheme used, such as
  /// "basic" or "digest", and will be empty if the source of the request is an
  /// FTP server. Return true to continue the request and call
  /// HoneycombAuthCallback::Continue() either in this method or at a later time when
  /// the authentication information is available. Return false to cancel the
  /// request immediately.
  ///
  /*--honey(optional_param=realm,optional_param=scheme)--*/
  virtual bool GetAuthCredentials(HoneycombRefPtr<HoneycombBrowser> browser,
                                  const HoneycombString& origin_url,
                                  bool isProxy,
                                  const HoneycombString& host,
                                  int port,
                                  const HoneycombString& realm,
                                  const HoneycombString& scheme,
                                  HoneycombRefPtr<HoneycombAuthCallback> callback) {
    return false;
  }

  ///
  /// Called on the UI thread to handle requests for URLs with an invalid
  /// SSL certificate. Return true and call HoneycombCallback methods either in this
  /// method or at a later time to continue or cancel the request. Return false
  /// to cancel the request immediately. If
  /// honey_settings_t.ignore_certificate_errors is set all invalid certificates
  /// will be accepted without calling this method.
  ///
  /*--honey()--*/
  virtual bool OnCertificateError(HoneycombRefPtr<HoneycombBrowser> browser,
                                  honey_errorcode_t cert_error,
                                  const HoneycombString& request_url,
                                  HoneycombRefPtr<HoneycombSSLInfo> ssl_info,
                                  HoneycombRefPtr<HoneycombCallback> callback) {
    return false;
  }

  ///
  /// Called on the UI thread when a client certificate is being requested for
  /// authentication. Return false to use the default behavior and automatically
  /// select the first certificate available. Return true and call
  /// HoneycombSelectClientCertificateCallback::Select either in this method or at a
  /// later time to select a certificate. Do not call Select or call it with
  /// NULL to continue without using any certificate. |isProxy| indicates
  /// whether the host is an HTTPS proxy or the origin server. |host| and |port|
  /// contains the hostname and port of the SSL server. |certificates| is the
  /// list of certificates to choose from; this list has already been pruned by
  /// Chromium so that it only contains certificates from issuers that the
  /// server trusts.
  ///
  /*--honey()--*/
  virtual bool OnSelectClientCertificate(
      HoneycombRefPtr<HoneycombBrowser> browser,
      bool isProxy,
      const HoneycombString& host,
      int port,
      const X509CertificateList& certificates,
      HoneycombRefPtr<HoneycombSelectClientCertificateCallback> callback) {
    return false;
  }

  ///
  /// Called on the browser process UI thread when the render view associated
  /// with |browser| is ready to receive/handle IPC messages in the render
  /// process.
  ///
  /*--honey()--*/
  virtual void OnRenderViewReady(HoneycombRefPtr<HoneycombBrowser> browser) {}

  ///
  /// Called on the browser process UI thread when the render process
  /// terminates unexpectedly. |status| indicates how the process
  /// terminated.
  ///
  /*--honey()--*/
  virtual void OnRenderProcessTerminated(HoneycombRefPtr<HoneycombBrowser> browser,
                                         TerminationStatus status) {}

  ///
  /// Called on the browser process UI thread when the window.document object of
  /// the main frame has been created.
  ///
  /*--honey()--*/
  virtual void OnDocumentAvailableInMainFrame(HoneycombRefPtr<HoneycombBrowser> browser) {}
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_REQUEST_HANDLER_H_
