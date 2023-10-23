// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_DISPLAY_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_DISPLAY_IMPL_H_
#pragma once

#include "include/views/honey_display.h"
#include "libhoneycomb/browser/thread_util.h"

#include "ui/display/display.h"

class HoneycombDisplayImpl : public HoneycombDisplay {
 public:
  explicit HoneycombDisplayImpl(const display::Display& display);

  HoneycombDisplayImpl(const HoneycombDisplayImpl&) = delete;
  HoneycombDisplayImpl& operator=(const HoneycombDisplayImpl&) = delete;

  ~HoneycombDisplayImpl() override;

  // HoneycombDisplay methods:
  int64_t GetID() override;
  float GetDeviceScaleFactor() override;
  void ConvertPointToPixels(HoneycombPoint& point) override;
  void ConvertPointFromPixels(HoneycombPoint& point) override;
  HoneycombRect GetBounds() override;
  HoneycombRect GetWorkArea() override;
  int GetRotation() override;

  const display::Display& display() const { return display_; }

 private:
  display::Display display_;

  IMPLEMENT_REFCOUNTING_DELETE_ON_UIT(HoneycombDisplayImpl);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_DISPLAY_IMPL_H_
