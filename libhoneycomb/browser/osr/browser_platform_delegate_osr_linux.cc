// Copyright 2015 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoneycomb/browser/osr/browser_platform_delegate_osr_linux.h"

#include <utility>

HoneycombBrowserPlatformDelegateOsrLinux::HoneycombBrowserPlatformDelegateOsrLinux(
    std::unique_ptr<HoneycombBrowserPlatformDelegateNative> native_delegate,
    bool use_external_begin_frame)
    : HoneycombBrowserPlatformDelegateOsr(std::move(native_delegate),
                                    /*use_shared_texture=*/false,
                                    use_external_begin_frame) {}

HoneycombWindowHandle HoneycombBrowserPlatformDelegateOsrLinux::GetHostWindowHandle()
    const {
  return native_delegate_->window_info().parent_window;
}
