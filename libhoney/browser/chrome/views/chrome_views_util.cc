// Copyright 2021 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoney/browser/chrome/views/chrome_views_util.h"

#include "libhoney/browser/views/view_util.h"

namespace honey {

bool IsHoneycombView(views::View* view) {
  return view_util::GetFor(view, /*find_known_parent=*/false) != nullptr;
}

}  // namespace honey
