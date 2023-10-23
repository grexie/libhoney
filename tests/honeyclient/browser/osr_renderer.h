// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_OSR_RENDERER_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_OSR_RENDERER_H_
#pragma once

#include "include/honey_browser.h"
#include "include/honey_render_handler.h"
#include "tests/honeyclient/browser/osr_renderer_settings.h"

namespace client {

class OsrRenderer {
 public:
  explicit OsrRenderer(const OsrRendererSettings& settings);
  ~OsrRenderer();

  // Initialize the OpenGL environment.
  void Initialize();

  // Clean up the OpenGL environment.
  void Cleanup();

  // Render to the screen.
  void Render();

  // Forwarded from HoneycombRenderHandler callbacks.
  void OnPopupShow(HoneycombRefPtr<HoneycombBrowser> browser, bool show);
  // |rect| must be in pixel coordinates.
  void OnPopupSize(HoneycombRefPtr<HoneycombBrowser> browser, const HoneycombRect& rect);
  void OnPaint(HoneycombRefPtr<HoneycombBrowser> browser,
               HoneycombRenderHandler::PaintElementType type,
               const HoneycombRenderHandler::RectList& dirtyRects,
               const void* buffer,
               int width,
               int height);

  // Apply spin.
  void SetSpin(float spinX, float spinY);
  void IncrementSpin(float spinDX, float spinDY);

  int GetViewWidth() const { return view_width_; }
  int GetViewHeight() const { return view_height_; }

  HoneycombRect popup_rect() const { return popup_rect_; }
  HoneycombRect original_popup_rect() const { return original_popup_rect_; }

  void ClearPopupRects();

 private:
  HoneycombRect GetPopupRectInWebView(const HoneycombRect& original_rect);

  inline bool IsTransparent() const {
    return HoneycombColorGetA(settings_.background_color) == 0;
  }

  const OsrRendererSettings settings_;
  bool initialized_;
  unsigned int texture_id_;
  int view_width_;
  int view_height_;
  HoneycombRect popup_rect_;
  HoneycombRect original_popup_rect_;
  float spin_x_;
  float spin_y_;
  HoneycombRect update_rect_;

  DISALLOW_COPY_AND_ASSIGN(OsrRenderer);
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_OSR_RENDERER_H_
