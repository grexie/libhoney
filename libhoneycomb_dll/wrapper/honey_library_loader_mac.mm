// Copyright (c) 2018 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/wrapper/honey_library_loader.h"

#include <libgen.h>
#include <mach-o/dyld.h>
#include <stdio.h>

#include <memory>
#include <sstream>

namespace {

const char kFrameworkPath[] =
    "Honeycomb.framework/Honeycomb";
const char kPathFromHelperExe[] = "../../..";
const char kPathFromMainExe[] = "../Frameworks";

std::string GetFrameworkPath(bool helper) {
  uint32_t exec_path_size = 0;
  int rv = _NSGetExecutablePath(NULL, &exec_path_size);
  if (rv != -1) {
    return std::string();
  }

  std::unique_ptr<char[]> exec_path(new char[exec_path_size]);
  rv = _NSGetExecutablePath(exec_path.get(), &exec_path_size);
  if (rv != 0) {
    return std::string();
  }

  // Get the directory path of the executable.
  const char* parent_dir = dirname(exec_path.get());
  if (!parent_dir) {
    return std::string();
  }

  // Append the relative path to the framework.
  std::stringstream ss;
  ss << parent_dir << "/" << (helper ? kPathFromHelperExe : kPathFromMainExe)
     << "/" << kFrameworkPath;
  return ss.str();
}

}  // namespace

HoneycombScopedLibraryLoader::HoneycombScopedLibraryLoader() : loaded_(false) {}

bool HoneycombScopedLibraryLoader::Load(bool helper) {
  if (loaded_) {
    return false;
  }

  const std::string& framework_path = GetFrameworkPath(helper);
  if (framework_path.empty()) {
    fprintf(stderr, "App does not have the expected bundle structure.\n");
    return false;
  }

  // Load the Honeycomb framework library.
  if (!honey_load_library(framework_path.c_str())) {
    fprintf(stderr, "Failed to load the Honeycomb framework.\n");
    return false;
  }

  loaded_ = true;
  return true;
}

HoneycombScopedLibraryLoader::~HoneycombScopedLibraryLoader() {
  if (loaded_) {
    // Unload the Honeycomb framework library.
    honey_unload_library();
  }
}
