// Copyright (C) 2023 Grexie. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_COMMON_WAITABLE_EVENT_IMPL_H_
#define HONEYCOMB_LIBHONEY_COMMON_WAITABLE_EVENT_IMPL_H_
#pragma once

#include "include/honey_waitable_event.h"

#include "base/synchronization/waitable_event.h"

class HoneycombWaitableEventImpl : public HoneycombWaitableEvent {
 public:
  HoneycombWaitableEventImpl(bool automatic_reset, bool initially_signaled);

  HoneycombWaitableEventImpl(const HoneycombWaitableEventImpl&) = delete;
  HoneycombWaitableEventImpl& operator=(const HoneycombWaitableEventImpl&) = delete;

  // HoneycombWaitableEvent methods:
  void Reset() override;
  void Signal() override;
  bool IsSignaled() override;
  void Wait() override;
  bool TimedWait(int64_t max_ms) override;

 private:
  base::WaitableEvent event_;

  IMPLEMENT_REFCOUNTING(HoneycombWaitableEventImpl);
};

#endif  // HONEYCOMB_LIBHONEY_COMMON_WAITABLE_EVENT_IMPL_H_
