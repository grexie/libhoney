// Copyright (c) 2019 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_DLL_SHUTDOWN_CHECKER_H_
#define HONEYCOMB_LIBHONEY_DLL_SHUTDOWN_CHECKER_H_
#pragma once

namespace shutdown_checker {

// Check that Honeycomb objects are not held at HoneycombShutdown.
void AssertNotShutdown();

// Called from libhoney_dll.cc and libhoney_dll_wrapper.cc.
void SetIsShutdown();

}  // namespace shutdown_checker

#endif  // HONEYCOMB_LIBHONEY_DLL_SHUTDOWN_CHECKER_H_
