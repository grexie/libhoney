// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_COMMON_HONEYCOMB_CRASH_REPORT_UPLOAD_THREAD_H_
#define HONEYCOMB_LIBHONEY_COMMON_HONEYCOMB_CRASH_REPORT_UPLOAD_THREAD_H_

#include "third_party/crashpad/crashpad/handler/crash_report_upload_thread.h"

class HoneycombCrashReportUploadThread : public crashpad::CrashReportUploadThread {
 public:
  HoneycombCrashReportUploadThread(crashpad::CrashReportDatabase* database,
                             const std::string& url,
                             const Options& options,
                             ProcessPendingReportsObservationCallback callback,
                             int max_uploads);

  HoneycombCrashReportUploadThread(const HoneycombCrashReportUploadThread&) = delete;
  HoneycombCrashReportUploadThread& operator=(const HoneycombCrashReportUploadThread&) =
      delete;

  ~HoneycombCrashReportUploadThread();

 private:
  void ProcessPendingReports() override;
  void ProcessPendingReport(
      const crashpad::CrashReportDatabase::Report& report) override;
  ParameterMap FilterParameters(const ParameterMap& parameters) override;

  bool UploadsEnabled() const;

  bool MaxUploadsEnabled() const;
  bool MaxUploadsExceeded() const;

  bool BackoffPending() const;
  void IncreaseBackoff();
  void ResetBackoff();

  int max_uploads_;

  // Track the number of uploads that have completed within the last 24 hours.
  // Only used when RateLimitEnabled() is true. Value is reset each time
  // ProcessPendingReports() is called.
  int recent_upload_ct_ = 0;
};

#endif  // HONEYCOMB_LIBHONEY_COMMON_HONEYCOMB_CRASH_REPORT_UPLOAD_THREAD_H_
