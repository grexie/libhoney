// Copyright (c) 2019 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "libhoneycomb/browser/osr/synthetic_gesture_target_osr.h"

#include "content/browser/renderer_host/render_widget_host_impl.h"
#include "ui/events/gesture_detection/gesture_configuration.h"

HoneycombSyntheticGestureTargetOSR::HoneycombSyntheticGestureTargetOSR(
    content::RenderWidgetHostImpl* host)
    : SyntheticGestureTargetBase(host) {}

void HoneycombSyntheticGestureTargetOSR::DispatchWebTouchEventToPlatform(
    const blink::WebTouchEvent& web_touch,
    const ui::LatencyInfo& latency_info) {
  // We assume that platforms supporting touch have their own implementation of
  // SyntheticGestureTarget to route the events through their respective input
  // stack.
  LOG(ERROR) << "Touch events not supported for this browser.";
}

void HoneycombSyntheticGestureTargetOSR::DispatchWebMouseWheelEventToPlatform(
    const blink::WebMouseWheelEvent& web_wheel,
    const ui::LatencyInfo& latency_info) {
  render_widget_host()->ForwardWheelEventWithLatencyInfo(web_wheel,
                                                         latency_info);
}

void HoneycombSyntheticGestureTargetOSR::DispatchWebGestureEventToPlatform(
    const blink::WebGestureEvent& web_gesture,
    const ui::LatencyInfo& latency_info) {
  render_widget_host()->ForwardGestureEventWithLatencyInfo(web_gesture,
                                                           latency_info);
}

void HoneycombSyntheticGestureTargetOSR::DispatchWebMouseEventToPlatform(
    const blink::WebMouseEvent& web_mouse,
    const ui::LatencyInfo& latency_info) {
  render_widget_host()->ForwardMouseEventWithLatencyInfo(web_mouse,
                                                         latency_info);
}

content::mojom::GestureSourceType
HoneycombSyntheticGestureTargetOSR::GetDefaultSyntheticGestureSourceType() const {
  return content::mojom::GestureSourceType::kMouseInput;
}

float HoneycombSyntheticGestureTargetOSR::GetTouchSlopInDips() const {
  return ui::GestureConfiguration::GetInstance()
      ->max_touch_move_in_pixels_for_click();
}

float HoneycombSyntheticGestureTargetOSR::GetSpanSlopInDips() const {
  return ui::GestureConfiguration::GetInstance()->span_slop();
}

float HoneycombSyntheticGestureTargetOSR::GetMinScalingSpanInDips() const {
  return ui::GestureConfiguration::GetInstance()->min_scaling_span_in_pixels();
}
