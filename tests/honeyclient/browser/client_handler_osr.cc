// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeyclient/browser/client_handler_osr.h"

#include "include/base/honey_callback.h"
#include "include/wrapper/honey_closure_task.h"
#include "include/wrapper/honey_helpers.h"

namespace client {

ClientHandlerOsr::ClientHandlerOsr(Delegate* delegate,
                                   OsrDelegate* osr_delegate,
                                   bool with_controls,
                                   const std::string& startup_url)
    : ClientHandler(delegate, /*is_osr=*/true, with_controls, startup_url),
      osr_delegate_(osr_delegate) {
  DCHECK(osr_delegate_);
}

void ClientHandlerOsr::DetachOsrDelegate() {
  if (!HoneycombCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
    HoneycombPostTask(TID_UI,
                base::BindOnce(&ClientHandlerOsr::DetachOsrDelegate, this));
    return;
  }

  DCHECK(osr_delegate_);
  osr_delegate_ = nullptr;
}

void ClientHandlerOsr::OnAfterCreated(HoneycombRefPtr<HoneycombBrowser> browser) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  if (osr_delegate_) {
    osr_delegate_->OnAfterCreated(browser);
  }
  ClientHandler::OnAfterCreated(browser);
}

void ClientHandlerOsr::OnBeforeClose(HoneycombRefPtr<HoneycombBrowser> browser) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  if (osr_delegate_) {
    osr_delegate_->OnBeforeClose(browser);
  }
  ClientHandler::OnBeforeClose(browser);
}

bool ClientHandlerOsr::GetRootScreenRect(HoneycombRefPtr<HoneycombBrowser> browser,
                                         HoneycombRect& rect) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  if (!osr_delegate_) {
    return false;
  }
  return osr_delegate_->GetRootScreenRect(browser, rect);
}

void ClientHandlerOsr::GetViewRect(HoneycombRefPtr<HoneycombBrowser> browser,
                                   HoneycombRect& rect) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  if (!osr_delegate_) {
    // Never return an empty rectangle.
    rect.width = rect.height = 1;
    return;
  }
  osr_delegate_->GetViewRect(browser, rect);
}

bool ClientHandlerOsr::GetScreenPoint(HoneycombRefPtr<HoneycombBrowser> browser,
                                      int viewX,
                                      int viewY,
                                      int& screenX,
                                      int& screenY) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  if (!osr_delegate_) {
    return false;
  }
  return osr_delegate_->GetScreenPoint(browser, viewX, viewY, screenX, screenY);
}

bool ClientHandlerOsr::GetScreenInfo(HoneycombRefPtr<HoneycombBrowser> browser,
                                     HoneycombScreenInfo& screen_info) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  if (!osr_delegate_) {
    return false;
  }
  return osr_delegate_->GetScreenInfo(browser, screen_info);
}

void ClientHandlerOsr::OnPopupShow(HoneycombRefPtr<HoneycombBrowser> browser, bool show) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  if (!osr_delegate_) {
    return;
  }
  return osr_delegate_->OnPopupShow(browser, show);
}

void ClientHandlerOsr::OnPopupSize(HoneycombRefPtr<HoneycombBrowser> browser,
                                   const HoneycombRect& rect) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  if (!osr_delegate_) {
    return;
  }
  return osr_delegate_->OnPopupSize(browser, rect);
}

void ClientHandlerOsr::OnPaint(HoneycombRefPtr<HoneycombBrowser> browser,
                               PaintElementType type,
                               const RectList& dirtyRects,
                               const void* buffer,
                               int width,
                               int height) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  if (!osr_delegate_) {
    return;
  }
  osr_delegate_->OnPaint(browser, type, dirtyRects, buffer, width, height);
}

void ClientHandlerOsr::OnAcceleratedPaint(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRenderHandler::PaintElementType type,
    const HoneycombRenderHandler::RectList& dirtyRects,
    void* share_handle) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  if (!osr_delegate_) {
    return;
  }
  osr_delegate_->OnAcceleratedPaint(browser, type, dirtyRects, share_handle);
}

bool ClientHandlerOsr::StartDragging(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombDragData> drag_data,
    HoneycombRenderHandler::DragOperationsMask allowed_ops,
    int x,
    int y) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  if (!osr_delegate_) {
    return false;
  }
  return osr_delegate_->StartDragging(browser, drag_data, allowed_ops, x, y);
}

void ClientHandlerOsr::UpdateDragCursor(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRenderHandler::DragOperation operation) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  if (!osr_delegate_) {
    return;
  }
  osr_delegate_->UpdateDragCursor(browser, operation);
}

void ClientHandlerOsr::OnImeCompositionRangeChanged(
    HoneycombRefPtr<HoneycombBrowser> browser,
    const HoneycombRange& selection_range,
    const HoneycombRenderHandler::RectList& character_bounds) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  if (!osr_delegate_) {
    return;
  }
  osr_delegate_->OnImeCompositionRangeChanged(browser, selection_range,
                                              character_bounds);
}

void ClientHandlerOsr::OnAccessibilityTreeChange(HoneycombRefPtr<HoneycombValue> value) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  if (!osr_delegate_) {
    return;
  }
  osr_delegate_->UpdateAccessibilityTree(value);
}

bool ClientHandlerOsr::OnCursorChange(HoneycombRefPtr<HoneycombBrowser> browser,
                                      HoneycombCursorHandle cursor,
                                      honey_cursor_type_t type,
                                      const HoneycombCursorInfo& custom_cursor_info) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  if (ClientHandler::OnCursorChange(browser, cursor, type,
                                    custom_cursor_info)) {
    return true;
  }
  if (osr_delegate_) {
    osr_delegate_->OnCursorChange(browser, cursor, type, custom_cursor_info);
  }
  return true;
}

void ClientHandlerOsr::OnAccessibilityLocationChange(
    HoneycombRefPtr<HoneycombValue> value) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  if (!osr_delegate_) {
    return;
  }
  osr_delegate_->UpdateAccessibilityLocation(value);
}

}  // namespace client
