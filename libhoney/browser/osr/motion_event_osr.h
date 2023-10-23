// Copyright (c) 2017 The Honeycomb Authors.
// Portions copyright (c) 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_OSR_MOTION_EVENT_OSR_H_
#define HONEYCOMB_LIBHONEY_BROWSER_OSR_MOTION_EVENT_OSR_H_
#pragma once

#include "include/honey_base.h"

#include "third_party/blink/public/common/input/web_touch_event.h"
#include "ui/events/gesture_detection/motion_event_generic.h"

// Implementation of MotionEvent which takes a stream of HoneycombTouchEvents.
// This class is based on ui::MotionEventAura.
class HoneycombMotionEventOSR : public ui::MotionEventGeneric {
 public:
  HoneycombMotionEventOSR();

  HoneycombMotionEventOSR(const HoneycombMotionEventOSR&) = delete;
  HoneycombMotionEventOSR& operator=(const HoneycombMotionEventOSR&) = delete;

  ~HoneycombMotionEventOSR() override;

  int GetSourceDeviceId(size_t pointer_index) const override;

  // Returns true if the touch was valid.
  bool OnTouch(const HoneycombTouchEvent& touch);

  // We can't cleanup removed touch points immediately upon receipt of a
  // TouchCancel or TouchRelease, as the MotionEvent needs to be able to report
  // information about those touch events. Once the MotionEvent has been
  // processed, we call CleanupRemovedTouchPoints to do the required
  // book-keeping.
  void CleanupRemovedTouchPoints(const HoneycombTouchEvent& event);

  // Reset unchanged touch point to StateStationary for touchmove and
  // touchcancel to make sure only send one ack per WebTouchEvent.
  void MarkUnchangedTouchPointsAsStationary(blink::WebTouchEvent* event,
                                            const HoneycombTouchEvent& honey_event);

 private:
  // Chromium can't cope with touch ids >31, so let's map the incoming
  // ids to a safe range.
  int id_map_[blink::WebTouchEvent::kTouchesLengthCap];

  int LookupId(int id);
  int AddId(int id);
  void RemoveId(int id);

  bool AddTouch(const HoneycombTouchEvent& touch, int id);
  void UpdateTouch(const HoneycombTouchEvent& touch, int id);
  void UpdateCachedAction(const HoneycombTouchEvent& touch, int id);
  bool IsValidIndex(int index) const;
  ui::PointerProperties GetPointerPropertiesFromTouchEvent(
      const HoneycombTouchEvent& touch,
      int id);
};

#endif
