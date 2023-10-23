// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_SHARED_BROWSER_MAIN_MESSAGE_LOOP_STD_H_
#define HONEYCOMB_TESTS_SHARED_BROWSER_MAIN_MESSAGE_LOOP_STD_H_
#pragma once

#include "tests/shared/browser/main_message_loop.h"

namespace client {

// Represents the main message loop in the browser process. This implementation
// is a light-weight wrapper around the Chromium UI thread.
class MainMessageLoopStd : public MainMessageLoop {
 public:
  MainMessageLoopStd();

  // MainMessageLoop methods.
  int Run() override;
  void Quit() override;
  void PostTask(HoneycombRefPtr<HoneycombTask> task) override;
  bool RunsTasksOnCurrentThread() const override;

#if defined(OS_WIN)
  void SetCurrentModelessDialog(HWND hWndDialog) override;
#endif

 private:
  DISALLOW_COPY_AND_ASSIGN(MainMessageLoopStd);
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_SHARED_BROWSER_MAIN_MESSAGE_LOOP_STD_H_
