// Copyright (c) 2014 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/wrapper/honey_closure_task.h"

#include <memory>

#include "include/base/honey_callback.h"

namespace {

class HoneycombOnceClosureTask : public HoneycombTask {
 public:
  explicit HoneycombOnceClosureTask(base::OnceClosure closure)
      : closure_(std::move(closure)) {}

  HoneycombOnceClosureTask(const HoneycombOnceClosureTask&) = delete;
  HoneycombOnceClosureTask& operator=(const HoneycombOnceClosureTask&) = delete;

  // HoneycombTask method
  void Execute() override { std::move(closure_).Run(); }

 private:
  base::OnceClosure closure_;

  IMPLEMENT_REFCOUNTING(HoneycombOnceClosureTask);
};

class HoneycombRepeatingClosureTask : public HoneycombTask {
 public:
  explicit HoneycombRepeatingClosureTask(const base::RepeatingClosure& closure)
      : closure_(closure) {}

  HoneycombRepeatingClosureTask(const HoneycombRepeatingClosureTask&) = delete;
  HoneycombRepeatingClosureTask& operator=(const HoneycombRepeatingClosureTask&) = delete;

  // HoneycombTask method
  void Execute() override {
    closure_.Run();
    closure_.Reset();
  }

 private:
  base::RepeatingClosure closure_;

  IMPLEMENT_REFCOUNTING(HoneycombRepeatingClosureTask);
};

}  // namespace

HoneycombRefPtr<HoneycombTask> HoneycombCreateClosureTask(base::OnceClosure closure) {
  return new HoneycombOnceClosureTask(std::move(closure));
}

HoneycombRefPtr<HoneycombTask> HoneycombCreateClosureTask(const base::RepeatingClosure& closure) {
  return new HoneycombRepeatingClosureTask(closure);
}

bool HoneycombPostTask(HoneycombThreadId threadId, base::OnceClosure closure) {
  return HoneycombPostTask(threadId, new HoneycombOnceClosureTask(std::move(closure)));
}

bool HoneycombPostTask(HoneycombThreadId threadId, const base::RepeatingClosure& closure) {
  return HoneycombPostTask(threadId, new HoneycombRepeatingClosureTask(closure));
}

bool HoneycombPostDelayedTask(HoneycombThreadId threadId,
                        base::OnceClosure closure,
                        int64_t delay_ms) {
  return HoneycombPostDelayedTask(
      threadId, new HoneycombOnceClosureTask(std::move(closure)), delay_ms);
}

bool HoneycombPostDelayedTask(HoneycombThreadId threadId,
                        const base::RepeatingClosure& closure,
                        int64_t delay_ms) {
  return HoneycombPostDelayedTask(threadId, new HoneycombRepeatingClosureTask(closure),
                            delay_ms);
}
