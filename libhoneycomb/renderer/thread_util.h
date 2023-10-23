// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_RENDERER_THREAD_UTIL_H_
#define HONEYCOMB_LIBHONEYCOMB_RENDERER_THREAD_UTIL_H_
#pragma once

#include "libhoneycomb/common/task_runner_manager.h"

#include "base/location.h"
#include "base/logging.h"
#include "content/public/renderer/render_thread.h"

#define HONEYCOMB_CURRENTLY_ON_RT() (!!content::RenderThread::Get())

#define HONEYCOMB_REQUIRE_RT() DCHECK(HONEYCOMB_CURRENTLY_ON_RT())

#define HONEYCOMB_REQUIRE_RT_RETURN(var)               \
  if (!HONEYCOMB_CURRENTLY_ON_RT()) {                  \
    DCHECK(false) << "called on invalid thread"; \
    return var;                                  \
  }

#define HONEYCOMB_REQUIRE_RT_RETURN_VOID()             \
  if (!HONEYCOMB_CURRENTLY_ON_RT()) {                  \
    DCHECK(false) << "called on invalid thread"; \
    return;                                      \
  }

#define HONEYCOMB_RENDER_TASK_RUNNER() \
  (HoneycombTaskRunnerManager::Get()->GetRenderTaskRunner())

#define HONEYCOMB_POST_TASK_RT(task) \
  HONEYCOMB_RENDER_TASK_RUNNER()->PostTask(FROM_HERE, task)
#define HONEYCOMB_POST_DELAYED_TASK_RT(task, delay_ms)             \
  HONEYCOMB_RENDER_TASK_RUNNER()->PostDelayedTask(FROM_HERE, task, \
                                            base::Milliseconds(delay_ms))

// Use this template in conjuction with RefCountedThreadSafe when you want to
// ensure that an object is deleted on the render thread.
struct HoneycombDeleteOnRenderThread {
  template <typename T>
  static void Destruct(const T* x) {
    if (HONEYCOMB_CURRENTLY_ON_RT()) {
      delete x;
    } else {
      if (!HONEYCOMB_RENDER_TASK_RUNNER()->DeleteSoon(FROM_HERE, x)) {
#if defined(UNIT_TEST)
        // Only logged under unit testing because leaks at shutdown
        // are acceptable under normal circumstances.
        LOG(ERROR) << "DeleteSoon failed on thread " << thread;
#endif  // UNIT_TEST
      }
    }
  }
};

#endif  // HONEYCOMB_LIBHONEYCOMB_RENDERER_THREAD_UTIL_H_
