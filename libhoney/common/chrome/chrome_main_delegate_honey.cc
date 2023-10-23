// Copyright 2020 The Honeycomb Authors.
// Portions copyright 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoney/common/chrome/chrome_main_delegate_honey.h"

#include <tuple>

#include "libhoney/browser/chrome/chrome_browser_context.h"
#include "libhoney/browser/chrome/chrome_content_browser_client_honey.h"
#include "libhoney/common/honey_switches.h"
#include "libhoney/common/command_line_impl.h"
#include "libhoney/common/crash_reporting.h"
#include "libhoney/common/resource_util.h"
#include "libhoney/renderer/chrome/chrome_content_renderer_client_honey.h"

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/lazy_instance.h"
#include "base/threading/threading_features.h"
#include "chrome/browser/metrics/chrome_feature_list_creator.h"
#include "chrome/browser/policy/chrome_browser_policy_connector.h"
#include "chrome/common/chrome_switches.h"
#include "chrome/common/pref_names.h"
#include "components/embedder_support/switches.h"
#include "content/public/common/content_switches.h"
#include "sandbox/policy/switches.h"
#include "third_party/blink/public/common/switches.h"
#include "ui/base/ui_base_switches.h"

#if BUILDFLAG(IS_MAC)
#include "libhoney/common/util_mac.h"
#elif BUILDFLAG(IS_POSIX)
#include "libhoney/common/util_linux.h"
#endif

#if BUILDFLAG(IS_MAC)
#include "libhoney/common/util_mac.h"
#endif

namespace {

base::LazyInstance<ChromeContentRendererClientHoneycomb>::DestructorAtExit
    g_chrome_content_renderer_client = LAZY_INSTANCE_INITIALIZER;

}  // namespace

ChromeMainDelegateHoneycomb::ChromeMainDelegateHoneycomb(HoneycombMainRunnerHandler* runner,
                                             HoneycombSettings* settings,
                                             HoneycombRefPtr<HoneycombApp> application)
    : ChromeMainDelegate(base::TimeTicks::Now()),
      runner_(runner),
      settings_(settings),
      application_(application) {
#if BUILDFLAG(IS_LINUX)
  resource_util::OverrideAssetPath();
#endif
}

ChromeMainDelegateHoneycomb::~ChromeMainDelegateHoneycomb() = default;

absl::optional<int> ChromeMainDelegateHoneycomb::BasicStartupComplete() {
  // Returns false if startup should proceed.
  auto result = ChromeMainDelegate::BasicStartupComplete();
  if (result.has_value()) {
    return result;
  }

  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();

#if BUILDFLAG(IS_POSIX)
  // Read the crash configuration file. On Windows this is done from chrome_elf.
  crash_reporting::BasicStartupComplete(command_line);
#endif

  const std::string& process_type =
      command_line->GetSwitchValueASCII(switches::kProcessType);
  if (process_type.empty()) {
    // In the browser process. Populate the global command-line object.
    // TODO(chrome-runtime): Copy more settings from AlloyMainDelegate and test.
    if (settings_->command_line_args_disabled) {
      // Remove any existing command-line arguments.
      base::CommandLine::StringVector argv;
      argv.push_back(command_line->GetProgram().value());
      command_line->InitFromArgv(argv);

      const base::CommandLine::SwitchMap& map = command_line->GetSwitches();
      const_cast<base::CommandLine::SwitchMap*>(&map)->clear();
    }

    bool no_sandbox = settings_->no_sandbox ? true : false;

    if (no_sandbox) {
      command_line->AppendSwitch(sandbox::policy::switches::kNoSandbox);
    }

    if (settings_->user_agent.length > 0) {
      command_line->AppendSwitchASCII(
          embedder_support::kUserAgent,
          HoneycombString(&settings_->user_agent).ToString());
    } else if (settings_->user_agent_product.length > 0) {
      command_line->AppendSwitchASCII(
          switches::kUserAgentProductAndVersion,
          HoneycombString(&settings_->user_agent_product).ToString());
    }

    if (settings_->locale.length > 0) {
      command_line->AppendSwitchASCII(switches::kLang,
                                      HoneycombString(&settings_->locale).ToString());
    } else if (!command_line->HasSwitch(switches::kLang)) {
      command_line->AppendSwitchASCII(switches::kLang, "en-US");
    }

    if (settings_->javascript_flags.length > 0) {
      command_line->AppendSwitchASCII(
          blink::switches::kJavaScriptFlags,
          HoneycombString(&settings_->javascript_flags).ToString());
    }

    if (settings_->remote_debugging_port >= 1024 &&
        settings_->remote_debugging_port <= 65535) {
      command_line->AppendSwitchASCII(
          switches::kRemoteDebuggingPort,
          base::NumberToString(settings_->remote_debugging_port));
    }

    if (settings_->uncaught_exception_stack_size > 0) {
      command_line->AppendSwitchASCII(
          switches::kUncaughtExceptionStackSize,
          base::NumberToString(settings_->uncaught_exception_stack_size));
    }

    std::vector<std::string> disable_features;

    if (!!settings_->multi_threaded_message_loop &&
        base::kEnableHangWatcher.default_state ==
            base::FEATURE_ENABLED_BY_DEFAULT) {
      // Disable EnableHangWatcher when running with multi-threaded-message-loop
      // to avoid shutdown crashes (see issue #3403).
      disable_features.push_back(base::kEnableHangWatcher.name);
    }

    if (!disable_features.empty()) {
      DCHECK(!base::FeatureList::GetInstance());
      std::string disable_features_str =
          command_line->GetSwitchValueASCII(switches::kDisableFeatures);
      for (auto feature_str : disable_features) {
        if (!disable_features_str.empty()) {
          disable_features_str += ",";
        }
        disable_features_str += feature_str;
      }
      command_line->AppendSwitchASCII(switches::kDisableFeatures,
                                      disable_features_str);
    }
  }

  if (application_) {
    // Give the application a chance to view/modify the command line.
    HoneycombRefPtr<HoneycombCommandLineImpl> commandLinePtr(
        new HoneycombCommandLineImpl(command_line, false, false));
    application_->OnBeforeCommandLineProcessing(process_type,
                                                commandLinePtr.get());
    std::ignore = commandLinePtr->Detach(nullptr);
  }

#if BUILDFLAG(IS_MAC)
  util_mac::BasicStartupComplete();
#endif

  return absl::nullopt;
}

void ChromeMainDelegateHoneycomb::PreSandboxStartup() {
  const base::CommandLine* command_line =
      base::CommandLine::ForCurrentProcess();
  const std::string& process_type =
      command_line->GetSwitchValueASCII(switches::kProcessType);

  if (process_type.empty()) {
#if BUILDFLAG(IS_MAC)
    util_mac::PreSandboxStartup();
#elif BUILDFLAG(IS_POSIX)
    util_linux::PreSandboxStartup();
#endif
  }

  // Since this may be configured via HoneycombSettings we override the value on
  // all platforms. We can't use the default implementation on macOS because
  // chrome::GetDefaultUserDataDirectory expects to find the Chromium version
  // number in the app bundle path.
  resource_util::OverrideUserDataDir(settings_, command_line);

  ChromeMainDelegate::PreSandboxStartup();

  // Initialize crash reporting state for this process/module.
  // chrome::DIR_CRASH_DUMPS must be configured before calling this function.
  crash_reporting::PreSandboxStartup(*command_line, process_type);
}

absl::optional<int> ChromeMainDelegateHoneycomb::PreBrowserMain() {
  // The parent ChromeMainDelegate implementation creates the NSApplication
  // instance on macOS, and we intentionally don't want to do that here.
  // TODO(macos): Do we need l10n_util::OverrideLocaleWithCocoaLocale()?
  runner_->PreBrowserMain();
  return absl::nullopt;
}

absl::optional<int> ChromeMainDelegateHoneycomb::PostEarlyInitialization(
    InvokedIn invoked_in) {
  // Configure this before ChromeMainDelegate::PostEarlyInitialization triggers
  // ChromeBrowserPolicyConnector creation.
  if (settings_ && settings_->chrome_policy_id.length > 0) {
    policy::ChromeBrowserPolicyConnector::EnablePlatformPolicySupport(
        HoneycombString(&settings_->chrome_policy_id).ToString());
  }

  const auto result = ChromeMainDelegate::PostEarlyInitialization(invoked_in);
  if (!result) {
    const auto* invoked_in_browser =
        absl::get_if<InvokedInBrowserProcess>(&invoked_in);
    if (invoked_in_browser) {
      // At this point local_state has been created but ownership has not yet
      // been passed to BrowserProcessImpl (g_browser_process is nullptr).
      auto* local_state = chrome_content_browser_client_->startup_data()
                              ->chrome_feature_list_creator()
                              ->local_state();

      // Don't show the profile picker on startup (see issue #3440).
      local_state->SetBoolean(prefs::kBrowserShowProfilePickerOnStartup, false);
    }
  }

  return result;
}

absl::variant<int, content::MainFunctionParams>
ChromeMainDelegateHoneycomb::RunProcess(
    const std::string& process_type,
    content::MainFunctionParams main_function_params) {
  if (process_type.empty()) {
    return runner_->RunMainProcess(std::move(main_function_params));
  }

  return ChromeMainDelegate::RunProcess(process_type,
                                        std::move(main_function_params));
}

#if BUILDFLAG(IS_LINUX)
void ChromeMainDelegateHoneycomb::ZygoteForked() {
  ChromeMainDelegate::ZygoteForked();

  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  const std::string& process_type =
      command_line->GetSwitchValueASCII(switches::kProcessType);

  // Initialize crash reporting state for the newly forked process.
  crash_reporting::ZygoteForked(command_line, process_type);
}
#endif  // BUILDFLAG(IS_LINUX)

content::ContentClient* ChromeMainDelegateHoneycomb::CreateContentClient() {
  return &chrome_content_client_honey_;
}

content::ContentBrowserClient*
ChromeMainDelegateHoneycomb::CreateContentBrowserClient() {
  // Match the logic in the parent ChromeMainDelegate implementation, but create
  // our own object type.
  chrome_content_browser_client_ =
      std::make_unique<ChromeContentBrowserClientHoneycomb>();
  return chrome_content_browser_client_.get();
}

content::ContentRendererClient*
ChromeMainDelegateHoneycomb::CreateContentRendererClient() {
  return g_chrome_content_renderer_client.Pointer();
}

HoneycombRefPtr<HoneycombRequestContext> ChromeMainDelegateHoneycomb::GetGlobalRequestContext() {
  auto browser_client = content_browser_client();
  if (browser_client) {
    return browser_client->request_context();
  }
  return nullptr;
}

HoneycombBrowserContext* ChromeMainDelegateHoneycomb::CreateNewBrowserContext(
    const HoneycombRequestContextSettings& settings,
    base::OnceClosure initialized_cb) {
  auto context = new ChromeBrowserContext(settings);
  context->InitializeAsync(std::move(initialized_cb));
  return context;
}

scoped_refptr<base::SingleThreadTaskRunner>
ChromeMainDelegateHoneycomb::GetBackgroundTaskRunner() {
  auto browser_client = content_browser_client();
  if (browser_client) {
    return browser_client->background_task_runner();
  }
  return nullptr;
}

scoped_refptr<base::SingleThreadTaskRunner>
ChromeMainDelegateHoneycomb::GetUserVisibleTaskRunner() {
  auto browser_client = content_browser_client();
  if (browser_client) {
    return browser_client->user_visible_task_runner();
  }
  return nullptr;
}

scoped_refptr<base::SingleThreadTaskRunner>
ChromeMainDelegateHoneycomb::GetUserBlockingTaskRunner() {
  auto browser_client = content_browser_client();
  if (browser_client) {
    return browser_client->user_blocking_task_runner();
  }
  return nullptr;
}

scoped_refptr<base::SingleThreadTaskRunner>
ChromeMainDelegateHoneycomb::GetRenderTaskRunner() {
  auto renderer_client = content_renderer_client();
  if (renderer_client) {
    return renderer_client->render_task_runner();
  }
  return nullptr;
}

scoped_refptr<base::SingleThreadTaskRunner>
ChromeMainDelegateHoneycomb::GetWebWorkerTaskRunner() {
  auto renderer_client = content_renderer_client();
  if (renderer_client) {
    return renderer_client->GetCurrentTaskRunner();
  }
  return nullptr;
}

ChromeContentBrowserClientHoneycomb* ChromeMainDelegateHoneycomb::content_browser_client()
    const {
  return static_cast<ChromeContentBrowserClientHoneycomb*>(
      chrome_content_browser_client_.get());
}

ChromeContentRendererClientHoneycomb* ChromeMainDelegateHoneycomb::content_renderer_client()
    const {
  if (!g_chrome_content_renderer_client.IsCreated()) {
    return nullptr;
  }
  return g_chrome_content_renderer_client.Pointer();
}
