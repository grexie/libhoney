// Copyright 2020 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_COMMON_TASK_RUNNER_MANAGER_H_
#define HONEYCOMB_LIBHONEYCOMB_COMMON_TASK_RUNNER_MANAGER_H_
#pragma once

#include "base/task/single_thread_task_runner.h"

// Exposes global sequenced task runners in the main and render processes.
// Prefer using base::ThreadPool for tasks that do not need to be globally
// sequenced and HoneycombTaskRunner for retrieving named HoneycombThreadId runners.
class HoneycombTaskRunnerManager {
 public:
  // Returns the singleton instance that is scoped to Honeycomb lifespan.
  static HoneycombTaskRunnerManager* Get();

  // Available in the main process:
  virtual scoped_refptr<base::SingleThreadTaskRunner>
  GetBackgroundTaskRunner() = 0;
  virtual scoped_refptr<base::SingleThreadTaskRunner>
  GetUserVisibleTaskRunner() = 0;
  virtual scoped_refptr<base::SingleThreadTaskRunner>
  GetUserBlockingTaskRunner() = 0;

  // Available in the render process:
  virtual scoped_refptr<base::SingleThreadTaskRunner> GetRenderTaskRunner() = 0;
  virtual scoped_refptr<base::SingleThreadTaskRunner>
  GetWebWorkerTaskRunner() = 0;

 protected:
  HoneycombTaskRunnerManager();
  virtual ~HoneycombTaskRunnerManager();
};

#endif  // HONEYCOMB_LIBHONEYCOMB_COMMON_TASK_RUNNER_MANAGER_H_
