// Copyright (c) 2017 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeyclient/browser/views_style.h"

#include "tests/honeyclient/browser/main_context.h"

namespace client {

namespace views_style {

namespace {

honey_color_t g_background_color = 0;
honey_color_t g_background_hover_color = 0;
honey_color_t g_text_color = 0;

int GetShade(int component) {
  return (component < 127) ? component + 75 : component - 75;
}

void MaybeInitialize() {
  static bool initialized = false;
  if (initialized) {
    return;
  }

  g_background_color = MainContext::Get()->GetBackgroundColor();
  if (g_background_color != 0) {
    // Use a slightly modified shade of the background color for hover.
    g_background_hover_color =
        HoneycombColorSetARGB(255, GetShade(HoneycombColorGetR(g_background_color)),
                        GetShade(HoneycombColorGetG(g_background_color)),
                        GetShade(HoneycombColorGetB(g_background_color)));

    // Invert the background color for text.
    g_text_color = HoneycombColorSetARGB(255, 255 - HoneycombColorGetR(g_background_color),
                                   255 - HoneycombColorGetG(g_background_color),
                                   255 - HoneycombColorGetB(g_background_color));
  }

  initialized = true;
}

}  // namespace

bool IsSet() {
  MaybeInitialize();
  return g_background_color != 0;
}

void ApplyBackgroundTo(HoneycombRefPtr<HoneycombView> view) {
  if (!IsSet()) {
    return;
  }

  view->SetBackgroundColor(g_background_color);
}

void ApplyTo(HoneycombRefPtr<HoneycombPanel> panel) {
  if (!IsSet()) {
    return;
  }

  panel->SetBackgroundColor(g_background_color);
}

void ApplyTo(HoneycombRefPtr<HoneycombLabelButton> label_button) {
  if (!IsSet()) {
    return;
  }

  // All text except disabled gets the same color.
  label_button->SetEnabledTextColors(g_text_color);
  label_button->SetTextColor(HONEYCOMB_BUTTON_STATE_DISABLED,
                             g_background_hover_color);

  label_button->SetBackgroundColor(g_background_color);
}

void ApplyTo(HoneycombRefPtr<HoneycombTextfield> textfield) {
  if (!IsSet()) {
    return;
  }

  textfield->SetBackgroundColor(g_background_color);
  textfield->SetTextColor(g_text_color);
}

void ApplyTo(HoneycombRefPtr<HoneycombMenuModel> menu_model) {
  if (!IsSet()) {
    return;
  }

  // All text except non-hovered accelerator gets the same color.
  menu_model->SetColorAt(-1, HONEYCOMB_MENU_COLOR_TEXT, g_text_color);
  menu_model->SetColorAt(-1, HONEYCOMB_MENU_COLOR_TEXT_HOVERED, g_text_color);
  menu_model->SetColorAt(-1, HONEYCOMB_MENU_COLOR_TEXT_ACCELERATOR,
                         g_background_hover_color);
  menu_model->SetColorAt(-1, HONEYCOMB_MENU_COLOR_TEXT_ACCELERATOR_HOVERED,
                         g_text_color);

  menu_model->SetColorAt(-1, HONEYCOMB_MENU_COLOR_BACKGROUND, g_background_color);
  menu_model->SetColorAt(-1, HONEYCOMB_MENU_COLOR_BACKGROUND_HOVERED,
                         g_background_hover_color);

  // Recursively color sub-menus.
  for (size_t i = 0; i < menu_model->GetCount(); ++i) {
    if (menu_model->GetTypeAt(i) == MENUITEMTYPE_SUBMENU) {
      ApplyTo(menu_model->GetSubMenuAt(i));
    }
  }
}

}  // namespace views_style

}  // namespace client
