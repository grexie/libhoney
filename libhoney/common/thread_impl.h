// Copyright (C) 2023 Grexie. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_COMMON_THREAD_IMPL_H_
#define HONEYCOMB_LIBHONEY_COMMON_THREAD_IMPL_H_
#pragma once

#include "include/honey_thread.h"

#include "base/threading/thread.h"

class HoneycombThreadImpl : public HoneycombThread {
 public:
  HoneycombThreadImpl();

  HoneycombThreadImpl(const HoneycombThreadImpl&) = delete;
  HoneycombThreadImpl& operator=(const HoneycombThreadImpl&) = delete;

  ~HoneycombThreadImpl();

  bool Create(const HoneycombString& display_name,
              honey_thread_priority_t priority,
              honey_message_loop_type_t message_loop_type,
              bool stoppable,
              honey_com_init_mode_t com_init_mode);

  // HoneycombThread methods:
  HoneycombRefPtr<HoneycombTaskRunner> GetTaskRunner() override;
  honey_platform_thread_id_t GetPlatformThreadId() override;
  void Stop() override;
  bool IsRunning() override;

 private:
  std::unique_ptr<base::Thread> thread_;
  honey_platform_thread_id_t thread_id_;
  HoneycombRefPtr<HoneycombTaskRunner> thread_task_runner_;

  // TaskRunner for the owner thread.
  scoped_refptr<base::SequencedTaskRunner> owner_task_runner_;

  IMPLEMENT_REFCOUNTING(HoneycombThreadImpl);
};

#endif  // HONEYCOMB_LIBHONEY_COMMON_THREAD_IMPL_H_
