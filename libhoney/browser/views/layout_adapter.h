// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_VIEWS_LAYOUT_ADAPTER_H_
#define HONEYCOMB_LIBHONEY_BROWSER_VIEWS_LAYOUT_ADAPTER_H_
#pragma once

#include "include/views/honey_layout.h"

namespace views {
class LayoutManager;
}

// Exposes a common interface from all HoneycombLayout implementation objects to
// simplify the layout_util implementation. See comments in view_impl.h for a
// usage overview.
class HoneycombLayoutAdapter {
 public:
  HoneycombLayoutAdapter() {}

  // Returns the HoneycombLayoutAdapter for the specified |layout|.
  static HoneycombLayoutAdapter* GetFor(HoneycombRefPtr<HoneycombLayout> layout);

  // Returns the underlying views::LayoutManager object. Does not transfer
  // ownership.
  virtual views::LayoutManager* Get() const = 0;

  // Release all references to the views::LayoutManager object. This is called
  // when the views::LayoutManager is deleted after being assigned to a
  // views::View.
  virtual void Detach() = 0;

 protected:
  virtual ~HoneycombLayoutAdapter() {}
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_VIEWS_LAYOUT_ADAPTER_H_
