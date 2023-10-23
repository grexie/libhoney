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

#ifndef HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_MENU_MODEL_DELEGATE_H_
#define HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_MENU_MODEL_DELEGATE_H_
#pragma once

#include "include/honey_base.h"

class HoneycombMenuModel;

///
/// Implement this interface to handle menu model events. The methods of this
/// class will be called on the browser process UI thread unless otherwise
/// indicated.
///
/*--honey(source=client)--*/
class HoneycombMenuModelDelegate : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Perform the action associated with the specified |command_id| and
  /// optional |event_flags|.
  ///
  /*--honey()--*/
  virtual void ExecuteCommand(HoneycombRefPtr<HoneycombMenuModel> menu_model,
                              int command_id,
                              honey_event_flags_t event_flags) = 0;

  ///
  /// Called when the user moves the mouse outside the menu and over the owning
  /// window.
  ///
  /*--honey()--*/
  virtual void MouseOutsideMenu(HoneycombRefPtr<HoneycombMenuModel> menu_model,
                                const HoneycombPoint& screen_point) {}

  ///
  /// Called on unhandled open submenu keyboard commands. |is_rtl| will be true
  /// if the menu is displaying a right-to-left language.
  ///
  /*--honey()--*/
  virtual void UnhandledOpenSubmenu(HoneycombRefPtr<HoneycombMenuModel> menu_model,
                                    bool is_rtl) {}

  ///
  /// Called on unhandled close submenu keyboard commands. |is_rtl| will be true
  /// if the menu is displaying a right-to-left language.
  ///
  /*--honey()--*/
  virtual void UnhandledCloseSubmenu(HoneycombRefPtr<HoneycombMenuModel> menu_model,
                                     bool is_rtl) {}

  ///
  /// The menu is about to show.
  ///
  /*--honey()--*/
  virtual void MenuWillShow(HoneycombRefPtr<HoneycombMenuModel> menu_model) {}

  ///
  /// The menu has closed.
  ///
  /*--honey()--*/
  virtual void MenuClosed(HoneycombRefPtr<HoneycombMenuModel> menu_model) {}

  ///
  /// Optionally modify a menu item label. Return true if |label| was modified.
  ///
  /*--honey()--*/
  virtual bool FormatLabel(HoneycombRefPtr<HoneycombMenuModel> menu_model,
                           HoneycombString& label) {
    return false;
  }
};

#endif  // HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_MENU_MODEL_DELEGATE_H_
