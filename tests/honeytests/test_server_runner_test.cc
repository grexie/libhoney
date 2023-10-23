// Copyright (c) 2022 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeytests/test_server_runner.h"

#include "include/test/honey_test_server.h"
#include "include/wrapper/honey_closure_task.h"
#include "include/wrapper/honey_helpers.h"
#include "tests/gtest/include/gtest/gtest.h"

namespace test_server {

namespace {

// Created on the UI thread and called on the dedicated server thread.
class ServerHandler : public HoneycombTestServerHandler {
 public:
  ServerHandler(Runner::Delegate* delegate, bool https_server)
      : delegate_(delegate), https_server_(https_server) {}

  ~ServerHandler() override {
    HONEYCOMB_REQUIRE_UI_THREAD();

    EXPECT_FALSE(server_);
    delegate_->OnServerHandlerDeleted();
  }

  void Start() {
    HONEYCOMB_REQUIRE_UI_THREAD();

    // Use any available port number for HTTPS and the legacy port number for
    // HTTP.
    const uint16_t port = https_server_ ? 0 : kHttpServerPort;

    // Use a "localhost" domain certificate instead of IP address. This is
    // required for HSTS tests (see https://crbug.com/456712).
    const auto cert_type = HONEYCOMB_TEST_CERT_OK_DOMAIN;

    server_ =
        HoneycombTestServer::CreateAndStart(port, https_server_, cert_type, this);

    // Always execute asynchronously.
    HoneycombPostTask(TID_UI, base::BindOnce(&ServerHandler::NotifyServerCreated,
                                       this, server_->GetOrigin()));
  }

  void Shutdown() {
    HONEYCOMB_REQUIRE_UI_THREAD();

    server_->Stop();
    server_ = nullptr;

    // Always execute asynchronously.
    HoneycombPostTask(TID_UI,
                base::BindOnce(&ServerHandler::NotifyServerDestroyed, this));
  }

  bool OnTestServerRequest(
      HoneycombRefPtr<HoneycombTestServer> server,
      HoneycombRefPtr<HoneycombRequest> request,
      HoneycombRefPtr<HoneycombTestServerConnection> connection) override {
    NotifyTestServerRequest(request, connection);
    return true;
  }

 private:
  void NotifyServerCreated(const std::string& server_origin) {
    HONEYCOMB_REQUIRE_UI_THREAD();
    delegate_->OnServerCreated(server_origin);
  }

  void NotifyServerDestroyed() {
    HONEYCOMB_REQUIRE_UI_THREAD();
    delegate_->OnServerDestroyed();
  }

  void NotifyTestServerRequest(HoneycombRefPtr<HoneycombRequest> request,
                               HoneycombRefPtr<HoneycombTestServerConnection> connection) {
    if (!HoneycombCurrentlyOn(TID_UI)) {
      HoneycombPostTask(TID_UI,
                  base::BindOnce(&ServerHandler::NotifyTestServerRequest, this,
                                 request, connection));
      return;
    }

    auto response_callback =
        base::BindRepeating(&ServerHandler::SendResponse, connection);
    delegate_->OnTestServerRequest(request, response_callback);
  }

  static void SendResponse(HoneycombRefPtr<HoneycombTestServerConnection> connection,
                           HoneycombRefPtr<HoneycombResponse> response,
                           const std::string& response_data) {
    const int response_code = response->GetStatus();
    if (response_code <= 0) {
      // Intentionally not responding for incomplete request tests.
      return;
    }

    // Incomplete response data is not supported.
    EXPECT_NE(kIncompleteDoNotSendData, response_data.c_str());

    const HoneycombString& content_type = response->GetMimeType();
    EXPECT_TRUE(!content_type.empty());

    HoneycombResponse::HeaderMap extra_headers;
    response->GetHeaderMap(extra_headers);

    connection->SendHttpResponse(response_code, content_type,
                                 response_data.c_str(), response_data.size(),
                                 extra_headers);
  }

  Runner::Delegate* const delegate_;
  const bool https_server_;

  HoneycombRefPtr<HoneycombTestServer> server_;

  IMPLEMENT_REFCOUNTING(ServerHandler);
  DISALLOW_COPY_AND_ASSIGN(ServerHandler);
};

class ServerRunner : public Runner {
 public:
  ServerRunner(Delegate* delegate, bool https_server)
      : Runner(delegate), https_server_(https_server) {}

  void StartServer() override {
    HONEYCOMB_REQUIRE_UI_THREAD();
    DCHECK(!handler_);
    handler_ = new ServerHandler(delegate_, https_server_);
    handler_->Start();
  }

  void ShutdownServer() override {
    HONEYCOMB_REQUIRE_UI_THREAD();
    DCHECK(handler_);
    handler_->Shutdown();
    handler_ = nullptr;
  }

 private:
  const bool https_server_;
  HoneycombRefPtr<ServerHandler> handler_;
};

}  // namespace

std::unique_ptr<Runner> Runner::CreateTest(Delegate* delegate,
                                           bool https_server) {
  return std::make_unique<ServerRunner>(delegate, https_server);
}

}  // namespace test_server
