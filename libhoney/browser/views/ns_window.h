// Copyright 2023 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_VIEWS_NS_WINDOW_H_
#define HONEYCOMB_LIBHONEY_BROWSER_VIEWS_NS_WINDOW_H_
#pragma once

#include "components/remote_cocoa/app_shim/native_widget_mac_nswindow.h"

@interface HoneycombNSWindow : NativeWidgetMacNSWindow {
 @private
  bool is_frameless_;
}
- (id)initWithStyle:(NSUInteger)style_mask isFrameless:(bool)is_frameless;

- (BOOL)shouldCenterTrafficLights;
@end

#endif  // HONEYCOMB_LIBHONEY_BROWSER_VIEWS_NS_WINDOW_H_
