// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_BROWSER_WINDOW_OSR_MAC_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_BROWSER_WINDOW_OSR_MAC_H_
#pragma once

#include "tests/honeyclient/browser/browser_window.h"
#include "tests/honeyclient/browser/client_handler_osr.h"
#include "tests/honeyclient/browser/osr_renderer.h"
#include "tests/honeyclient/browser/text_input_client_osr_mac.h"

namespace client {

class BrowserWindowOsrMacImpl;

// Represents a native child window hosting a single off-screen browser
// instance. The methods of this class must be called on the main thread unless
// otherwise indicated.
class BrowserWindowOsrMac : public BrowserWindow,
                            public ClientHandlerOsr::OsrDelegate {
 public:
  // Constructor may be called on any thread.
  // |delegate| must outlive this object.
  BrowserWindowOsrMac(BrowserWindow::Delegate* delegate,
                      bool with_controls,
                      const std::string& startup_url,
                      const OsrRendererSettings& settings);
  ~BrowserWindowOsrMac();

  // BrowserWindow methods.
  void CreateBrowser(ClientWindowHandle parent_handle,
                     const HoneycombRect& rect,
                     const HoneycombBrowserSettings& settings,
                     HoneycombRefPtr<HoneycombDictionaryValue> extra_info,
                     HoneycombRefPtr<HoneycombRequestContext> request_context) override;
  void GetPopupConfig(HoneycombWindowHandle temp_handle,
                      HoneycombWindowInfo& windowInfo,
                      HoneycombRefPtr<HoneycombClient>& client,
                      HoneycombBrowserSettings& settings) override;
  void ShowPopup(ClientWindowHandle parent_handle,
                 int x,
                 int y,
                 size_t width,
                 size_t height) override;
  void Show() override;
  void Hide() override;
  void SetBounds(int x, int y, size_t width, size_t height) override;
  void SetFocus(bool focus) override;
  void SetDeviceScaleFactor(float device_scale_factor) override;
  float GetDeviceScaleFactor() const override;
  ClientWindowHandle GetWindowHandle() const override;

  // ClientHandlerOsr::OsrDelegate methods.
  void OnAfterCreated(HoneycombRefPtr<HoneycombBrowser> browser) override;
  void OnBeforeClose(HoneycombRefPtr<HoneycombBrowser> browser) override;
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
  void OnCursorChange(HoneycombRefPtr<HoneycombBrowser> browser,
                      HoneycombCursorHandle cursor,
                      honey_cursor_type_t type,
                      const HoneycombCursorInfo& custom_cursor_info) override;
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

  void UpdateAccessibilityTree(HoneycombRefPtr<HoneycombValue> value) override;
  void UpdateAccessibilityLocation(HoneycombRefPtr<HoneycombValue> value) override;

 private:
  std::unique_ptr<BrowserWindowOsrMacImpl> impl_;

  DISALLOW_COPY_AND_ASSIGN(BrowserWindowOsrMac);

  friend class BrowserWindowOsrMacImpl;
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_BROWSER_WINDOW_OSR_MAC_H_
