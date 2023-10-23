// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoney/browser/views/view_adapter.h"

#include "libhoney/browser/chrome/views/toolbar_view_impl.h"
#include "libhoney/browser/views/basic_label_button_impl.h"
#include "libhoney/browser/views/basic_panel_impl.h"
#include "libhoney/browser/views/browser_view_impl.h"
#include "libhoney/browser/views/menu_button_impl.h"
#include "libhoney/browser/views/scroll_view_impl.h"
#include "libhoney/browser/views/textfield_impl.h"
#include "libhoney/browser/views/view_util.h"
#include "libhoney/browser/views/window_impl.h"

// static
HoneycombViewAdapter* HoneycombViewAdapter::GetFor(HoneycombRefPtr<HoneycombView> view) {
  HoneycombViewAdapter* adapter = nullptr;
  if (view->AsBrowserView()) {
    adapter = static_cast<HoneycombBrowserViewImpl*>(view->AsBrowserView().get());
  } else if (view->AsButton()) {
    HoneycombRefPtr<HoneycombButton> button = view->AsButton();
    if (button->AsLabelButton()) {
      HoneycombRefPtr<HoneycombLabelButton> label_button = button->AsLabelButton();
      if (label_button->AsMenuButton()) {
        adapter =
            static_cast<HoneycombMenuButtonImpl*>(label_button->AsMenuButton().get());
      } else {
        adapter = static_cast<HoneycombBasicLabelButtonImpl*>(label_button.get());
      }
    }
  } else if (view->AsPanel()) {
    HoneycombRefPtr<HoneycombPanel> panel = view->AsPanel();
    if (panel->AsWindow()) {
      adapter = static_cast<HoneycombWindowImpl*>(panel->AsWindow().get());
    } else {
      adapter = static_cast<HoneycombBasicPanelImpl*>(panel.get());
    }
  } else if (view->AsScrollView()) {
    adapter = static_cast<HoneycombScrollViewImpl*>(view->AsScrollView().get());
  } else if (view->AsTextfield()) {
    adapter = static_cast<HoneycombTextfieldImpl*>(view->AsTextfield().get());
  } else if (view->GetTypeString().ToString() ==
             HoneycombToolbarViewImpl::kTypeString) {
    adapter = static_cast<HoneycombToolbarViewImpl*>(view.get());
  }

  DCHECK(adapter);
  return adapter;
}

// static
HoneycombViewAdapter* HoneycombViewAdapter::GetFor(views::View* view) {
  HoneycombRefPtr<HoneycombView> honey_view = view_util::GetFor(view, false);
  if (honey_view) {
    return GetFor(honey_view);
  }
  return nullptr;
}
