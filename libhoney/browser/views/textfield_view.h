// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_VIEWS_TEXTFIELD_VIEW_H_
#define HONEYCOMB_LIBHONEY_BROWSER_VIEWS_TEXTFIELD_VIEW_H_
#pragma once

#include "include/views/honey_textfield_delegate.h"

#include "include/views/honey_textfield.h"
#include "libhoney/browser/views/view_view.h"

#include "ui/views/controls/textfield/textfield.h"
#include "ui/views/controls/textfield/textfield_controller.h"

class HoneycombTextfieldView
    : public HoneycombViewView<views::Textfield, HoneycombTextfieldDelegate>,
      public views::TextfieldController {
 public:
  using ParentClass = HoneycombViewView<views::Textfield, HoneycombTextfieldDelegate>;

  // |honey_delegate| may be nullptr.
  explicit HoneycombTextfieldView(HoneycombTextfieldDelegate* honey_delegate);

  HoneycombTextfieldView(const HoneycombTextfieldView&) = delete;
  HoneycombTextfieldView& operator=(const HoneycombTextfieldView&) = delete;

  void Initialize() override;

  // Returns the HoneycombTextfield associated with this view. See comments on
  // HoneycombViewView::GetHoneycombView.
  HoneycombRefPtr<HoneycombTextfield> GetHoneycombTextfield() const {
    HoneycombRefPtr<HoneycombTextfield> textfield = GetHoneycombView()->AsTextfield();
    DCHECK(textfield);
    return textfield;
  }

  // TextfieldController methods:
  bool HandleKeyEvent(views::Textfield* sender,
                      const ui::KeyEvent& key_event) override;
  void OnAfterUserAction(views::Textfield* sender) override;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_VIEWS_TEXTFIELD_VIEW_H_
