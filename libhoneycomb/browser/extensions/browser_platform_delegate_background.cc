// Copyright 2017 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoneycomb/browser/extensions/browser_platform_delegate_background.h"

#include <utility>

#include "libhoneycomb/browser/alloy/alloy_browser_host_impl.h"
#include "libhoneycomb/browser/thread_util.h"
#include "libhoneycomb/features/runtime_checks.h"

#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host.h"

HoneycombBrowserPlatformDelegateBackground::HoneycombBrowserPlatformDelegateBackground(
    std::unique_ptr<HoneycombBrowserPlatformDelegateNative> native_delegate)
    : native_delegate_(std::move(native_delegate)) {
  REQUIRE_ALLOY_RUNTIME();
  native_delegate_->set_windowless_handler(this);
}

bool HoneycombBrowserPlatformDelegateBackground::CreateHostWindow() {
  // Nothing to do here.
  return true;
}

void HoneycombBrowserPlatformDelegateBackground::CloseHostWindow() {
  // No host window, so continue browser destruction now. Do it asynchronously
  // so the call stack has a chance to unwind.
  HONEYCOMB_POST_TASK(HONEYCOMB_UIT,
                base::BindOnce(&AlloyBrowserHostImpl::WindowDestroyed,
                               static_cast<AlloyBrowserHostImpl*>(browser_)));
}

HoneycombWindowHandle HoneycombBrowserPlatformDelegateBackground::GetHostWindowHandle()
    const {
  return kNullWindowHandle;
}

SkColor HoneycombBrowserPlatformDelegateBackground::GetBackgroundColor() const {
  return native_delegate_->GetBackgroundColor();
}

void HoneycombBrowserPlatformDelegateBackground::WasResized() {
  // Nothing to do here.
}

void HoneycombBrowserPlatformDelegateBackground::SendKeyEvent(
    const HoneycombKeyEvent& event) {
  // Nothing to do here.
}

void HoneycombBrowserPlatformDelegateBackground::SendMouseClickEvent(
    const HoneycombMouseEvent& event,
    HoneycombBrowserHost::MouseButtonType type,
    bool mouseUp,
    int clickCount) {
  // Nothing to do here.
}

void HoneycombBrowserPlatformDelegateBackground::SendMouseMoveEvent(
    const HoneycombMouseEvent& event,
    bool mouseLeave) {
  // Nothing to do here.
}

void HoneycombBrowserPlatformDelegateBackground::SendMouseWheelEvent(
    const HoneycombMouseEvent& event,
    int deltaX,
    int deltaY) {
  // Nothing to do here.
}

void HoneycombBrowserPlatformDelegateBackground::SendTouchEvent(
    const HoneycombTouchEvent& event) {
  // Nothing to do here.
}

void HoneycombBrowserPlatformDelegateBackground::SetFocus(bool setFocus) {
  // Nothing to do here.
}

gfx::Point HoneycombBrowserPlatformDelegateBackground::GetScreenPoint(
    const gfx::Point& view_pt,
    bool want_dip_coords) const {
  // Nothing to do here.
  return view_pt;
}

void HoneycombBrowserPlatformDelegateBackground::ViewText(const std::string& text) {
  native_delegate_->ViewText(text);
}

bool HoneycombBrowserPlatformDelegateBackground::HandleKeyboardEvent(
    const content::NativeWebKeyboardEvent& event) {
  // Nothing to do here.
  return false;
}

HoneycombEventHandle HoneycombBrowserPlatformDelegateBackground::GetEventHandle(
    const content::NativeWebKeyboardEvent& event) const {
  return native_delegate_->GetEventHandle(event);
}

std::unique_ptr<HoneycombMenuRunner>
HoneycombBrowserPlatformDelegateBackground::CreateMenuRunner() {
  // No default menu implementation for background browsers.
  return nullptr;
}

HoneycombWindowHandle HoneycombBrowserPlatformDelegateBackground::GetParentWindowHandle()
    const {
  return GetHostWindowHandle();
}

gfx::Point HoneycombBrowserPlatformDelegateBackground::GetParentScreenPoint(
    const gfx::Point& view,
    bool want_dip_coords) const {
  return GetScreenPoint(view, want_dip_coords);
}
