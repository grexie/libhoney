// Copyright (c) 2022 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_RENDERER_IPC_PERFORMANCE_TEST_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_RENDERER_IPC_PERFORMANCE_TEST_H_
#pragma once

#include "tests/shared/renderer/client_app_renderer.h"

namespace client {
namespace ipc_performance_test {

void CreateDelegates(ClientAppRenderer::DelegateSet& delegates);

}  // namespace ipc_performance_test
}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_RENDERER_IPC_PERFORMANCE_TEST_H_
