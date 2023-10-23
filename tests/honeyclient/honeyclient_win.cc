// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <windows.h>

#include <memory>

#include "include/honey_command_line.h"
#include "include/honey_sandbox_win.h"
#include "tests/honeyclient/browser/main_context_impl.h"
#include "tests/honeyclient/browser/main_message_loop_multithreaded_win.h"
#include "tests/honeyclient/browser/root_window_manager.h"
#include "tests/honeyclient/browser/test_runner.h"
#include "tests/shared/browser/client_app_browser.h"
#include "tests/shared/browser/main_message_loop_external_pump.h"
#include "tests/shared/browser/main_message_loop_std.h"
#include "tests/shared/common/client_app_other.h"
#include "tests/shared/common/client_switches.h"
#include "tests/shared/renderer/client_app_renderer.h"

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

namespace client {
namespace {

int RunMain(HINSTANCE hInstance, int nCmdShow) {
  HoneycombMainArgs main_args(hInstance);

  void* sandbox_info = nullptr;

#if defined(HONEYCOMB_USE_SANDBOX)
  // Manage the life span of the sandbox information object. This is necessary
  // for sandbox support on Windows. See honey_sandbox_win.h for complete details.
  HoneycombScopedSandboxInfo scoped_sandbox;
  sandbox_info = scoped_sandbox.sandbox_info();
#endif

  // Parse command-line arguments.
  HoneycombRefPtr<HoneycombCommandLine> command_line = HoneycombCommandLine::CreateCommandLine();
  command_line->InitFromString(::GetCommandLineW());

  // Create a ClientApp of the correct type.
  HoneycombRefPtr<HoneycombApp> app;
  ClientApp::ProcessType process_type = ClientApp::GetProcessType(command_line);
  if (process_type == ClientApp::BrowserProcess) {
    app = new ClientAppBrowser();
  } else if (process_type == ClientApp::RendererProcess) {
    app = new ClientAppRenderer();
  } else if (process_type == ClientApp::OtherProcess) {
    app = new ClientAppOther();
  }

  // Execute the secondary process, if any.
  int exit_code = HoneycombExecuteProcess(main_args, app, sandbox_info);
  if (exit_code >= 0) {
    return exit_code;
  }

  // Create the main context object.
  auto context = std::make_unique<MainContextImpl>(command_line, true);

  HoneycombSettings settings;

#if !defined(HONEYCOMB_USE_SANDBOX)
  settings.no_sandbox = true;
#endif

  // Populate the settings based on command line arguments.
  context->PopulateSettings(&settings);

  // Create the main message loop object.
  std::unique_ptr<MainMessageLoop> message_loop;
  if (settings.multi_threaded_message_loop) {
    message_loop.reset(new MainMessageLoopMultithreadedWin);
  } else if (settings.external_message_pump) {
    message_loop = MainMessageLoopExternalPump::Create();
  } else {
    message_loop.reset(new MainMessageLoopStd);
  }

  // Initialize Honeycomb.
  context->Initialize(main_args, settings, app, sandbox_info);

  // Register scheme handlers.
  test_runner::RegisterSchemeHandlers();

  auto window_config = std::make_unique<RootWindowConfig>();
  window_config->always_on_top =
      command_line->HasSwitch(switches::kAlwaysOnTop);
  window_config->with_controls =
      !command_line->HasSwitch(switches::kHideControls);
  window_config->with_osr =
      settings.windowless_rendering_enabled ? true : false;

  // Create the first window.
  context->GetRootWindowManager()->CreateRootWindow(std::move(window_config));

  // Run the message loop. This will block until Quit() is called by the
  // RootWindowManager after all windows have been destroyed.
  int result = message_loop->Run();

  // Shut down Honeycomb.
  context->Shutdown();

  // Release objects in reverse order of creation.
  message_loop.reset();
  context.reset();

  return result;
}

}  // namespace
}  // namespace client

// Program entry point function.
int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPTSTR lpCmdLine,
                      int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

#if defined(ARCH_CPU_32_BITS)
  // Run the main thread on 32-bit Windows using a fiber with the preferred 4MiB
  // stack size. This function must be called at the top of the executable entry
  // point function (`main()` or `wWinMain()`). It is used in combination with
  // the initial stack size of 0.5MiB configured via the `/STACK:0x80000` linker
  // flag on executable targets. This saves significant memory on threads (like
  // those in the Windows thread pool, and others) whose stack size can only be
  // controlled via the linker flag.
  int exit_code = HoneycombRunWinMainWithPreferredStackSize(wWinMain, hInstance,
                                                      lpCmdLine, nCmdShow);
  if (exit_code >= 0) {
    // The fiber has completed so return here.
    return exit_code;
  }
#endif

  return client::RunMain(hInstance, nCmdShow);
}
