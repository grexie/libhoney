// Copyright 2022 The Honeycomb Authors. Portions copyright
// 2011 The Chromium Authors and 2016 the CEF Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#include "libhoneycomb/common/util_linux.h"

#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/path_service.h"
#include "content/public/common/content_paths.h"
#include "content/public/common/content_switches.h"

namespace util_linux {

namespace {

void OverrideChildProcessPath() {
  base::FilePath child_process_path =
      base::CommandLine::ForCurrentProcess()->GetSwitchValuePath(
          switches::kBrowserSubprocessPath);
  if (child_process_path.empty()) {
    return;
  }

  // Used by ChildProcessHost::GetChildPath and PlatformCrashpadInitialization.
  base::PathService::OverrideAndCreateIfNeeded(
      content::CHILD_PROCESS_EXE, child_process_path, /*is_absolute=*/true,
      /*create=*/false);
}

}  // namespace

void PreSandboxStartup() {
  OverrideChildProcessPath();
}

}  // namespace util_linux
