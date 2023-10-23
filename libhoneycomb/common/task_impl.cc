// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "include/honey_task.h"
#include "libhoneycomb/common/task_runner_impl.h"

#include "base/functional/bind.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/time/time.h"

bool HoneycombCurrentlyOn(HoneycombThreadId threadId) {
  scoped_refptr<base::SequencedTaskRunner> task_runner =
      HoneycombTaskRunnerImpl::GetTaskRunner(threadId);
  if (task_runner.get()) {
    return task_runner->RunsTasksInCurrentSequence();
  }

  LOG(WARNING) << "No task runner for threadId " << threadId;
  return false;
}

bool HoneycombPostTask(HoneycombThreadId threadId, HoneycombRefPtr<HoneycombTask> task) {
  scoped_refptr<base::SequencedTaskRunner> task_runner =
      HoneycombTaskRunnerImpl::GetTaskRunner(threadId);
  if (task_runner.get()) {
    return task_runner->PostTask(FROM_HERE,
                                 base::BindOnce(&HoneycombTask::Execute, task.get()));
  }

  LOG(WARNING) << "No task runner for threadId " << threadId;
  return false;
}

bool HoneycombPostDelayedTask(HoneycombThreadId threadId,
                        HoneycombRefPtr<HoneycombTask> task,
                        int64_t delay_ms) {
  scoped_refptr<base::SequencedTaskRunner> task_runner =
      HoneycombTaskRunnerImpl::GetTaskRunner(threadId);
  if (task_runner.get()) {
    return task_runner->PostDelayedTask(
        FROM_HERE, base::BindOnce(&HoneycombTask::Execute, task.get()),
        base::Milliseconds(delay_ms));
  }

  LOG(WARNING) << "No task runner for threadId " << threadId;
  return false;
}
