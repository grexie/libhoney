// Copyright 2023 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoneycomb/browser/views/native_widget_mac.h"

#include "include/views/honey_window.h"
#include "include/views/honey_window_delegate.h"
#include "libhoneycomb/browser/views/ns_window.h"
#include "libhoneycomb/browser/views/window_impl.h"

#include "chrome/browser/apps/app_shim/app_shim_host_mac.h"
#include "chrome/browser/apps/app_shim/app_shim_manager_mac.h"
#import "chrome/browser/ui/cocoa/browser_window_command_handler.h"
#import "chrome/browser/ui/cocoa/chrome_command_dispatcher_delegate.h"
#include "chrome/browser/ui/views/frame/browser_frame_mac.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#import "components/remote_cocoa/app_shim/native_widget_ns_window_bridge.h"
#import "ui/views/cocoa/native_widget_mac_ns_window_host.h"

namespace {

AppShimHost* GetHostForBrowser(Browser* browser) {
  auto* const shim_manager = apps::AppShimManager::Get();
  if (!shim_manager) {
    return nullptr;
  }
  return shim_manager->GetHostForRemoteCocoaBrowser(browser);
}

}  // namespace

HoneycombNativeWidgetMac::HoneycombNativeWidgetMac(
    views::internal::NativeWidgetDelegate* delegate,
    HoneycombRefPtr<HoneycombWindow> window,
    HoneycombWindowDelegate* window_delegate)
    : views::NativeWidgetMac(delegate),
      window_(window),
      window_delegate_(window_delegate) {}

void HoneycombNativeWidgetMac::SetBrowserView(BrowserView* browser_view) {
  browser_view_ = browser_view;
}

void HoneycombNativeWidgetMac::ValidateUserInterfaceItem(
    int32_t tag,
    remote_cocoa::mojom::ValidateUserInterfaceItemResult* result) {
  if (!browser_view_) {
    result->enable = false;
    return;
  }

  return BrowserFrameMac::ValidateUserInterfaceItem(browser_view_->browser(),
                                                    tag, result);
}

bool HoneycombNativeWidgetMac::WillExecuteCommand(
    int32_t command,
    WindowOpenDisposition window_open_disposition,
    bool is_before_first_responder) {
  if (!browser_view_) {
    return false;
  }

  return BrowserFrameMac::WillExecuteCommand(browser_view_->browser(), command,
                                             window_open_disposition,
                                             is_before_first_responder);
}

bool HoneycombNativeWidgetMac::ExecuteCommand(
    int32_t command,
    WindowOpenDisposition window_open_disposition,
    bool is_before_first_responder) {
  if (!browser_view_) {
    return false;
  }

  return BrowserFrameMac::ExecuteCommand(browser_view_->browser(), command,
                                         window_open_disposition,
                                         is_before_first_responder);
}

NativeWidgetMacNSWindow* HoneycombNativeWidgetMac::CreateNSWindow(
    const remote_cocoa::mojom::CreateWindowParams* params) {
  NSUInteger style_mask =
      NSWindowStyleMaskTitled | NSWindowStyleMaskMiniaturizable |
      NSWindowStyleMaskClosable | NSWindowStyleMaskResizable |
      NSWindowStyleMaskTexturedBackground;

  bool is_frameless = window_delegate_->IsFrameless(window_);

  auto window = [[HoneycombNSWindow alloc] initWithStyle:style_mask
                                       isFrameless:is_frameless];

  if (is_frameless) {
    [window setTitlebarAppearsTransparent:YES];
    [window setTitleVisibility:NSWindowTitleHidden];
  }

  if (!window_delegate_->WithStandardWindowButtons(window_)) {
    [[window standardWindowButton:NSWindowCloseButton] setHidden:YES];
    [[window standardWindowButton:NSWindowMiniaturizeButton] setHidden:YES];
    [[window standardWindowButton:NSWindowZoomButton] setHidden:YES];
  }

  return window;
}

void HoneycombNativeWidgetMac::GetWindowFrameTitlebarHeight(
    bool* override_titlebar_height,
    float* titlebar_height) {
  if (window_delegate_->GetTitlebarHeight(window_, titlebar_height)) {
    *override_titlebar_height = true;
  } else {
    views::NativeWidgetMac::GetWindowFrameTitlebarHeight(
        override_titlebar_height, titlebar_height);
  }
}

void HoneycombNativeWidgetMac::OnWindowFullscreenTransitionStart() {
  views::NativeWidgetMac::OnWindowFullscreenTransitionStart();
  if (IsHoneycombWindowInitialized()) {
    window_delegate_->OnWindowFullscreenTransition(window_, false);
  }
}

void HoneycombNativeWidgetMac::OnWindowFullscreenTransitionComplete() {
  views::NativeWidgetMac::OnWindowFullscreenTransitionComplete();
  if (IsHoneycombWindowInitialized()) {
    window_delegate_->OnWindowFullscreenTransition(window_, true);
  }
}

void HoneycombNativeWidgetMac::OnWindowInitialized() {
  if (!browser_view_) {
    return;
  }

  // From BrowserFrameMac::OnWindowInitialized.
  if (auto* bridge = GetInProcessNSWindowBridge()) {
    bridge->SetCommandDispatcher([[ChromeCommandDispatcherDelegate alloc] init],
                                 [[BrowserWindowCommandHandler alloc] init]);
  } else {
    if (auto* host = GetHostForBrowser(browser_view_->browser())) {
      host->GetAppShim()->CreateCommandDispatcherForWidget(
          GetNSWindowHost()->bridged_native_widget_id());
    }
  }
}

bool HoneycombNativeWidgetMac::IsHoneycombWindowInitialized() const {
  return static_cast<HoneycombWindowImpl*>(window_.get())->initialized();
}
