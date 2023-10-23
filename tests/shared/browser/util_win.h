// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_SHARED_BROWSER_UTIL_WIN_H_
#define HONEYCOMB_TESTS_SHARED_BROWSER_UTIL_WIN_H_
#pragma once

#include <windows.h>
#include <string>

#include "include/internal/honey_types_wrappers.h"

namespace client {

// Returns the current time in microseconds.
uint64_t GetTimeNow();

// Set the window's user data pointer.
void SetUserDataPtr(HWND hWnd, void* ptr);

// Return the window's user data pointer.
template <typename T>
T GetUserDataPtr(HWND hWnd) {
  return reinterpret_cast<T>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
}

// Set the window's window procedure pointer and return the old value.
WNDPROC SetWndProcPtr(HWND hWnd, WNDPROC wndProc);

// Return the resource string with the specified id.
std::wstring GetResourceString(UINT id);

int GetHoneycombMouseModifiers(WPARAM wparam);
int GetHoneycombKeyboardModifiers(WPARAM wparam, LPARAM lparam);
bool IsKeyDown(WPARAM wparam);

// Returns the device scale factor. For example, 200% display scaling will
// return 2.0.
float GetDeviceScaleFactor();

}  // namespace client

#endif  // HONEYCOMB_TESTS_SHARED_BROWSER_UTIL_WIN_H_
