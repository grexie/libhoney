// Copyright (c) 2013 Marshall A. Greenblatt. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the name Chromium Embedded
// Framework nor the names of its contributors may be used to endorse
// or promote products derived from this software without specific prior
// written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ---------------------------------------------------------------------------
//
// The contents of this file must follow a specific format in order to
// support the Honeycomb translator tool. See the translator.README.txt file in the
// tools directory for more information.
//

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_TASK_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_TASK_H_

#include "include/honey_base.h"

typedef honey_thread_id_t HoneycombThreadId;

///
/// Implement this interface for asynchronous task execution. If the task is
/// posted successfully and if the associated message loop is still running then
/// the Execute() method will be called on the target thread. If the task fails
/// to post then the task object may be destroyed on the source thread instead
/// of the target thread. For this reason be cautious when performing work in
/// the task object destructor.
///
/*--honey(source=client,no_debugct_check)--*/
class HoneycombTask : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Method that will be executed on the target thread.
  ///
  /*--honey()--*/
  virtual void Execute() = 0;
};

///
/// Class that asynchronously executes tasks on the associated thread. It is
/// safe to call the methods of this class on any thread.
///
/// Honeycomb maintains multiple internal threads that are used for handling different
/// types of tasks in different processes. The honey_thread_id_t definitions in
/// honey_types.h list the common Honeycomb threads. Task runners are also available for
/// other Honeycomb threads as appropriate (for example, V8 WebWorker threads).
///
/*--honey(source=library)--*/
class HoneycombTaskRunner : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Returns the task runner for the current thread. Only Honeycomb threads will have
  /// task runners. An empty reference will be returned if this method is called
  /// on an invalid thread.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombTaskRunner> GetForCurrentThread();

  ///
  /// Returns the task runner for the specified Honeycomb thread.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombTaskRunner> GetForThread(HoneycombThreadId threadId);

  ///
  /// Returns true if this object is pointing to the same task runner as |that|
  /// object.
  ///
  /*--honey()--*/
  virtual bool IsSame(HoneycombRefPtr<HoneycombTaskRunner> that) = 0;

  ///
  /// Returns true if this task runner belongs to the current thread.
  ///
  /*--honey()--*/
  virtual bool BelongsToCurrentThread() = 0;

  ///
  /// Returns true if this task runner is for the specified Honeycomb thread.
  ///
  /*--honey()--*/
  virtual bool BelongsToThread(HoneycombThreadId threadId) = 0;

  ///
  /// Post a task for execution on the thread associated with this task runner.
  /// Execution will occur asynchronously.
  ///
  /*--honey()--*/
  virtual bool PostTask(HoneycombRefPtr<HoneycombTask> task) = 0;

  ///
  /// Post a task for delayed execution on the thread associated with this task
  /// runner. Execution will occur asynchronously. Delayed tasks are not
  /// supported on V8 WebWorker threads and will be executed without the
  /// specified delay.
  ///
  /*--honey()--*/
  virtual bool PostDelayedTask(HoneycombRefPtr<HoneycombTask> task, int64_t delay_ms) = 0;
};

///
/// Returns true if called on the specified thread. Equivalent to using
/// HoneycombTaskRunner::GetForThread(threadId)->BelongsToCurrentThread().
///
/*--honey()--*/
bool HoneycombCurrentlyOn(HoneycombThreadId threadId);

///
/// Post a task for execution on the specified thread. Equivalent to
/// using HoneycombTaskRunner::GetForThread(threadId)->PostTask(task).
///
/*--honey()--*/
bool HoneycombPostTask(HoneycombThreadId threadId, HoneycombRefPtr<HoneycombTask> task);

///
/// Post a task for delayed execution on the specified thread. Equivalent to
/// using HoneycombTaskRunner::GetForThread(threadId)->PostDelayedTask(task,
/// delay_ms).
///
/*--honey()--*/
bool HoneycombPostDelayedTask(HoneycombThreadId threadId,
                        HoneycombRefPtr<HoneycombTask> task,
                        int64_t delay_ms);

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_TASK_H_
