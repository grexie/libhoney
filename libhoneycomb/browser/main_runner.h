// Copyright 2020 The Honeycomb Authors.
// Portions copyright 2014 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_MAIN_RUNNER_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_MAIN_RUNNER_H_
#pragma once

#include "include/honey_app.h"
#include "libhoneycomb/common/main_runner_delegate.h"
#include "libhoneycomb/common/main_runner_handler.h"

#include "base/functional/callback.h"
#include "content/public/browser/browser_main_runner.h"

namespace base {
class WaitableEvent;
}

namespace content {
class ContentMainRunner;
}  // namespace content

class HoneycombUIThread;

// Manages the main process lifespan and related objects.
class HoneycombMainRunner : public HoneycombMainRunnerHandler {
 public:
  HoneycombMainRunner(bool multi_threaded_message_loop, bool external_message_pump);

  HoneycombMainRunner(const HoneycombMainRunner&) = delete;
  HoneycombMainRunner& operator=(const HoneycombMainRunner&) = delete;

  ~HoneycombMainRunner();

  // Called from HoneycombContext::Initialize.
  bool Initialize(HoneycombSettings* settings,
                  HoneycombRefPtr<HoneycombApp> application,
                  const HoneycombMainArgs& args,
                  void* windows_sandbox_info,
                  bool* initialized,
                  base::OnceClosure context_initialized);

  // Called from HoneycombContext::Shutdown.
  void Shutdown(base::OnceClosure shutdown_on_ui_thread,
                base::OnceClosure finalize_shutdown);

  void RunMessageLoop();
  void QuitMessageLoop();

  // Called from HoneycombExecuteProcess.
  static int RunAsHelperProcess(const HoneycombMainArgs& args,
                                HoneycombRefPtr<HoneycombApp> application,
                                void* windows_sandbox_info);

 private:
  // Called from Initialize().
  int ContentMainInitialize(const HoneycombMainArgs& args,
                            void* windows_sandbox_info,
                            int* no_sandbox);
  bool ContentMainRun(bool* initialized, base::OnceClosure context_initialized);

  // HoneycombMainRunnerHandler methods:
  void PreBrowserMain() override;
  int RunMainProcess(content::MainFunctionParams main_function_params) override;

  // Create the UI thread when running with multi-threaded message loop mode.
  bool CreateUIThread(base::OnceClosure setup_callback);

  // Called on the UI thread after the context is initialized.
  void OnContextInitialized(base::OnceClosure context_initialized);

  // Performs shutdown actions that need to occur on the UI thread before any
  // threads are destroyed.
  void FinishShutdownOnUIThread(base::OnceClosure shutdown_on_ui_thread,
                                base::WaitableEvent* uithread_shutdown_event);

  // Destroys the runtime and related objects.
  void FinalizeShutdown(base::OnceClosure finalize_shutdown);

  const bool multi_threaded_message_loop_;
  const bool external_message_pump_;

  std::unique_ptr<HoneycombMainRunnerDelegate> main_delegate_;
  std::unique_ptr<content::ContentMainRunner> main_runner_;

  std::unique_ptr<content::BrowserMainRunner> browser_runner_;
  std::unique_ptr<HoneycombUIThread> ui_thread_;

  // Used to quit the current base::RunLoop.
  base::OnceClosure quit_callback_;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_MAIN_RUNNER_H_
