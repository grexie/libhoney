// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeytests/thread_helper.h"

#include <memory>

#include "include/wrapper/honey_closure_task.h"

void SignalEvent(HoneycombRefPtr<HoneycombWaitableEvent> event) {
  event->Signal();
}

void WaitForThread(HoneycombThreadId thread_id, int64_t delay_ms) {
  HoneycombRefPtr<HoneycombWaitableEvent> event =
      HoneycombWaitableEvent::CreateWaitableEvent(true, false);
  HoneycombPostDelayedTask(thread_id, base::BindOnce(SignalEvent, event), delay_ms);
  event->Wait();
}

void WaitForThread(HoneycombRefPtr<HoneycombTaskRunner> task_runner, int64_t delay_ms) {
  HoneycombRefPtr<HoneycombWaitableEvent> event =
      HoneycombWaitableEvent::CreateWaitableEvent(true, false);
  task_runner->PostDelayedTask(
      HoneycombCreateClosureTask(base::BindOnce(SignalEvent, event)), delay_ms);
  event->Wait();
}

void RunOnThread(HoneycombThreadId thread_id,
                 base::OnceClosure test_impl,
                 HoneycombRefPtr<HoneycombWaitableEvent> event) {
  if (!HoneycombCurrentlyOn(thread_id)) {
    HoneycombPostTask(thread_id, base::BindOnce(RunOnThread, thread_id,
                                          std::move(test_impl), event));
    return;
  }

  std::move(test_impl).Run();
  SignalEvent(event);
}

void RunOnThreadAsync(
    HoneycombThreadId thread_id,
    base::OnceCallback<void(HoneycombRefPtr<HoneycombWaitableEvent>)> test_impl,
    HoneycombRefPtr<HoneycombWaitableEvent> event) {
  if (!HoneycombCurrentlyOn(thread_id)) {
    HoneycombPostTask(thread_id, base::BindOnce(RunOnThreadAsync, thread_id,
                                          std::move(test_impl), event));
    return;
  }

  std::move(test_impl).Run(event);
}
