// Copyright 2022 The Honeycomb Authors.
// Portions copyright 2015 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoneycomb/browser/osr/touch_handle_drawable_osr.h"

#include <cmath>

#include "libhoneycomb/browser/osr/render_widget_host_view_osr.h"

#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/geometry/size.h"

namespace {
// Copied from touch_handle_drawable_aura.cc

// The distance by which a handle image is offset from the focal point (i.e.
// text baseline) downwards.
constexpr int kSelectionHandleVerticalVisualOffset = 2;

// The padding around the selection handle image can be used to extend the
// handle so that touch events near the selection handle image are
// targeted to the selection handle.
constexpr int kSelectionHandlePadding = 0;

}  // namespace

int HoneycombTouchHandleDrawableOSR::counter_ = 0;

HoneycombTouchHandleDrawableOSR::HoneycombTouchHandleDrawableOSR(
    HoneycombRenderWidgetHostViewOSR* rwhv)
    : rwhv_(rwhv), id_(counter_++) {}

void HoneycombTouchHandleDrawableOSR::SetEnabled(bool enabled) {
  if (enabled == enabled_) {
    return;
  }

  enabled_ = enabled;

  HoneycombTouchHandleState touch_handle_state;
  touch_handle_state.touch_handle_id = id_;
  touch_handle_state.flags = HONEYCOMB_THS_FLAG_ENABLED;
  touch_handle_state.enabled = enabled_;
  TouchHandleStateChanged(touch_handle_state);
}

void HoneycombTouchHandleDrawableOSR::SetOrientation(
    ui::TouchHandleOrientation orientation,
    bool mirror_vertical,
    bool mirror_horizontal) {
  if (orientation == orientation_) {
    return;
  }

  orientation_ = orientation;

  HoneycombSize size;
  auto browser = rwhv_->browser_impl();
  auto handler = browser->GetClient()->GetRenderHandler();
  handler->GetTouchHandleSize(
      browser.get(), static_cast<honey_horizontal_alignment_t>(orientation_),
      size);

  const gfx::Size& image_size = gfx::Size(size.width, size.height);
  int handle_width = image_size.width() + 2 * kSelectionHandlePadding;
  int handle_height = image_size.height() + 2 * kSelectionHandlePadding;
  relative_bounds_ =
      gfx::RectF(-kSelectionHandlePadding,
                 kSelectionHandleVerticalVisualOffset - kSelectionHandlePadding,
                 handle_width, handle_height);

  HoneycombTouchHandleState touch_handle_state;
  touch_handle_state.touch_handle_id = id_;
  touch_handle_state.flags = HONEYCOMB_THS_FLAG_ORIENTATION;
  touch_handle_state.orientation =
      static_cast<honey_horizontal_alignment_t>(orientation_);
  touch_handle_state.mirror_vertical = mirror_vertical;
  touch_handle_state.mirror_horizontal = mirror_horizontal;
  TouchHandleStateChanged(touch_handle_state);
}

void HoneycombTouchHandleDrawableOSR::SetOrigin(const gfx::PointF& position) {
  if (position == origin_position_) {
    return;
  }

  origin_position_ = position;

  HoneycombTouchHandleState touch_handle_state;
  touch_handle_state.touch_handle_id = id_;
  touch_handle_state.flags = HONEYCOMB_THS_FLAG_ORIGIN;
  touch_handle_state.origin = {static_cast<int>(std::round(position.x())),
                               static_cast<int>(std::round(position.y()))};
  TouchHandleStateChanged(touch_handle_state);
}

void HoneycombTouchHandleDrawableOSR::SetAlpha(float alpha) {
  if (alpha == alpha_) {
    return;
  }

  alpha_ = alpha;

  HoneycombTouchHandleState touch_handle_state;
  touch_handle_state.touch_handle_id = id_;
  touch_handle_state.flags = HONEYCOMB_THS_FLAG_ALPHA;
  touch_handle_state.alpha = alpha_;
  TouchHandleStateChanged(touch_handle_state);
}

gfx::RectF HoneycombTouchHandleDrawableOSR::GetVisibleBounds() const {
  gfx::RectF bounds = relative_bounds_;
  bounds.Offset(origin_position_.x(), origin_position_.y());
  bounds.Inset(gfx::InsetsF::TLBR(
      kSelectionHandlePadding,
      kSelectionHandlePadding + kSelectionHandleVerticalVisualOffset,
      kSelectionHandlePadding, kSelectionHandlePadding));
  return bounds;
}

float HoneycombTouchHandleDrawableOSR::GetDrawableHorizontalPaddingRatio() const {
  return 0.0f;
}

void HoneycombTouchHandleDrawableOSR::TouchHandleStateChanged(
    const HoneycombTouchHandleState& state) {
  auto browser = rwhv_->browser_impl();
  auto handler = browser->GetClient()->GetRenderHandler();
  handler->OnTouchHandleStateChanged(browser.get(), state);
}
