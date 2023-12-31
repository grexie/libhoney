// Copyright (c) 2020 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBTESTS_TEST_SERVER_RUNNER_H_
#define HONEYCOMB_TESTS_HONEYCOMBTESTS_TEST_SERVER_RUNNER_H_
#pragma once

#include <memory>
#include <string>

#include "include/honey_request.h"
#include "include/internal/honey_types.h"
#include "tests/honeytests/test_server.h"

namespace test_server {

// Runs the server. All methods will be called on the UI thread.
class Runner {
 public:
  // Interface implemented by the Manager that creates/owns the Runner.
  class Delegate {
   public:
    // Server created notification.
    virtual void OnServerCreated(const std::string& server_origin) = 0;

    // Server destroyed notification. May delete the Runner.
    virtual void OnServerDestroyed() = 0;

    // Server handler deleted notification. May delete the Manager.
    virtual void OnServerHandlerDeleted() = 0;

    // Server request notification.
    virtual void OnTestServerRequest(
        HoneycombRefPtr<HoneycombRequest> request,
        const ResponseCallback& response_callback) = 0;

   protected:
    virtual ~Delegate() = default;
  };

  // |delegate| will outlive this object.
  explicit Runner(Delegate* delegate);
  virtual ~Runner() = default;

  // Called by the Manager to create the Runner.
  static std::unique_ptr<Runner> Create(Delegate* delegate, bool https_server);

  // Called by the Manager to create/destroy the server handler.
  virtual void StartServer() = 0;
  virtual void ShutdownServer() = 0;

 private:
  // Create a Runner based on HoneycombServer.
  static std::unique_ptr<Runner> CreateNormal(Delegate* delegate);

  // Create a Runner based on HoneycombTestServer.
  static std::unique_ptr<Runner> CreateTest(Delegate* delegate,
                                            bool https_server);

 protected:
  Delegate* const delegate_;

  DISALLOW_COPY_AND_ASSIGN(Runner);
};

}  // namespace test_server

#endif  // HONEYCOMB_TESTS_HONEYCOMBTESTS_TEST_SERVER_RUNNER_H_
