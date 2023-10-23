// Copyright (c) 2020 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeytests/test_server_runner.h"

#include "include/honey_server.h"
#include "include/wrapper/honey_closure_task.h"
#include "include/wrapper/honey_helpers.h"
#include "tests/gtest/include/gtest/gtest.h"

namespace test_server {

namespace {

const int kHttpServerBacklog = 10;

// Created on the UI thread and called on the dedicated server thread.
class ServerHandler : public HoneycombServerHandler {
 public:
  explicit ServerHandler(Runner::Delegate* delegate) : delegate_(delegate) {}

  ~ServerHandler() override {
    HONEYCOMB_REQUIRE_UI_THREAD();
    EXPECT_FALSE(server_);
    delegate_->OnServerHandlerDeleted();
  }

  void Shutdown() { server_->Shutdown(); }

  // HoneycombServerHandler methods:
  void OnServerCreated(HoneycombRefPtr<HoneycombServer> server) override {
    EXPECT_TRUE(server->GetTaskRunner()->BelongsToCurrentThread());
    server_ = server;
    NotifyServerCreated("http://" + server->GetAddress().ToString());
  }

  void OnServerDestroyed(HoneycombRefPtr<HoneycombServer> server) override {
    EXPECT_TRUE(server->GetTaskRunner()->BelongsToCurrentThread());
    server_ = nullptr;
    NotifyServerDestroyed();
  }

  void OnClientConnected(HoneycombRefPtr<HoneycombServer> server,
                         int connection_id) override {
    EXPECT_TRUE(server->GetTaskRunner()->BelongsToCurrentThread());
    EXPECT_TRUE(server->HasConnection());
    EXPECT_TRUE(server->IsValidConnection(connection_id));
  }

  void OnClientDisconnected(HoneycombRefPtr<HoneycombServer> server,
                            int connection_id) override {
    EXPECT_TRUE(server->GetTaskRunner()->BelongsToCurrentThread());
    EXPECT_FALSE(server->IsValidConnection(connection_id));
  }

  void OnHttpRequest(HoneycombRefPtr<HoneycombServer> server,
                     int connection_id,
                     const HoneycombString& client_address,
                     HoneycombRefPtr<HoneycombRequest> request) override {
    EXPECT_TRUE(server->GetTaskRunner()->BelongsToCurrentThread());
    NotifyTestServerRequest(server, connection_id, client_address, request);
  }

  void OnWebSocketRequest(HoneycombRefPtr<HoneycombServer> server,
                          int connection_id,
                          const HoneycombString& client_address,
                          HoneycombRefPtr<HoneycombRequest> request,
                          HoneycombRefPtr<HoneycombCallback> callback) override {}
  void OnWebSocketConnected(HoneycombRefPtr<HoneycombServer> server,
                            int connection_id) override {}
  void OnWebSocketMessage(HoneycombRefPtr<HoneycombServer> server,
                          int connection_id,
                          const void* data,
                          size_t data_size) override {}

 private:
  void NotifyServerCreated(const std::string& server_origin) {
    if (!HoneycombCurrentlyOn(TID_UI)) {
      HoneycombPostTask(TID_UI, base::BindOnce(&ServerHandler::NotifyServerCreated,
                                         this, server_origin));
      return;
    }

    delegate_->OnServerCreated(server_origin);
  }

  void NotifyServerDestroyed() {
    if (!HoneycombCurrentlyOn(TID_UI)) {
      HoneycombPostTask(TID_UI,
                  base::BindOnce(&ServerHandler::NotifyServerDestroyed, this));
      return;
    }

    delegate_->OnServerDestroyed();
  }

  void NotifyTestServerRequest(HoneycombRefPtr<HoneycombServer> server,
                               int connection_id,
                               const HoneycombString& client_address,
                               HoneycombRefPtr<HoneycombRequest> request) {
    if (!HoneycombCurrentlyOn(TID_UI)) {
      HoneycombPostTask(TID_UI, base::BindOnce(
                              &ServerHandler::NotifyTestServerRequest, this,
                              server, connection_id, client_address, request));
      return;
    }

    auto response_callback = base::BindRepeating(&ServerHandler::SendResponse,
                                                 server, connection_id);
    delegate_->OnTestServerRequest(request, response_callback);
  }

  static void SendResponse(HoneycombRefPtr<HoneycombServer> server,
                           int connection_id,
                           HoneycombRefPtr<HoneycombResponse> response,
                           const std::string& response_data) {
    // Execute on the server thread because some methods require it.
    HoneycombRefPtr<HoneycombTaskRunner> task_runner = server->GetTaskRunner();
    if (!task_runner->BelongsToCurrentThread()) {
      task_runner->PostTask(HoneycombCreateClosureTask(
          base::BindOnce(ServerHandler::SendResponse, server, connection_id,
                         response, response_data)));
      return;
    }

    if (!server->IsValidConnection(connection_id)) {
      // This can occur if the connected browser has already closed.
      return;
    }

    const int response_code = response->GetStatus();
    if (response_code <= 0) {
      // Intentionally not responding for incomplete request tests.
      return;
    }

    const HoneycombString& content_type = response->GetMimeType();
    int64_t content_length = static_cast<int64_t>(response_data.size());

    HoneycombResponse::HeaderMap extra_headers;
    response->GetHeaderMap(extra_headers);

    server->SendHttpResponse(connection_id, response_code, content_type,
                             content_length, extra_headers);

    if (response_data == kIncompleteDoNotSendData) {
      // Intentionally not sending data for incomplete request tests.
      return;
    }

    if (content_length != 0) {
      server->SendRawData(connection_id, response_data.data(),
                          response_data.size());
      server->CloseConnection(connection_id);
    }

    // The connection should be closed.
    EXPECT_FALSE(server->IsValidConnection(connection_id));
  }

  Runner::Delegate* const delegate_;
  HoneycombRefPtr<HoneycombServer> server_;

  IMPLEMENT_REFCOUNTING(ServerHandler);
  DISALLOW_COPY_AND_ASSIGN(ServerHandler);
};

class ServerRunner : public Runner {
 public:
  explicit ServerRunner(Delegate* delegate) : Runner(delegate) {}

  void StartServer() override {
    HONEYCOMB_REQUIRE_UI_THREAD();
    DCHECK(!handler_);
    handler_ = new ServerHandler(delegate_);
    HoneycombServer::CreateServer(kHttpServerAddress, kHttpServerPort,
                            kHttpServerBacklog, handler_);
  }

  void ShutdownServer() override {
    HONEYCOMB_REQUIRE_UI_THREAD();
    DCHECK(handler_);
    handler_->Shutdown();
    handler_ = nullptr;
  }

 private:
  HoneycombRefPtr<ServerHandler> handler_;
};

}  // namespace

std::unique_ptr<Runner> Runner::CreateNormal(Delegate* delegate) {
  return std::make_unique<ServerRunner>(delegate);
}

}  // namespace test_server
