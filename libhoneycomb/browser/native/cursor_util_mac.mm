// Copyright 2023 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "include/internal/honey_types_mac.h"
#include "libhoneycomb/browser/native/cursor_util.h"

#import "ui/base/cocoa/cursor_utils.h"

namespace cursor_util {

namespace {

class ScopedCursorHandleImpl : public ScopedCursorHandle {
 public:
  explicit ScopedCursorHandleImpl(NSCursor* native_cursor) {
    if (native_cursor) {
      cursor_ = native_cursor;
    }
  }

  honey_cursor_handle_t GetCursorHandle() override {
    return CAST_NSCURSOR_TO_HONEYCOMB_CURSOR_HANDLE(cursor_);
  }

 private:
  NSCursor* __strong cursor_;
};

}  // namespace

// static
std::unique_ptr<ScopedCursorHandle> ScopedCursorHandle::Create(
    HoneycombRefPtr<HoneycombBrowser> /*browser*/,
    const ui::Cursor& ui_cursor) {
  return std::make_unique<ScopedCursorHandleImpl>(
      ui::GetNativeCursor(ui_cursor));
}

}  // namespace cursor_util
