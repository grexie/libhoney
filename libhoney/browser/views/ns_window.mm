// Copyright 2023 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoney/browser/views/ns_window.h"

#include "base/i18n/rtl.h"
#include "components/remote_cocoa/app_shim/native_widget_ns_window_bridge.h"
#include "components/remote_cocoa/common/native_widget_ns_window_host.mojom.h"
#include "ui/base/cocoa/window_size_constants.h"

@interface HoneycombThemeFrame : NativeWidgetMacNSWindowTitledFrame
@end

// NSThemeFrame (PrivateAPI) definitions.
@interface NSThemeFrame (PrivateAPI)
- (void)setStyleMask:(NSUInteger)styleMask;
- (CGFloat)_titlebarHeight;
- (BOOL)_shouldCenterTrafficLights;
@end

@implementation HoneycombThemeFrame {
  bool in_full_screen_;
}

// NSThemeFrame (PrivateAPI) overrides.
- (void)setStyleMask:(NSUInteger)styleMask {
  in_full_screen_ = (styleMask & NSWindowStyleMaskFullScreen) != 0;
  [super setStyleMask:styleMask];
}

- (CGFloat)_titlebarHeight {
  if (!in_full_screen_) {
    bool override_titlebar_height = false;
    float titlebar_height = 0;
    auto* window = base::apple::ObjCCast<HoneycombNSWindow>([self window]);
    if (auto* bridge = [window bridge]) {
      bridge->host()->GetWindowFrameTitlebarHeight(&override_titlebar_height,
                                                   &titlebar_height);

      if (override_titlebar_height) {
        return titlebar_height;
      }
    }
  }

  return [super _titlebarHeight];
}

- (BOOL)_shouldCenterTrafficLights {
  auto* window = base::apple::ObjCCast<HoneycombNSWindow>([self window]);
  return [window shouldCenterTrafficLights];
}

- (BOOL)_shouldFlipTrafficLightsForRTL {
  return base::i18n::IsRTL() ? YES : NO;
}

@end

@interface NSWindow (PrivateAPI)
+ (Class)frameViewClassForStyleMask:(NSUInteger)windowStyle;
@end

@implementation HoneycombNSWindow

- (id)initWithStyle:(NSUInteger)style_mask isFrameless:(bool)is_frameless {
  if ((self = [super initWithContentRect:ui::kWindowSizeDeterminedLater
                               styleMask:style_mask
                                 backing:NSBackingStoreBuffered
                                   defer:NO])) {
    is_frameless_ = is_frameless;
  }
  return self;
}

- (BOOL)shouldCenterTrafficLights {
  return is_frameless_ ? YES : NO;
}

// NSWindow overrides.
- (NSRect)contentRectForFrameRect:(NSRect)frameRect {
  if (is_frameless_) {
    return frameRect;
  }
  return [super contentRectForFrameRect:frameRect];
}

- (NSRect)frameRectForContentRect:(NSRect)contentRect {
  if (is_frameless_) {
    return contentRect;
  }
  return [super frameRectForContentRect:contentRect];
}

// NSWindow (PrivateAPI) overrides.
+ (Class)frameViewClassForStyleMask:(NSUInteger)windowStyle {
  if (Class custom_frame = [HoneycombThemeFrame class]) {
    return custom_frame;
  }

  return [super frameViewClassForStyleMask:windowStyle];
}

@end
