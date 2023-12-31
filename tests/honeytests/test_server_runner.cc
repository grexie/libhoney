// Copyright (c) 2020 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeytests/test_server_runner.h"

#include "tests/shared/common/client_switches.h"

#include "include/base/honey_logging.h"
#include "include/honey_command_line.h"

namespace test_server {

Runner::Runner(Delegate* delegate) : delegate_(delegate) {
  DCHECK(delegate_);
}

// static
std::unique_ptr<Runner> Runner::Create(Runner::Delegate* delegate,
                                       bool https_server) {
  static bool use_test_http_server = [] {
    auto command_line = HoneycombCommandLine::GetGlobalCommandLine();
    return command_line->HasSwitch(client::switches::kUseTestHttpServer);
  }();

  if (https_server || use_test_http_server) {
    return CreateTest(delegate, https_server);
  }

  return CreateNormal(delegate);
}

}  // namespace test_server
