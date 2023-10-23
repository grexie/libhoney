// Copyright 2022 The Honeycomb Authors. Portions copyright
// 2011 The Chromium Authors and 2016 the CEF Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_COMMON_UTIL_LINUX_H_
#define HONEYCOMB_LIBHONEY_COMMON_UTIL_LINUX_H_
#pragma once

namespace util_linux {

// Called from MainDelegate::PreSandboxStartup for the main process.
void PreSandboxStartup();

}  // namespace util_linux

#endif  // HONEYCOMB_LIBHONEY_COMMON_UTIL_LINUX_H_
