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

#ifndef HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_LABEL_BUTTON_H_
#define HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_LABEL_BUTTON_H_
#pragma once

#include "include/honey_image.h"
#include "include/views/honey_button.h"
#include "include/views/honey_button_delegate.h"

class HoneycombMenuButton;

///
/// LabelButton is a button with optional text and/or icon. Methods must be
/// called on the browser process UI thread unless otherwise indicated.
///
/*--honey(source=library)--*/
class HoneycombLabelButton : public HoneycombButton {
 public:
  ///
  /// Create a new LabelButton. A |delegate| must be provided to handle the
  /// button click. |text| will be shown on the LabelButton and used as the
  /// default accessible name.
  ///
  /*--honey(optional_param=text)--*/
  static HoneycombRefPtr<HoneycombLabelButton> CreateLabelButton(
      HoneycombRefPtr<HoneycombButtonDelegate> delegate,
      const HoneycombString& text);

  ///
  /// Returns this LabelButton as a MenuButton or NULL if this is not a
  /// MenuButton.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombMenuButton> AsMenuButton() = 0;

  ///
  /// Sets the text shown on the LabelButton. By default |text| will also be
  /// used as the accessible name.
  ///
  /*--honey()--*/
  virtual void SetText(const HoneycombString& text) = 0;

  ///
  /// Returns the text shown on the LabelButton.
  ///
  /*--honey()--*/
  virtual HoneycombString GetText() = 0;

  ///
  /// Sets the image shown for |button_state|. When this Button is drawn if no
  /// image exists for the current state then the image for
  /// HONEYCOMB_BUTTON_STATE_NORMAL, if any, will be shown.
  ///
  /*--honey(optional_param=image)--*/
  virtual void SetImage(honey_button_state_t button_state,
                        HoneycombRefPtr<HoneycombImage> image) = 0;

  ///
  /// Returns the image shown for |button_state|. If no image exists for that
  /// state then the image for HONEYCOMB_BUTTON_STATE_NORMAL will be returned.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombImage> GetImage(honey_button_state_t button_state) = 0;

  ///
  /// Sets the text color shown for the specified button |for_state| to |color|.
  ///
  /*--honey()--*/
  virtual void SetTextColor(honey_button_state_t for_state,
                            honey_color_t color) = 0;

  ///
  /// Sets the text colors shown for the non-disabled states to |color|.
  ///
  /*--honey()--*/
  virtual void SetEnabledTextColors(honey_color_t color) = 0;

  ///
  /// Sets the font list. The format is "<FONT_FAMILY_LIST>,[STYLES] <SIZE>",
  /// where:
  /// - FONT_FAMILY_LIST is a comma-separated list of font family names,
  /// - STYLES is an optional space-separated list of style names
  /// (case-sensitive
  ///   "Bold" and "Italic" are supported), and
  /// - SIZE is an integer font size in pixels with the suffix "px".
  ///
  /// Here are examples of valid font description strings:
  /// - "Arial, Helvetica, Bold Italic 14px"
  /// - "Arial, 14px"
  ///
  /*--honey()--*/
  virtual void SetFontList(const HoneycombString& font_list) = 0;

  ///
  /// Sets the horizontal alignment; reversed in RTL. Default is
  /// HONEYCOMB_HORIZONTAL_ALIGNMENT_CENTER.
  ///
  /*--honey()--*/
  virtual void SetHorizontalAlignment(honey_horizontal_alignment_t alignment) = 0;

  ///
  /// Reset the minimum size of this LabelButton to |size|.
  ///
  /*--honey()--*/
  virtual void SetMinimumSize(const HoneycombSize& size) = 0;

  ///
  /// Reset the maximum size of this LabelButton to |size|.
  ///
  /*--honey()--*/
  virtual void SetMaximumSize(const HoneycombSize& size) = 0;
};

#endif  // HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_LABEL_BUTTON_H_
