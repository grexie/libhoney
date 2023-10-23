// Copyright (c) 2012 Marshall A. Greenblatt. Portons copyright (c) 2012
// Google Inc. All rights reserved.
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

// See honey_trace_event.h for trace macros and additonal documentation.

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_TRACE_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_TRACE_H_
#pragma once

#include "include/honey_base.h"
#include "include/honey_callback.h"

///
/// Implement this interface to receive notification when tracing has completed.
/// The methods of this class will be called on the browser process UI thread.
///
/*--honey(source=client)--*/
class HoneycombEndTracingCallback : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Called after all processes have sent their trace data. |tracing_file| is
  /// the path at which tracing data was written. The client is responsible for
  /// deleting |tracing_file|.
  ///
  /*--honey()--*/
  virtual void OnEndTracingComplete(const HoneycombString& tracing_file) = 0;
};

///
/// Start tracing events on all processes. Tracing is initialized asynchronously
/// and |callback| will be executed on the UI thread after initialization is
/// complete.
///
/// If HoneycombBeginTracing was called previously, or if a HoneycombEndTracingAsync call is
/// pending, HoneycombBeginTracing will fail and return false.
///
/// |categories| is a comma-delimited list of category wildcards. A category can
/// have an optional '-' prefix to make it an excluded category. Having both
/// included and excluded categories in the same list is not supported.
///
/// Examples:
/// - "test_MyTest*"
/// - "test_MyTest*,test_OtherStuff"
/// - "-excluded_category1,-excluded_category2"
///
/// This function must be called on the browser process UI thread.
///
/*--honey(optional_param=categories,optional_param=callback)--*/
bool HoneycombBeginTracing(const HoneycombString& categories,
                     HoneycombRefPtr<HoneycombCompletionCallback> callback);

///
/// Stop tracing events on all processes.
///
/// This function will fail and return false if a previous call to
/// HoneycombEndTracingAsync is already pending or if HoneycombBeginTracing was not called.
///
/// |tracing_file| is the path at which tracing data will be written and
/// |callback| is the callback that will be executed once all processes have
/// sent their trace data. If |tracing_file| is empty a new temporary file path
/// will be used. If |callback| is empty no trace data will be written.
///
/// This function must be called on the browser process UI thread.
///
/*--honey(optional_param=tracing_file,optional_param=callback)--*/
bool HoneycombEndTracing(const HoneycombString& tracing_file,
                   HoneycombRefPtr<HoneycombEndTracingCallback> callback);

///
/// Returns the current system trace time or, if none is defined, the current
/// high-res time. Can be used by clients to synchronize with the time
/// information in trace events.
///
/*--honey()--*/
int64_t HoneycombNowFromSystemTraceTime();

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_TRACE_H_
