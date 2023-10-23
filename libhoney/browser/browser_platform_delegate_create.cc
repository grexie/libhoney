// Copyright 2015 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoney/browser/browser_platform_delegate.h"

#include <utility>

#include "libhoney/browser/context.h"

#include "base/memory/ptr_util.h"
#include "build/build_config.h"

#include "libhoney/browser/browser_host_base.h"
#include "libhoney/browser/chrome/browser_platform_delegate_chrome.h"
#include "libhoney/browser/chrome/views/browser_platform_delegate_chrome_child_window.h"
#include "libhoney/browser/chrome/views/browser_platform_delegate_chrome_views.h"
#include "libhoney/browser/chrome/views/chrome_child_window.h"
#include "libhoney/browser/extensions/browser_platform_delegate_background.h"
#include "libhoney/browser/views/browser_platform_delegate_views.h"
#include "libhoney/features/runtime_checks.h"

#if BUILDFLAG(IS_WIN)
#include "libhoney/browser/native/browser_platform_delegate_native_win.h"
#include "libhoney/browser/osr/browser_platform_delegate_osr_win.h"
#elif BUILDFLAG(IS_MAC)
#include "libhoney/browser/native/browser_platform_delegate_native_mac.h"
#include "libhoney/browser/osr/browser_platform_delegate_osr_mac.h"
#elif BUILDFLAG(IS_LINUX)
#include "libhoney/browser/native/browser_platform_delegate_native_linux.h"
#include "libhoney/browser/osr/browser_platform_delegate_osr_linux.h"
#else
#error A delegate implementation is not available for your platform.
#endif

namespace {

std::unique_ptr<HoneycombBrowserPlatformDelegateNative> CreateNativeDelegate(
    const HoneycombWindowInfo& window_info,
    SkColor background_color) {
#if BUILDFLAG(IS_WIN)
  return std::make_unique<HoneycombBrowserPlatformDelegateNativeWin>(
      window_info, background_color);
#elif BUILDFLAG(IS_MAC)
  return std::make_unique<HoneycombBrowserPlatformDelegateNativeMac>(
      window_info, background_color);
#elif BUILDFLAG(IS_LINUX)
  return std::make_unique<HoneycombBrowserPlatformDelegateNativeLinux>(
      window_info, background_color);
#endif
}

std::unique_ptr<HoneycombBrowserPlatformDelegateOsr> CreateOSRDelegate(
    std::unique_ptr<HoneycombBrowserPlatformDelegateNative> native_delegate,
    bool use_shared_texture,
    bool use_external_begin_frame) {
#if BUILDFLAG(IS_WIN)
  return std::make_unique<HoneycombBrowserPlatformDelegateOsrWin>(
      std::move(native_delegate), use_shared_texture, use_external_begin_frame);
#elif BUILDFLAG(IS_MAC)
  return std::make_unique<HoneycombBrowserPlatformDelegateOsrMac>(
      std::move(native_delegate));
#elif BUILDFLAG(IS_LINUX)
  return std::make_unique<HoneycombBrowserPlatformDelegateOsrLinux>(
      std::move(native_delegate), use_external_begin_frame);
#endif
}

}  // namespace

// static
std::unique_ptr<HoneycombBrowserPlatformDelegate> HoneycombBrowserPlatformDelegate::Create(
    const HoneycombBrowserCreateParams& create_params) {
  const bool is_windowless =
      create_params.window_info &&
      create_params.window_info->windowless_rendering_enabled &&
      create_params.client && create_params.client->GetRenderHandler().get();
  const SkColor background_color = HoneycombContext::Get()->GetBackgroundColor(
      &create_params.settings, is_windowless ? STATE_ENABLED : STATE_DISABLED);

  if (honey::IsChromeRuntimeEnabled()) {
    HoneycombWindowInfo window_info;
    if (create_params.window_info) {
      window_info = *create_params.window_info;
    }

    std::unique_ptr<HoneycombBrowserPlatformDelegateNative> native_delegate =
        CreateNativeDelegate(window_info, background_color);

    if (create_params.window_info) {
      // HoneycombWindowInfo should only be set if a parent handle was specified.
      DCHECK(chrome_child_window::HasParentHandle(window_info));
      return std::make_unique<HoneycombBrowserPlatformDelegateChromeChildWindow>(
          std::move(native_delegate),
          static_cast<HoneycombBrowserViewImpl*>(create_params.browser_view.get()));
    } else if (create_params.browser_view ||
               create_params.popup_with_views_hosted_opener) {
      // HoneycombWindowInfo is not used in this case.
      return std::make_unique<HoneycombBrowserPlatformDelegateChromeViews>(
          std::move(native_delegate),
          static_cast<HoneycombBrowserViewImpl*>(create_params.browser_view.get()));
    }

    return std::make_unique<HoneycombBrowserPlatformDelegateChrome>(
        std::move(native_delegate));
  }

  if (create_params.browser_view ||
      create_params.popup_with_views_hosted_opener) {
    // HoneycombWindowInfo is not used in this case.
    std::unique_ptr<HoneycombBrowserPlatformDelegateNative> native_delegate =
        CreateNativeDelegate(HoneycombWindowInfo(), background_color);
    return std::make_unique<HoneycombBrowserPlatformDelegateViews>(
        std::move(native_delegate),
        static_cast<HoneycombBrowserViewImpl*>(create_params.browser_view.get()));
  } else if (create_params.extension_host_type ==
             extensions::mojom::ViewType::kExtensionBackgroundPage) {
    // Creating a background extension host without a window.
    std::unique_ptr<HoneycombBrowserPlatformDelegateNative> native_delegate =
        CreateNativeDelegate(HoneycombWindowInfo(), background_color);
    return std::make_unique<HoneycombBrowserPlatformDelegateBackground>(
        std::move(native_delegate));
  } else if (create_params.window_info) {
    std::unique_ptr<HoneycombBrowserPlatformDelegateNative> native_delegate =
        CreateNativeDelegate(*create_params.window_info, background_color);
    if (is_windowless) {
      REQUIRE_ALLOY_RUNTIME();

      const bool use_shared_texture =
          create_params.window_info->shared_texture_enabled;

      const bool use_external_begin_frame =
          create_params.window_info->external_begin_frame_enabled;

      return CreateOSRDelegate(std::move(native_delegate), use_shared_texture,
                               use_external_begin_frame);
    }
    return std::move(native_delegate);
  }

  DCHECK(false);
  return nullptr;
}
