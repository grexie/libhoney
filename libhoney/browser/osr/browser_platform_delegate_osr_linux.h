// Copyright 2015 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_OSR_BROWSER_PLATFORM_DELEGATE_OSR_LINUX_H_
#define HONEYCOMB_LIBHONEY_BROWSER_OSR_BROWSER_PLATFORM_DELEGATE_OSR_LINUX_H_

#include "libhoney/browser/osr/browser_platform_delegate_osr.h"

// Windowless browser implementation for Linux.
class HoneycombBrowserPlatformDelegateOsrLinux
    : public HoneycombBrowserPlatformDelegateOsr {
 public:
  HoneycombBrowserPlatformDelegateOsrLinux(
      std::unique_ptr<HoneycombBrowserPlatformDelegateNative> native_delegate,
      bool use_external_begin_frame);

  // HoneycombBrowserPlatformDelegate methods:
  HoneycombWindowHandle GetHostWindowHandle() const override;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_NATIVE_BROWSER_PLATFORM_DELEGATE_OSR_LINUX_H_
