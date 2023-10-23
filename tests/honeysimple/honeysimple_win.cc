// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <windows.h>

#include "include/honey_command_line.h"
#include "include/honey_sandbox_win.h"
#include "tests/honeysimple/simple_app.h"

// When generating projects with CMake the HONEYCOMB_USE_SANDBOX value will be defined
// automatically if using the required compiler version. Pass -DUSE_SANDBOX=OFF
// to the CMake command-line to disable use of the sandbox.
// Uncomment this line to manually enable sandbox support.
// #define HONEYCOMB_USE_SANDBOX 1

#if defined(HONEYCOMB_USE_SANDBOX)
// The honey_sandbox.lib static library may not link successfully with all VS
// versions.
#pragma comment(lib, "honey_sandbox.lib")
#endif

// Entry point function for all processes.
int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPTSTR lpCmdLine,
                      int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  int exit_code;

#if defined(ARCH_CPU_32_BITS)
  // Run the main thread on 32-bit Windows using a fiber with the preferred 4MiB
  // stack size. This function must be called at the top of the executable entry
  // point function (`main()` or `wWinMain()`). It is used in combination with
  // the initial stack size of 0.5MiB configured via the `/STACK:0x80000` linker
  // flag on executable targets. This saves significant memory on threads (like
  // those in the Windows thread pool, and others) whose stack size can only be
  // controlled via the linker flag.
  exit_code = HoneycombRunWinMainWithPreferredStackSize(wWinMain, hInstance,
                                                  lpCmdLine, nCmdShow);
  if (exit_code >= 0) {
    // The fiber has completed so return here.
    return exit_code;
  }
#endif

  void* sandbox_info = nullptr;

#if defined(HONEYCOMB_USE_SANDBOX)
  // Manage the life span of the sandbox information object. This is necessary
  // for sandbox support on Windows. See honey_sandbox_win.h for complete details.
  HoneycombScopedSandboxInfo scoped_sandbox;
  sandbox_info = scoped_sandbox.sandbox_info();
#endif

  // Provide Honeycomb with command-line arguments.
  HoneycombMainArgs main_args(hInstance);

  // Honeycomb applications have multiple sub-processes (render, GPU, etc) that share
  // the same executable. This function checks the command-line and, if this is
  // a sub-process, executes the appropriate logic.
  exit_code = HoneycombExecuteProcess(main_args, nullptr, sandbox_info);
  if (exit_code >= 0) {
    // The sub-process has completed so return here.
    return exit_code;
  }

  // Parse command-line arguments for use in this method.
  HoneycombRefPtr<HoneycombCommandLine> command_line = HoneycombCommandLine::CreateCommandLine();
  command_line->InitFromString(::GetCommandLineW());

  // Specify Honeycomb global settings here.
  HoneycombSettings settings;

  if (command_line->HasSwitch("enable-chrome-runtime")) {
    // Enable experimental Chrome runtime. See issue #2969 for details.
    settings.chrome_runtime = true;
  }

#if !defined(HONEYCOMB_USE_SANDBOX)
  settings.no_sandbox = true;
#endif

  // SimpleApp implements application-level callbacks for the browser process.
  // It will create the first browser instance in OnContextInitialized() after
  // Honeycomb has initialized.
  HoneycombRefPtr<SimpleApp> app(new SimpleApp);

  // Initialize Honeycomb.
  HoneycombInitialize(main_args, settings, app.get(), sandbox_info);

  // Run the Honeycomb message loop. This will block until HoneycombQuitMessageLoop() is
  // called.
  HoneycombRunMessageLoop();

  // Shut down Honeycomb.
  HoneycombShutdown();

  return 0;
}
