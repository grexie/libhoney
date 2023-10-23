// Copyright (C) 2023 Grexie. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "libhoneycomb/common/waitable_event_impl.h"

#include "include/honey_task.h"

#include "base/notreached.h"
#include "base/time/time.h"

namespace {

bool AllowWait() {
  if (HoneycombCurrentlyOn(TID_UI) || HoneycombCurrentlyOn(TID_IO)) {
    DCHECK(false) << "waiting is not allowed on the current thread";
    return false;
  }
  return true;
}

}  // namespace

// static
HoneycombRefPtr<HoneycombWaitableEvent> HoneycombWaitableEvent::CreateWaitableEvent(
    bool automatic_reset,
    bool initially_signaled) {
  return new HoneycombWaitableEventImpl(automatic_reset, initially_signaled);
}

HoneycombWaitableEventImpl::HoneycombWaitableEventImpl(bool automatic_reset,
                                           bool initially_signaled)
    : event_(automatic_reset ? base::WaitableEvent::ResetPolicy::AUTOMATIC
                             : base::WaitableEvent::ResetPolicy::MANUAL,
             initially_signaled
                 ? base::WaitableEvent::InitialState::SIGNALED
                 : base::WaitableEvent::InitialState::NOT_SIGNALED) {}

void HoneycombWaitableEventImpl::Reset() {
  event_.Reset();
}

void HoneycombWaitableEventImpl::Signal() {
  event_.Signal();
}

bool HoneycombWaitableEventImpl::IsSignaled() {
  return event_.IsSignaled();
}

void HoneycombWaitableEventImpl::Wait() {
  if (!AllowWait()) {
    return;
  }
  event_.Wait();
}

bool HoneycombWaitableEventImpl::TimedWait(int64_t max_ms) {
  if (!AllowWait()) {
    return false;
  }
  return event_.TimedWait(base::Milliseconds(max_ms));
}
