// Copyright (c) 2017 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_SERVER_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_SERVER_IMPL_H_
#pragma once

#include <map>
#include <memory>

#include "include/honey_server.h"

#include "base/task/single_thread_task_runner.h"
#include "net/server/http_server.h"

namespace base {
class Thread;
}

class HoneycombServerImpl : public HoneycombServer, net::HttpServer::Delegate {
 public:
  explicit HoneycombServerImpl(HoneycombRefPtr<HoneycombServerHandler> handler);

  HoneycombServerImpl(const HoneycombServerImpl&) = delete;
  HoneycombServerImpl& operator=(const HoneycombServerImpl&) = delete;

  void Start(const std::string& address, uint16_t port, int backlog);

  // HoneycombServer methods:
  HoneycombRefPtr<HoneycombTaskRunner> GetTaskRunner() override;
  void Shutdown() override;
  bool IsRunning() override;
  HoneycombString GetAddress() override;
  bool HasConnection() override;
  bool IsValidConnection(int connection_id) override;
  void SendHttp200Response(int connection_id,
                           const HoneycombString& content_type,
                           const void* data,
                           size_t data_size) override;
  void SendHttp404Response(int connection_id) override;
  void SendHttp500Response(int connection_id,
                           const HoneycombString& error_message) override;
  void SendHttpResponse(int connection_id,
                        int response_code,
                        const HoneycombString& content_type,
                        int64_t content_length,
                        const HeaderMap& extra_headers) override;
  void SendRawData(int connection_id,
                   const void* data,
                   size_t data_size) override;
  void CloseConnection(int connection_id) override;
  void SendWebSocketMessage(int connection_id,
                            const void* data,
                            size_t data_size) override;

  void ContinueWebSocketRequest(int connection_id,
                                const net::HttpServerRequestInfo& request_info,
                                bool allow);

 private:
  void SendHttp200ResponseInternal(int connection_id,
                                   const HoneycombString& content_type,
                                   std::unique_ptr<std::string> data);
  void SendRawDataInternal(int connection_id,
                           std::unique_ptr<std::string> data);
  void SendWebSocketMessageInternal(int connection_id,
                                    std::unique_ptr<std::string> data);

  // HttpServer::Delegate methods:
  void OnConnect(int connection_id) override;
  void OnHttpRequest(int connection_id,
                     const net::HttpServerRequestInfo& request_info) override;
  void OnWebSocketRequest(
      int connection_id,
      const net::HttpServerRequestInfo& request_info) override;
  void OnWebSocketMessage(int connection_id, std::string data) override;
  void OnClose(int connection_id) override;

  void StartOnUIThread(const std::string& address, uint16_t port, int backlog);
  void StartOnHandlerThread(const std::string& address,
                            uint16_t port,
                            int backlog);

  void ShutdownOnHandlerThread();
  void ShutdownOnUIThread();

  bool ValidateServer() const;

  struct ConnectionInfo;
  ConnectionInfo* CreateConnectionInfo(int connection_id);
  ConnectionInfo* GetConnectionInfo(int connection_id) const;
  void RemoveConnectionInfo(int connection_id);

  bool CurrentlyOnHandlerThread() const;

  // Safe to access from any thread.
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  std::string address_;

  // Only accessed on the UI thread.
  std::unique_ptr<base::Thread> thread_;

  // Only accessed on the server thread.
  HoneycombRefPtr<HoneycombServerHandler> handler_;
  std::unique_ptr<net::HttpServer> server_;

  // Map of connection_id to ConnectionInfo.
  using ConnectionInfoMap = std::map<int, std::unique_ptr<ConnectionInfo>>;
  ConnectionInfoMap connection_info_map_;

  IMPLEMENT_REFCOUNTING(HoneycombServerImpl);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_SERVER_IMPL_H_
