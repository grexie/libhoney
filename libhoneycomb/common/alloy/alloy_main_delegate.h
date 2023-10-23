// Copyright (c) 2011 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_COMMON_ALLOY_ALLOY_MAIN_DELEGATE_H_
#define HONEYCOMB_LIBHONEYCOMB_COMMON_ALLOY_ALLOY_MAIN_DELEGATE_H_
#pragma once

#include <string>

#include "include/honey_app.h"
#include "libhoneycomb/common/alloy/alloy_content_client.h"
#include "libhoneycomb/common/app_manager.h"
#include "libhoneycomb/common/main_runner_handler.h"
#include "libhoneycomb/common/resource_bundle_delegate.h"
#include "libhoneycomb/common/task_runner_manager.h"

#include "content/public/app/content_main_delegate.h"

#if BUILDFLAG(IS_WIN)
#include "components/spellcheck/common/spellcheck_features.h"
#endif

namespace base {
class CommandLine;
}

class AlloyContentBrowserClient;
class AlloyContentRendererClient;
class ChromeContentUtilityClient;

// Manages state specific to the Honeycomb runtime.
class AlloyMainDelegate : public content::ContentMainDelegate,
                          public HoneycombAppManager,
                          public HoneycombTaskRunnerManager {
 public:
  // |runner| and |settings| will be non-nullptr for the main process only,
  // and will outlive this object.
  AlloyMainDelegate(HoneycombMainRunnerHandler* runner,
                    HoneycombSettings* settings,
                    HoneycombRefPtr<HoneycombApp> application);

  AlloyMainDelegate(const AlloyMainDelegate&) = delete;
  AlloyMainDelegate& operator=(const AlloyMainDelegate&) = delete;

  ~AlloyMainDelegate() override;

  // content::ContentMainDelegate overrides.
  absl::optional<int> PreBrowserMain() override;
  absl::optional<int> BasicStartupComplete() override;
  void PreSandboxStartup() override;
  absl::variant<int, content::MainFunctionParams> RunProcess(
      const std::string& process_type,
      content::MainFunctionParams main_function_params) override;
  void ProcessExiting(const std::string& process_type) override;
#if BUILDFLAG(IS_LINUX)
  void ZygoteForked() override;
#endif
  content::ContentBrowserClient* CreateContentBrowserClient() override;
  content::ContentRendererClient* CreateContentRendererClient() override;
  content::ContentUtilityClient* CreateContentUtilityClient() override;

 protected:
  // HoneycombAppManager overrides.
  HoneycombRefPtr<HoneycombApp> GetApplication() override { return application_; }
  content::ContentClient* GetContentClient() override {
    return &content_client_;
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
  void InitializeResourceBundle();

  HoneycombMainRunnerHandler* const runner_;
  HoneycombSettings* const settings_;
  HoneycombRefPtr<HoneycombApp> application_;

  std::unique_ptr<AlloyContentBrowserClient> browser_client_;
  std::unique_ptr<AlloyContentRendererClient> renderer_client_;
  std::unique_ptr<ChromeContentUtilityClient> utility_client_;
  AlloyContentClient content_client_;

  HoneycombResourceBundleDelegate resource_bundle_delegate_;

#if BUILDFLAG(IS_WIN)
  // TODO: Add support for windows spellcheck service (see issue #3055).
  spellcheck::ScopedDisableBrowserSpellCheckerForTesting
      disable_browser_spellchecker_;
#endif
};

#endif  // HONEYCOMB_LIBHONEYCOMB_COMMON_ALLOY_ALLOY_MAIN_DELEGATE_H_
