// Copyright 2020 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_NATIVE_CURSOR_UTIL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_NATIVE_CURSOR_UTIL_H_

#include "include/honey_browser.h"

#include <memory>

#include "ui/base/cursor/cursor.h"
#include "ui/base/cursor/mojom/cursor_type.mojom-forward.h"

#if defined(USE_AURA)
#include "ui/base/cursor/platform_cursor.h"
#endif

namespace cursor_util {

// Scoped ownership of a native cursor handle.
class ScopedCursorHandle {
 public:
  virtual ~ScopedCursorHandle() = default;

  static std::unique_ptr<ScopedCursorHandle> Create(
      HoneycombRefPtr<HoneycombBrowser> browser,
      const ui::Cursor& ui_cursor);

  virtual honey_cursor_handle_t GetCursorHandle() = 0;
};

// Returns true if the client handled the cursor change.
bool OnCursorChange(HoneycombRefPtr<HoneycombBrowser> browser, const ui::Cursor& ui_cursor);

}  // namespace cursor_util

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_NATIVE_CURSOR_UTIL_H_
