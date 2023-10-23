// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_TEMP_WINDOW_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_TEMP_WINDOW_H_
#pragma once

#include "tests/honeyclient/browser/client_types.h"

#if defined(OS_WIN)
#include "tests/honeyclient/browser/temp_window_win.h"
#elif defined(OS_LINUX)
#include "tests/honeyclient/browser/temp_window_x11.h"
#elif defined(OS_MAC)
#include "tests/honeyclient/browser/temp_window_mac.h"
#endif

namespace client {

#if defined(OS_WIN)
typedef TempWindowWin TempWindow;
#elif defined(OS_LINUX)
typedef TempWindowX11 TempWindow;
#elif defined(OS_MAC)
typedef TempWindowMac TempWindow;
#endif

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_TEMP_WINDOW_H_
