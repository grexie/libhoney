// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_VIEWS_LAYOUT_UTIL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_VIEWS_LAYOUT_UTIL_H_
#pragma once

#include "include/views/honey_layout.h"

#include "ui/views/layout/layout_manager.h"

namespace views {
class View;
}

// The below functions manage the relationship between HoneycombLayout and
// views::LayoutManager instances. See comments in view_impl.h for a usage
// overview.

namespace layout_util {

// Returns the HoneycombLayout object associated with |owner_view|.
HoneycombRefPtr<HoneycombLayout> GetFor(const views::View* owner_view);

// Assign ownership of |layout| to |owner_view|. If a HoneycombLayout is already
// associated with |owner_view| it will be invalidated.
void Assign(HoneycombRefPtr<HoneycombLayout> layout, views::View* owner_view);

}  // namespace layout_util

#endif  // HONEYCOMB_LIBHONEY_BROWSER_VIEWS_LAYOUT_UTIL_H_
