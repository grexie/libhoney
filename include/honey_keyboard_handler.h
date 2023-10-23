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

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_KEYBOARD_HANDLER_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_KEYBOARD_HANDLER_H_
#pragma once

#include "include/honey_base.h"
#include "include/honey_browser.h"

///
/// Implement this interface to handle events related to keyboard input. The
/// methods of this class will be called on the UI thread.
///
/*--honey(source=client)--*/
class HoneycombKeyboardHandler : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Called before a keyboard event is sent to the renderer. |event| contains
  /// information about the keyboard event. |os_event| is the operating system
  /// event message, if any. Return true if the event was handled or false
  /// otherwise. If the event will be handled in OnKeyEvent() as a keyboard
  /// shortcut set |is_keyboard_shortcut| to true and return false.
  ///
  /*--honey()--*/
  virtual bool OnPreKeyEvent(HoneycombRefPtr<HoneycombBrowser> browser,
                             const HoneycombKeyEvent& event,
                             HoneycombEventHandle os_event,
                             bool* is_keyboard_shortcut) {
    return false;
  }

  ///
  /// Called after the renderer and JavaScript in the page has had a chance to
  /// handle the event. |event| contains information about the keyboard event.
  /// |os_event| is the operating system event message, if any. Return true if
  /// the keyboard event was handled or false otherwise.
  ///
  /*--honey()--*/
  virtual bool OnKeyEvent(HoneycombRefPtr<HoneycombBrowser> browser,
                          const HoneycombKeyEvent& event,
                          HoneycombEventHandle os_event) {
    return false;
  }
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_KEYBOARD_HANDLER_H_
