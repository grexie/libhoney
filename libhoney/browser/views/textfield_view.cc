// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoney/browser/views/textfield_view.h"

#include "libhoney/browser/browser_util.h"

HoneycombTextfieldView::HoneycombTextfieldView(HoneycombTextfieldDelegate* honey_delegate)
    : ParentClass(honey_delegate) {
  set_controller(this);
}

void HoneycombTextfieldView::Initialize() {
  ParentClass::Initialize();

  // Use our defaults instead of the Views framework defaults.
  SetFontList(gfx::FontList(view_util::kDefaultFontList));
}

bool HoneycombTextfieldView::HandleKeyEvent(views::Textfield* sender,
                                      const ui::KeyEvent& key_event) {
  DCHECK_EQ(sender, this);

  if (!honey_delegate()) {
    return false;
  }

  HoneycombKeyEvent honey_key_event;
  if (!browser_util::GetHoneycombKeyEvent(key_event, honey_key_event)) {
    return false;
  }

  return honey_delegate()->OnKeyEvent(GetHoneycombTextfield(), honey_key_event);
}

void HoneycombTextfieldView::OnAfterUserAction(views::Textfield* sender) {
  DCHECK_EQ(sender, this);
  if (honey_delegate()) {
    honey_delegate()->OnAfterUserAction(GetHoneycombTextfield());
  }
}
