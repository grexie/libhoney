// Copyright (C) 2023 Grexie. Portions copyright
// 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#include "include/base/honey_callback.h"
#include "include/honey_thread.h"
#include "include/honey_waitable_event.h"
#include "include/wrapper/honey_closure_task.h"
#include "tests/gtest/include/gtest/gtest.h"

// Test manual reset.
TEST(WaitableEventTest, ManualReset) {
  HoneycombRefPtr<HoneycombWaitableEvent> event =
      HoneycombWaitableEvent::CreateWaitableEvent(false, false);

  EXPECT_FALSE(event->IsSignaled());

  event->Signal();
  EXPECT_TRUE(event->IsSignaled());
  EXPECT_TRUE(event->IsSignaled());

  event->Reset();
  EXPECT_FALSE(event->IsSignaled());
  EXPECT_FALSE(event->TimedWait(10));

  event->Signal();
  event->Wait();
  EXPECT_TRUE(event->TimedWait(10));
}

// Test automatic reset.
TEST(WaitableEventTest, AutomaticReset) {
  HoneycombRefPtr<HoneycombWaitableEvent> event =
      HoneycombWaitableEvent::CreateWaitableEvent(true, false);

  EXPECT_FALSE(event->IsSignaled());

  event->Signal();
  EXPECT_TRUE(event->IsSignaled());
  EXPECT_FALSE(event->IsSignaled());

  event->Reset();
  EXPECT_FALSE(event->IsSignaled());
  EXPECT_FALSE(event->TimedWait(10));

  event->Signal();
  event->Wait();
  EXPECT_FALSE(event->TimedWait(10));

  event->Signal();
  EXPECT_TRUE(event->TimedWait(10));
}

namespace {

void SignalEvent(HoneycombWaitableEvent* event) {
  event->Signal();
}

}  // namespace

// Tests that a WaitableEvent can be safely deleted when |Wait| is done without
// additional synchronization.
TEST(WaitableEventTest, WaitAndDelete) {
  HoneycombRefPtr<HoneycombWaitableEvent> event =
      HoneycombWaitableEvent::CreateWaitableEvent(true, false);

  HoneycombRefPtr<HoneycombThread> thread = HoneycombThread::CreateThread("waitable_event_test");
  thread->GetTaskRunner()->PostDelayedTask(
      HoneycombCreateClosureTask(
          base::BindOnce(SignalEvent, base::Unretained(event.get()))),
      10);

  event->Wait();
  event = nullptr;

  thread->Stop();
  thread = nullptr;
}
