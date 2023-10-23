// Copyright (c) 2019 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_DLL_SHUTDOWN_CHECKER_H_
#define HONEYCOMB_LIBHONEYCOMB_DLL_SHUTDOWN_CHECKER_H_
#pragma once

namespace shutdown_checker {

// Check that Honeycomb objects are not held at HoneycombShutdown.
void AssertNotShutdown();

// Called from libhoneycomb_dll.cc and libhoneycomb_dll_wrapper.cc.
void SetIsShutdown();

}  // namespace shutdown_checker

#endif  // HONEYCOMB_LIBHONEYCOMB_DLL_SHUTDOWN_CHECKER_H_
