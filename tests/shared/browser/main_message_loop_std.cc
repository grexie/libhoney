// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/shared/browser/main_message_loop_std.h"

#include "include/honey_app.h"

namespace client {

MainMessageLoopStd::MainMessageLoopStd() {}

int MainMessageLoopStd::Run() {
  HoneycombRunMessageLoop();
  return 0;
}

void MainMessageLoopStd::Quit() {
  HoneycombQuitMessageLoop();
}

void MainMessageLoopStd::PostTask(HoneycombRefPtr<HoneycombTask> task) {
  HoneycombPostTask(TID_UI, task);
}

bool MainMessageLoopStd::RunsTasksOnCurrentThread() const {
  return HoneycombCurrentlyOn(TID_UI);
}

#if defined(OS_WIN)
void MainMessageLoopStd::SetCurrentModelessDialog(HWND hWndDialog) {
  // Nothing to do here. The Chromium message loop implementation will
  // internally route dialog messages.
}
#endif

}  // namespace client
