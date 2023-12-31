// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeyclient/browser/window_test_runner_win.h"

#include "tests/shared/browser/main_message_loop.h"

namespace client {
namespace window_test {

namespace {

HWND GetRootHwnd(HoneycombRefPtr<HoneycombBrowser> browser) {
  return ::GetAncestor(browser->GetHost()->GetWindowHandle(), GA_ROOT);
}

// Toggles the current display state.
void Toggle(HWND root_hwnd, UINT nCmdShow) {
  // Retrieve current window placement information.
  WINDOWPLACEMENT placement;
  ::GetWindowPlacement(root_hwnd, &placement);

  if (placement.showCmd == nCmdShow) {
    ::ShowWindow(root_hwnd, SW_RESTORE);
  } else {
    ::ShowWindow(root_hwnd, nCmdShow);
  }
}

void SetPosImpl(HoneycombRefPtr<HoneycombBrowser> browser,
                int x,
                int y,
                int width,
                int height) {
  HWND root_hwnd = GetRootHwnd(browser);
  if (!root_hwnd) {
    return;
  }

  // Retrieve current window placement information.
  WINDOWPLACEMENT placement;
  ::GetWindowPlacement(root_hwnd, &placement);

  // Retrieve information about the display that contains the window.
  HMONITOR monitor =
      MonitorFromRect(&placement.rcNormalPosition, MONITOR_DEFAULTTONEAREST);
  MONITORINFO info;
  info.cbSize = sizeof(info);
  GetMonitorInfo(monitor, &info);

  // Make sure the window is inside the display.
  HoneycombRect display_rect(info.rcWork.left, info.rcWork.top,
                       info.rcWork.right - info.rcWork.left,
                       info.rcWork.bottom - info.rcWork.top);
  HoneycombRect window_rect(x, y, width, height);
  WindowTestRunner::ModifyBounds(display_rect, window_rect);

  if (placement.showCmd == SW_SHOWMINIMIZED ||
      placement.showCmd == SW_SHOWMAXIMIZED) {
    // The window is currently minimized or maximized. Restore it to the desired
    // position.
    placement.rcNormalPosition.left = window_rect.x;
    placement.rcNormalPosition.right = window_rect.x + window_rect.width;
    placement.rcNormalPosition.top = window_rect.y;
    placement.rcNormalPosition.bottom = window_rect.y + window_rect.height;
    ::SetWindowPlacement(root_hwnd, &placement);
    ::ShowWindow(root_hwnd, SW_RESTORE);
  } else {
    // Set the window position.
    ::SetWindowPos(root_hwnd, nullptr, window_rect.x, window_rect.y,
                   window_rect.width, window_rect.height, SWP_NOZORDER);
  }
}

void MinimizeImpl(HoneycombRefPtr<HoneycombBrowser> browser) {
  HWND root_hwnd = GetRootHwnd(browser);
  if (!root_hwnd) {
    return;
  }
  Toggle(root_hwnd, SW_MINIMIZE);
}

void MaximizeImpl(HoneycombRefPtr<HoneycombBrowser> browser) {
  HWND root_hwnd = GetRootHwnd(browser);
  if (!root_hwnd) {
    return;
  }
  Toggle(root_hwnd, SW_MAXIMIZE);
}

void RestoreImpl(HoneycombRefPtr<HoneycombBrowser> browser) {
  HWND root_hwnd = GetRootHwnd(browser);
  if (!root_hwnd) {
    return;
  }
  ::ShowWindow(root_hwnd, SW_RESTORE);
}

}  // namespace

WindowTestRunnerWin::WindowTestRunnerWin() {}

void WindowTestRunnerWin::SetPos(HoneycombRefPtr<HoneycombBrowser> browser,
                                 int x,
                                 int y,
                                 int width,
                                 int height) {
  if (CURRENTLY_ON_MAIN_THREAD()) {
    SetPosImpl(browser, x, y, width, height);
  } else {
    // Execute on the main application thread.
    MAIN_POST_CLOSURE(base::BindOnce(SetPosImpl, browser, x, y, width, height));
  }
}

void WindowTestRunnerWin::Minimize(HoneycombRefPtr<HoneycombBrowser> browser) {
  if (CURRENTLY_ON_MAIN_THREAD()) {
    MinimizeImpl(browser);
  } else {
    // Execute on the main application thread.
    MAIN_POST_CLOSURE(base::BindOnce(MinimizeImpl, browser));
  }
}

void WindowTestRunnerWin::Maximize(HoneycombRefPtr<HoneycombBrowser> browser) {
  if (CURRENTLY_ON_MAIN_THREAD()) {
    MaximizeImpl(browser);
  } else {
    // Execute on the main application thread.
    MAIN_POST_CLOSURE(base::BindOnce(MaximizeImpl, browser));
  }
}

void WindowTestRunnerWin::Restore(HoneycombRefPtr<HoneycombBrowser> browser) {
  if (CURRENTLY_ON_MAIN_THREAD()) {
    RestoreImpl(browser);
  } else {
    // Execute on the main application thread.
    MAIN_POST_CLOSURE(base::BindOnce(RestoreImpl, browser));
  }
}

}  // namespace window_test
}  // namespace client
