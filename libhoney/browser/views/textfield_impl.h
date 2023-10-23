// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_VIEWS_TEXTFIELD_IMPL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_VIEWS_TEXTFIELD_IMPL_H_
#pragma once

#include "include/views/honey_textfield.h"
#include "include/views/honey_textfield_delegate.h"

#include "libhoney/browser/views/textfield_view.h"
#include "libhoney/browser/views/view_impl.h"

class HoneycombTextfieldImpl
    : public HoneycombViewImpl<HoneycombTextfieldView, HoneycombTextfield, HoneycombTextfieldDelegate> {
 public:
  using ParentClass =
      HoneycombViewImpl<HoneycombTextfieldView, HoneycombTextfield, HoneycombTextfieldDelegate>;

  HoneycombTextfieldImpl(const HoneycombTextfieldImpl&) = delete;
  HoneycombTextfieldImpl& operator=(const HoneycombTextfieldImpl&) = delete;

  // Create a new HoneycombTextfield instance. |delegate| may be nullptr.
  static HoneycombRefPtr<HoneycombTextfieldImpl> Create(
      HoneycombRefPtr<HoneycombTextfieldDelegate> delegate);

  // HoneycombTextfield methods:
  void SetPasswordInput(bool password_input) override;
  bool IsPasswordInput() override;
  void SetReadOnly(bool read_only) override;
  bool IsReadOnly() override;
  HoneycombString GetText() override;
  void SetText(const HoneycombString& text) override;
  void AppendText(const HoneycombString& text) override;
  void InsertOrReplaceText(const HoneycombString& text) override;
  bool HasSelection() override;
  HoneycombString GetSelectedText() override;
  void SelectAll(bool reversed) override;
  void ClearSelection() override;
  HoneycombRange GetSelectedRange() override;
  void SelectRange(const HoneycombRange& range) override;
  size_t GetCursorPosition() override;
  void SetTextColor(honey_color_t color) override;
  honey_color_t GetTextColor() override;
  void SetSelectionTextColor(honey_color_t color) override;
  honey_color_t GetSelectionTextColor() override;
  void SetSelectionBackgroundColor(honey_color_t color) override;
  honey_color_t GetSelectionBackgroundColor() override;
  void SetFontList(const HoneycombString& font_list) override;
  void ApplyTextColor(honey_color_t color, const HoneycombRange& range) override;
  void ApplyTextStyle(honey_text_style_t style,
                      bool add,
                      const HoneycombRange& range) override;
  bool IsCommandEnabled(honey_text_field_commands_t command_id) override;
  void ExecuteCommand(honey_text_field_commands_t command_id) override;
  void ClearEditHistory() override;
  void SetPlaceholderText(const HoneycombString& text) override;
  HoneycombString GetPlaceholderText() override;
  void SetPlaceholderTextColor(honey_color_t color) override;
  void SetAccessibleName(const HoneycombString& name) override;

  // HoneycombView methods:
  HoneycombRefPtr<HoneycombTextfield> AsTextfield() override { return this; }
  void SetBackgroundColor(honey_color_t color) override;
  honey_color_t GetBackgroundColor() override;

  // HoneycombViewAdapter methods:
  std::string GetDebugType() override { return "Textfield"; }

 private:
  // Create a new implementation object.
  // Always call Initialize() after creation.
  // |delegate| may be nullptr.
  explicit HoneycombTextfieldImpl(HoneycombRefPtr<HoneycombTextfieldDelegate> delegate);

  // HoneycombViewImpl methods:
  HoneycombTextfieldView* CreateRootView() override;
  void InitializeRootView() override;

  IMPLEMENT_REFCOUNTING_DELETE_ON_UIT(HoneycombTextfieldImpl);
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_VIEWS_TEXTFIELD_IMPL_H_
