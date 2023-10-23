// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_SHARED_COMMON_CLIENT_APP_H_
#define HONEYCOMB_TESTS_SHARED_COMMON_CLIENT_APP_H_
#pragma once

#include <vector>

#include "include/honey_app.h"

namespace client {

// Base class for customizing process-type-based behavior.
class ClientApp : public HoneycombApp {
 public:
  ClientApp();

  enum ProcessType {
    BrowserProcess,
    RendererProcess,
    ZygoteProcess,
    OtherProcess,
  };

  // Determine the process type based on command-line arguments.
  static ProcessType GetProcessType(HoneycombRefPtr<HoneycombCommandLine> command_line);

 private:
  // Registers custom schemes. Implemented by honeyclient in
  // client_app_delegates_common.cc
  static void RegisterCustomSchemes(HoneycombRawPtr<HoneycombSchemeRegistrar> registrar);

  // HoneycombApp methods.
  void OnRegisterCustomSchemes(
      HoneycombRawPtr<HoneycombSchemeRegistrar> registrar) override;

  DISALLOW_COPY_AND_ASSIGN(ClientApp);
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_SHARED_COMMON_CLIENT_APP_H_
