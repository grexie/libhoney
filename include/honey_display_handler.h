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

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_DISPLAY_HANDLER_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_DISPLAY_HANDLER_H_
#pragma once

#include "include/honey_base.h"
#include "include/honey_browser.h"
#include "include/honey_frame.h"

///
/// Implement this interface to handle events related to browser display state.
/// The methods of this class will be called on the UI thread.
///
/*--honey(source=client)--*/
class HoneycombDisplayHandler : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Called when a frame's address has changed.
  ///
  /*--honey()--*/
  virtual void OnAddressChange(HoneycombRefPtr<HoneycombBrowser> browser,
                               HoneycombRefPtr<HoneycombFrame> frame,
                               const HoneycombString& url) {}

  ///
  /// Called when the page title changes.
  ///
  /*--honey(optional_param=title)--*/
  virtual void OnTitleChange(HoneycombRefPtr<HoneycombBrowser> browser,
                             const HoneycombString& title) {}

  ///
  /// Called when the page icon changes.
  ///
  /*--honey(optional_param=icon_urls)--*/
  virtual void OnFaviconURLChange(HoneycombRefPtr<HoneycombBrowser> browser,
                                  const std::vector<HoneycombString>& icon_urls) {}

  ///
  /// Called when web content in the page has toggled fullscreen mode. If
  /// |fullscreen| is true the content will automatically be sized to fill the
  /// browser content area. If |fullscreen| is false the content will
  /// automatically return to its original size and position. With the Alloy
  /// runtime the client is responsible for triggering the fullscreen transition
  /// (for example, by calling HoneycombWindow::SetFullscreen when using Views). With
  /// the Chrome runtime the fullscreen transition will be triggered
  /// automatically. The HoneycombWindowDelegate::OnWindowFullscreenTransition method
  /// will be called during the fullscreen transition for notification purposes.
  ///
  /*--honey()--*/
  virtual void OnFullscreenModeChange(HoneycombRefPtr<HoneycombBrowser> browser,
                                      bool fullscreen) {}

  ///
  /// Called when the browser is about to display a tooltip. |text| contains the
  /// text that will be displayed in the tooltip. To handle the display of the
  /// tooltip yourself return true. Otherwise, you can optionally modify |text|
  /// and then return false to allow the browser to display the tooltip.
  /// When window rendering is disabled the application is responsible for
  /// drawing tooltips and the return value is ignored.
  ///
  /*--honey(optional_param=text)--*/
  virtual bool OnTooltip(HoneycombRefPtr<HoneycombBrowser> browser, HoneycombString& text) {
    return false;
  }

  ///
  /// Called when the browser receives a status message. |value| contains the
  /// text that will be displayed in the status message.
  ///
  /*--honey(optional_param=value)--*/
  virtual void OnStatusMessage(HoneycombRefPtr<HoneycombBrowser> browser,
                               const HoneycombString& value) {}

  ///
  /// Called to display a console message. Return true to stop the message from
  /// being output to the console.
  ///
  /*--honey(optional_param=message,optional_param=source)--*/
  virtual bool OnConsoleMessage(HoneycombRefPtr<HoneycombBrowser> browser,
                                honey_log_severity_t level,
                                const HoneycombString& message,
                                const HoneycombString& source,
                                int line) {
    return false;
  }

  ///
  /// Called when auto-resize is enabled via
  /// HoneycombBrowserHost::SetAutoResizeEnabled and the contents have auto-resized.
  /// |new_size| will be the desired size in view coordinates. Return true if
  /// the resize was handled or false for default handling.
  ///
  /*--honey()--*/
  virtual bool OnAutoResize(HoneycombRefPtr<HoneycombBrowser> browser,
                            const HoneycombSize& new_size) {
    return false;
  }

  ///
  /// Called when the overall page loading progress has changed. |progress|
  /// ranges from 0.0 to 1.0.
  ///
  /*--honey()--*/
  virtual void OnLoadingProgressChange(HoneycombRefPtr<HoneycombBrowser> browser,
                                       double progress) {}

  ///
  /// Called when the browser's cursor has changed. If |type| is CT_CUSTOM then
  /// |custom_cursor_info| will be populated with the custom cursor information.
  /// Return true if the cursor change was handled or false for default
  /// handling.
  ///
  /*--honey()--*/
  virtual bool OnCursorChange(HoneycombRefPtr<HoneycombBrowser> browser,
                              HoneycombCursorHandle cursor,
                              honey_cursor_type_t type,
                              const HoneycombCursorInfo& custom_cursor_info) {
    return false;
  }

  ///
  /// Called when the browser's access to an audio and/or video source has
  /// changed.
  ///
  /*--honey()--*/
  virtual void OnMediaAccessChange(HoneycombRefPtr<HoneycombBrowser> browser,
                                   bool has_video_access,
                                   bool has_audio_access) {}
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_DISPLAY_HANDLER_H_
