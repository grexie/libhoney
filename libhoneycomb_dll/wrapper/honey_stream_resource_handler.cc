// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/wrapper/honey_stream_resource_handler.h"

#include <algorithm>

#include "include/base/honey_logging.h"
#include "include/honey_task.h"
#include "include/wrapper/honey_helpers.h"

HoneycombStreamResourceHandler::HoneycombStreamResourceHandler(
    const HoneycombString& mime_type,
    HoneycombRefPtr<HoneycombStreamReader> stream)
    : status_code_(200),
      status_text_("OK"),
      mime_type_(mime_type),
      stream_(stream) {
  DCHECK(!mime_type_.empty());
}

HoneycombStreamResourceHandler::HoneycombStreamResourceHandler(
    int status_code,
    const HoneycombString& status_text,
    const HoneycombString& mime_type,
    HoneycombResponse::HeaderMap header_map,
    HoneycombRefPtr<HoneycombStreamReader> stream)
    : status_code_(status_code),
      status_text_(status_text),
      mime_type_(mime_type),
      header_map_(header_map),
      stream_(stream) {
  DCHECK(!mime_type_.empty());
}

bool HoneycombStreamResourceHandler::Open(HoneycombRefPtr<HoneycombRequest> request,
                                    bool& handle_request,
                                    HoneycombRefPtr<HoneycombCallback> callback) {
  DCHECK(!HoneycombCurrentlyOn(TID_UI) && !HoneycombCurrentlyOn(TID_IO));

  // Continue the request immediately.
  handle_request = true;
  return true;
}

void HoneycombStreamResourceHandler::GetResponseHeaders(
    HoneycombRefPtr<HoneycombResponse> response,
    int64_t& response_length,
    HoneycombString& redirectUrl) {
  HONEYCOMB_REQUIRE_IO_THREAD();

  response->SetStatus(status_code_);
  response->SetStatusText(status_text_);
  response->SetMimeType(mime_type_);

  if (!header_map_.empty()) {
    response->SetHeaderMap(header_map_);
  }

  response_length = stream_ ? -1 : 0;
}

bool HoneycombStreamResourceHandler::Read(
    void* data_out,
    int bytes_to_read,
    int& bytes_read,
    HoneycombRefPtr<HoneycombResourceReadCallback> callback) {
  DCHECK(!HoneycombCurrentlyOn(TID_UI) && !HoneycombCurrentlyOn(TID_IO));
  DCHECK_GT(bytes_to_read, 0);
  DCHECK(stream_);

  // Read until the buffer is full or until Read() returns 0 to indicate no
  // more data.
  bytes_read = 0;
  int read = 0;
  do {
    read = static_cast<int>(
        stream_->Read(static_cast<char*>(data_out) + bytes_read, 1,
                      bytes_to_read - bytes_read));
    bytes_read += read;
  } while (read != 0 && bytes_read < bytes_to_read);

  return (bytes_read > 0);
}

void HoneycombStreamResourceHandler::Cancel() {}
