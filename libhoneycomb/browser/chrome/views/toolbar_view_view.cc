// Copyright 2021 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoneycomb/browser/chrome/views/toolbar_view_view.h"

HoneycombToolbarViewView::HoneycombToolbarViewView(HoneycombViewDelegate* honey_delegate,
                                       Browser* browser,
                                       BrowserView* browser_view,
                                       absl::optional<DisplayMode> display_mode)
    : ParentClass(honey_delegate, browser, browser_view, display_mode) {}
