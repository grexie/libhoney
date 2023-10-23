// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoney/browser/views/menu_runner_views.h"

#include "libhoney/browser/alloy/alloy_browser_host_impl.h"
#include "libhoney/browser/views/browser_view_impl.h"

HoneycombMenuRunnerViews::HoneycombMenuRunnerViews(HoneycombBrowserViewImpl* browser_view)
    : browser_view_(browser_view) {}

bool HoneycombMenuRunnerViews::RunContextMenu(
    AlloyBrowserHostImpl* browser,
    HoneycombMenuModelImpl* model,
    const content::ContextMenuParams& params) {
  HoneycombRefPtr<HoneycombWindow> window = browser_view_->GetWindow();
  if (!window) {
    return false;
  }

  HoneycombPoint screen_point(params.x, params.y);
  browser_view_->ConvertPointToScreen(screen_point);

  window->ShowMenu(model, screen_point, HONEYCOMB_MENU_ANCHOR_TOPRIGHT);
  return true;
}

void HoneycombMenuRunnerViews::CancelContextMenu() {
  HoneycombRefPtr<HoneycombWindow> window = browser_view_->GetWindow();
  if (window) {
    window->CancelMenu();
  }
}

bool HoneycombMenuRunnerViews::FormatLabel(std::u16string& label) {
  // Remove the accelerator indicator (&) from label strings.
  const std::u16string::value_type replace[] = {L'&', 0};
  return base::ReplaceChars(label, replace, std::u16string(), &label);
}
