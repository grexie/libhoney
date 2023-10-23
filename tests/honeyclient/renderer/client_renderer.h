// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_RENDERER_CLIENT_RENDERER_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_RENDERER_CLIENT_RENDERER_H_
#pragma once

#include "include/honey_base.h"
#include "tests/shared/renderer/client_app_renderer.h"

namespace client {
namespace renderer {

// Create the renderer delegate. Called from client_app_delegates_renderer.cc.
void CreateDelegates(ClientAppRenderer::DelegateSet& delegates);

}  // namespace renderer
}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_RENDERER_CLIENT_RENDERER_H_
