// Copyright (c) 2017 Marshall A. Greenblatt. All rights reserved.
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

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_EXTENSION_HANDLER_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_EXTENSION_HANDLER_H_
#pragma once

#include "include/honey_base.h"
#include "include/honey_browser.h"
#include "include/honey_extension.h"
#include "include/honey_stream.h"

class HoneycombClient;

///
/// Callback interface used for asynchronous continuation of
/// HoneycombExtensionHandler::GetExtensionResource.
///
/*--honey(source=library)--*/
class HoneycombGetExtensionResourceCallback : public HoneycombBaseRefCounted {
 public:
  ///
  /// Continue the request. Read the resource contents from |stream|.
  ///
  /*--honey(capi_name=cont,optional_param=stream)--*/
  virtual void Continue(HoneycombRefPtr<HoneycombStreamReader> stream) = 0;

  ///
  /// Cancel the request.
  ///
  /*--honey()--*/
  virtual void Cancel() = 0;
};

///
/// Implement this interface to handle events related to browser extensions.
/// The methods of this class will be called on the UI thread. See
/// HoneycombRequestContext::LoadExtension for information about extension loading.
///
/*--honey(source=client)--*/
class HoneycombExtensionHandler : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Called if the HoneycombRequestContext::LoadExtension request fails. |result|
  /// will be the error code.
  ///
  /*--honey()--*/
  virtual void OnExtensionLoadFailed(honey_errorcode_t result) {}

  ///
  /// Called if the HoneycombRequestContext::LoadExtension request succeeds.
  /// |extension| is the loaded extension.
  ///
  /*--honey()--*/
  virtual void OnExtensionLoaded(HoneycombRefPtr<HoneycombExtension> extension) {}

  ///
  /// Called after the HoneycombExtension::Unload request has completed.
  ///
  /*--honey()--*/
  virtual void OnExtensionUnloaded(HoneycombRefPtr<HoneycombExtension> extension) {}

  ///
  /// Called when an extension needs a browser to host a background script
  /// specified via the "background" manifest key. The browser will have no
  /// visible window and cannot be displayed. |extension| is the extension that
  /// is loading the background script. |url| is an internally generated
  /// reference to an HTML page that will be used to load the background script
  /// via a "<script>" src attribute. To allow creation of the browser
  /// optionally modify |client| and |settings| and return false. To cancel
  /// creation of the browser (and consequently cancel load of the background
  /// script) return true. Successful creation will be indicated by a call to
  /// HoneycombLifeSpanHandler::OnAfterCreated, and HoneycombBrowserHost::IsBackgroundHost
  /// will return true for the resulting browser. See
  /// https://developer.chrome.com/extensions/event_pages for more information
  /// about extension background script usage.
  ///
  /*--honey()--*/
  virtual bool OnBeforeBackgroundBrowser(HoneycombRefPtr<HoneycombExtension> extension,
                                         const HoneycombString& url,
                                         HoneycombRefPtr<HoneycombClient>& client,
                                         HoneycombBrowserSettings& settings) {
    return false;
  }

  ///
  /// Called when an extension API (e.g. chrome.tabs.create) requests creation
  /// of a new browser. |extension| and |browser| are the source of the API
  /// call. |active_browser| may optionally be specified via the windowId
  /// property or returned via the GetActiveBrowser() callback and provides the
  /// default |client| and |settings| values for the new browser. |index| is the
  /// position value optionally specified via the index property. |url| is the
  /// URL that will be loaded in the browser. |active| is true if the new
  /// browser should be active when opened.  To allow creation of the browser
  /// optionally modify |windowInfo|, |client| and |settings| and return false.
  /// To cancel creation of the browser return true. Successful creation will be
  /// indicated by a call to HoneycombLifeSpanHandler::OnAfterCreated. Any
  /// modifications to |windowInfo| will be ignored if |active_browser| is
  /// wrapped in a HoneycombBrowserView.
  ///
  /*--honey()--*/
  virtual bool OnBeforeBrowser(HoneycombRefPtr<HoneycombExtension> extension,
                               HoneycombRefPtr<HoneycombBrowser> browser,
                               HoneycombRefPtr<HoneycombBrowser> active_browser,
                               int index,
                               const HoneycombString& url,
                               bool active,
                               HoneycombWindowInfo& windowInfo,
                               HoneycombRefPtr<HoneycombClient>& client,
                               HoneycombBrowserSettings& settings) {
    return false;
  }

  ///
  /// Called when no tabId is specified to an extension API call that accepts a
  /// tabId parameter (e.g. chrome.tabs.*). |extension| and |browser| are the
  /// source of the API call. Return the browser that will be acted on by the
  /// API call or return NULL to act on |browser|. The returned browser must
  /// share the same HoneycombRequestContext as |browser|. Incognito browsers should
  /// not be considered unless the source extension has incognito access
  /// enabled, in which case |include_incognito| will be true.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombBrowser> GetActiveBrowser(
      HoneycombRefPtr<HoneycombExtension> extension,
      HoneycombRefPtr<HoneycombBrowser> browser,
      bool include_incognito) {
    return nullptr;
  }

  ///
  /// Called when the tabId associated with |target_browser| is specified to an
  /// extension API call that accepts a tabId parameter (e.g. chrome.tabs.*).
  /// |extension| and |browser| are the source of the API call. Return true
  /// to allow access of false to deny access. Access to incognito browsers
  /// should not be allowed unless the source extension has incognito access
  /// enabled, in which case |include_incognito| will be true.
  ///
  /*--honey()--*/
  virtual bool CanAccessBrowser(HoneycombRefPtr<HoneycombExtension> extension,
                                HoneycombRefPtr<HoneycombBrowser> browser,
                                bool include_incognito,
                                HoneycombRefPtr<HoneycombBrowser> target_browser) {
    return true;
  }

  ///
  /// Called to retrieve an extension resource that would normally be loaded
  /// from disk (e.g. if a file parameter is specified to
  /// chrome.tabs.executeScript). |extension| and |browser| are the source of
  /// the resource request. |file| is the requested relative file path. To
  /// handle the resource request return true and execute |callback| either
  /// synchronously or asynchronously. For the default behavior which reads the
  /// resource from the extension directory on disk return false. Localization
  /// substitutions will not be applied to resources handled via this method.
  ///
  /*--honey()--*/
  virtual bool GetExtensionResource(
      HoneycombRefPtr<HoneycombExtension> extension,
      HoneycombRefPtr<HoneycombBrowser> browser,
      const HoneycombString& file,
      HoneycombRefPtr<HoneycombGetExtensionResourceCallback> callback) {
    return false;
  }
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_EXTENSION_HANDLER_H_
