// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeysimple/simple_handler.h"

#include <windows.h>
#include <string>

#include "include/honey_browser.h"

void SimpleHandler::PlatformTitleChange(HoneycombRefPtr<HoneycombBrowser> browser,
                                        const HoneycombString& title) {
  HoneycombWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
  if (hwnd) {
    SetWindowText(hwnd, std::wstring(title).c_str());
  }
}
