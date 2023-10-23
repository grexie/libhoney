// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoney/browser/views/basic_panel_impl.h"

#include "libhoney/browser/views/basic_panel_view.h"

// static
HoneycombRefPtr<HoneycombPanel> HoneycombPanel::CreatePanel(
    HoneycombRefPtr<HoneycombPanelDelegate> delegate) {
  return HoneycombBasicPanelImpl::Create(delegate);
}

// static
HoneycombRefPtr<HoneycombBasicPanelImpl> HoneycombBasicPanelImpl::Create(
    HoneycombRefPtr<HoneycombPanelDelegate> delegate) {
  HONEYCOMB_REQUIRE_UIT_RETURN(nullptr);
  HoneycombRefPtr<HoneycombBasicPanelImpl> panel = new HoneycombBasicPanelImpl(delegate);
  panel->Initialize();
  return panel;
}

HoneycombBasicPanelImpl::HoneycombBasicPanelImpl(HoneycombRefPtr<HoneycombPanelDelegate> delegate)
    : ParentClass(delegate) {}

views::View* HoneycombBasicPanelImpl::CreateRootView() {
  return new HoneycombBasicPanelView(delegate());
}

void HoneycombBasicPanelImpl::InitializeRootView() {
  static_cast<HoneycombBasicPanelView*>(root_view())->Initialize();
}
