// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoneycomb/browser/views/fill_layout_impl.h"

#include "libhoneycomb/browser/thread_util.h"

// static
HoneycombRefPtr<HoneycombFillLayout> HoneycombFillLayoutImpl::Create(views::View* owner_view) {
  HONEYCOMB_REQUIRE_UIT_RETURN(nullptr);
  HoneycombRefPtr<HoneycombFillLayoutImpl> impl = new HoneycombFillLayoutImpl();
  impl->Initialize(owner_view);
  return impl;
}

HoneycombFillLayoutImpl::HoneycombFillLayoutImpl() {}

views::FillLayout* HoneycombFillLayoutImpl::CreateLayout() {
  return new views::FillLayout();
}
