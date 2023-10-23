// Copyright 2020 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_COMMON_MAIN_RUNNER_DELEGATE_H_
#define HONEYCOMB_LIBHONEYCOMB_COMMON_MAIN_RUNNER_DELEGATE_H_
#pragma once

#include "include/honey_app.h"

namespace base {
class RunLoop;
}

namespace content {
class ContentMainDelegate;
}

class HoneycombMainRunnerDelegate {
 public:
  virtual content::ContentMainDelegate* GetContentMainDelegate() = 0;

  virtual void BeforeMainThreadInitialize(const HoneycombMainArgs& args) {}
  virtual void BeforeMainThreadRun(bool multi_threaded_message_loop) {}
  virtual void BeforeMainMessageLoopRun(base::RunLoop* run_loop) {}
  virtual bool HandleMainMessageLoopQuit() { return false; }
  virtual void BeforeUIThreadInitialize() {}
  virtual void AfterUIThreadInitialize() {}
  virtual void AfterUIThreadShutdown() {}
  virtual void BeforeMainThreadShutdown() {}
  virtual void AfterMainThreadShutdown() {}
  virtual void BeforeExecuteProcess(const HoneycombMainArgs& args) {}
  virtual void AfterExecuteProcess() {}

  virtual ~HoneycombMainRunnerDelegate() {}
};

#endif  // HONEYCOMB_LIBHONEYCOMB_COMMON_MAIN_RUNNER_DELEGATE_H_
