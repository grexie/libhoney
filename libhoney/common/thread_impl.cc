// Copyright (C) 2023 Grexie. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "libhoney/common/thread_impl.h"

#include "libhoney/common/task_runner_impl.h"

#include "base/functional/bind.h"
#include "base/threading/thread_restrictions.h"

namespace {

void StopAndDestroy(base::Thread* thread) {
  // Calling PlatformThread::Join() on the UI thread is otherwise disallowed.
  base::ScopedAllowBaseSyncPrimitivesForTesting scoped_allow_sync_primitives;

  // Deleting |thread| will implicitly stop and join it.
  delete thread;
}

}  // namespace

// static
HoneycombRefPtr<HoneycombThread> HoneycombThread::CreateThread(
    const HoneycombString& display_name,
    honey_thread_priority_t priority,
    honey_message_loop_type_t message_loop_type,
    bool stoppable,
    honey_com_init_mode_t com_init_mode) {
  if (!HoneycombTaskRunnerImpl::GetCurrentTaskRunner()) {
    DCHECK(false) << "called on invalid thread";
    return nullptr;
  }

  HoneycombRefPtr<HoneycombThreadImpl> thread_impl = new HoneycombThreadImpl();
  if (!thread_impl->Create(display_name, priority, message_loop_type, stoppable,
                           com_init_mode)) {
    return nullptr;
  }
  return thread_impl;
}

HoneycombThreadImpl::HoneycombThreadImpl() : thread_id_(kInvalidPlatformThreadId) {}

HoneycombThreadImpl::~HoneycombThreadImpl() {
  if (thread_.get()) {
    if (!owner_task_runner_->RunsTasksInCurrentSequence()) {
      // Delete |thread_| on the correct thread.
      owner_task_runner_->PostTask(
          FROM_HERE,
          base::BindOnce(StopAndDestroy, base::Unretained(thread_.release())));
    } else {
      StopAndDestroy(thread_.release());
    }
  }
}

bool HoneycombThreadImpl::Create(const HoneycombString& display_name,
                           honey_thread_priority_t priority,
                           honey_message_loop_type_t message_loop_type,
                           bool stoppable,
                           honey_com_init_mode_t com_init_mode) {
  owner_task_runner_ = HoneycombTaskRunnerImpl::GetCurrentTaskRunner();
  DCHECK(owner_task_runner_);
  if (!owner_task_runner_) {
    return false;
  }

  thread_.reset(new base::Thread(display_name));

  base::Thread::Options options;

  switch (priority) {
    case TP_BACKGROUND:
      options.thread_type = base::ThreadType::kBackground;
      break;
    case TP_DISPLAY:
      options.thread_type = base::ThreadType::kDisplayCritical;
      break;
    case TP_REALTIME_AUDIO:
      options.thread_type = base::ThreadType::kRealtimeAudio;
      break;
    default:
      break;
  }

  switch (message_loop_type) {
    case ML_TYPE_UI:
      options.message_pump_type = base::MessagePumpType::UI;
      break;
    case ML_TYPE_IO:
      options.message_pump_type = base::MessagePumpType::IO;
      break;
    default:
      break;
  }

  options.joinable = stoppable;

#if BUILDFLAG(IS_WIN)
  if (com_init_mode != COM_INIT_MODE_NONE) {
    if (com_init_mode == COM_INIT_MODE_STA) {
      options.message_pump_type = base::MessagePumpType::UI;
    }
    thread_->init_com_with_mta(com_init_mode == COM_INIT_MODE_MTA);
  }
#endif

  if (!thread_->StartWithOptions(std::move(options))) {
    thread_.reset();
    return false;
  }

  thread_task_runner_ = new HoneycombTaskRunnerImpl(thread_->task_runner());
  thread_id_ = thread_->GetThreadId();
  return true;
}

HoneycombRefPtr<HoneycombTaskRunner> HoneycombThreadImpl::GetTaskRunner() {
  return thread_task_runner_;
}

honey_platform_thread_id_t HoneycombThreadImpl::GetPlatformThreadId() {
  return thread_id_;
}

void HoneycombThreadImpl::Stop() {
  if (!owner_task_runner_) {
    return;
  }
  if (!owner_task_runner_->RunsTasksInCurrentSequence()) {
    DCHECK(false) << "called on invalid thread";
    return;
  }

  if (thread_) {
    StopAndDestroy(thread_.release());
  }
}

bool HoneycombThreadImpl::IsRunning() {
  if (!owner_task_runner_) {
    return false;
  }
  if (!owner_task_runner_->RunsTasksInCurrentSequence()) {
    DCHECK(false) << "called on invalid thread";
    return false;
  }

  return thread_ && thread_->IsRunning();
}
