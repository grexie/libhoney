// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoneycomb/browser/views/display_impl.h"

#include "libhoneycomb/browser/views/view_util.h"

#include "ui/display/screen.h"

// static
HoneycombRefPtr<HoneycombDisplay> HoneycombDisplay::GetPrimaryDisplay() {
  HONEYCOMB_REQUIRE_UIT_RETURN(nullptr);
  return new HoneycombDisplayImpl(display::Screen::GetScreen()->GetPrimaryDisplay());
}

// static
HoneycombRefPtr<HoneycombDisplay> HoneycombDisplay::GetDisplayNearestPoint(
    const HoneycombPoint& point,
    bool input_pixel_coords) {
  HONEYCOMB_REQUIRE_UIT_RETURN(nullptr);
  return new HoneycombDisplayImpl(view_util::GetDisplayNearestPoint(
      gfx::Point(point.x, point.y), input_pixel_coords));
}

// static
HoneycombRefPtr<HoneycombDisplay> HoneycombDisplay::GetDisplayMatchingBounds(
    const HoneycombRect& bounds,
    bool input_pixel_coords) {
  HONEYCOMB_REQUIRE_UIT_RETURN(nullptr);
  return new HoneycombDisplayImpl(view_util::GetDisplayMatchingBounds(
      gfx::Rect(bounds.x, bounds.y, bounds.width, bounds.height),
      input_pixel_coords));
}

// static
size_t HoneycombDisplay::GetDisplayCount() {
  HONEYCOMB_REQUIRE_UIT_RETURN(0U);
  return static_cast<size_t>(display::Screen::GetScreen()->GetNumDisplays());
}

// static
void HoneycombDisplay::GetAllDisplays(std::vector<HoneycombRefPtr<HoneycombDisplay>>& displays) {
  HONEYCOMB_REQUIRE_UIT_RETURN_VOID();

  displays.clear();

  using DisplayVector = std::vector<display::Display>;
  DisplayVector vec = display::Screen::GetScreen()->GetAllDisplays();
  for (size_t i = 0; i < vec.size(); ++i) {
    displays.push_back(new HoneycombDisplayImpl(vec[i]));
  }
}

// static
HoneycombPoint HoneycombDisplay::ConvertScreenPointToPixels(const HoneycombPoint& point) {
  HONEYCOMB_REQUIRE_UIT_RETURN(HoneycombPoint());
#if BUILDFLAG(IS_WIN)
  const gfx::Point pix_point =
      view_util::ConvertPointToPixels(gfx::Point(point.x, point.y));
  return HoneycombPoint(pix_point.x(), pix_point.y());
#else
  return point;
#endif
}

// static
HoneycombPoint HoneycombDisplay::ConvertScreenPointFromPixels(const HoneycombPoint& point) {
  HONEYCOMB_REQUIRE_UIT_RETURN(HoneycombPoint());
#if BUILDFLAG(IS_WIN)
  const gfx::Point dip_point =
      view_util::ConvertPointFromPixels(gfx::Point(point.x, point.y));
  return HoneycombPoint(dip_point.x(), dip_point.y());
#else
  return point;
#endif
}

// static
HoneycombRect HoneycombDisplay::ConvertScreenRectToPixels(const HoneycombRect& rect) {
  HONEYCOMB_REQUIRE_UIT_RETURN(HoneycombRect());
#if BUILDFLAG(IS_WIN)
  const gfx::Rect pix_rect = view_util::ConvertRectToPixels(
      gfx::Rect(rect.x, rect.y, rect.width, rect.height));
  return HoneycombRect(pix_rect.x(), pix_rect.y(), pix_rect.width(),
                 pix_rect.height());
#else
  return rect;
#endif
}

// static
HoneycombRect HoneycombDisplay::ConvertScreenRectFromPixels(const HoneycombRect& rect) {
  HONEYCOMB_REQUIRE_UIT_RETURN(HoneycombRect());
#if BUILDFLAG(IS_WIN)
  const gfx::Rect dip_rect = view_util::ConvertRectFromPixels(
      gfx::Rect(rect.x, rect.y, rect.width, rect.height));
  return HoneycombRect(dip_rect.x(), dip_rect.y(), dip_rect.width(),
                 dip_rect.height());
#else
  return rect;
#endif
}

HoneycombDisplayImpl::HoneycombDisplayImpl(const display::Display& display)
    : display_(display) {
  HONEYCOMB_REQUIRE_UIT();
}

HoneycombDisplayImpl::~HoneycombDisplayImpl() {
  HONEYCOMB_REQUIRE_UIT();
}

int64_t HoneycombDisplayImpl::GetID() {
  HONEYCOMB_REQUIRE_UIT_RETURN(-1);
  return display_.id();
}

float HoneycombDisplayImpl::GetDeviceScaleFactor() {
  HONEYCOMB_REQUIRE_UIT_RETURN(0.0f);
  return display_.device_scale_factor();
}

void HoneycombDisplayImpl::ConvertPointToPixels(HoneycombPoint& point) {
  HONEYCOMB_REQUIRE_UIT_RETURN_VOID();
  gfx::Point gfx_point(point.x, point.y);
  view_util::ConvertPointToPixels(&gfx_point, display_.device_scale_factor());
  point = HoneycombPoint(gfx_point.x(), gfx_point.y());
}

void HoneycombDisplayImpl::ConvertPointFromPixels(HoneycombPoint& point) {
  HONEYCOMB_REQUIRE_UIT_RETURN_VOID();
  gfx::Point gfx_point(point.x, point.y);
  view_util::ConvertPointFromPixels(&gfx_point, display_.device_scale_factor());
  point = HoneycombPoint(gfx_point.x(), gfx_point.y());
}

HoneycombRect HoneycombDisplayImpl::GetBounds() {
  HONEYCOMB_REQUIRE_UIT_RETURN(HoneycombRect());
  const gfx::Rect& gfx_rect = display_.bounds();
  return HoneycombRect(gfx_rect.x(), gfx_rect.y(), gfx_rect.width(),
                 gfx_rect.height());
}

HoneycombRect HoneycombDisplayImpl::GetWorkArea() {
  HONEYCOMB_REQUIRE_UIT_RETURN(HoneycombRect());
  const gfx::Rect& gfx_rect = display_.work_area();
  return HoneycombRect(gfx_rect.x(), gfx_rect.y(), gfx_rect.width(),
                 gfx_rect.height());
}

int HoneycombDisplayImpl::GetRotation() {
  HONEYCOMB_REQUIRE_UIT_RETURN(0);
  return display_.RotationAsDegree();
}
