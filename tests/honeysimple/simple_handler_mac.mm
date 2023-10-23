// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeysimple/simple_handler.h"

#import <Cocoa/Cocoa.h>

#include "include/honey_browser.h"

void SimpleHandler::PlatformTitleChange(HoneycombRefPtr<HoneycombBrowser> browser,
                                        const HoneycombString& title) {
  NSView* view =
      CAST_HONEYCOMB_WINDOW_HANDLE_TO_NSVIEW(browser->GetHost()->GetWindowHandle());
  NSWindow* window = [view window];
  std::string titleStr(title);
  NSString* str = [NSString stringWithUTF8String:titleStr.c_str()];
  [window setTitle:str];
}
