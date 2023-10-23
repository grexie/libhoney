// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoney/browser/views/layout_adapter.h"

#include "libhoney/browser/views/box_layout_impl.h"
#include "libhoney/browser/views/fill_layout_impl.h"

// static
HoneycombLayoutAdapter* HoneycombLayoutAdapter::GetFor(HoneycombRefPtr<HoneycombLayout> layout) {
  HoneycombLayoutAdapter* adapter = nullptr;
  if (layout->AsBoxLayout()) {
    adapter = static_cast<HoneycombBoxLayoutImpl*>(layout->AsBoxLayout().get());
  } else if (layout->AsFillLayout()) {
    adapter = static_cast<HoneycombFillLayoutImpl*>(layout->AsFillLayout().get());
  }

  DCHECK(adapter);
  return adapter;
}
