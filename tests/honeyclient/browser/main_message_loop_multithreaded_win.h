// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_MAIN_MESSAGE_LOOP_MULTITHREADED_WIN_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_MAIN_MESSAGE_LOOP_MULTITHREADED_WIN_H_
#pragma once

#include <windows.h>
#include <queue>

#include "include/base/honey_lock.h"
#include "include/base/honey_platform_thread.h"
#include "tests/shared/browser/main_message_loop.h"

namespace client {

// Represents the main message loop in the browser process when using multi-
// threaded message loop mode on Windows. In this mode there is no Chromium
// message loop running on the main application thread. Instead, this
// implementation utilizes a hidden message window for running tasks.
class MainMessageLoopMultithreadedWin : public MainMessageLoop {
 public:
  MainMessageLoopMultithreadedWin();
  ~MainMessageLoopMultithreadedWin();

  // MainMessageLoop methods.
  int Run() override;
  void Quit() override;
  void PostTask(HoneycombRefPtr<HoneycombTask> task) override;
  bool RunsTasksOnCurrentThread() const override;
  void SetCurrentModelessDialog(HWND hWndDialog) override;

 private:
  // Create the message window.
  static HWND CreateMessageWindow(HINSTANCE hInstance);

  // Window procedure for the message window.
  static LRESULT CALLBACK MessageWndProc(HWND hWnd,
                                         UINT message,
                                         WPARAM wParam,
                                         LPARAM lParam);

  void PostTaskInternal(HoneycombRefPtr<HoneycombTask> task);

  base::PlatformThreadId thread_id_;
  UINT task_message_id_;

  // Only accessed on the main thread.
  HWND dialog_hwnd_;

  base::Lock lock_;

  // Must be protected by |lock_|.
  HWND message_hwnd_;
  std::queue<HoneycombRefPtr<HoneycombTask>> queued_tasks_;

  DISALLOW_COPY_AND_ASSIGN(MainMessageLoopMultithreadedWin);
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_MAIN_MESSAGE_LOOP_MULTITHREADED_WIN_H_
