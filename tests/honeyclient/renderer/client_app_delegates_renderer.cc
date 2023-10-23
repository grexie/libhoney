// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeyclient/renderer/client_renderer.h"
#include "tests/honeyclient/renderer/ipc_performance_test.h"
#include "tests/honeyclient/renderer/performance_test.h"
#include "tests/shared/renderer/client_app_renderer.h"

namespace client {

// static
void ClientAppRenderer::CreateDelegates(DelegateSet& delegates) {
  renderer::CreateDelegates(delegates);
  performance_test::CreateDelegates(delegates);
  ipc_performance_test::CreateDelegates(delegates);
}

}  // namespace client
