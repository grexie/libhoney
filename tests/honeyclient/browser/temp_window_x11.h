// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_TEMP_WINDOW_X11_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_TEMP_WINDOW_X11_H_
#pragma once

#include "include/honey_base.h"

namespace client {

// Represents a singleton hidden window that acts as a temporary parent for
// popup browsers. Only accessed on the UI thread.
class TempWindowX11 {
 public:
  // Returns the singleton window handle.
  static HoneycombWindowHandle GetWindowHandle();

 private:
  // A single instance will be created/owned by RootWindowManager.
  friend class RootWindowManager;
  // Allow deletion via std::unique_ptr only.
  friend std::default_delete<TempWindowX11>;

  TempWindowX11();
  ~TempWindowX11();

  HoneycombWindowHandle xwindow_;

  DISALLOW_COPY_AND_ASSIGN(TempWindowX11);
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_TEMP_WINDOW_X11_H_
