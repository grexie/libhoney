// Copyright (c) 2013 The Honeycomb Authors.
// Portions copyright (c) 2011 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tests/shared/browser/resource_util.h"

#import <Foundation/Foundation.h>
#include <mach-o/dyld.h>
#include <stdio.h>

#include "include/base/honey_logging.h"

namespace client {

namespace {

// Implementation adapted from Chromium's base/mac/foundation_util.mm
bool UncachedAmIBundled() {
  return [[[NSBundle mainBundle] bundlePath] hasSuffix:@".app"];
}

bool AmIBundled() {
  static bool am_i_bundled = UncachedAmIBundled();
  return am_i_bundled;
}

}  // namespace

// Implementation adapted from Chromium's base/base_path_mac.mm
bool GetResourceDir(std::string& dir) {
  // Retrieve the executable directory.
  uint32_t pathSize = 0;
  _NSGetExecutablePath(nullptr, &pathSize);
  if (pathSize > 0) {
    dir.resize(pathSize);
    _NSGetExecutablePath(const_cast<char*>(dir.c_str()), &pathSize);
  }

  if (AmIBundled()) {
    // Trim executable name up to the last separator.
    auto last_separator = dir.find_last_of("/");
    dir.resize(last_separator);
    // Trim directory ("MacOS") up to the last separator.
    last_separator = dir.find_last_of("/");
    dir.resize(last_separator);
  }

  dir.append("/Resources");
  return true;
}

}  // namespace client
