// Copyright 2017 The Honeycomb Authors. Portions copyright
// 2011 The Chromium Authors and 2016 the CEF Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_COMMON_RESOURCE_UTIL_H_
#define HONEYCOMB_LIBHONEYCOMB_COMMON_RESOURCE_UTIL_H_
#pragma once

#include "include/honey_base.h"

#include "ui/base/resource/resource_scale_factor.h"

namespace base {
class CommandLine;
class FilePath;
}  // namespace base

namespace resource_util {

// Returns the directory that contains resource files (*.bin, *.dat, *.pak,
// etc).
base::FilePath GetResourcesDir();

// Returns the default path for the debug.log file.
base::FilePath GetDefaultLogFilePath();

// Called from MainDelegate::PreSandboxStartup.
void OverrideDefaultDownloadDir();
void OverrideUserDataDir(HoneycombSettings* settings,
                         const base::CommandLine* command_line);

// Returns true if |scale_factor| is supported by this platform.
bool IsScaleFactorSupported(ui::ResourceScaleFactor scale_factor);

#if BUILDFLAG(IS_LINUX)
// Look for binary files (*.bin, *.dat, *.pak, chrome-sandbox, libGLESv2.so,
// libEGL.so, locales/*.pak, swiftshader/*.so) next to libhoneycomb instead of the exe
// on Linux. This is already the default on Windows.
void OverrideAssetPath();
#endif

}  // namespace resource_util

#endif  // HONEYCOMB_LIBHONEYCOMB_COMMON_RESOURCE_UTIL_H_
