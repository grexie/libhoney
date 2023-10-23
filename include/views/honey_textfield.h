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

#ifndef HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_TEXTFIELD_H_
#define HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_TEXTFIELD_H_
#pragma once

#include "include/views/honey_textfield_delegate.h"
#include "include/views/honey_view.h"

///
/// A Textfield supports editing of text. This control is custom rendered with
/// no platform-specific code. Methods must be called on the browser process UI
/// thread unless otherwise indicated.
///
/*--honey(source=library)--*/
class HoneycombTextfield : public HoneycombView {
 public:
  ///
  /// Create a new Textfield.
  ///
  /*--honey(optional_param=delegate)--*/
  static HoneycombRefPtr<HoneycombTextfield> CreateTextfield(
      HoneycombRefPtr<HoneycombTextfieldDelegate> delegate);

  ///
  /// Sets whether the text will be displayed as asterisks.
  ///
  /*--honey()--*/
  virtual void SetPasswordInput(bool password_input) = 0;

  ///
  /// Returns true if the text will be displayed as asterisks.
  ///
  /*--honey()--*/
  virtual bool IsPasswordInput() = 0;

  ///
  /// Sets whether the text will read-only.
  ///
  /*--honey()--*/
  virtual void SetReadOnly(bool read_only) = 0;

  ///
  /// Returns true if the text is read-only.
  ///
  /*--honey()--*/
  virtual bool IsReadOnly() = 0;

  ///
  /// Returns the currently displayed text.
  ///
  /*--honey()--*/
  virtual HoneycombString GetText() = 0;

  ///
  /// Sets the contents to |text|. The cursor will be moved to end of the text
  /// if the current position is outside of the text range.
  ///
  /*--honey()--*/
  virtual void SetText(const HoneycombString& text) = 0;

  ///
  /// Appends |text| to the previously-existing text.
  ///
  /*--honey()--*/
  virtual void AppendText(const HoneycombString& text) = 0;

  ///
  /// Inserts |text| at the current cursor position replacing any selected text.
  ///
  /*--honey()--*/
  virtual void InsertOrReplaceText(const HoneycombString& text) = 0;

  ///
  /// Returns true if there is any selected text.
  ///
  /*--honey()--*/
  virtual bool HasSelection() = 0;

  ///
  /// Returns the currently selected text.
  ///
  /*--honey()--*/
  virtual HoneycombString GetSelectedText() = 0;

  ///
  /// Selects all text. If |reversed| is true the range will end at the logical
  /// beginning of the text; this generally shows the leading portion of text
  /// that overflows its display area.
  ///
  /*--honey()--*/
  virtual void SelectAll(bool reversed) = 0;

  ///
  /// Clears the text selection and sets the caret to the end.
  ///
  /*--honey()--*/
  virtual void ClearSelection() = 0;

  ///
  /// Returns the selected logical text range.
  ///
  /*--honey()--*/
  virtual HoneycombRange GetSelectedRange() = 0;

  ///
  /// Selects the specified logical text range.
  ///
  /*--honey()--*/
  virtual void SelectRange(const HoneycombRange& range) = 0;

  ///
  /// Returns the current cursor position.
  ///
  /*--honey()--*/
  virtual size_t GetCursorPosition() = 0;

  ///
  /// Sets the text color.
  ///
  /*--honey()--*/
  virtual void SetTextColor(honey_color_t color) = 0;

  ///
  /// Returns the text color.
  ///
  /*--honey()--*/
  virtual honey_color_t GetTextColor() = 0;

  ///
  /// Sets the selection text color.
  ///
  /*--honey()--*/
  virtual void SetSelectionTextColor(honey_color_t color) = 0;

  ///
  /// Returns the selection text color.
  ///
  /*--honey()--*/
  virtual honey_color_t GetSelectionTextColor() = 0;

  ///
  /// Sets the selection background color.
  ///
  /*--honey()--*/
  virtual void SetSelectionBackgroundColor(honey_color_t color) = 0;

  ///
  /// Returns the selection background color.
  ///
  /*--honey()--*/
  virtual honey_color_t GetSelectionBackgroundColor() = 0;

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
  /// Applies |color| to the specified |range| without changing the default
  /// color. If |range| is empty the color will be set on the complete text
  /// contents.
  ///
  /*--honey()--*/
  virtual void ApplyTextColor(honey_color_t color, const HoneycombRange& range) = 0;

  ///
  /// Applies |style| to the specified |range| without changing the default
  /// style. If |add| is true the style will be added, otherwise the style will
  /// be removed. If |range| is empty the style will be set on the complete text
  /// contents.
  ///
  /*--honey()--*/
  virtual void ApplyTextStyle(honey_text_style_t style,
                              bool add,
                              const HoneycombRange& range) = 0;

  ///
  /// Returns true if the action associated with the specified command id is
  /// enabled. See additional comments on ExecuteCommand().
  ///
  /*--honey()--*/
  virtual bool IsCommandEnabled(honey_text_field_commands_t command_id) = 0;

  ///
  /// Performs the action associated with the specified command id.
  ///
  /*--honey()--*/
  virtual void ExecuteCommand(honey_text_field_commands_t command_id) = 0;

  ///
  /// Clears Edit history.
  ///
  /*--honey()--*/
  virtual void ClearEditHistory() = 0;

  ///
  /// Sets the placeholder text that will be displayed when the Textfield is
  /// empty.
  ///
  /*--honey()--*/
  virtual void SetPlaceholderText(const HoneycombString& text) = 0;

  ///
  /// Returns the placeholder text that will be displayed when the Textfield is
  /// empty.
  ///
  /*--honey()--*/
  virtual HoneycombString GetPlaceholderText() = 0;

  ///
  /// Sets the placeholder text color.
  ///
  /*--honey()--*/
  virtual void SetPlaceholderTextColor(honey_color_t color) = 0;

  ///
  /// Set the accessible name that will be exposed to assistive technology (AT).
  ///
  /*--honey()--*/
  virtual void SetAccessibleName(const HoneycombString& name) = 0;
};

#endif  // HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_TEXTFIELD_H_
