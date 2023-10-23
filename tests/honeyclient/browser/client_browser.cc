// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeyclient/browser/client_browser.h"
#include "tests/honeyclient/browser/main_context.h"

#include "include/base/honey_logging.h"
#include "include/honey_command_line.h"
#include "include/honey_crash_util.h"
#include "include/honey_file_util.h"
#include "tests/honeyclient/browser/client_prefs.h"
#include "tests/honeyclient/browser/default_client_handler.h"
#include "tests/shared/common/client_switches.h"

namespace client {
namespace browser {

namespace {

class ClientBrowserDelegate : public ClientAppBrowser::Delegate {
 public:
  ClientBrowserDelegate() {}

  void OnRegisterCustomPreferences(
      HoneycombRefPtr<client::ClientAppBrowser> app,
      honey_preferences_type_t type,
      HoneycombRawPtr<HoneycombPreferenceRegistrar> registrar) override {
    if (type == HONEYCOMB_PREFERENCES_TYPE_GLOBAL) {
      // Register global preferences with default values.
      prefs::RegisterGlobalPreferences(registrar);
    }
  }

  void OnContextInitialized(HoneycombRefPtr<ClientAppBrowser> app) override {
    if (HoneycombCrashReportingEnabled()) {
      // Set some crash keys for testing purposes. Keys must be defined in the
      // "crash_reporter.cfg" file. See honey_crash_util.h for details.
      HoneycombSetCrashKeyValue("testkey_small1", "value1_small_browser");
      HoneycombSetCrashKeyValue("testkey_small2", "value2_small_browser");
      HoneycombSetCrashKeyValue("testkey_medium1", "value1_medium_browser");
      HoneycombSetCrashKeyValue("testkey_medium2", "value2_medium_browser");
      HoneycombSetCrashKeyValue("testkey_large1", "value1_large_browser");
      HoneycombSetCrashKeyValue("testkey_large2", "value2_large_browser");
    }

    const std::string& crl_sets_path =
        HoneycombCommandLine::GetGlobalCommandLine()->GetSwitchValue(
            switches::kCRLSetsPath);
    if (!crl_sets_path.empty()) {
      // Load the CRLSets file from the specified path.
      HoneycombLoadCRLSetsFile(crl_sets_path);
    }
  }

  void OnBeforeCommandLineProcessing(
      HoneycombRefPtr<ClientAppBrowser> app,
      HoneycombRefPtr<HoneycombCommandLine> command_line) override {
    // Append Chromium command line parameters if touch events are enabled
    if (client::MainContext::Get()->TouchEventsEnabled()) {
      command_line->AppendSwitchWithValue("touch-events", "enabled");
    }
  }

  HoneycombRefPtr<HoneycombClient> GetDefaultClient(
      HoneycombRefPtr<ClientAppBrowser> app) override {
    // Default client handler for unmanaged browser windows. Used with the
    // Chrome runtime only.
    LOG(INFO) << "Creating a chrome browser with the default client";
    return new DefaultClientHandler();
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(ClientBrowserDelegate);
  IMPLEMENT_REFCOUNTING(ClientBrowserDelegate);
};

}  // namespace

void CreateDelegates(ClientAppBrowser::DelegateSet& delegates) {
  delegates.insert(new ClientBrowserDelegate);
}

}  // namespace browser
}  // namespace client
