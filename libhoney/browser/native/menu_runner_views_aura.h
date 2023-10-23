// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_NATIVE_MENU_RUNNER_VIEWS_AURA_H_
#define HONEYCOMB_LIBHONEY_BROWSER_NATIVE_MENU_RUNNER_VIEWS_AURA_H_
#pragma once

#include "libhoney/browser/menu_runner.h"

#include "ui/views/controls/menu/menu_runner.h"

class HoneycombMenuRunnerViewsAura : public HoneycombMenuRunner {
 public:
  HoneycombMenuRunnerViewsAura();

  // HoneycombMenuRunner methods.
  bool RunContextMenu(AlloyBrowserHostImpl* browser,
                      HoneycombMenuModelImpl* model,
                      const content::ContextMenuParams& params) override;
  void CancelContextMenu() override;
  bool FormatLabel(std::u16string& label) override;

 private:
  std::unique_ptr<views::MenuRunner> menu_;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_NATIVE_MENU_RUNNER_VIEWS_AURA_H_
