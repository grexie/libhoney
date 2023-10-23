// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_TRACE_SUBSCRIBER_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_TRACE_SUBSCRIBER_H_
#pragma once

#include "include/honey_trace.h"

#include "base/files/file_path.h"
#include "base/memory/ref_counted_memory.h"
#include "base/memory/weak_ptr.h"

// May only be accessed on the browser process UI thread.
class HoneycombTraceSubscriber {
 public:
  HoneycombTraceSubscriber();
  virtual ~HoneycombTraceSubscriber();

  bool BeginTracing(const std::string& categories,
                    HoneycombRefPtr<HoneycombCompletionCallback> callback);
  bool EndTracing(const base::FilePath& tracing_file,
                  HoneycombRefPtr<HoneycombEndTracingCallback> callback);

 private:
  void ContinueEndTracing(HoneycombRefPtr<HoneycombEndTracingCallback> callback,
                          const base::FilePath& tracing_file);
  void OnTracingFileResult(HoneycombRefPtr<HoneycombEndTracingCallback> callback,
                           const base::FilePath& tracing_file);

  bool collecting_trace_data_;
  base::WeakPtrFactory<HoneycombTraceSubscriber> weak_factory_;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_TRACE_SUBSCRIBER_H_
