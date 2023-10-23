// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_MENU_RUNNER_VIEWS_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_MENU_RUNNER_VIEWS_H_
#pragma once

#include "libhoneycomb/browser/menu_runner.h"

class HoneycombBrowserViewImpl;

class HoneycombMenuRunnerViews : public HoneycombMenuRunner {
 public:
  // |browser_view| is guaranteed to outlive this object.
  explicit HoneycombMenuRunnerViews(HoneycombBrowserViewImpl* browser_view);

  // HoneycombMenuRunner methods.
  bool RunContextMenu(AlloyBrowserHostImpl* browser,
                      HoneycombMenuModelImpl* model,
                      const content::ContextMenuParams& params) override;
  void CancelContextMenu() override;
  bool FormatLabel(std::u16string& label) override;

 private:
  HoneycombBrowserViewImpl* browser_view_;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_MENU_RUNNER_VIEWS_H_
