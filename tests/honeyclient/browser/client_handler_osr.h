// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_CLIENT_HANDLER_OSR_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_CLIENT_HANDLER_OSR_H_
#pragma once

#include "tests/honeyclient/browser/client_handler.h"

namespace client {

// Client handler implementation for windowless browsers. There will only ever
// be one browser per handler instance.
class ClientHandlerOsr : public ClientHandler,
                         public HoneycombAccessibilityHandler,
                         public HoneycombRenderHandler {
 public:
  // Implement this interface to receive notification of ClientHandlerOsr
  // events. The methods of this class will be called on the Honeycomb UI thread.
  class OsrDelegate {
   public:
    // These methods match the HoneycombLifeSpanHandler interface.
    virtual void OnAfterCreated(HoneycombRefPtr<HoneycombBrowser> browser) = 0;
    virtual void OnBeforeClose(HoneycombRefPtr<HoneycombBrowser> browser) = 0;

    // These methods match the HoneycombRenderHandler interface.
    virtual bool GetRootScreenRect(HoneycombRefPtr<HoneycombBrowser> browser,
                                   HoneycombRect& rect) = 0;
    virtual void GetViewRect(HoneycombRefPtr<HoneycombBrowser> browser, HoneycombRect& rect) = 0;
    virtual bool GetScreenPoint(HoneycombRefPtr<HoneycombBrowser> browser,
                                int viewX,
                                int viewY,
                                int& screenX,
                                int& screenY) = 0;
    virtual bool GetScreenInfo(HoneycombRefPtr<HoneycombBrowser> browser,
                               HoneycombScreenInfo& screen_info) = 0;
    virtual void OnPopupShow(HoneycombRefPtr<HoneycombBrowser> browser, bool show) = 0;
    virtual void OnPopupSize(HoneycombRefPtr<HoneycombBrowser> browser,
                             const HoneycombRect& rect) = 0;
    virtual void OnPaint(HoneycombRefPtr<HoneycombBrowser> browser,
                         HoneycombRenderHandler::PaintElementType type,
                         const HoneycombRenderHandler::RectList& dirtyRects,
                         const void* buffer,
                         int width,
                         int height) = 0;
    virtual void OnAcceleratedPaint(
        HoneycombRefPtr<HoneycombBrowser> browser,
        HoneycombRenderHandler::PaintElementType type,
        const HoneycombRenderHandler::RectList& dirtyRects,
        void* share_handle) {}
    virtual bool StartDragging(HoneycombRefPtr<HoneycombBrowser> browser,
                               HoneycombRefPtr<HoneycombDragData> drag_data,
                               HoneycombRenderHandler::DragOperationsMask allowed_ops,
                               int x,
                               int y) = 0;
    virtual void UpdateDragCursor(
        HoneycombRefPtr<HoneycombBrowser> browser,
        HoneycombRenderHandler::DragOperation operation) = 0;
    virtual void OnImeCompositionRangeChanged(
        HoneycombRefPtr<HoneycombBrowser> browser,
        const HoneycombRange& selection_range,
        const HoneycombRenderHandler::RectList& character_bounds) = 0;

    // These methods match the HoneycombDisplayHandler interface.
    virtual void OnCursorChange(HoneycombRefPtr<HoneycombBrowser> browser,
                                HoneycombCursorHandle cursor,
                                honey_cursor_type_t type,
                                const HoneycombCursorInfo& custom_cursor_info) = 0;

    virtual void UpdateAccessibilityTree(HoneycombRefPtr<HoneycombValue> value) = 0;
    virtual void UpdateAccessibilityLocation(HoneycombRefPtr<HoneycombValue> value) = 0;

   protected:
    virtual ~OsrDelegate() {}
  };

  ClientHandlerOsr(Delegate* delegate,
                   OsrDelegate* osr_delegate,
                   bool with_controls,
                   const std::string& startup_url);

  // This object may outlive the OsrDelegate object so it's necessary for the
  // OsrDelegate to detach itself before destruction.
  void DetachOsrDelegate();

  // HoneycombClient methods.
  HoneycombRefPtr<HoneycombRenderHandler> GetRenderHandler() override { return this; }
  HoneycombRefPtr<HoneycombAccessibilityHandler> GetAccessibilityHandler() override {
    return this;
  }

  // HoneycombLifeSpanHandler methods.
  void OnAfterCreated(HoneycombRefPtr<HoneycombBrowser> browser) override;
  void OnBeforeClose(HoneycombRefPtr<HoneycombBrowser> browser) override;

  // HoneycombRenderHandler methods.
  bool GetRootScreenRect(HoneycombRefPtr<HoneycombBrowser> browser, HoneycombRect& rect) override;
  void GetViewRect(HoneycombRefPtr<HoneycombBrowser> browser, HoneycombRect& rect) override;
  bool GetScreenPoint(HoneycombRefPtr<HoneycombBrowser> browser,
                      int viewX,
                      int viewY,
                      int& screenX,
                      int& screenY) override;
  bool GetScreenInfo(HoneycombRefPtr<HoneycombBrowser> browser,
                     HoneycombScreenInfo& screen_info) override;
  void OnPopupShow(HoneycombRefPtr<HoneycombBrowser> browser, bool show) override;
  void OnPopupSize(HoneycombRefPtr<HoneycombBrowser> browser, const HoneycombRect& rect) override;
  void OnPaint(HoneycombRefPtr<HoneycombBrowser> browser,
               HoneycombRenderHandler::PaintElementType type,
               const HoneycombRenderHandler::RectList& dirtyRects,
               const void* buffer,
               int width,
               int height) override;
  void OnAcceleratedPaint(HoneycombRefPtr<HoneycombBrowser> browser,
                          HoneycombRenderHandler::PaintElementType type,
                          const HoneycombRenderHandler::RectList& dirtyRects,
                          void* share_handle) override;
  bool StartDragging(HoneycombRefPtr<HoneycombBrowser> browser,
                     HoneycombRefPtr<HoneycombDragData> drag_data,
                     HoneycombRenderHandler::DragOperationsMask allowed_ops,
                     int x,
                     int y) override;
  void UpdateDragCursor(HoneycombRefPtr<HoneycombBrowser> browser,
                        HoneycombRenderHandler::DragOperation operation) override;
  void OnImeCompositionRangeChanged(
      HoneycombRefPtr<HoneycombBrowser> browser,
      const HoneycombRange& selection_range,
      const HoneycombRenderHandler::RectList& character_bounds) override;

  // HoneycombDisplayHandler methods.
  bool OnCursorChange(HoneycombRefPtr<HoneycombBrowser> browser,
                      HoneycombCursorHandle cursor,
                      honey_cursor_type_t type,
                      const HoneycombCursorInfo& custom_cursor_info) override;

  // HoneycombAccessibilityHandler methods.
  void OnAccessibilityTreeChange(HoneycombRefPtr<HoneycombValue> value) override;
  void OnAccessibilityLocationChange(HoneycombRefPtr<HoneycombValue> value) override;

 private:
  // Only accessed on the UI thread.
  OsrDelegate* osr_delegate_;

  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(ClientHandlerOsr);
  DISALLOW_COPY_AND_ASSIGN(ClientHandlerOsr);
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_CLIENT_HANDLER_OSR_H_
