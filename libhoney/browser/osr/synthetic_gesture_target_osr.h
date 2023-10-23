// Copyright (c) 2019 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_OSR_SYNTHETIC_GESTURE_TARGET_OSR_H_
#define HONEYCOMB_LIBHONEY_BROWSER_OSR_SYNTHETIC_GESTURE_TARGET_OSR_H_

#include "content/browser/renderer_host/input/synthetic_gesture_target_base.h"

// SyntheticGestureTarget implementation for OSR.
class HoneycombSyntheticGestureTargetOSR
    : public content::SyntheticGestureTargetBase {
 public:
  explicit HoneycombSyntheticGestureTargetOSR(content::RenderWidgetHostImpl* host);

  HoneycombSyntheticGestureTargetOSR(const HoneycombSyntheticGestureTargetOSR&) = delete;
  HoneycombSyntheticGestureTargetOSR& operator=(const HoneycombSyntheticGestureTargetOSR&) =
      delete;

  // SyntheticGestureTargetBase:
  void DispatchWebTouchEventToPlatform(
      const blink::WebTouchEvent& web_touch,
      const ui::LatencyInfo& latency_info) override;
  void DispatchWebMouseWheelEventToPlatform(
      const blink::WebMouseWheelEvent& web_wheel,
      const ui::LatencyInfo& latency_info) override;
  void DispatchWebGestureEventToPlatform(
      const blink::WebGestureEvent& web_gesture,
      const ui::LatencyInfo& latency_info) override;
  void DispatchWebMouseEventToPlatform(
      const blink::WebMouseEvent& web_mouse,
      const ui::LatencyInfo& latency_info) override;

  // SyntheticGestureTarget:
  content::mojom::GestureSourceType GetDefaultSyntheticGestureSourceType()
      const override;
  float GetTouchSlopInDips() const override;
  float GetSpanSlopInDips() const override;
  float GetMinScalingSpanInDips() const override;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_OSR_SYNTHETIC_GESTURE_TARGET_OSR_H_
