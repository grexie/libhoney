// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_VIEWS_BASIC_PANEL_IMPL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_VIEWS_BASIC_PANEL_IMPL_H_
#pragma once

#include "include/views/honey_panel.h"
#include "include/views/honey_panel_delegate.h"

#include "libhoney/browser/views/panel_impl.h"

#include "ui/views/view.h"

class HoneycombBasicPanelImpl
    : public HoneycombPanelImpl<views::View, HoneycombPanel, HoneycombPanelDelegate> {
 public:
  using ParentClass = HoneycombPanelImpl<views::View, HoneycombPanel, HoneycombPanelDelegate>;

  HoneycombBasicPanelImpl(const HoneycombBasicPanelImpl&) = delete;
  HoneycombBasicPanelImpl& operator=(const HoneycombBasicPanelImpl&) = delete;

  // Create a new HoneycombPanel instance. |delegate| may be nullptr.
  static HoneycombRefPtr<HoneycombBasicPanelImpl> Create(
      HoneycombRefPtr<HoneycombPanelDelegate> delegate);

  // HoneycombViewAdapter methods:
  std::string GetDebugType() override { return "Panel"; }

 private:
  // Create a new implementation object.
  // Always call Initialize() after creation.
  // |delegate| may be nullptr.
  explicit HoneycombBasicPanelImpl(HoneycombRefPtr<HoneycombPanelDelegate> delegate);

  // HoneycombViewImpl methods:
  views::View* CreateRootView() override;
  void InitializeRootView() override;

  IMPLEMENT_REFCOUNTING_DELETE_ON_UIT(HoneycombBasicPanelImpl);
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_VIEWS_BASIC_PANEL_IMPL_H_
