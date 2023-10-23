// Copyright (C) 2023 Grexie. Portions copyright
// 2016 The Chromium Authors and 2016 the CEF Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#include "libhoney/common/crash_reporting.h"

#include "include/honey_crash_util.h"
#include "libhoney/common/honey_switches.h"
#include "libhoney/features/runtime.h"

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/debug/crash_logging.h"
#include "base/logging.h"
#include "base/stl_util.h"
#include "base/strings/string_piece.h"
#include "base/strings/string_util.h"
#include "chrome/common/crash_keys.h"
#include "components/crash/core/common/crash_key.h"
#include "components/crash/core/common/crash_keys.h"
#include "content/public/common/content_switches.h"

#if BUILDFLAG(IS_MAC)
#include "base/apple/foundation_util.h"
#include "components/crash/core/common/crash_keys.h"
#include "content/public/common/content_paths.h"
#endif

#if BUILDFLAG(IS_POSIX)
#include "base/lazy_instance.h"
#include "components/crash/core/app/crashpad.h"
#include "libhoney/common/crash_reporter_client.h"
#endif

namespace crash_reporting
{

  namespace
  {

#if BUILDFLAG(IS_WIN)

    const base::FilePath::CharType kChromeElfDllName[] =
        FILE_PATH_LITERAL("chrome_elf.dll");

    // exported in crash_reporter_client.cc:
    //    int __declspec(dllexport) __cdecl SetCrashKeyValueImpl.
    typedef int(__cdecl *SetCrashKeyValue)(const char *,
                                           size_t,
                                           const char *,
                                           size_t);

    //    int __declspec(dllexport) __cdecl IsCrashReportingEnabledImpl.
    typedef int(__cdecl *IsCrashReportingEnabled)();

    bool SetCrashKeyValueTrampoline(const base::StringPiece &key,
                                    const base::StringPiece &value)
    {
      static SetCrashKeyValue set_crash_key = []()
      {
        HMODULE elf_module = GetModuleHandle(kChromeElfDllName);
        return reinterpret_cast<SetCrashKeyValue>(
            elf_module ? GetProcAddress(elf_module, "SetCrashKeyValueImpl")
                       : nullptr);
      }();
      if (set_crash_key)
      {
        return !!(set_crash_key)(key.data(), key.size(), value.data(),
                                 value.size());
      }
      return false;
    }

    bool IsCrashReportingEnabledTrampoline()
    {
      static IsCrashReportingEnabled is_crash_reporting_enabled = []()
      {
        HMODULE elf_module = GetModuleHandle(kChromeElfDllName);
        return reinterpret_cast<IsCrashReportingEnabled>(
            elf_module ? GetProcAddress(elf_module, "IsCrashReportingEnabledImpl")
                       : nullptr);
      }();
      if (is_crash_reporting_enabled)
      {
        return !!(is_crash_reporting_enabled)();
      }
      return false;
    }

#endif // BUILDFLAG(IS_WIN)

    bool g_crash_reporting_enabled = false;

#if BUILDFLAG(IS_POSIX)
    base::LazyInstance<HoneycombCrashReporterClient>::Leaky g_crash_reporter_client =
        LAZY_INSTANCE_INITIALIZER;

    void InitCrashReporter(const base::CommandLine &command_line,
                           const std::string &process_type)
    {
      HoneycombCrashReporterClient *crash_client = g_crash_reporter_client.Pointer();
      if (!crash_client->HasCrashConfigFile())
      {
        return;
      }

      crash_reporter::SetCrashReporterClient(crash_client);

#if BUILDFLAG(IS_MAC)
      // TODO(mark): Right now, InitializeCrashpad() needs to be called after
      // CommandLine::Init() and configuration of chrome::DIR_CRASH_DUMPS. Ideally,
      // Crashpad initialization could occur sooner, preferably even before the
      // framework dylib is even loaded, to catch potential early crashes.
      crash_reporter::InitializeCrashpad(process_type.empty(), process_type);

      if (base::apple::AmIBundled())
      {
        // Mac Chrome is packaged with a main app bundle and a helper app bundle.
        // The main app bundle should only be used for the browser process, so it
        // should never see a --type switch (switches::kProcessType).  Likewise,
        // the helper should always have a --type switch.
        //
        // This check is done this late so there is already a call to
        // base::apple::IsBackgroundOnlyProcess(), so there is no change in
        // startup/initialization order.

        // The helper's Info.plist marks it as a background only app.
        if (base::apple::IsBackgroundOnlyProcess())
        {
          CHECK(command_line.HasSwitch(switches::kProcessType) &&
                !process_type.empty())
              << "Helper application requires --type.";
        }
        else
        {
          CHECK(!command_line.HasSwitch(switches::kProcessType) &&
                process_type.empty())
              << "Main application forbids --type, saw " << process_type;
        }
      }

      g_crash_reporting_enabled = true;
#else  // !BUILDFLAG(IS_MAC)
      if (process_type != switches::kZygoteProcess)
      {
        // Crash reporting for subprocesses created using the zygote will be
        // initialized in ZygoteForked.
        crash_reporter::InitializeCrashpad(process_type.empty(), process_type);

        g_crash_reporting_enabled = true;
      }
#endif // !BUILDFLAG(IS_MAC)
    }
#endif // BUILDFLAG(IS_POSIX)

    // Used to exclude command-line flags from crash reporting.
    bool IsBoringHoneycombSwitch(const std::string &flag)
    {
      if (crash_keys::IsBoringChromeSwitch(flag))
      {
        return true;
      }

      static const char *const kIgnoreSwitches[] = {
          // Honeycomb internals.
          switches::kLogFile,

          // Chromium internals.
          "content-image-texture-target",
          "mojo-platform-channel-handle",
          "primordial-pipe-token",
          "service-pipe-token",
          "service-request-channel-token",
      };

      if (!base::StartsWith(flag, "--", base::CompareCase::SENSITIVE))
      {
        return false;
      }

      size_t end = flag.find("=");
      size_t len = (end == std::string::npos) ? flag.length() - 2 : end - 2;
      for (size_t i = 0; i < std::size(kIgnoreSwitches); ++i)
      {
        if (flag.compare(2, len, kIgnoreSwitches[i]) == 0)
        {
          return true;
        }
      }
      return false;
    }

  } // namespace

  bool Enabled()
  {
    return g_crash_reporting_enabled;
  }

  bool SetCrashKeyValue(const base::StringPiece &key,
                        const base::StringPiece &value)
  {
    if (!g_crash_reporting_enabled)
    {
      return false;
    }

#if BUILDFLAG(IS_WIN)
    return SetCrashKeyValueTrampoline(key, value);
#else
    return g_crash_reporter_client.Pointer()->SetCrashKeyValue(key, value);
#endif
  }

#if BUILDFLAG(IS_POSIX)
  // Be aware that logging is not initialized at the time this method is called.
  void BasicStartupComplete(base::CommandLine *command_line)
  {
    HoneycombCrashReporterClient *crash_client = g_crash_reporter_client.Pointer();
    if (crash_client->ReadCrashConfigFile())
    {
#if !BUILDFLAG(IS_MAC)
      // Crashpad requires this switch on Linux.
      command_line->AppendSwitch(switches::kEnableCrashpad);
#endif
    }
  }
#endif

  void PreSandboxStartup(const base::CommandLine &command_line,
                         const std::string &process_type)
  {
#if BUILDFLAG(IS_POSIX)
    // Initialize crash reporting here on macOS and Linux. Crash reporting on
    // Windows is initialized from context.cc.
    InitCrashReporter(command_line, process_type);
#elif BUILDFLAG(IS_WIN)
    g_crash_reporting_enabled = IsCrashReportingEnabledTrampoline();
#endif

    if (g_crash_reporting_enabled)
    {
      LOG(INFO) << "Crash reporting enabled for process: "
                << (process_type.empty() ? "browser" : process_type.c_str());
    }

    crash_reporter::InitializeCrashKeys();

    // After platform crash reporting have been initialized, store the command
    // line for crash reporting.
    crash_keys::SetSwitchesFromCommandLine(command_line, &IsBoringHoneycombSwitch);
  }

#if BUILDFLAG(IS_POSIX) && !BUILDFLAG(IS_ANDROID) && !BUILDFLAG(IS_MAC)
  void ZygoteForked(base::CommandLine *command_line,
                    const std::string &process_type)
  {
    HoneycombCrashReporterClient *crash_client = g_crash_reporter_client.Pointer();
    if (crash_client->HasCrashConfigFile())
    {
      // Crashpad requires this switch on Linux.
      command_line->AppendSwitch(switches::kEnableCrashpad);
    }

    InitCrashReporter(*command_line, process_type);

    if (g_crash_reporting_enabled)
    {
      LOG(INFO) << "Crash reporting enabled for process: " << process_type;
    }

    // Reset the command line for the newly spawned process.
    crash_keys::SetSwitchesFromCommandLine(*command_line, &IsBoringHoneycombSwitch);
  }
#endif

} // namespace crash_reporting

bool HoneycombCrashReportingEnabled()
{
  return crash_reporting::Enabled();
}

void HoneycombSetCrashKeyValue(const HoneycombString &key, const HoneycombString &value)
{
  if (!crash_reporting::SetCrashKeyValue(key.ToString(), value.ToString()))
  {
    LOG(WARNING) << "Failed to set crash key: " << key.ToString()
                 << " with value: " << value.ToString();
  }
}

// From libhoney/features/runtime.h:
namespace honey
{

  bool IsCrashReportingEnabled()
  {
    return crash_reporting::Enabled();
  }

} // namespace honey
