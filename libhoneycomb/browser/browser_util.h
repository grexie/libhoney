// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_BROWSER_UTIL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_BROWSER_UTIL_H_
#pragma once

#include "include/internal/honey_types_wrappers.h"

namespace content {
struct NativeWebKeyboardEvent;
}

namespace ui {
class KeyEvent;
}

namespace browser_util {

// Convert a content::NativeWebKeyboardEvent to a HoneycombKeyEvent.
bool GetHoneycombKeyEvent(const content::NativeWebKeyboardEvent& event,
                    HoneycombKeyEvent& honey_event);

// Convert a ui::KeyEvent to a HoneycombKeyEvent.
bool GetHoneycombKeyEvent(const ui::KeyEvent& event, HoneycombKeyEvent& honey_event);

}  // namespace browser_util

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_BROWSER_UTIL_H_
