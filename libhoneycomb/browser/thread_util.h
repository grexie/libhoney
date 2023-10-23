// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_THREAD_UTIL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_THREAD_UTIL_H_
#pragma once

#include "base/location.h"
#include "base/logging.h"
#include "base/task/thread_pool.h"
#include "base/threading/scoped_blocking_call.h"
#include "base/threading/thread_restrictions.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"

#define HONEYCOMB_UIT content::BrowserThread::UI
#define HONEYCOMB_IOT content::BrowserThread::IO

#define HONEYCOMB_CURRENTLY_ON(id) content::BrowserThread::CurrentlyOn(id)
#define HONEYCOMB_CURRENTLY_ON_UIT() HONEYCOMB_CURRENTLY_ON(HONEYCOMB_UIT)
#define HONEYCOMB_CURRENTLY_ON_IOT() HONEYCOMB_CURRENTLY_ON(HONEYCOMB_IOT)

#define HONEYCOMB_REQUIRE(id) DCHECK(HONEYCOMB_CURRENTLY_ON(id))
#define HONEYCOMB_REQUIRE_UIT() HONEYCOMB_REQUIRE(HONEYCOMB_UIT)
#define HONEYCOMB_REQUIRE_IOT() HONEYCOMB_REQUIRE(HONEYCOMB_IOT)

#define HONEYCOMB_REQUIRE_RETURN(id, var)              \
  if (!HONEYCOMB_CURRENTLY_ON(id)) {                   \
    DCHECK(false) << "called on invalid thread"; \
    return var;                                  \
  }
#define HONEYCOMB_REQUIRE_UIT_RETURN(var) HONEYCOMB_REQUIRE_RETURN(HONEYCOMB_UIT, var)
#define HONEYCOMB_REQUIRE_IOT_RETURN(var) HONEYCOMB_REQUIRE_RETURN(HONEYCOMB_IOT, var)

#define HONEYCOMB_REQUIRE_RETURN_VOID(id)              \
  if (!HONEYCOMB_CURRENTLY_ON(id)) {                   \
    DCHECK(false) << "called on invalid thread"; \
    return;                                      \
  }
#define HONEYCOMB_REQUIRE_UIT_RETURN_VOID() HONEYCOMB_REQUIRE_RETURN_VOID(HONEYCOMB_UIT)
#define HONEYCOMB_REQUIRE_IOT_RETURN_VOID() HONEYCOMB_REQUIRE_RETURN_VOID(HONEYCOMB_IOT)

template <int id, std::enable_if_t<id == HONEYCOMB_UIT, bool> = true>
auto HONEYCOMB_TASK_RUNNER() {
  return content::GetUIThreadTaskRunner({});
}
template <int id, std::enable_if_t<id == HONEYCOMB_IOT, bool> = true>
auto HONEYCOMB_TASK_RUNNER() {
  return content::GetIOThreadTaskRunner({});
}

#define HONEYCOMB_POST_TASK(id, task) HONEYCOMB_TASK_RUNNER<id>()->PostTask(FROM_HERE, task)
#define HONEYCOMB_POST_DELAYED_TASK(id, task, delay_ms)         \
  HONEYCOMB_TASK_RUNNER<id>()->PostDelayedTask(FROM_HERE, task, \
                                         base::Milliseconds(delay_ms))

// Post a blocking task with the specified |priority|. Tasks that have not
// started executing at shutdown will never run. However, any task that has
// already begun executing when shutdown is invoked will be allowed to continue
// and will block shutdown until completion.
// Tasks posted with this method are not guaranteed to run sequentially. Use
// base::CreateSequencedTaskRunner instead if sequence is important.
// Sequenced runners at various priorities that always execute all pending tasks
// before shutdown are available via HoneycombTaskRunnerManager and exposed by the Honeycomb
// API.
#define HONEYCOMB_POST_BLOCKING_TASK(priority, task)                 \
  base::ThreadPool::PostTask(                                  \
      FROM_HERE,                                               \
      {priority, base::TaskShutdownBehavior::SKIP_ON_SHUTDOWN, \
       base::MayBlock()},                                      \
      task)

// Post a blocking task that affects UI or responsiveness of future user
// interactions. Do not use if an immediate response to a user interaction is
// expected.
#define HONEYCOMB_POST_USER_VISIBLE_TASK(task) \
  HONEYCOMB_POST_BLOCKING_TASK(base::TaskPriority::USER_VISIBLE, task)

// Post a blocking task where the user won't notice if it takes an arbitrarily
// long time to complete.
#define HONEYCOMB_POST_BACKGROUND_TASK(task) \
  HONEYCOMB_POST_BLOCKING_TASK(base::TaskPriority::BEST_EFFORT, task)

// Assert that blocking is allowed on the current thread.
#define HONEYCOMB_REQUIRE_BLOCKING()                   \
  base::ScopedBlockingCall scoped_blocking_call( \
      FROM_HERE, base::BlockingType::WILL_BLOCK)

// Same as IMPLEMENT_REFCOUNTING() but using the specified Destructor.
#define IMPLEMENT_REFCOUNTING_EX(ClassName, Destructor) \
 public:                                                \
  void AddRef() const override {                        \
    ref_count_.AddRef();                                \
  }                                                     \
  bool Release() const override {                       \
    if (ref_count_.Release()) {                         \
      Destructor::Destruct(this);                       \
      return true;                                      \
    }                                                   \
    return false;                                       \
  }                                                     \
  bool HasOneRef() const override {                     \
    return ref_count_.HasOneRef();                      \
  }                                                     \
  bool HasAtLeastOneRef() const override {              \
    return ref_count_.HasAtLeastOneRef();               \
  }                                                     \
                                                        \
 private:                                               \
  HoneycombRefCount ref_count_

#define IMPLEMENT_REFCOUNTING_DELETE_ON_UIT(ClassName) \
  IMPLEMENT_REFCOUNTING_EX(ClassName, content::BrowserThread::DeleteOnUIThread)

#define IMPLEMENT_REFCOUNTING_DELETE_ON_IOT(ClassName) \
  IMPLEMENT_REFCOUNTING_EX(ClassName, content::BrowserThread::DeleteOnIOThread)

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_THREAD_UTIL_H_
