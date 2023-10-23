// Copyright 2022 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_CHROME_VIEWS_CHROME_CHILD_WINDOW_H_
#define HONEYCOMB_LIBHONEY_BROWSER_CHROME_VIEWS_CHROME_CHILD_WINDOW_H_
#pragma once

#include "libhoney/browser/browser_host_base.h"

#include "ui/gfx/native_widget_types.h"

namespace chrome_child_window {

bool HasParentHandle(const HoneycombWindowInfo& window_info);
gfx::AcceleratedWidget GetParentHandle(const HoneycombWindowInfo& window_info);

// Called from HoneycombBrowserHostBase::Create.
HoneycombRefPtr<HoneycombBrowserHostBase> MaybeCreateChildBrowser(
    const HoneycombBrowserCreateParams& create_params);

}  // namespace chrome_child_window

#endif  // HONEYCOMB_LIBHONEY_BROWSER_CHROME_VIEWS_CHROME_CHILD_WINDOW_H_
