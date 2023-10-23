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

#ifndef HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_BUTTON_H_
#define HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_BUTTON_H_
#pragma once

#include "include/views/honey_view.h"

class HoneycombLabelButton;

///
/// A View representing a button. Depending on the specific type, the button
/// could be implemented by a native control or custom rendered. Methods must be
/// called on the browser process UI thread unless otherwise indicated.
///
/*--honey(source=library)--*/
class HoneycombButton : public HoneycombView {
 public:
  ///
  /// Returns this Button as a LabelButton or NULL if this is not a LabelButton.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombLabelButton> AsLabelButton() = 0;

  ///
  /// Sets the current display state of the Button.
  ///
  /*--honey()--*/
  virtual void SetState(honey_button_state_t state) = 0;

  ///
  /// Returns the current display state of the Button.
  ///
  /*--honey(default_retval=HONEYCOMB_BUTTON_STATE_NORMAL)--*/
  virtual honey_button_state_t GetState() = 0;

  ///
  /// Sets the Button will use an ink drop effect for displaying state changes.
  ///
  /*--honey()--*/
  virtual void SetInkDropEnabled(bool enabled) = 0;

  ///
  /// Sets the tooltip text that will be displayed when the user hovers the
  /// mouse cursor over the Button.
  ///
  /*--honey()--*/
  virtual void SetTooltipText(const HoneycombString& tooltip_text) = 0;

  ///
  /// Sets the accessible name that will be exposed to assistive technology
  /// (AT).
  ///
  /*--honey()--*/
  virtual void SetAccessibleName(const HoneycombString& name) = 0;
};

#endif  // HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_BUTTON_H_
