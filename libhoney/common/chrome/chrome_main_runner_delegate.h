// Copyright 2020 The Honeycomb Authors.
// Portions copyright 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_COMMON_CHROME_CHROME_MAIN_RUNNER_DELEGATE_HONEYCOMB_
#define HONEYCOMB_LIBHONEY_COMMON_CHROME_CHROME_MAIN_RUNNER_DELEGATE_HONEYCOMB_

#include <memory>

#include "include/honey_app.h"
#include "libhoney/common/main_runner_delegate.h"
#include "libhoney/common/main_runner_handler.h"

class ChromeMainDelegateHoneycomb;
class MainThreadStackSamplingProfiler;
class ScopedKeepAlive;

class ChromeMainRunnerDelegate : public HoneycombMainRunnerDelegate {
 public:
  // |runner| will be non-nullptr for the main process only, and will outlive
  // this object.
  ChromeMainRunnerDelegate(HoneycombMainRunnerHandler* runner,
                           HoneycombSettings* settings,
                           HoneycombRefPtr<HoneycombApp> application);

  ChromeMainRunnerDelegate(const ChromeMainRunnerDelegate&) = delete;
  ChromeMainRunnerDelegate& operator=(const ChromeMainRunnerDelegate&) = delete;

  ~ChromeMainRunnerDelegate() override;

 protected:
  // HoneycombMainRunnerDelegate overrides.
  content::ContentMainDelegate* GetContentMainDelegate() override;
  void BeforeMainThreadInitialize(const HoneycombMainArgs& args) override;
  void BeforeMainThreadRun(bool multi_threaded_message_loop) override;
  void BeforeMainMessageLoopRun(base::RunLoop* run_loop) override;
  bool HandleMainMessageLoopQuit() override;
  void BeforeUIThreadInitialize() override;
  void AfterUIThreadShutdown() override;
  void BeforeExecuteProcess(const HoneycombMainArgs& args) override;
  void AfterExecuteProcess() override;

 private:
  std::unique_ptr<ChromeMainDelegateHoneycomb> main_delegate_;

  std::unique_ptr<MainThreadStackSamplingProfiler> sampling_profiler_;
  std::unique_ptr<ScopedKeepAlive> keep_alive_;

  HoneycombMainRunnerHandler* const runner_;
  HoneycombSettings* const settings_;
  HoneycombRefPtr<HoneycombApp> application_;
};

#endif  // HONEYCOMB_LIBHONEY_COMMON_CHROME_CHROME_MAIN_RUNNER_DELEGATE_HONEYCOMB_
