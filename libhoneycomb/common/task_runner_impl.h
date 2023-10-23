// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_COMMON_TASK_RUNNER_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_COMMON_TASK_RUNNER_IMPL_H_
#pragma once

#include "include/honey_task.h"

#include "base/task/single_thread_task_runner.h"

class HoneycombTaskRunnerImpl : public HoneycombTaskRunner {
 public:
  explicit HoneycombTaskRunnerImpl(
      scoped_refptr<base::SingleThreadTaskRunner> task_runner);

  HoneycombTaskRunnerImpl(const HoneycombTaskRunnerImpl&) = delete;
  HoneycombTaskRunnerImpl& operator=(const HoneycombTaskRunnerImpl&) = delete;

  // Returns the task runner associated with |threadId|.
  static scoped_refptr<base::SingleThreadTaskRunner> GetTaskRunner(
      HoneycombThreadId threadId);
  // Returns the current task runner.
  static scoped_refptr<base::SingleThreadTaskRunner> GetCurrentTaskRunner();

  // HoneycombTaskRunner methods:
  bool IsSame(HoneycombRefPtr<HoneycombTaskRunner> that) override;
  bool BelongsToCurrentThread() override;
  bool BelongsToThread(HoneycombThreadId threadId) override;
  bool PostTask(HoneycombRefPtr<HoneycombTask> task) override;
  bool PostDelayedTask(HoneycombRefPtr<HoneycombTask> task, int64_t delay_ms) override;

 private:
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;

  IMPLEMENT_REFCOUNTING(HoneycombTaskRunnerImpl);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_COMMON_TASK_RUNNER_IMPL_H_
