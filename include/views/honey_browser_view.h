// Copyright (c) 2016 Marshall A. Greenblatt. All rights reserved.
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

#ifndef HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_BROWSER_VIEW_H_
#define HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_BROWSER_VIEW_H_
#pragma once

#include "include/honey_browser.h"
#include "include/views/honey_browser_view_delegate.h"
#include "include/views/honey_view.h"

///
/// A View hosting a HoneycombBrowser instance. Methods must be called on the browser
/// process UI thread unless otherwise indicated.
///
/*--honey(source=library)--*/
class HoneycombBrowserView : public HoneycombView {
 public:
  ///
  /// Create a new BrowserView. The underlying HoneycombBrowser will not be created
  /// until this view is added to the views hierarchy. The optional |extra_info|
  /// parameter provides an opportunity to specify extra information specific
  /// to the created browser that will be passed to
  /// HoneycombRenderProcessHandler::OnBrowserCreated() in the render process.
  ///
  /*--honey(optional_param=client,optional_param=url,
          optional_param=request_context,optional_param=delegate,
          optional_param=extra_info)--*/
  static HoneycombRefPtr<HoneycombBrowserView> CreateBrowserView(
      HoneycombRefPtr<HoneycombClient> client,
      const HoneycombString& url,
      const HoneycombBrowserSettings& settings,
      HoneycombRefPtr<HoneycombDictionaryValue> extra_info,
      HoneycombRefPtr<HoneycombRequestContext> request_context,
      HoneycombRefPtr<HoneycombBrowserViewDelegate> delegate);

  ///
  /// Returns the BrowserView associated with |browser|.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombBrowserView> GetForBrowser(HoneycombRefPtr<HoneycombBrowser> browser);

  ///
  /// Returns the HoneycombBrowser hosted by this BrowserView. Will return NULL if the
  /// browser has not yet been created or has already been destroyed.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombBrowser> GetBrowser() = 0;

  ///
  /// Returns the Chrome toolbar associated with this BrowserView. Only
  /// supported when using the Chrome runtime. The HoneycombBrowserViewDelegate::
  /// GetChromeToolbarType() method must return a value other than
  /// HONEYCOMB_CTT_NONE and the toolbar will not be available until after this
  /// BrowserView is added to a HoneycombWindow and HoneycombViewDelegate::OnWindowChanged()
  /// has been called.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombView> GetChromeToolbar() = 0;

  ///
  /// Sets whether accelerators registered with HoneycombWindow::SetAccelerator are
  /// triggered before or after the event is sent to the HoneycombBrowser. If
  /// |prefer_accelerators| is true then the matching accelerator will be
  /// triggered immediately and the event will not be sent to the HoneycombBrowser. If
  /// |prefer_accelerators| is false then the matching accelerator will only be
  /// triggered if the event is not handled by web content or by
  /// HoneycombKeyboardHandler. The default value is false.
  ///
  /*--honey()--*/
  virtual void SetPreferAccelerators(bool prefer_accelerators) = 0;
};

#endif  // HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_BROWSER_VIEW_H_
