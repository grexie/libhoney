// Copyright 2020 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_COMMON_MAIN_RUNNER_HANDLER_H_
#define HONEYCOMB_LIBHONEY_COMMON_MAIN_RUNNER_HANDLER_H_
#pragma once

namespace content {
struct MainFunctionParams;
}

// Handles running of the main process.
class HoneycombMainRunnerHandler {
 public:
  virtual void PreBrowserMain() = 0;
  virtual int RunMainProcess(
      content::MainFunctionParams main_function_params) = 0;

 protected:
  virtual ~HoneycombMainRunnerHandler() {}
};

#endif  // HONEYCOMB_LIBHONEY_COMMON_MAIN_RUNNER_HANDLER_H_
