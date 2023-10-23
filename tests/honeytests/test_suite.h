// Copyright (C) 2023 Grexie. Postions copyright
// 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBTESTS_TEST_SUITE_H_
#define HONEYCOMB_TESTS_HONEYCOMBTESTS_TEST_SUITE_H_
#pragma once

#include <string>
#include <vector>

#include "include/honey_command_line.h"
#include "include/wrapper/honey_helpers.h"

// A single instance of this object will be created by main() in
// run_all_unittests.cc.
class HoneycombTestSuite {
 public:
  HoneycombTestSuite(int argc, char** argv);
  ~HoneycombTestSuite();

  static HoneycombTestSuite* GetInstance();

  void InitMainProcess();
  int Run();

  void GetSettings(HoneycombSettings& settings) const;

  // Register a temp directory that should be deleted on shutdown.
  void RegisterTempDirectory(const HoneycombString& directory);

  // Called after shutdown to delete any registered temp directories.
  void DeleteTempDirectories();

  HoneycombRefPtr<HoneycombCommandLine> command_line() const { return command_line_; }
  HoneycombString root_cache_path() const { return root_cache_path_; }

  // The return value from Run().
  int retval() const { return retval_; }

 private:
  void PreInitialize();
  void Initialize();
  void Shutdown();

  int argc_;
  HoneycombScopedArgArray argv_;
  HoneycombRefPtr<HoneycombCommandLine> command_line_;

  std::vector<HoneycombString> temp_directories_;
  base::Lock temp_directories_lock_;

  HoneycombString root_cache_path_;

  int retval_;
};

#define HONEYCOMB_SETTINGS_ACCEPT_LANGUAGE "en-GB"

#endif  // HONEYCOMB_TESTS_HONEYCOMBTESTS_TEST_SUITE_H_
