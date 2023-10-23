// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_NATIVE_MENU_RUNNER_MAC_H_
#define HONEYCOMB_LIBHONEY_BROWSER_NATIVE_MENU_RUNNER_MAC_H_
#pragma once

#include "libhoney/browser/menu_runner.h"

#if __OBJC__
@class MenuControllerCocoa;
#else
class MenuControllerCocoa;
#endif

class HoneycombMenuRunnerMac : public HoneycombMenuRunner {
 public:
  HoneycombMenuRunnerMac();
  ~HoneycombMenuRunnerMac() override;

  // HoneycombMenuRunner methods.
  bool RunContextMenu(AlloyBrowserHostImpl* browser,
                      HoneycombMenuModelImpl* model,
                      const content::ContextMenuParams& params) override;
  void CancelContextMenu() override;

 private:
  MenuControllerCocoa* __strong menu_controller_;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_NATIVE_MENU_RUNNER_MAC_H_
