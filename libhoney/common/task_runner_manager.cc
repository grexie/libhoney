// Copyright 2020 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoney/common/task_runner_manager.h"

#include "base/logging.h"

namespace {

HoneycombTaskRunnerManager* g_manager = nullptr;

}  // namespace

// static
HoneycombTaskRunnerManager* HoneycombTaskRunnerManager::Get() {
  return g_manager;
}

HoneycombTaskRunnerManager::HoneycombTaskRunnerManager() {
  // Only a single instance should exist.
  DCHECK(!g_manager);
  g_manager = this;
}

HoneycombTaskRunnerManager::~HoneycombTaskRunnerManager() {
  g_manager = nullptr;
}
