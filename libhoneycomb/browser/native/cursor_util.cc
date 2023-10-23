// Copyright 2020 The Honeycomb Authors. Portions copyright
// 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#include "libhoneycomb/browser/native/cursor_util.h"

#include "include/honey_client.h"

#include "ui/base/cursor/mojom/cursor_type.mojom.h"

namespace cursor_util {

bool OnCursorChange(HoneycombRefPtr<HoneycombBrowser> browser,
                    const ui::Cursor& ui_cursor) {
  auto client = browser->GetHost()->GetClient();
  if (!client) {
    return false;
  }
  auto handler = client->GetDisplayHandler();
  if (!handler) {
    return false;
  }

  const honey_cursor_type_t cursor_type =
      static_cast<honey_cursor_type_t>(ui_cursor.type());
  HoneycombCursorInfo custom_cursor_info;
  if (ui_cursor.type() == ui::mojom::CursorType::kCustom) {
    custom_cursor_info.hotspot.x = ui_cursor.custom_hotspot().x();
    custom_cursor_info.hotspot.y = ui_cursor.custom_hotspot().y();
    custom_cursor_info.image_scale_factor = ui_cursor.image_scale_factor();
    custom_cursor_info.buffer = ui_cursor.custom_bitmap().getPixels();
    custom_cursor_info.size.width = ui_cursor.custom_bitmap().width();
    custom_cursor_info.size.height = ui_cursor.custom_bitmap().height();
  }

  auto scoped_cursor_handle(ScopedCursorHandle::Create(browser, ui_cursor));
  return handler->OnCursorChange(browser,
                                 scoped_cursor_handle->GetCursorHandle(),
                                 cursor_type, custom_cursor_info);
}

}  // namespace cursor_util
