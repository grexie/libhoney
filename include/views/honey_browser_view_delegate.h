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

#ifndef HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_BROWSER_VIEW_DELEGATE_H_
#define HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_BROWSER_VIEW_DELEGATE_H_
#pragma once

#include "include/honey_client.h"
#include "include/views/honey_view_delegate.h"

class HoneycombBrowser;
class HoneycombBrowserView;

///
/// Implement this interface to handle BrowserView events. The methods of this
/// class will be called on the browser process UI thread unless otherwise
/// indicated.
///
/*--honey(source=client)--*/
class HoneycombBrowserViewDelegate : public HoneycombViewDelegate {
 public:
  typedef honey_chrome_toolbar_type_t ChromeToolbarType;

  ///
  /// Called when |browser| associated with |browser_view| is created. This
  /// method will be called after HoneycombLifeSpanHandler::OnAfterCreated() is called
  /// for |browser| and before OnPopupBrowserViewCreated() is called for
  /// |browser|'s parent delegate if |browser| is a popup.
  ///
  /*--honey()--*/
  virtual void OnBrowserCreated(HoneycombRefPtr<HoneycombBrowserView> browser_view,
                                HoneycombRefPtr<HoneycombBrowser> browser) {}

  ///
  /// Called when |browser| associated with |browser_view| is destroyed. Release
  /// all references to |browser| and do not attempt to execute any methods on
  /// |browser| after this callback returns. This method will be called before
  /// HoneycombLifeSpanHandler::OnBeforeClose() is called for |browser|.
  ///
  /*--honey()--*/
  virtual void OnBrowserDestroyed(HoneycombRefPtr<HoneycombBrowserView> browser_view,
                                  HoneycombRefPtr<HoneycombBrowser> browser) {}

  ///
  /// Called before a new popup BrowserView is created. The popup originated
  /// from |browser_view|. |settings| and |client| are the values returned from
  /// HoneycombLifeSpanHandler::OnBeforePopup(). |is_devtools| will be true if the
  /// popup will be a DevTools browser. Return the delegate that will be used
  /// for the new popup BrowserView.
  ///
  /*--honey(optional_param=client)--*/
  virtual HoneycombRefPtr<HoneycombBrowserViewDelegate> GetDelegateForPopupBrowserView(
      HoneycombRefPtr<HoneycombBrowserView> browser_view,
      const HoneycombBrowserSettings& settings,
      HoneycombRefPtr<HoneycombClient> client,
      bool is_devtools) {
    return this;
  }

  ///
  /// Called after |popup_browser_view| is created. This method will be called
  /// after HoneycombLifeSpanHandler::OnAfterCreated() and OnBrowserCreated() are
  /// called for the new popup browser. The popup originated from
  /// |browser_view|. |is_devtools| will be true if the popup is a DevTools
  /// browser. Optionally add |popup_browser_view| to the views hierarchy
  /// yourself and return true. Otherwise return false and a default HoneycombWindow
  /// will be created for the popup.
  ///
  /*--honey()--*/
  virtual bool OnPopupBrowserViewCreated(
      HoneycombRefPtr<HoneycombBrowserView> browser_view,
      HoneycombRefPtr<HoneycombBrowserView> popup_browser_view,
      bool is_devtools) {
    return false;
  }

  ///
  /// Returns the Chrome toolbar type that will be available via
  /// HoneycombBrowserView::GetChromeToolbar(). See that method for related
  /// documentation.
  ///
  /*--honey(default_retval=HONEYCOMB_CTT_NONE)--*/
  virtual ChromeToolbarType GetChromeToolbarType(
      HoneycombRefPtr<HoneycombBrowserView> browser_view) {
    return HONEYCOMB_CTT_NONE;
  }

  ///
  /// Return true to create frameless windows for Document picture-in-picture
  /// popups. Content in frameless windows should specify draggable regions
  /// using "-webkit-app-region: drag" CSS.
  ///
  /*--honey()--*/
  virtual bool UseFramelessWindowForPictureInPicture(
      HoneycombRefPtr<HoneycombBrowserView> browser_view) {
    return false;
  }

  ///
  /// Called when |browser_view| receives a gesture command. Return true to
  /// handle (or disable) a |gesture_command| or false to propagate the gesture
  /// to the browser for default handling. With the Chrome runtime these
  /// commands can also be handled via HoneycombCommandHandler::OnChromeCommand.
  ///
  /*--honey()--*/
  virtual bool OnGestureCommand(HoneycombRefPtr<HoneycombBrowserView> browser_view,
                                honey_gesture_command_t gesture_command) {
    return false;
  }
};

#endif  // HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_BROWSER_VIEW_DELEGATE_H_
