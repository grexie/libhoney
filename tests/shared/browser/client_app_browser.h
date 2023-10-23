// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_SHARED_BROWSER_CLIENT_APP_BROWSER_H_
#define HONEYCOMB_TESTS_SHARED_BROWSER_CLIENT_APP_BROWSER_H_
#pragma once

#include <set>

#include "tests/shared/common/client_app.h"

namespace client {

// Client app implementation for the browser process.
class ClientAppBrowser : public ClientApp, public HoneycombBrowserProcessHandler {
 public:
  // Interface for browser delegates. All Delegates must be returned via
  // CreateDelegates. Do not perform work in the Delegate
  // constructor. See HoneycombBrowserProcessHandler for documentation.
  class Delegate : public virtual HoneycombBaseRefCounted {
   public:
    virtual void OnBeforeCommandLineProcessing(
        HoneycombRefPtr<ClientAppBrowser> app,
        HoneycombRefPtr<HoneycombCommandLine> command_line) {}

    virtual void OnRegisterCustomPreferences(
        HoneycombRefPtr<ClientAppBrowser> app,
        honey_preferences_type_t type,
        HoneycombRawPtr<HoneycombPreferenceRegistrar> registrar) {}

    virtual void OnContextInitialized(HoneycombRefPtr<ClientAppBrowser> app) {}

    virtual void OnBeforeChildProcessLaunch(
        HoneycombRefPtr<ClientAppBrowser> app,
        HoneycombRefPtr<HoneycombCommandLine> command_line) {}

    virtual HoneycombRefPtr<HoneycombClient> GetDefaultClient(
        HoneycombRefPtr<ClientAppBrowser> app) {
      return nullptr;
    }
  };

  typedef std::set<HoneycombRefPtr<Delegate>> DelegateSet;

  ClientAppBrowser();

  // Called to populate |settings| based on |command_line| and other global
  // state.
  static void PopulateSettings(HoneycombRefPtr<HoneycombCommandLine> command_line,
                               HoneycombSettings& settings);

 private:
  // Registers cookieable schemes. Implemented by honeyclient in
  // client_app_delegates_browser.cc
  static void RegisterCookieableSchemes(
      std::vector<std::string>& cookieable_schemes);

  // Creates all of the Delegate objects. Implemented by honeyclient in
  // client_app_delegates_browser.cc
  static void CreateDelegates(DelegateSet& delegates);

  // HoneycombApp methods.
  void OnBeforeCommandLineProcessing(
      const HoneycombString& process_type,
      HoneycombRefPtr<HoneycombCommandLine> command_line) override;
  HoneycombRefPtr<HoneycombBrowserProcessHandler> GetBrowserProcessHandler() override {
    return this;
  }

  // HoneycombBrowserProcessHandler methods.
  void OnRegisterCustomPreferences(
      honey_preferences_type_t type,
      HoneycombRawPtr<HoneycombPreferenceRegistrar> registrar) override;
  void OnContextInitialized() override;
  void OnBeforeChildProcessLaunch(
      HoneycombRefPtr<HoneycombCommandLine> command_line) override;
  void OnScheduleMessagePumpWork(int64_t delay) override;
  HoneycombRefPtr<HoneycombClient> GetDefaultClient() override;

  // Set of supported Delegates.
  DelegateSet delegates_;

  IMPLEMENT_REFCOUNTING(ClientAppBrowser);
  DISALLOW_COPY_AND_ASSIGN(ClientAppBrowser);
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_SHARED_BROWSER_CLIENT_APP_BROWSER_H_
