// Copyright 2020 The Honeycomb Authors.
// Portions copyright 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_COMMON_CHROME_CHROME_MAIN_DELEGATE_HONEYCOMB_
#define HONEYCOMB_LIBHONEYCOMB_COMMON_CHROME_CHROME_MAIN_DELEGATE_HONEYCOMB_

#include <memory>

#include "include/honey_app.h"
#include "libhoneycomb/common/app_manager.h"
#include "libhoneycomb/common/chrome/chrome_content_client_honey.h"
#include "libhoneycomb/common/main_runner_handler.h"
#include "libhoneycomb/common/task_runner_manager.h"

#include "chrome/app/chrome_main_delegate.h"

class ChromeContentBrowserClientHoneycomb;
class ChromeContentRendererClientHoneycomb;

// Honeycomb override of ChromeMainDelegate
class ChromeMainDelegateHoneycomb : public ChromeMainDelegate,
                              public HoneycombAppManager,
                              public HoneycombTaskRunnerManager {
 public:
  // |runner| will be non-nullptr for the main process only, and will outlive
  // this object.
  ChromeMainDelegateHoneycomb(HoneycombMainRunnerHandler* runner,
                        HoneycombSettings* settings,
                        HoneycombRefPtr<HoneycombApp> application);

  ChromeMainDelegateHoneycomb(const ChromeMainDelegateHoneycomb&) = delete;
  ChromeMainDelegateHoneycomb& operator=(const ChromeMainDelegateHoneycomb&) = delete;

  ~ChromeMainDelegateHoneycomb() override;

  // ChromeMainDelegate overrides.
  absl::optional<int> BasicStartupComplete() override;
  void PreSandboxStartup() override;
  absl::optional<int> PreBrowserMain() override;
  absl::optional<int> PostEarlyInitialization(InvokedIn invoked_in) override;
  absl::variant<int, content::MainFunctionParams> RunProcess(
      const std::string& process_type,
      content::MainFunctionParams main_function_params) override;
#if BUILDFLAG(IS_LINUX)
  void ZygoteForked() override;
#endif
  content::ContentClient* CreateContentClient() override;
  content::ContentBrowserClient* CreateContentBrowserClient() override;
  content::ContentRendererClient* CreateContentRendererClient() override;

 protected:
  // HoneycombAppManager overrides.
  HoneycombRefPtr<HoneycombApp> GetApplication() override { return application_; }
  content::ContentClient* GetContentClient() override {
    return &chrome_content_client_honey_;
  }
  HoneycombRefPtr<HoneycombRequestContext> GetGlobalRequestContext() override;
  HoneycombBrowserContext* CreateNewBrowserContext(
      const HoneycombRequestContextSettings& settings,
      base::OnceClosure initialized_cb) override;

  // HoneycombTaskRunnerManager overrides.
  scoped_refptr<base::SingleThreadTaskRunner> GetBackgroundTaskRunner()
      override;
  scoped_refptr<base::SingleThreadTaskRunner> GetUserVisibleTaskRunner()
      override;
  scoped_refptr<base::SingleThreadTaskRunner> GetUserBlockingTaskRunner()
      override;
  scoped_refptr<base::SingleThreadTaskRunner> GetRenderTaskRunner() override;
  scoped_refptr<base::SingleThreadTaskRunner> GetWebWorkerTaskRunner() override;

 private:
  ChromeContentBrowserClientHoneycomb* content_browser_client() const;
  ChromeContentRendererClientHoneycomb* content_renderer_client() const;

  HoneycombMainRunnerHandler* const runner_;
  HoneycombSettings* const settings_;
  HoneycombRefPtr<HoneycombApp> application_;

  // We use this instead of ChromeMainDelegate::chrome_content_client_.
  ChromeContentClientHoneycomb chrome_content_client_honey_;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_COMMON_CHROME_CHROME_MAIN_DELEGATE_HONEYCOMB_
