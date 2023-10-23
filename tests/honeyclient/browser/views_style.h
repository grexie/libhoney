// Copyright (c) 2017 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_VIEWS_STYLE_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_VIEWS_STYLE_H_
#pragma once

#include "include/honey_menu_model.h"
#include "include/views/honey_label_button.h"
#include "include/views/honey_panel.h"
#include "include/views/honey_textfield.h"

namespace client {

namespace views_style {

// Returns true if a style is set.
bool IsSet();

// Apply style to views objects.
void ApplyBackgroundTo(HoneycombRefPtr<HoneycombView> view);
void ApplyTo(HoneycombRefPtr<HoneycombPanel> panel);
void ApplyTo(HoneycombRefPtr<HoneycombLabelButton> label_button);
void ApplyTo(HoneycombRefPtr<HoneycombTextfield> textfield);
void ApplyTo(HoneycombRefPtr<HoneycombMenuModel> menu_model);

}  // namespace views_style

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_VIEWS_STYLE_H_
