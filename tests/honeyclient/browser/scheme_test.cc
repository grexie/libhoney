// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeyclient/browser/scheme_test.h"

#include <algorithm>
#include <string>

#include "include/honey_browser.h"
#include "include/honey_callback.h"
#include "include/honey_frame.h"
#include "include/honey_request.h"
#include "include/honey_resource_handler.h"
#include "include/honey_response.h"
#include "include/honey_scheme.h"
#include "include/wrapper/honey_helpers.h"
#include "tests/honeyclient/browser/test_runner.h"
#include "tests/shared/browser/resource_util.h"

namespace client {
namespace scheme_test {

namespace {

// Implementation of the schema handler for client:// requests.
class ClientSchemeHandler : public HoneycombResourceHandler {
 public:
  ClientSchemeHandler() : offset_(0) {}

  bool Open(HoneycombRefPtr<HoneycombRequest> request,
            bool& handle_request,
            HoneycombRefPtr<HoneycombCallback> callback) override {
    DCHECK(!HoneycombCurrentlyOn(TID_UI) && !HoneycombCurrentlyOn(TID_IO));

    // The request will be continued or canceled based on the return value.
    handle_request = true;

    bool handled = false;

    std::string url = request->GetURL();
    if (strstr(url.c_str(), "handler.html") != nullptr) {
      // Build the response html
      data_ =
          "<html><head><title>Client Scheme Handler</title></head>"
          "<body bgcolor=\"white\">"
          "This contents of this page page are served by the "
          "ClientSchemeHandler class handling the client:// protocol."
          "<br/>You should see an image:"
          "<br/><img src=\"client://tests/logo.png\"><pre>";

      // Output a string representation of the request
      const std::string& dump = test_runner::DumpRequestContents(request);
      data_.append(dump);

      data_.append(
          "</pre><br/>Try the test form:"
          "<form method=\"POST\" action=\"handler.html\">"
          "<input type=\"text\" name=\"field1\">"
          "<input type=\"text\" name=\"field2\">"
          "<input type=\"submit\">"
          "</form></body></html>");

      handled = true;

      // Set the resulting mime type
      mime_type_ = "text/html";
    } else if (strstr(url.c_str(), "logo.png") != nullptr) {
      // Load the response image
      if (LoadBinaryResource("logo.png", data_)) {
        handled = true;
        // Set the resulting mime type
        mime_type_ = "image/png";
      }
    }

    return handled;
  }

  void GetResponseHeaders(HoneycombRefPtr<HoneycombResponse> response,
                          int64_t& response_length,
                          HoneycombString& redirectUrl) override {
    HONEYCOMB_REQUIRE_IO_THREAD();

    DCHECK(!data_.empty());

    response->SetMimeType(mime_type_);
    response->SetStatus(200);

    // Set the resulting response length
    response_length = data_.length();
  }

  void Cancel() override { HONEYCOMB_REQUIRE_IO_THREAD(); }

  bool Read(void* data_out,
            int bytes_to_read,
            int& bytes_read,
            HoneycombRefPtr<HoneycombResourceReadCallback> callback) override {
    DCHECK(!HoneycombCurrentlyOn(TID_UI) && !HoneycombCurrentlyOn(TID_IO));

    bool has_data = false;
    bytes_read = 0;

    if (offset_ < data_.length()) {
      // Copy the next block of data into the buffer.
      int transfer_size =
          std::min(bytes_to_read, static_cast<int>(data_.length() - offset_));
      memcpy(data_out, data_.c_str() + offset_, transfer_size);
      offset_ += transfer_size;

      bytes_read = transfer_size;
      has_data = true;
    }

    return has_data;
  }

 private:
  std::string data_;
  std::string mime_type_;
  size_t offset_;

  IMPLEMENT_REFCOUNTING(ClientSchemeHandler);
  DISALLOW_COPY_AND_ASSIGN(ClientSchemeHandler);
};

// Implementation of the factory for for creating schema handlers.
class ClientSchemeHandlerFactory : public HoneycombSchemeHandlerFactory {
 public:
  ClientSchemeHandlerFactory() {}

  // Return a new scheme handler instance to handle the request.
  HoneycombRefPtr<HoneycombResourceHandler> Create(HoneycombRefPtr<HoneycombBrowser> browser,
                                       HoneycombRefPtr<HoneycombFrame> frame,
                                       const HoneycombString& scheme_name,
                                       HoneycombRefPtr<HoneycombRequest> request) override {
    HONEYCOMB_REQUIRE_IO_THREAD();
    return new ClientSchemeHandler();
  }

  IMPLEMENT_REFCOUNTING(ClientSchemeHandlerFactory);
  DISALLOW_COPY_AND_ASSIGN(ClientSchemeHandlerFactory);
};

}  // namespace

void RegisterSchemeHandlers() {
  HoneycombRegisterSchemeHandlerFactory("client", "tests",
                                  new ClientSchemeHandlerFactory());
}

}  // namespace scheme_test
}  // namespace client
