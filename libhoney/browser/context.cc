// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "libhoney/browser/context.h"

#include "libhoney/browser/browser_info_manager.h"
#include "libhoney/browser/request_context_impl.h"
#include "libhoney/browser/thread_util.h"
#include "libhoney/browser/trace_subscriber.h"
#include "libhoney/common/honey_switches.h"

#include "base/files/file_util.h"
#include "base/functional/bind.h"
#include "base/run_loop.h"
#include "base/task/current_thread.h"
#include "base/threading/thread_restrictions.h"
#include "components/network_session_configurator/common/network_switches.h"
#include "content/public/browser/notification_service.h"
#include "content/public/browser/notification_types.h"
#include "ui/base/ui_base_switches.h"

#if BUILDFLAG(IS_WIN)
#include "base/debug/alias.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/chrome_elf/chrome_elf_main.h"
#include "chrome/install_static/initialize_from_primary_module.h"
#include "include/internal/honey_win.h"
#endif

namespace {

HoneycombContext* g_context = nullptr;

#if DCHECK_IS_ON()
// When the process terminates check if HoneycombShutdown() has been called.
class HoneycombShutdownChecker {
 public:
  ~HoneycombShutdownChecker() { DCHECK(!g_context) << "HoneycombShutdown was not called"; }
} g_shutdown_checker;
#endif  // DCHECK_IS_ON()

#if BUILDFLAG(IS_WIN)

// Transfer state from chrome_elf.dll to the libhoney.dll. Accessed when
// loading chrome://system.
void InitInstallDetails() {
  static bool initialized = false;
  if (initialized) {
    return;
  }
  initialized = true;
  install_static::InitializeFromPrimaryModule();
}

// Signal chrome_elf to initialize crash reporting, rather than doing it in
// DllMain. See https://crbug.com/656800 for details.
void InitCrashReporter() {
  static bool initialized = false;
  if (initialized) {
    return;
  }
  initialized = true;
  SignalInitializeCrashReporting();
}

#endif  // BUILDFLAG(IS_WIN)

bool GetColor(const honey_color_t honey_in, bool is_windowless, SkColor* sk_out) {
  // Windowed browser colors must be fully opaque.
  if (!is_windowless && HoneycombColorGetA(honey_in) != SK_AlphaOPAQUE) {
    return false;
  }

  // Windowless browser colors may be fully transparent.
  if (is_windowless && HoneycombColorGetA(honey_in) == SK_AlphaTRANSPARENT) {
    *sk_out = SK_ColorTRANSPARENT;
    return true;
  }

  // Ignore the alpha component.
  *sk_out = SkColorSetRGB(HoneycombColorGetR(honey_in), HoneycombColorGetG(honey_in),
                          HoneycombColorGetB(honey_in));
  return true;
}

// Convert |path_str| to a normalized FilePath.
base::FilePath NormalizePath(const honey_string_t& path_str,
                             const char* name,
                             bool* has_error = nullptr) {
  if (has_error) {
    *has_error = false;
  }

  base::FilePath path = base::FilePath(HoneycombString(&path_str));
  if (path.EndsWithSeparator()) {
    // Remove the trailing separator because it will interfere with future
    // equality checks.
    path = path.StripTrailingSeparators();
  }

  if (!path.empty() && !path.IsAbsolute()) {
    LOG(ERROR) << "The " << name << " directory (" << path.value()
               << ") is not an absolute path. Defaulting to empty.";
    if (has_error) {
      *has_error = true;
    }
    path = base::FilePath();
  }

  return path;
}

void SetPath(honey_string_t& path_str, const base::FilePath& path) {
#if BUILDFLAG(IS_WIN)
  HoneycombString(&path_str).FromWString(path.value());
#else
  HoneycombString(&path_str).FromString(path.value());
#endif
}

// Convert |path_str| to a normalized FilePath and update the |path_str| value.
base::FilePath NormalizePathAndSet(honey_string_t& path_str, const char* name) {
  const base::FilePath& path = NormalizePath(path_str, name);
  SetPath(path_str, path);
  return path;
}

// Verify that |cache_path| is valid and create it if necessary.
bool ValidateCachePath(const base::FilePath& cache_path,
                       const base::FilePath& root_cache_path) {
  if (cache_path.empty()) {
    return true;
  }

  if (!root_cache_path.empty() && root_cache_path != cache_path &&
      !root_cache_path.IsParent(cache_path)) {
    LOG(ERROR) << "The cache_path directory (" << cache_path.value()
               << ") is not a child of the root_cache_path directory ("
               << root_cache_path.value() << ")";
    return false;
  }

  base::ScopedAllowBlockingForTesting allow_blocking;
  if (!base::DirectoryExists(cache_path) &&
      !base::CreateDirectory(cache_path)) {
    LOG(ERROR) << "The cache_path directory (" << cache_path.value()
               << ") could not be created.";
    return false;
  }

  return true;
}

// Like NormalizePathAndSet but with additional checks specific to the
// cache_path value.
base::FilePath NormalizeCachePathAndSet(honey_string_t& path_str,
                                        const base::FilePath& root_cache_path) {
  bool has_error = false;
  base::FilePath path = NormalizePath(path_str, "cache_path", &has_error);
  if (has_error || !ValidateCachePath(path, root_cache_path)) {
    LOG(ERROR) << "The cache_path is invalid. Defaulting to in-memory storage.";
    path = base::FilePath();
  }
  SetPath(path_str, path);
  return path;
}

// Based on chrome/app/chrome_exe_main_win.cc.
// In 32-bit builds, the main thread starts with the default (small) stack size.
// The ARCH_CPU_32_BITS blocks here and below are in support of moving the main
// thread to a fiber with a larger stack size.
#if BUILDFLAG(IS_WIN) && defined(ARCH_CPU_32_BITS)
// The information needed to transfer control to the large-stack fiber and later
// pass the main routine's exit code back to the small-stack fiber prior to
// termination.
struct FiberState {
  FiberState(wWinMainPtr wWinMain,
             HINSTANCE hInstance,
             LPWSTR lpCmdLine,
             int nCmdShow) {
    this->wWinMain = wWinMain;
    this->hInstance = hInstance;
    this->lpCmdLine = lpCmdLine;
    this->nCmdShow = nCmdShow;
  }

  FiberState(mainPtr main, int argc, char** argv) {
    this->main = main;
    this->argc = argc;
    this->argv = argv;
  }

  wWinMainPtr wWinMain = nullptr;
  HINSTANCE hInstance;
  LPWSTR lpCmdLine;
  int nCmdShow;

  mainPtr main = nullptr;
  int argc;
  char** argv;

  LPVOID original_fiber;
  int fiber_result;
};

// A PFIBER_START_ROUTINE function run on a large-stack fiber that calls the
// main routine, stores its return value, and returns control to the small-stack
// fiber. |params| must be a pointer to a FiberState struct.
void WINAPI FiberBinder(void* params) {
  auto* fiber_state = static_cast<FiberState*>(params);
  // Call the main routine from the fiber. Reusing the entry point minimizes
  // confusion when examining call stacks in crash reports - seeing wWinMain on
  // the stack is a handy hint that this is the main thread of the process.
  if (fiber_state->main) {
    fiber_state->fiber_result =
        fiber_state->main(fiber_state->argc, fiber_state->argv);
  } else {
    fiber_state->fiber_result =
        fiber_state->wWinMain(fiber_state->hInstance, nullptr,
                              fiber_state->lpCmdLine, fiber_state->nCmdShow);
  }

  // Switch back to the main thread to exit.
  ::SwitchToFiber(fiber_state->original_fiber);
}

int RunMainWithPreferredStackSize(FiberState& fiber_state) {
  enum class FiberStatus { kConvertFailed, kCreateFiberFailed, kSuccess };
  FiberStatus fiber_status = FiberStatus::kSuccess;
  // GetLastError result if fiber conversion failed.
  DWORD fiber_error = ERROR_SUCCESS;
  if (!::IsThreadAFiber()) {
    // Make the main thread's stack size 4 MiB so that it has roughly the same
    // effective size as the 64-bit build's 8 MiB stack.
    constexpr size_t kStackSize = 4 * 1024 * 1024;  // 4 MiB
    // Leak the fiber on exit.
    LPVOID original_fiber =
        ::ConvertThreadToFiberEx(nullptr, FIBER_FLAG_FLOAT_SWITCH);
    if (original_fiber) {
      fiber_state.original_fiber = original_fiber;
      // Create a fiber with a bigger stack and switch to it. Leak the fiber on
      // exit.
      LPVOID big_stack_fiber = ::CreateFiberEx(
          0, kStackSize, FIBER_FLAG_FLOAT_SWITCH, FiberBinder, &fiber_state);
      if (big_stack_fiber) {
        ::SwitchToFiber(big_stack_fiber);
        // The fibers must be cleaned up to avoid obscure TLS-related shutdown
        // crashes.
        ::DeleteFiber(big_stack_fiber);
        ::ConvertFiberToThread();
        // Control returns here after Honeycomb has finished running on FiberMain.
        return fiber_state.fiber_result;
      }
      fiber_status = FiberStatus::kCreateFiberFailed;
    } else {
      fiber_status = FiberStatus::kConvertFailed;
    }
    // If we reach here then creating and switching to a fiber has failed. This
    // probably means we are low on memory and will soon crash. Try to report
    // this error once crash reporting is initialized.
    fiber_error = ::GetLastError();
    base::debug::Alias(&fiber_error);
  }

  // If we are already a fiber then continue normal execution.
  // Intentionally crash if converting to a fiber failed.
  CHECK_EQ(fiber_status, FiberStatus::kSuccess);
  return -1;
}
#endif  // BUILDFLAG(IS_WIN) && defined(ARCH_CPU_32_BITS)

}  // namespace

int HoneycombExecuteProcess(const HoneycombMainArgs& args,
                      HoneycombRefPtr<HoneycombApp> application,
                      void* windows_sandbox_info) {
#if BUILDFLAG(IS_WIN)
  InitInstallDetails();
  InitCrashReporter();
#endif

  return HoneycombMainRunner::RunAsHelperProcess(args, application,
                                           windows_sandbox_info);
}

bool HoneycombInitialize(const HoneycombMainArgs& args,
                   const HoneycombSettings& settings,
                   HoneycombRefPtr<HoneycombApp> application,
                   void* windows_sandbox_info) {
#if BUILDFLAG(IS_WIN)
  InitInstallDetails();
  InitCrashReporter();
#endif

  // Return true if the global context already exists.
  if (g_context) {
    return true;
  }

  if (settings.size != sizeof(honey_settings_t)) {
    DCHECK(false) << "invalid HoneycombSettings structure size";
    return false;
  }

  // Create the new global context object.
  g_context = new HoneycombContext();

  // Initialize the global context.
  return g_context->Initialize(args, settings, application,
                               windows_sandbox_info);
}

void HoneycombShutdown() {
  // Verify that the context is in a valid state.
  if (!CONTEXT_STATE_VALID()) {
    DCHECK(false) << "context not valid";
    return;
  }

  // Must always be called on the same thread as Initialize.
  if (!g_context->OnInitThread()) {
    DCHECK(false) << "called on invalid thread";
    return;
  }

  // Shut down the global context. This will block until shutdown is complete.
  g_context->Shutdown();

  // Delete the global context object.
  delete g_context;
  g_context = nullptr;
}

void HoneycombDoMessageLoopWork() {
  // Verify that the context is in a valid state.
  if (!CONTEXT_STATE_VALID()) {
    DCHECK(false) << "context not valid";
    return;
  }

  // Must always be called on the same thread as Initialize.
  if (!g_context->OnInitThread()) {
    DCHECK(false) << "called on invalid thread";
    return;
  }

  base::RunLoop run_loop;
  run_loop.RunUntilIdle();
}

void HoneycombRunMessageLoop() {
  // Verify that the context is in a valid state.
  if (!CONTEXT_STATE_VALID()) {
    DCHECK(false) << "context not valid";
    return;
  }

  // Must always be called on the same thread as Initialize.
  if (!g_context->OnInitThread()) {
    DCHECK(false) << "called on invalid thread";
    return;
  }

  g_context->RunMessageLoop();
}

void HoneycombQuitMessageLoop() {
  // Verify that the context is in a valid state.
  if (!CONTEXT_STATE_VALID()) {
    DCHECK(false) << "context not valid";
    return;
  }

  // Must always be called on the same thread as Initialize.
  if (!g_context->OnInitThread()) {
    DCHECK(false) << "called on invalid thread";
    return;
  }

  g_context->QuitMessageLoop();
}

#if BUILDFLAG(IS_WIN)

#if defined(ARCH_CPU_32_BITS)
int HoneycombRunWinMainWithPreferredStackSize(wWinMainPtr wWinMain,
                                        HINSTANCE hInstance,
                                        LPWSTR lpCmdLine,
                                        int nCmdShow) {
  CHECK(wWinMain && hInstance);
  FiberState fiber_state(wWinMain, hInstance, lpCmdLine, nCmdShow);
  return RunMainWithPreferredStackSize(fiber_state);
}

int HoneycombRunMainWithPreferredStackSize(mainPtr main, int argc, char* argv[]) {
  CHECK(main);
  FiberState fiber_state(main, argc, argv);
  return RunMainWithPreferredStackSize(fiber_state);
}
#endif  // defined(ARCH_CPU_32_BITS)

void HoneycombSetOSModalLoop(bool osModalLoop) {
  // Verify that the context is in a valid state.
  if (!CONTEXT_STATE_VALID()) {
    DCHECK(false) << "context not valid";
    return;
  }

  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT, base::BindOnce(HoneycombSetOSModalLoop, osModalLoop));
    return;
  }

  base::CurrentThread::Get()->set_os_modal_loop(osModalLoop);
}

#endif  // BUILDFLAG(IS_WIN)

// HoneycombContext

HoneycombContext::HoneycombContext()
    : initialized_(false), shutting_down_(false), init_thread_id_(0) {}

HoneycombContext::~HoneycombContext() {}

// static
HoneycombContext* HoneycombContext::Get() {
  return g_context;
}

bool HoneycombContext::Initialize(const HoneycombMainArgs& args,
                            const HoneycombSettings& settings,
                            HoneycombRefPtr<HoneycombApp> application,
                            void* windows_sandbox_info) {
  init_thread_id_ = base::PlatformThread::CurrentId();
  settings_ = settings;
  application_ = application;

#if !(BUILDFLAG(IS_WIN) || BUILDFLAG(IS_LINUX))
  if (settings.multi_threaded_message_loop) {
    NOTIMPLEMENTED() << "multi_threaded_message_loop is not supported.";
    return false;
  }
#endif

#if BUILDFLAG(IS_WIN)
  // Signal Chrome Elf that Chrome has begun to start.
  SignalChromeElf();
#endif

  const base::FilePath& root_cache_path =
      NormalizePathAndSet(settings_.root_cache_path, "root_cache_path");
  const base::FilePath& cache_path =
      NormalizeCachePathAndSet(settings_.cache_path, root_cache_path);
  if (root_cache_path.empty() && !cache_path.empty()) {
    HoneycombString(&settings_.root_cache_path) = cache_path.value();
  }

  // All other paths that need to be normalized.
  NormalizePathAndSet(settings_.browser_subprocess_path,
                      "browser_subprocess_path");
  NormalizePathAndSet(settings_.framework_dir_path, "framework_dir_path");
  NormalizePathAndSet(settings_.main_bundle_path, "main_bundle_path");
  NormalizePathAndSet(settings_.resources_dir_path, "resources_dir_path");
  NormalizePathAndSet(settings_.locales_dir_path, "locales_dir_path");

  browser_info_manager_.reset(new HoneycombBrowserInfoManager);

  main_runner_.reset(new HoneycombMainRunner(settings_.multi_threaded_message_loop,
                                       settings_.external_message_pump));
  return main_runner_->Initialize(
      &settings_, application, args, windows_sandbox_info, &initialized_,
      base::BindOnce(&HoneycombContext::OnContextInitialized,
                     base::Unretained(this)));
}

void HoneycombContext::RunMessageLoop() {
  // Must always be called on the same thread as Initialize.
  DCHECK(OnInitThread());

  // Blocks until QuitMessageLoop() is called.
  main_runner_->RunMessageLoop();
}

void HoneycombContext::QuitMessageLoop() {
  // Must always be called on the same thread as Initialize.
  DCHECK(OnInitThread());

  main_runner_->QuitMessageLoop();
}

void HoneycombContext::Shutdown() {
  // Must always be called on the same thread as Initialize.
  DCHECK(OnInitThread());

  shutting_down_ = true;

  main_runner_->Shutdown(
      base::BindOnce(&HoneycombContext::ShutdownOnUIThread, base::Unretained(this)),
      base::BindOnce(&HoneycombContext::FinalizeShutdown, base::Unretained(this)));
}

bool HoneycombContext::OnInitThread() {
  return (base::PlatformThread::CurrentId() == init_thread_id_);
}

SkColor HoneycombContext::GetBackgroundColor(
    const HoneycombBrowserSettings* browser_settings,
    honey_state_t windowless_state) const {
  bool is_windowless = windowless_state == STATE_ENABLED
                           ? true
                           : (windowless_state == STATE_DISABLED
                                  ? false
                                  : !!settings_.windowless_rendering_enabled);

  // Default to opaque white if no acceptable color values are found.
  SkColor sk_color = SK_ColorWHITE;

  if (!browser_settings ||
      !GetColor(browser_settings->background_color, is_windowless, &sk_color)) {
    GetColor(settings_.background_color, is_windowless, &sk_color);
  }
  return sk_color;
}

HoneycombTraceSubscriber* HoneycombContext::GetTraceSubscriber() {
  HONEYCOMB_REQUIRE_UIT();
  if (shutting_down_) {
    return nullptr;
  }
  if (!trace_subscriber_.get()) {
    trace_subscriber_.reset(new HoneycombTraceSubscriber());
  }
  return trace_subscriber_.get();
}

void HoneycombContext::PopulateGlobalRequestContextSettings(
    HoneycombRequestContextSettings* settings) {
  HoneycombRefPtr<HoneycombCommandLine> command_line =
      HoneycombCommandLine::GetGlobalCommandLine();

  // This value was already normalized in Initialize.
  HoneycombString(&settings->cache_path) = HoneycombString(&settings_.cache_path);

  settings->persist_session_cookies =
      settings_.persist_session_cookies ||
      command_line->HasSwitch(switches::kPersistSessionCookies);
  settings->persist_user_preferences =
      settings_.persist_user_preferences ||
      command_line->HasSwitch(switches::kPersistUserPreferences);

  HoneycombString(&settings->cookieable_schemes_list) =
      HoneycombString(&settings_.cookieable_schemes_list);
  settings->cookieable_schemes_exclude_defaults =
      settings_.cookieable_schemes_exclude_defaults;
}

void HoneycombContext::NormalizeRequestContextSettings(
    HoneycombRequestContextSettings* settings) {
  // The |root_cache_path| value was already normalized in Initialize.
  const base::FilePath& root_cache_path = HoneycombString(&settings_.root_cache_path);
  NormalizeCachePathAndSet(settings->cache_path, root_cache_path);
}

void HoneycombContext::AddObserver(Observer* observer) {
  HONEYCOMB_REQUIRE_UIT();
  observers_.AddObserver(observer);
}

void HoneycombContext::RemoveObserver(Observer* observer) {
  HONEYCOMB_REQUIRE_UIT();
  observers_.RemoveObserver(observer);
}

bool HoneycombContext::HasObserver(Observer* observer) const {
  HONEYCOMB_REQUIRE_UIT();
  return observers_.HasObserver(observer);
}

void HoneycombContext::OnContextInitialized() {
  HONEYCOMB_REQUIRE_UIT();

  if (application_) {
    // Notify the handler after the global browser context has initialized.
    HoneycombRefPtr<HoneycombRequestContext> request_context =
        HoneycombRequestContext::GetGlobalContext();
    auto impl = static_cast<HoneycombRequestContextImpl*>(request_context.get());
    impl->ExecuteWhenBrowserContextInitialized(base::BindOnce(
        [](HoneycombRefPtr<HoneycombApp> app) {
          HoneycombRefPtr<HoneycombBrowserProcessHandler> handler =
              app->GetBrowserProcessHandler();
          if (handler) {
            handler->OnContextInitialized();
          }
        },
        application_));
  }
}

void HoneycombContext::ShutdownOnUIThread() {
  HONEYCOMB_REQUIRE_UIT();

  browser_info_manager_->DestroyAllBrowsers();

  for (auto& observer : observers_) {
    observer.OnContextDestroyed();
  }

  if (trace_subscriber_.get()) {
    trace_subscriber_.reset(nullptr);
  }
}

void HoneycombContext::FinalizeShutdown() {
  browser_info_manager_.reset(nullptr);
  application_ = nullptr;
}
