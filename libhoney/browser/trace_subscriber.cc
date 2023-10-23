// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "libhoney/browser/trace_subscriber.h"
#include "include/honey_trace.h"
#include "libhoney/browser/thread_util.h"

#include "base/files/file_util.h"
#include "base/task/single_thread_task_runner.h"
#include "base/trace_event/trace_event.h"
#include "content/public/browser/tracing_controller.h"

namespace {

// Create the temporary file and then execute |callback| on the thread
// represented by |message_loop_proxy|.
void CreateTemporaryFileOnBackgroundThread(
    scoped_refptr<base::SequencedTaskRunner> message_loop_proxy,
    base::OnceCallback<void(const base::FilePath&)> callback) {
  HONEYCOMB_REQUIRE_BLOCKING();
  base::FilePath file_path;
  if (!base::CreateTemporaryFile(&file_path)) {
    LOG(ERROR) << "Failed to create temporary file.";
  }
  message_loop_proxy->PostTask(FROM_HERE,
                               base::BindOnce(std::move(callback), file_path));
}

// Release the wrapped callback object after completion.
class HoneycombCompletionCallbackWrapper : public HoneycombCompletionCallback {
 public:
  explicit HoneycombCompletionCallbackWrapper(
      HoneycombRefPtr<HoneycombCompletionCallback> callback)
      : callback_(callback) {}

  HoneycombCompletionCallbackWrapper(const HoneycombCompletionCallbackWrapper&) = delete;
  HoneycombCompletionCallbackWrapper& operator=(const HoneycombCompletionCallbackWrapper&) =
      delete;

  void OnComplete() override {
    if (callback_) {
      callback_->OnComplete();
      callback_ = nullptr;
    }
  }

 private:
  HoneycombRefPtr<HoneycombCompletionCallback> callback_;

  IMPLEMENT_REFCOUNTING(HoneycombCompletionCallbackWrapper);
};

}  // namespace

using content::TracingController;

HoneycombTraceSubscriber::HoneycombTraceSubscriber()
    : collecting_trace_data_(false), weak_factory_(this) {
  HONEYCOMB_REQUIRE_UIT();
}

HoneycombTraceSubscriber::~HoneycombTraceSubscriber() {
  HONEYCOMB_REQUIRE_UIT();
  if (collecting_trace_data_) {
    TracingController::GetInstance()->StopTracing(nullptr);
  }
}

bool HoneycombTraceSubscriber::BeginTracing(
    const std::string& categories,
    HoneycombRefPtr<HoneycombCompletionCallback> callback) {
  HONEYCOMB_REQUIRE_UIT();

  if (collecting_trace_data_) {
    return false;
  }

  collecting_trace_data_ = true;

  TracingController::StartTracingDoneCallback done_callback;
  if (callback.get()) {
    // Work around a bug introduced in http://crbug.com/542390#c22 that keeps a
    // reference to |done_callback| after execution.
    callback = new HoneycombCompletionCallbackWrapper(callback);
    done_callback =
        base::BindOnce(&HoneycombCompletionCallback::OnComplete, callback.get());
  }

  TracingController::GetInstance()->StartTracing(
      base::trace_event::TraceConfig(categories, ""), std::move(done_callback));
  return true;
}

bool HoneycombTraceSubscriber::EndTracing(const base::FilePath& tracing_file,
                                    HoneycombRefPtr<HoneycombEndTracingCallback> callback) {
  HONEYCOMB_REQUIRE_UIT();

  if (!collecting_trace_data_) {
    return false;
  }

  if (!callback.get()) {
    // Discard the trace data.
    collecting_trace_data_ = false;
    TracingController::GetInstance()->StopTracing(nullptr);
    return true;
  }

  if (tracing_file.empty()) {
    // Create a new temporary file path on the FILE thread, then continue.
    HONEYCOMB_POST_USER_VISIBLE_TASK(
        base::BindOnce(CreateTemporaryFileOnBackgroundThread,
                       base::SingleThreadTaskRunner::GetCurrentDefault(),
                       base::BindOnce(&HoneycombTraceSubscriber::ContinueEndTracing,
                                      weak_factory_.GetWeakPtr(), callback)));
    return true;
  }

  auto result_callback =
      base::BindOnce(&HoneycombTraceSubscriber::OnTracingFileResult,
                     weak_factory_.GetWeakPtr(), callback, tracing_file);

  TracingController::GetInstance()->StopTracing(
      TracingController::CreateFileEndpoint(tracing_file,
                                            std::move(result_callback),
                                            base::TaskPriority::USER_VISIBLE));
  return true;
}

void HoneycombTraceSubscriber::ContinueEndTracing(
    HoneycombRefPtr<HoneycombEndTracingCallback> callback,
    const base::FilePath& tracing_file) {
  HONEYCOMB_REQUIRE_UIT();
  if (!tracing_file.empty()) {
    EndTracing(tracing_file, callback);
  }
}

void HoneycombTraceSubscriber::OnTracingFileResult(
    HoneycombRefPtr<HoneycombEndTracingCallback> callback,
    const base::FilePath& tracing_file) {
  HONEYCOMB_REQUIRE_UIT();

  collecting_trace_data_ = false;

  callback->OnEndTracingComplete(tracing_file.value());
}
