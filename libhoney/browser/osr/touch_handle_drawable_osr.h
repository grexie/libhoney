// Copyright 2022 The Honeycomb Authors.
// Portions copyright 2015 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_OSR_TOUCH_HANDLE_DRAWABLE_OSR_H_
#define HONEYCOMB_LIBHONEY_BROWSER_OSR_TOUCH_HANDLE_DRAWABLE_OSR_H_

#include "include/internal/honey_types_wrappers.h"

#include "base/memory/raw_ptr.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/touch_selection/touch_handle.h"
#include "ui/touch_selection/touch_handle_orientation.h"
#include "ui/touch_selection/ui_touch_selection_export.h"

class HoneycombRenderWidgetHostViewOSR;

// Copied from TouchHandleDrawableAura.
class HoneycombTouchHandleDrawableOSR : public ui::TouchHandleDrawable {
 public:
  explicit HoneycombTouchHandleDrawableOSR(HoneycombRenderWidgetHostViewOSR* rwhv);

  HoneycombTouchHandleDrawableOSR(const HoneycombTouchHandleDrawableOSR&) = delete;
  HoneycombTouchHandleDrawableOSR& operator=(const HoneycombTouchHandleDrawableOSR&) =
      delete;

 private:
  // TouchHandleDrawable:
  void SetEnabled(bool enabled) override;
  void SetOrientation(ui::TouchHandleOrientation orientation,
                      bool mirror_vertical,
                      bool mirror_horizontal) override;
  void SetOrigin(const gfx::PointF& position) override;
  void SetAlpha(float alpha) override;
  gfx::RectF GetVisibleBounds() const override;
  float GetDrawableHorizontalPaddingRatio() const override;

  // Pass the current touch handle state to the HoneycombRenderHandler.
  void TouchHandleStateChanged(const HoneycombTouchHandleState& state);

  raw_ptr<HoneycombRenderWidgetHostViewOSR> rwhv_;

  float alpha_ = 0.f;
  static int counter_;
  bool enabled_ = false;
  int id_;
  ui::TouchHandleOrientation orientation_ =
      ui::TouchHandleOrientation::UNDEFINED;

  // Origin position of the handle set via SetOrigin, in coordinate space of
  // selection controller client (i.e. handle's parent).
  gfx::PointF origin_position_;

  // Handle bounds relative to the focal position.
  gfx::RectF relative_bounds_ = gfx::RectF(0.0F, 0.0F, 24.0F, 24.0F);
};

#endif
