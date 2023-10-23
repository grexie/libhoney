// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/base/honey_build.h"
#include "include/honey_config.h"

#if defined(OS_LINUX) && defined(HONEYCOMB_X11)
#include <X11/Xlib.h>
// Definitions conflict with gtest.
#undef None
#undef Bool
#endif

#if defined(OS_POSIX)
#include <unistd.h>
#endif

#include "include/base/honey_callback.h"
#include "include/honey_app.h"
#include "include/honey_task.h"
#include "include/honey_thread.h"
#include "include/honey_waitable_event.h"
#include "include/wrapper/honey_closure_task.h"
#include "include/wrapper/honey_helpers.h"
#include "tests/honeytests/test_handler.h"
#include "tests/honeytests/test_server.h"
#include "tests/honeytests/test_suite.h"
#include "tests/shared/browser/client_app_browser.h"
#include "tests/shared/browser/main_message_loop_external_pump.h"
#include "tests/shared/browser/main_message_loop_std.h"
#include "tests/shared/common/client_app_other.h"
#include "tests/shared/renderer/client_app_renderer.h"

#if defined(OS_MAC)
#include "include/wrapper/honey_library_loader.h"
#endif

// When generating projects with CMake the HONEYCOMB_USE_SANDBOX value will be defined
// automatically if using the required compiler version. Pass -DUSE_SANDBOX=OFF
// to the CMake command-line to disable use of the sandbox.
#if defined(OS_WIN) && defined(HONEYCOMB_USE_SANDBOX)
#include "include/honey_sandbox_win.h"

// The honey_sandbox.lib static library may not link successfully with all VS
// versions.
#pragma comment(lib, "honey_sandbox.lib")
#endif

namespace {

void QuitMessageLoop() {
  HONEYCOMB_REQUIRE_UI_THREAD();
  client::MainMessageLoop* message_loop = client::MainMessageLoop::Get();
  if (message_loop) {
    message_loop->Quit();
  } else {
    HoneycombQuitMessageLoop();
  }
}

void sleep(int64_t ms) {
#if defined(OS_WIN)
  Sleep(ms);
#elif defined(OS_POSIX)
  usleep(static_cast<useconds_t>(ms * 1000));
#else
#error Unsupported platform
#endif
}

// Called on the test thread.
void RunTestsOnTestThread() {
  // Run the test suite.
  HoneycombTestSuite::GetInstance()->Run();

  // Wait for all TestHandlers to be destroyed.
  while (TestHandler::HasTestHandler()) {
    sleep(100);
  }

  // Wait for the test server to stop, and then quit the Honeycomb message loop.
  test_server::Stop(base::BindOnce(QuitMessageLoop));
}

// Called on the UI thread.
void ContinueOnUIThread(HoneycombRefPtr<HoneycombTaskRunner> test_task_runner) {
  // Run the test suite on the test thread.
  test_task_runner->PostTask(
      HoneycombCreateClosureTask(base::BindOnce(&RunTestsOnTestThread)));
}

#if defined(OS_LINUX) && defined(HONEYCOMB_X11)
int XErrorHandlerImpl(Display* display, XErrorEvent* event) {
  LOG(WARNING) << "X error received: "
               << "type " << event->type << ", "
               << "serial " << event->serial << ", "
               << "error_code " << static_cast<int>(event->error_code) << ", "
               << "request_code " << static_cast<int>(event->request_code)
               << ", "
               << "minor_code " << static_cast<int>(event->minor_code);
  return 0;
}

int XIOErrorHandlerImpl(Display* display) {
  return 0;
}
#endif  // defined(OS_LINUX) && defined(HONEYCOMB_X11)

}  // namespace

int main(int argc, char* argv[]) {
#if !defined(OS_MAC)
  int exit_code;
#endif

#if defined(OS_WIN) && defined(ARCH_CPU_32_BITS)
  // Run the main thread on 32-bit Windows using a fiber with the preferred 4MiB
  // stack size. This function must be called at the top of the executable entry
  // point function (`main()` or `wWinMain()`). It is used in combination with
  // the initial stack size of 0.5MiB configured via the `/STACK:0x80000` linker
  // flag on executable targets. This saves significant memory on threads (like
  // those in the Windows thread pool, and others) whose stack size can only be
  // controlled via the linker flag.
  exit_code = HoneycombRunMainWithPreferredStackSize(main, argc, argv);
  if (exit_code >= 0) {
    // The fiber has completed so return here.
    return exit_code;
  }
#endif

#if defined(OS_MAC)
  // Load the Honeycomb framework library at runtime instead of linking directly
  // as required by the macOS sandbox implementation.
  HoneycombScopedLibraryLoader library_loader;
  if (!library_loader.LoadInMain()) {
    return 1;
  }
#endif

  // Create the singleton test suite object.
  HoneycombTestSuite test_suite(argc, argv);

#if defined(OS_WIN)
  HoneycombMainArgs main_args(::GetModuleHandle(nullptr));
#else
  HoneycombMainArgs main_args(argc, argv);
#endif

  void* windows_sandbox_info = nullptr;

#if defined(OS_WIN) && defined(HONEYCOMB_USE_SANDBOX)
  // Manages the life span of the sandbox information object.
  HoneycombScopedSandboxInfo scoped_sandbox;
  windows_sandbox_info = scoped_sandbox.sandbox_info();
#endif

  // Create a ClientApp of the correct type.
  HoneycombRefPtr<HoneycombApp> app;
  client::ClientApp::ProcessType process_type =
      client::ClientApp::GetProcessType(test_suite.command_line());
  if (process_type == client::ClientApp::BrowserProcess) {
    app = new client::ClientAppBrowser();
#if !defined(OS_MAC)
  } else if (process_type == client::ClientApp::RendererProcess ||
             process_type == client::ClientApp::ZygoteProcess) {
    app = new client::ClientAppRenderer();
  } else if (process_type == client::ClientApp::OtherProcess) {
    app = new client::ClientAppOther();
  }

  // Execute the secondary process, if any.
  exit_code = HoneycombExecuteProcess(main_args, app, windows_sandbox_info);
  if (exit_code >= 0) {
    return exit_code;
  }
#else
  } else {
    // On OS X this executable is only used for the main process.
    NOTREACHED();
  }
#endif

  HoneycombSettings settings;

#if !defined(HONEYCOMB_USE_SANDBOX)
  settings.no_sandbox = true;
#endif

  client::ClientAppBrowser::PopulateSettings(test_suite.command_line(),
                                             settings);
  test_suite.GetSettings(settings);

#if defined(OS_MAC)
  // Platform-specific initialization.
  extern void PlatformInit();
  PlatformInit();
#endif

#if defined(OS_LINUX) && defined(HONEYCOMB_X11)
  // Install xlib error handlers so that the application won't be terminated
  // on non-fatal errors.
  XSetErrorHandler(XErrorHandlerImpl);
  XSetIOErrorHandler(XIOErrorHandlerImpl);
#endif

  // Create the MessageLoop.
  std::unique_ptr<client::MainMessageLoop> message_loop;
  if (!settings.multi_threaded_message_loop) {
    if (settings.external_message_pump) {
      message_loop = client::MainMessageLoopExternalPump::Create();
    } else {
      message_loop.reset(new client::MainMessageLoopStd);
    }
  }

  // Initialize Honeycomb.
  HoneycombInitialize(main_args, settings, app, windows_sandbox_info);

  // Initialize the testing framework.
  test_suite.InitMainProcess();

  int retval;

  if (settings.multi_threaded_message_loop) {
    // Run the test suite on the main thread.
    retval = test_suite.Run();

    // Wait for the test server to stop.
    HoneycombRefPtr<HoneycombWaitableEvent> event =
        HoneycombWaitableEvent::CreateWaitableEvent(true, false);
    test_server::Stop(base::BindOnce(&HoneycombWaitableEvent::Signal, event));
    event->Wait();
  } else {
    // Create and start the test thread.
    HoneycombRefPtr<HoneycombThread> thread = HoneycombThread::CreateThread("test_thread");
    if (!thread) {
      return 1;
    }

    // Start the tests from the UI thread so that any pending UI tasks get a
    // chance to execute first.
    HoneycombPostTask(TID_UI,
                base::BindOnce(&ContinueOnUIThread, thread->GetTaskRunner()));

    // Run the Honeycomb message loop.
    message_loop->Run();

    // The test suite has completed.
    retval = test_suite.retval();

    // Terminate the test thread.
    thread->Stop();
    thread = nullptr;
  }

  // Shut down Honeycomb.
  HoneycombShutdown();

  test_suite.DeleteTempDirectories();

  // Destroy the MessageLoop.
  message_loop.reset(nullptr);

#if defined(OS_MAC)
  // Platform-specific cleanup.
  extern void PlatformCleanup();
  PlatformCleanup();
#endif

  return retval;
}
