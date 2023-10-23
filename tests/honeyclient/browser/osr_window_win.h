// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_OSR_WINDOW_WIN_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_OSR_WINDOW_WIN_H_
#pragma once

#include "include/base/honey_callback.h"
#include "include/base/honey_ref_counted.h"
#include "include/wrapper/honey_closure_task.h"
#include "include/wrapper/honey_helpers.h"
#include "tests/honeyclient/browser/client_handler_osr.h"
#include "tests/honeyclient/browser/osr_accessibility_node.h"
#include "tests/honeyclient/browser/osr_dragdrop_win.h"
#include "tests/honeyclient/browser/osr_render_handler_win.h"
#include "tests/honeyclient/browser/osr_renderer_settings.h"

namespace client {

class OsrAccessibilityHelper;
class OsrImeHandlerWin;

// Represents the native parent window for an off-screen browser. This object
// must live on the Honeycomb UI thread in order to handle HoneycombRenderHandler callbacks.
// The methods of this class are thread-safe unless otherwise indicated.
class OsrWindowWin
    : public base::RefCountedThreadSafe<OsrWindowWin, HoneycombDeleteOnUIThread>,
      public ClientHandlerOsr::OsrDelegate
#if defined(HONEYCOMB_USE_ATL)
    ,
      public OsrDragEvents
#endif
{
 public:
  // This interface is implemented by the owner of the OsrWindowWin. The
  // methods of this class will be called on the main thread.
  class Delegate {
   public:
    // Called after the native window has been created.
    virtual void OnOsrNativeWindowCreated(HWND hwnd) = 0;

   protected:
    virtual ~Delegate() {}
  };

  // |delegate| must outlive this object.
  OsrWindowWin(Delegate* delegate, const OsrRendererSettings& settings);

  // Create a new browser and native window.
  void CreateBrowser(HWND parent_hwnd,
                     const RECT& rect,
                     HoneycombRefPtr<HoneycombClient> handler,
                     const HoneycombBrowserSettings& settings,
                     HoneycombRefPtr<HoneycombDictionaryValue> extra_info,
                     HoneycombRefPtr<HoneycombRequestContext> request_context,
                     const std::string& startup_url);

  // Show the popup window with correct parent and bounds in parent coordinates.
  void ShowPopup(HWND parent_hwnd, int x, int y, size_t width, size_t height);

  void Show();
  void Hide();
  void SetBounds(int x, int y, size_t width, size_t height);
  void SetFocus();
  void SetDeviceScaleFactor(float device_scale_factor);

  const OsrRendererSettings& settings() const { return settings_; }

 private:
  // Only allow deletion via scoped_refptr.
  friend struct HoneycombDeleteOnThread<TID_UI>;
  friend class base::RefCountedThreadSafe<OsrWindowWin, HoneycombDeleteOnUIThread>;

  ~OsrWindowWin();

  // Manage native window lifespan.
  void Create(HWND parent_hwnd, const RECT& rect);
  void Destroy();

  void NotifyNativeWindowCreated(HWND hwnd);

  static void RegisterOsrClass(HINSTANCE hInstance, HBRUSH background_brush);
  static LRESULT CALLBACK OsrWndProc(HWND hWnd,
                                     UINT message,
                                     WPARAM wParam,
                                     LPARAM lParam);

  // WndProc message handlers.
  void OnMouseEvent(UINT message, WPARAM wParam, LPARAM lParam);
  void OnSize();
  void OnFocus(bool setFocus);
  void OnCaptureLost();
  void OnKeyEvent(UINT message, WPARAM wParam, LPARAM lParam);
  void OnPaint();
  bool OnEraseBkgnd();
  bool OnTouchEvent(UINT message, WPARAM wParam, LPARAM lParam);

  void OnIMESetContext(UINT message, WPARAM wParam, LPARAM lParam);
  void OnIMEStartComposition();
  void OnIMEComposition(UINT message, WPARAM wParam, LPARAM lParam);
  void OnIMECancelCompositionEvent();

  // Manage popup bounds.
  bool IsOverPopupWidget(int x, int y) const;
  int GetPopupXOffset() const;
  int GetPopupYOffset() const;
  void ApplyPopupOffset(int& x, int& y) const;

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
  void OnAcceleratedPaint(HoneycombRefPtr<HoneycombBrowser> browser,
                          HoneycombRenderHandler::PaintElementType type,
                          const HoneycombRenderHandler::RectList& dirtyRects,
                          void* share_handle) override;
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

#if defined(HONEYCOMB_USE_ATL)
  // OsrDragEvents methods.
  HoneycombBrowserHost::DragOperationsMask OnDragEnter(
      HoneycombRefPtr<HoneycombDragData> drag_data,
      HoneycombMouseEvent ev,
      HoneycombBrowserHost::DragOperationsMask effect) override;
  HoneycombBrowserHost::DragOperationsMask OnDragOver(
      HoneycombMouseEvent ev,
      HoneycombBrowserHost::DragOperationsMask effect) override;
  void OnDragLeave() override;
  HoneycombBrowserHost::DragOperationsMask OnDrop(
      HoneycombMouseEvent ev,
      HoneycombBrowserHost::DragOperationsMask effect) override;
#endif  // defined(HONEYCOMB_USE_ATL)

  void EnsureRenderHandler();

  // Only accessed on the main thread.
  Delegate* delegate_;

  const OsrRendererSettings settings_;
  HWND hwnd_;
  std::unique_ptr<OsrRenderHandlerWin> render_handler_;

  // Class that encapsulates IMM32 APIs and controls IMEs attached to a window.
  std::unique_ptr<OsrImeHandlerWin> ime_handler_;

  RECT client_rect_;
  float device_scale_factor_;

  HoneycombRefPtr<HoneycombBrowser> browser_;

#if defined(HONEYCOMB_USE_ATL)
  CComPtr<DropTargetWin> drop_target_;
  HoneycombRenderHandler::DragOperation current_drag_op_;

  // Class that abstracts the accessibility information received from the
  // renderer.
  std::unique_ptr<OsrAccessibilityHelper> accessibility_handler_;
  IAccessible* accessibility_root_;
#endif

  bool hidden_;

  // Mouse state tracking.
  POINT last_mouse_pos_;
  POINT current_mouse_pos_;
  bool mouse_rotation_;
  bool mouse_tracking_;
  int last_click_x_;
  int last_click_y_;
  HoneycombBrowserHost::MouseButtonType last_click_button_;
  int last_click_count_;
  double last_click_time_;
  bool last_mouse_down_on_view_;

  DISALLOW_COPY_AND_ASSIGN(OsrWindowWin);
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_OSR_WINDOW_WIN_H_
