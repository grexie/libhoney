// Copyright (c) 2017 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeyclient/browser/server_test.h"

#include <algorithm>
#include <memory>
#include <string>

#include "include/base/honey_callback.h"
#include "include/base/honey_weak_ptr.h"
#include "include/honey_parser.h"
#include "include/honey_server.h"
#include "include/wrapper/honey_closure_task.h"
#include "tests/shared/browser/resource_util.h"

namespace client {
namespace server_test {

namespace {

// Application-specific error codes.
const int kMessageFormatError = 1;
const int kActionStateError = 1;

// JSON dictionary keys.
const char kActionKey[] = "action";
const char kResultKey[] = "result";
const char kPortKey[] = "port";
const char kStatusKey[] = "status";
const char kMessageKey[] = "message";

// Server default values.
const char kServerAddress[] = "127.0.0.1";
const int kServerPortDefault = 8099;
const int kServerBacklog = 10;
const char kDefaultPath[] = "websocket.html";

// Handles the HTTP/WebSocket server.
class ServerHandler : public HoneycombServerHandler {
 public:
  using CompleteCallback = base::OnceCallback<void(bool /* success */)>;

  ServerHandler() {}

  // |complete_callback| will be executed on the UI thread after completion.
  void StartServer(int port, CompleteCallback complete_callback) {
    HONEYCOMB_REQUIRE_UI_THREAD();
    DCHECK(!server_);
    DCHECK(port >= 1025 && port <= 65535);
    port_ = port;
    complete_callback_ = std::move(complete_callback);
    HoneycombServer::CreateServer(kServerAddress, port, kServerBacklog, this);
  }

  // |complete_callback| will be executed on the UI thread after completion.
  void StopServer(CompleteCallback complete_callback) {
    HONEYCOMB_REQUIRE_UI_THREAD();
    DCHECK(server_);
    complete_callback_ = std::move(complete_callback);
    server_->Shutdown();
  }

  // HoneycombServerHandler methods are called on the server thread.

  void OnServerCreated(HoneycombRefPtr<HoneycombServer> server) override {
    DCHECK(!server_);
    server_ = server;
    RunCompleteCallback(server->IsRunning());
  }

  void OnServerDestroyed(HoneycombRefPtr<HoneycombServer> server) override {
    DCHECK(server_);
    server_ = nullptr;
    RunCompleteCallback(true);
  }

  void OnClientConnected(HoneycombRefPtr<HoneycombServer> server,
                         int connection_id) override {}

  void OnClientDisconnected(HoneycombRefPtr<HoneycombServer> server,
                            int connection_id) override {}

  void OnHttpRequest(HoneycombRefPtr<HoneycombServer> server,
                     int connection_id,
                     const HoneycombString& client_address,
                     HoneycombRefPtr<HoneycombRequest> request) override {
    // Parse the request URL and retrieve the path without leading slash.
    HoneycombURLParts url_parts;
    HoneycombParseURL(request->GetURL(), url_parts);
    std::string path = HoneycombString(&url_parts.path);
    if (!path.empty() && path[0] == '/') {
      path = path.substr(1);
    }

    if (path.empty()) {
      path = kDefaultPath;
    }

    std::string mime_type;
    const size_t sep = path.find_last_of(".");
    if (sep != std::string::npos) {
      // Determine the mime type based on the extension.
      mime_type = HoneycombGetMimeType(path.substr(sep + 1));
    } else {
      // No extension. Assume html.
      path += ".html";
    }
    if (mime_type.empty()) {
      mime_type = "text/html";
    }

    HoneycombRefPtr<HoneycombStreamReader> stream;
    HoneycombResponse::HeaderMap extra_headers;

    if (path == "request.html") {
      // Return the request contents.
      stream = test_runner::GetDumpResponse(request, extra_headers);
    }

    if (!stream) {
      // Load any resource supported by honeyclient.
      stream = GetBinaryResourceReader(path.c_str());
    }

    if (stream) {
      SendHttpResponseStream(server, connection_id, mime_type, stream,
                             extra_headers);
    } else {
      server->SendHttp404Response(connection_id);
    }
  }

  void OnWebSocketRequest(HoneycombRefPtr<HoneycombServer> server,
                          int connection_id,
                          const HoneycombString& client_address,
                          HoneycombRefPtr<HoneycombRequest> request,
                          HoneycombRefPtr<HoneycombCallback> callback) override {
    // Always accept WebSocket connections.
    callback->Continue();
  }

  void OnWebSocketConnected(HoneycombRefPtr<HoneycombServer> server,
                            int connection_id) override {}

  void OnWebSocketMessage(HoneycombRefPtr<HoneycombServer> server,
                          int connection_id,
                          const void* data,
                          size_t data_size) override {
    // Echo the reverse of the message.
    std::string message(static_cast<const char*>(data), data_size);
    std::reverse(message.begin(), message.end());

    server->SendWebSocketMessage(connection_id, message.data(), message.size());
  }

  int port() const { return port_; }

 private:
  void RunCompleteCallback(bool success) {
    if (!HoneycombCurrentlyOn(TID_UI)) {
      HoneycombPostTask(TID_UI, base::BindOnce(&ServerHandler::RunCompleteCallback,
                                         this, success));
      return;
    }

    if (!complete_callback_.is_null()) {
      std::move(complete_callback_).Run(success);
    }
  }

  static void SendHttpResponseStream(HoneycombRefPtr<HoneycombServer> server,
                                     int connection_id,
                                     const std::string& mime_type,
                                     HoneycombRefPtr<HoneycombStreamReader> stream,
                                     HoneycombResponse::HeaderMap extra_headers) {
    // Determine the stream size.
    stream->Seek(0, SEEK_END);
    int64_t content_length = stream->Tell();
    stream->Seek(0, SEEK_SET);

    // Send response headers.
    server->SendHttpResponse(connection_id, 200, mime_type, content_length,
                             extra_headers);

    // Send stream contents.
    char buffer[8192];
    size_t read;
    do {
      read = stream->Read(buffer, 1, sizeof(buffer));
      if (read > 0) {
        server->SendRawData(connection_id, buffer, read);
      }
    } while (!stream->Eof() && read != 0);

    // Close the connection.
    server->CloseConnection(connection_id);
  }

  HoneycombRefPtr<HoneycombServer> server_;

  // The below members are only accessed on the UI thread.
  int port_;
  CompleteCallback complete_callback_;

  IMPLEMENT_REFCOUNTING(ServerHandler);
  DISALLOW_COPY_AND_ASSIGN(ServerHandler);
};

// Handle messages in the browser process.
class Handler : public HoneycombMessageRouterBrowserSide::Handler {
 public:
  Handler() : weak_ptr_factory_(this) {}

  virtual ~Handler() {
    if (handler_) {
      handler_->StopServer(ServerHandler::CompleteCallback());
      handler_ = nullptr;
    }
  }

  // Called due to honeyQuery execution in server.html.
  virtual bool OnQuery(HoneycombRefPtr<HoneycombBrowser> browser,
                       HoneycombRefPtr<HoneycombFrame> frame,
                       int64_t query_id,
                       const HoneycombString& request,
                       bool persistent,
                       HoneycombRefPtr<Callback> callback) override {
    HONEYCOMB_REQUIRE_UI_THREAD();

    // Only handle messages from the test URL.
    const std::string& url = frame->GetURL();
    if (url.find(test_runner::GetTestURL("server")) != 0) {
      return false;
    }

    // Parse |request| as a JSON dictionary.
    HoneycombRefPtr<HoneycombDictionaryValue> request_dict = ParseJSON(request);
    if (!request_dict) {
      callback->Failure(kMessageFormatError, "Incorrect message format");
      return true;
    }

    if (!VerifyKey(request_dict, kActionKey, VTYPE_STRING, callback)) {
      return true;
    }

    const std::string& action = request_dict->GetString(kActionKey);
    if (action == "query") {
      HandleQueryAction(request_dict, callback);
    } else if (action == "start") {
      HandleStartAction(request_dict, callback);
    } else if (action == "stop") {
      HandleStopAction(request_dict, callback);
    } else {
      callback->Failure(kMessageFormatError, "Unrecognized action: " + action);
    }

    return true;
  }

 private:
  // Return current server status.
  void HandleQueryAction(HoneycombRefPtr<HoneycombDictionaryValue> request_dict,
                         HoneycombRefPtr<Callback> callback) {
    HoneycombRefPtr<HoneycombDictionaryValue> result_dict = HoneycombDictionaryValue::Create();
    if (handler_) {
      result_dict->SetInt(kPortKey, handler_->port());
      result_dict->SetString(kStatusKey, "running");
    } else {
      result_dict->SetInt(kPortKey, kServerPortDefault);
      result_dict->SetString(kStatusKey, "stopped");
    }
    SendResponse(callback, true, result_dict);
  }

  // Start the server.
  void HandleStartAction(HoneycombRefPtr<HoneycombDictionaryValue> request_dict,
                         HoneycombRefPtr<Callback> callback) {
    if (handler_) {
      callback->Failure(kActionStateError, "Server is currently running");
      return;
    }

    if (!VerifyKey(request_dict, kPortKey, VTYPE_INT, callback)) {
      return;
    }

    const int port = request_dict->GetInt(kPortKey);
    if (port < 8000 || port > 65535) {
      callback->Failure(kMessageFormatError, "Invalid port number specified");
      return;
    }

    handler_ = new ServerHandler();

    // Start the server. OnComplete will be executed upon completion.
    handler_->StartServer(
        port, base::BindOnce(&Handler::OnStartComplete,
                             weak_ptr_factory_.GetWeakPtr(), callback));
  }

  // Stop the server.
  void HandleStopAction(HoneycombRefPtr<HoneycombDictionaryValue> request_dict,
                        HoneycombRefPtr<Callback> callback) {
    if (!handler_) {
      callback->Failure(kActionStateError, "Server is not currently running");
      return;
    }

    // Stop the server. OnComplete will be executed upon completion.
    handler_->StopServer(base::BindOnce(
        &Handler::OnStopComplete, weak_ptr_factory_.GetWeakPtr(), callback));

    handler_ = nullptr;
  }

  // Server start completed.
  void OnStartComplete(HoneycombRefPtr<Callback> callback, bool success) {
    HONEYCOMB_REQUIRE_UI_THREAD();
    HoneycombRefPtr<HoneycombDictionaryValue> result_dict = HoneycombDictionaryValue::Create();
    if (!success) {
      handler_ = nullptr;
      result_dict->SetString(kMessageKey, "Server failed to start.");
    }
    SendResponse(callback, success, result_dict);
  }

  // Server stop completed.
  void OnStopComplete(HoneycombRefPtr<Callback> callback, bool success) {
    HONEYCOMB_REQUIRE_UI_THREAD();
    HoneycombRefPtr<HoneycombDictionaryValue> result_dict = HoneycombDictionaryValue::Create();
    if (!success) {
      result_dict->SetString(kMessageKey, "Server failed to stop.");
    }
    SendResponse(callback, success, result_dict);
  }

  // Send a response in the format expected by server.html.
  static void SendResponse(HoneycombRefPtr<Callback> callback,
                           bool success,
                           HoneycombRefPtr<HoneycombDictionaryValue> result_dict) {
    if (!result_dict) {
      result_dict = HoneycombDictionaryValue::Create();
    }
    result_dict->SetString(kResultKey, success ? "success" : "failure");
    HoneycombRefPtr<HoneycombValue> value = HoneycombValue::Create();
    value->SetDictionary(result_dict);
    const std::string& response = HoneycombWriteJSON(value, JSON_WRITER_DEFAULT);
    callback->Success(response);
  }

  // Convert a JSON string to a dictionary value.
  static HoneycombRefPtr<HoneycombDictionaryValue> ParseJSON(const HoneycombString& string) {
    HoneycombRefPtr<HoneycombValue> value = HoneycombParseJSON(string, JSON_PARSER_RFC);
    if (value.get() && value->GetType() == VTYPE_DICTIONARY) {
      return value->GetDictionary();
    }
    return nullptr;
  }

  // Verify that |key| exists in |dictionary| and has type |value_type|. Fails
  // |callback| and returns false on failure.
  static bool VerifyKey(HoneycombRefPtr<HoneycombDictionaryValue> dictionary,
                        const char* key,
                        honey_value_type_t value_type,
                        HoneycombRefPtr<Callback> callback) {
    if (!dictionary->HasKey(key) || dictionary->GetType(key) != value_type) {
      callback->Failure(
          kMessageFormatError,
          "Missing or incorrectly formatted message key: " + std::string(key));
      return false;
    }
    return true;
  }

  // Non-nullptr while the server is running.
  HoneycombRefPtr<ServerHandler> handler_;

  // Must be the last member.
  base::WeakPtrFactory<Handler> weak_ptr_factory_;
};

}  // namespace

void CreateMessageHandlers(test_runner::MessageHandlerSet& handlers) {
  handlers.insert(new Handler());
}

}  // namespace server_test
}  // namespace client
