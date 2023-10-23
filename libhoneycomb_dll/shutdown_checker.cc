// Copyright (c) 2019 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoneycomb_dll/shutdown_checker.h"

#include <atomic>

#include "include/base/honey_logging.h"

namespace shutdown_checker {

#if DCHECK_IS_ON()

namespace {

std::atomic_bool g_honey_shutdown{false};

bool IsHoneycombShutdown() {
  return g_honey_shutdown.load();
}

void SetHoneycombShutdown() {
  g_honey_shutdown.store(true);
}

}  // namespace

void AssertNotShutdown() {
  DCHECK(!IsHoneycombShutdown())
      << "Object reference incorrectly held at HoneycombShutdown";
}

void SetIsShutdown() {
  DCHECK(!IsHoneycombShutdown());
  SetHoneycombShutdown();
}

#else  // !DCHECK_IS_ON()

void AssertNotShutdown() {}

#endif  // !DCHECK_IS_ON()

}  // namespace shutdown_checker
