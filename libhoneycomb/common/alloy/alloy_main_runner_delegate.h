// Copyright 2020 The Honeycomb Authors.
// Portions copyright 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_COMMON_ALLOY_ALLOY_MAIN_RUNNER_DELEGATE_
#define HONEYCOMB_LIBHONEYCOMB_COMMON_ALLOY_ALLOY_MAIN_RUNNER_DELEGATE_

#include <memory>

#include "include/honey_base.h"
#include "libhoneycomb/common/main_runner_delegate.h"
#include "libhoneycomb/common/main_runner_handler.h"

class AlloyMainDelegate;

class AlloyMainRunnerDelegate : public HoneycombMainRunnerDelegate {
 public:
  // |runner| and |settings| will be non-nullptr for the main process only, and
  // will outlive this object.
  AlloyMainRunnerDelegate(HoneycombMainRunnerHandler* runner,
                          HoneycombSettings* settings,
                          HoneycombRefPtr<HoneycombApp> application);

  AlloyMainRunnerDelegate(const AlloyMainRunnerDelegate&) = delete;
  AlloyMainRunnerDelegate& operator=(const AlloyMainRunnerDelegate&) = delete;

  ~AlloyMainRunnerDelegate() override;

 protected:
  // HoneycombMainRunnerDelegate overrides.
  content::ContentMainDelegate* GetContentMainDelegate() override;
  void BeforeMainThreadInitialize(const HoneycombMainArgs& args) override;
  void BeforeMainThreadRun(bool multi_threaded_message_loop) override;
  void AfterUIThreadInitialize() override;
  void AfterUIThreadShutdown() override;
  void BeforeMainThreadShutdown() override;
  void AfterMainThreadShutdown() override;

 private:
  std::unique_ptr<AlloyMainDelegate> main_delegate_;

  HoneycombMainRunnerHandler* const runner_;
  HoneycombSettings* const settings_;
  HoneycombRefPtr<HoneycombApp> application_;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_COMMON_ALLOY_ALLOY_MAIN_RUNNER_DELEGATE_
