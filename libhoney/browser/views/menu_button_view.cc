// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoney/browser/views/menu_button_view.h"

#include "libhoney/browser/thread_util.h"

#include "ui/gfx/canvas.h"
#include "ui/views/controls/button/menu_button_controller.h"
#include "ui/views/controls/menu/menu_config.h"

namespace {

class ButtonPressedLock : public HoneycombMenuButtonPressedLock {
 public:
  explicit ButtonPressedLock(views::MenuButton* menu_button)
      : pressed_lock_(menu_button->button_controller()) {}

  ButtonPressedLock(const ButtonPressedLock&) = delete;
  ButtonPressedLock& operator=(const ButtonPressedLock&) = delete;

 private:
  views::MenuButtonController::PressedLock pressed_lock_;

  IMPLEMENT_REFCOUNTING_DELETE_ON_UIT(ButtonPressedLock);
};

}  // namespace

HoneycombMenuButtonView::HoneycombMenuButtonView(HoneycombMenuButtonDelegate* honey_delegate)
    : ParentClass(honey_delegate) {
  DCHECK(honey_delegate);
}

void HoneycombMenuButtonView::Initialize() {
  ParentClass::Initialize();

  SetDrawStringsFlags(IsFocusable() ? gfx::Canvas::SHOW_PREFIX
                                    : gfx::Canvas::HIDE_PREFIX);

  // Use the same default font as MenuItemView.
  SetFontList(views::MenuConfig::instance().font_list);
}

HoneycombRefPtr<HoneycombMenuButton> HoneycombMenuButtonView::GetHoneycombMenuButton() const {
  HoneycombRefPtr<HoneycombMenuButton> menu_button = GetHoneycombLabelButton()->AsMenuButton();
  DCHECK(menu_button);
  return menu_button;
}

void HoneycombMenuButtonView::SetDrawStringsFlags(int flags) {
  label()->SetDrawStringsFlags(flags);
}

void HoneycombMenuButtonView::ButtonPressed(const ui::Event& event) {
  auto position = GetMenuPosition();
  honey_delegate()->OnMenuButtonPressed(GetHoneycombMenuButton(),
                                      HoneycombPoint(position.x(), position.y()),
                                      new ButtonPressedLock(this));
}
