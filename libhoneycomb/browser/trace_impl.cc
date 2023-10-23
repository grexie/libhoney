// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "include/honey_trace.h"
#include "libhoneycomb/browser/context.h"
#include "libhoneycomb/browser/thread_util.h"
#include "libhoneycomb/browser/trace_subscriber.h"

#include "base/time/time.h"

bool HoneycombBeginTracing(const HoneycombString& categories,
                     HoneycombRefPtr<HoneycombCompletionCallback> callback) {
  if (!CONTEXT_STATE_VALID()) {
    DCHECK(false) << "context not valid";
    return false;
  }

  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    DCHECK(false) << "called on invalid thread";
    return false;
  }

  HoneycombTraceSubscriber* subscriber = HoneycombContext::Get()->GetTraceSubscriber();
  if (!subscriber) {
    return false;
  }

  return subscriber->BeginTracing(categories, callback);
}

bool HoneycombEndTracing(const HoneycombString& tracing_file,
                   HoneycombRefPtr<HoneycombEndTracingCallback> callback) {
  if (!CONTEXT_STATE_VALID()) {
    DCHECK(false) << "context not valid";
    return false;
  }

  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    DCHECK(false) << "called on invalid thread";
    return false;
  }

  HoneycombTraceSubscriber* subscriber = HoneycombContext::Get()->GetTraceSubscriber();
  if (!subscriber) {
    return false;
  }

  return subscriber->EndTracing(base::FilePath(tracing_file), callback);
}

int64_t HoneycombNowFromSystemTraceTime() {
  return base::TimeTicks::Now().ToInternalValue();
}
