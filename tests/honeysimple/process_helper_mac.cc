// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "include/honey_app.h"
#include "include/wrapper/honey_library_loader.h"

// When generating projects with CMake the HONEYCOMB_USE_SANDBOX value will be defined
// automatically. Pass -DUSE_SANDBOX=OFF to the CMake command-line to disable
// use of the sandbox.
#if defined(HONEYCOMB_USE_SANDBOX)
#include "include/honey_sandbox_mac.h"
#endif

// Entry point function for sub-processes.
int main(int argc, char* argv[]) {
#if defined(HONEYCOMB_USE_SANDBOX)
  // Initialize the macOS sandbox for this helper process.
  HoneycombScopedSandboxContext sandbox_context;
  if (!sandbox_context.Initialize(argc, argv)) {
    return 1;
  }
#endif

  // Load the Honeycomb framework library at runtime instead of linking directly
  // as required by the macOS sandbox implementation.
  HoneycombScopedLibraryLoader library_loader;
  if (!library_loader.LoadInHelper()) {
    return 1;
  }

  // Provide Honeycomb with command-line arguments.
  HoneycombMainArgs main_args(argc, argv);

  // Execute the sub-process.
  return HoneycombExecuteProcess(main_args, nullptr, nullptr);
}
