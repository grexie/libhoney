// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoneycomb/browser/views/textfield_impl.h"

#include "libhoneycomb/browser/thread_util.h"

namespace {
static int HoneycombCommandIdToChromeId(honey_text_field_commands_t command_id) {
  switch (command_id) {
    case honey_text_field_commands_t::HONEYCOMB_TFC_CUT:
      return views::Textfield::kCut;
    case honey_text_field_commands_t::HONEYCOMB_TFC_COPY:
      return views::Textfield::kCopy;
    case honey_text_field_commands_t::HONEYCOMB_TFC_PASTE:
      return views::Textfield::kPaste;
    case honey_text_field_commands_t::HONEYCOMB_TFC_UNDO:
      return views::Textfield::kUndo;
    case honey_text_field_commands_t::HONEYCOMB_TFC_DELETE:
      return views::Textfield::kDelete;
    case honey_text_field_commands_t::HONEYCOMB_TFC_SELECT_ALL:
      return views::Textfield::kSelectAll;
  }
}
}  // namespace

// static
HoneycombRefPtr<HoneycombTextfield> HoneycombTextfield::CreateTextfield(
    HoneycombRefPtr<HoneycombTextfieldDelegate> delegate) {
  return HoneycombTextfieldImpl::Create(delegate);
}

// static
HoneycombRefPtr<HoneycombTextfieldImpl> HoneycombTextfieldImpl::Create(
    HoneycombRefPtr<HoneycombTextfieldDelegate> delegate) {
  HONEYCOMB_REQUIRE_UIT_RETURN(nullptr);
  HoneycombRefPtr<HoneycombTextfieldImpl> textfield = new HoneycombTextfieldImpl(delegate);
  textfield->Initialize();
  return textfield;
}

void HoneycombTextfieldImpl::SetPasswordInput(bool password_input) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->SetTextInputType(password_input ? ui::TEXT_INPUT_TYPE_PASSWORD
                                               : ui::TEXT_INPUT_TYPE_TEXT);
}

bool HoneycombTextfieldImpl::IsPasswordInput() {
  HONEYCOMB_REQUIRE_VALID_RETURN(false);
  return (root_view()->GetTextInputType() == ui::TEXT_INPUT_TYPE_PASSWORD);
}

void HoneycombTextfieldImpl::SetReadOnly(bool read_only) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->SetReadOnly(read_only);
}

bool HoneycombTextfieldImpl::IsReadOnly() {
  HONEYCOMB_REQUIRE_VALID_RETURN(false);
  return root_view()->GetReadOnly();
}

HoneycombString HoneycombTextfieldImpl::GetText() {
  HONEYCOMB_REQUIRE_VALID_RETURN(HoneycombString());
  return root_view()->GetText();
}

void HoneycombTextfieldImpl::SetText(const HoneycombString& text) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->SetText(text);
}

void HoneycombTextfieldImpl::AppendText(const HoneycombString& text) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->AppendText(text);
}

void HoneycombTextfieldImpl::InsertOrReplaceText(const HoneycombString& text) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->InsertOrReplaceText(text);
}

bool HoneycombTextfieldImpl::HasSelection() {
  HONEYCOMB_REQUIRE_VALID_RETURN(false);
  return root_view()->HasSelection();
}

HoneycombString HoneycombTextfieldImpl::GetSelectedText() {
  HONEYCOMB_REQUIRE_VALID_RETURN(HoneycombString());
  return root_view()->GetSelectedText();
}

void HoneycombTextfieldImpl::SelectAll(bool reversed) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->SelectAll(reversed);
}

void HoneycombTextfieldImpl::ClearSelection() {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->ClearSelection();
}

HoneycombRange HoneycombTextfieldImpl::GetSelectedRange() {
  HONEYCOMB_REQUIRE_VALID_RETURN(HoneycombRange());
  const gfx::Range& range = root_view()->GetSelectedRange();
  return HoneycombRange(range.start(), range.end());
}

void HoneycombTextfieldImpl::SelectRange(const HoneycombRange& range) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->SetSelectedRange(gfx::Range(range.from, range.to));
}

size_t HoneycombTextfieldImpl::GetCursorPosition() {
  HONEYCOMB_REQUIRE_VALID_RETURN(0U);
  return root_view()->GetCursorPosition();
}

void HoneycombTextfieldImpl::SetTextColor(honey_color_t color) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->SetTextColor(color);
}

honey_color_t HoneycombTextfieldImpl::GetTextColor() {
  HONEYCOMB_REQUIRE_VALID_RETURN(0U);
  return root_view()->GetTextColor();
}

void HoneycombTextfieldImpl::SetSelectionTextColor(honey_color_t color) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->SetSelectionTextColor(color);
}

honey_color_t HoneycombTextfieldImpl::GetSelectionTextColor() {
  HONEYCOMB_REQUIRE_VALID_RETURN(0U);
  return root_view()->GetSelectionTextColor();
}

void HoneycombTextfieldImpl::SetSelectionBackgroundColor(honey_color_t color) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->SetSelectionBackgroundColor(color);
}

honey_color_t HoneycombTextfieldImpl::GetSelectionBackgroundColor() {
  HONEYCOMB_REQUIRE_VALID_RETURN(0U);
  return root_view()->GetSelectionBackgroundColor();
}

void HoneycombTextfieldImpl::SetFontList(const HoneycombString& font_list) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->SetFontList(gfx::FontList(font_list));
}

void HoneycombTextfieldImpl::ApplyTextColor(honey_color_t color,
                                      const HoneycombRange& range) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (range.from == range.to) {
    root_view()->SetColor(color);
  } else {
    root_view()->ApplyColor(color, gfx::Range(range.from, range.to));
  }
}

void HoneycombTextfieldImpl::ApplyTextStyle(honey_text_style_t style,
                                      bool add,
                                      const HoneycombRange& range) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (range.from == range.to) {
    root_view()->SetStyle(static_cast<gfx::TextStyle>(style), add);
  } else {
    root_view()->ApplyStyle(static_cast<gfx::TextStyle>(style), add,
                            gfx::Range(range.from, range.to));
  }
}

bool HoneycombTextfieldImpl::IsCommandEnabled(honey_text_field_commands_t command_id) {
  HONEYCOMB_REQUIRE_VALID_RETURN(false);
  return root_view()->IsCommandIdEnabled(HoneycombCommandIdToChromeId(command_id));
}

void HoneycombTextfieldImpl::ExecuteCommand(honey_text_field_commands_t command_id) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (root_view()->IsCommandIdEnabled(HoneycombCommandIdToChromeId(command_id))) {
    root_view()->ExecuteCommand(HoneycombCommandIdToChromeId(command_id),
                                ui::EF_NONE);
  }
}

void HoneycombTextfieldImpl::ClearEditHistory() {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->ClearEditHistory();
}

void HoneycombTextfieldImpl::SetPlaceholderText(const HoneycombString& text) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->SetPlaceholderText(text);
}

HoneycombString HoneycombTextfieldImpl::GetPlaceholderText() {
  HONEYCOMB_REQUIRE_VALID_RETURN(HoneycombString());
  return root_view()->GetPlaceholderText();
}

void HoneycombTextfieldImpl::SetPlaceholderTextColor(honey_color_t color) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->set_placeholder_text_color(color);
}

void HoneycombTextfieldImpl::SetBackgroundColor(honey_color_t color) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->SetBackgroundColor(color);
}

honey_color_t HoneycombTextfieldImpl::GetBackgroundColor() {
  HONEYCOMB_REQUIRE_VALID_RETURN(0U);
  return root_view()->GetBackgroundColor();
}

void HoneycombTextfieldImpl::SetAccessibleName(const HoneycombString& name) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->SetAccessibleName(name);
}

HoneycombTextfieldImpl::HoneycombTextfieldImpl(HoneycombRefPtr<HoneycombTextfieldDelegate> delegate)
    : ParentClass(delegate) {}

HoneycombTextfieldView* HoneycombTextfieldImpl::CreateRootView() {
  return new HoneycombTextfieldView(delegate());
}

void HoneycombTextfieldImpl::InitializeRootView() {
  static_cast<HoneycombTextfieldView*>(root_view())->Initialize();
}
