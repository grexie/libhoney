// Copyright 2018 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_OSR_RENDER_HANDLER_WIN_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_OSR_RENDER_HANDLER_WIN_H_
#pragma once

#include "include/base/honey_weak_ptr.h"
#include "include/honey_render_handler.h"
#include "tests/honeyclient/browser/osr_renderer_settings.h"

namespace client {

// Abstract base class for implementing OSR rendering with different backends on
// Windows. Methods are only called on the UI thread.
class OsrRenderHandlerWin {
 public:
  OsrRenderHandlerWin(const OsrRendererSettings& settings, HWND hwnd);
  virtual ~OsrRenderHandlerWin();

  void SetBrowser(HoneycombRefPtr<HoneycombBrowser> browser);

  // Rotate the texture based on mouse events.
  virtual void SetSpin(float spinX, float spinY) = 0;
  virtual void IncrementSpin(float spinDX, float spinDY) = 0;

  // Popup hit testing.
  virtual bool IsOverPopupWidget(int x, int y) const = 0;
  virtual int GetPopupXOffset() const = 0;
  virtual int GetPopupYOffset() const = 0;

  // HoneycombRenderHandler callbacks.
  virtual void OnPopupShow(HoneycombRefPtr<HoneycombBrowser> browser, bool show) = 0;
  // |rect| must be in pixel coordinates.
  virtual void OnPopupSize(HoneycombRefPtr<HoneycombBrowser> browser,
                           const HoneycombRect& rect) = 0;

  // Used when not rendering with shared textures.
  virtual void OnPaint(HoneycombRefPtr<HoneycombBrowser> browser,
                       HoneycombRenderHandler::PaintElementType type,
                       const HoneycombRenderHandler::RectList& dirtyRects,
                       const void* buffer,
                       int width,
                       int height) = 0;

  // Used when rendering with shared textures.
  virtual void OnAcceleratedPaint(HoneycombRefPtr<HoneycombBrowser> browser,
                                  HoneycombRenderHandler::PaintElementType type,
                                  const HoneycombRenderHandler::RectList& dirtyRects,
                                  void* share_handle) = 0;

  bool send_begin_frame() const {
    return settings_.external_begin_frame_enabled;
  }
  HWND hwnd() const { return hwnd_; }

 protected:
  // Called to trigger the BeginFrame timer.
  void Invalidate();

  // Called by the BeginFrame timer.
  virtual void Render() = 0;

 private:
  void TriggerBeginFrame(uint64_t last_time_us, float delay_us);

  // The below members are only accessed on the UI thread.
  const OsrRendererSettings settings_;
  const HWND hwnd_;
  bool begin_frame_pending_;
  HoneycombRefPtr<HoneycombBrowser> browser_;

  // Must be the last member.
  base::WeakPtrFactory<OsrRenderHandlerWin> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(OsrRenderHandlerWin);
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_OSR_RENDER_HANDLER_WIN_H_
