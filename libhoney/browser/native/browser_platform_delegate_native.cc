// Copyright 2015 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoney/browser/native/browser_platform_delegate_native.h"

#include "libhoney/browser/alloy/alloy_browser_host_impl.h"

#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host.h"
#include "third_party/blink/public/common/input/web_mouse_event.h"

HoneycombBrowserPlatformDelegateNative::HoneycombBrowserPlatformDelegateNative(
    const HoneycombWindowInfo& window_info,
    SkColor background_color)
    : window_info_(window_info),
      background_color_(background_color),
      windowless_handler_(nullptr) {}

SkColor HoneycombBrowserPlatformDelegateNative::GetBackgroundColor() const {
  return background_color_;
}

void HoneycombBrowserPlatformDelegateNative::WasResized() {
  content::RenderViewHost* host = web_contents_->GetRenderViewHost();
  if (host) {
    host->GetWidget()->SynchronizeVisualProperties();
  }
}
