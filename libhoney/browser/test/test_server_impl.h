// Copyright 2022 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_TEST_TEST_SERVER_IMPL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_TEST_TEST_SERVER_IMPL_H_
#pragma once

#include <memory>

#include "include/test/honey_test_server.h"

class HoneycombTestServerImpl : public HoneycombTestServer {
 public:
  HoneycombTestServerImpl() = default;

  HoneycombTestServerImpl(const HoneycombTestServerImpl&) = delete;
  HoneycombTestServerImpl& operator=(const HoneycombTestServerImpl&) = delete;

  bool Start(uint16_t port,
             bool https_server,
             honey_test_cert_type_t https_cert_type,
             HoneycombRefPtr<HoneycombTestServerHandler> handler);

  // HoneycombTestServer methods:
  void Stop() override;
  HoneycombString GetOrigin() override;

 private:
  // Only accessed on the creation thread.
  class Context;
  std::unique_ptr<Context> context_;

  // Safe to access on any thread.
  HoneycombString origin_;

  IMPLEMENT_REFCOUNTING(HoneycombTestServerImpl);
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_TEST_TEST_SERVER_IMPL_H_
