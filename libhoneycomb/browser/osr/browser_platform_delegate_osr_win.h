// Copyright 2015 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_OSR_BROWSER_PLATFORM_DELEGATE_OSR_WIN_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_OSR_BROWSER_PLATFORM_DELEGATE_OSR_WIN_H_

#include "libhoneycomb/browser/osr/browser_platform_delegate_osr.h"

// Windowless browser implementation for Windows.
class HoneycombBrowserPlatformDelegateOsrWin : public HoneycombBrowserPlatformDelegateOsr {
 public:
  explicit HoneycombBrowserPlatformDelegateOsrWin(
      std::unique_ptr<HoneycombBrowserPlatformDelegateNative> native_delegate,
      bool use_shared_texture,
      bool use_external_begin_frame);

  // HoneycombBrowserPlatformDelegate methods:
  HoneycombWindowHandle GetHostWindowHandle() const override;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_NATIVE_BROWSER_PLATFORM_DELEGATE_OSR_WIN_H_
