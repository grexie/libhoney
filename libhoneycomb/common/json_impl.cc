// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "include/honey_parser.h"
#include "libhoneycomb/common/values_impl.h"

#include "base/json/json_reader.h"
#include "base/json/json_writer.h"
#include "base/values.h"

namespace {

int GetJSONReaderOptions(honey_json_parser_options_t options) {
  int op = base::JSON_PARSE_RFC;
  if (options & JSON_PARSER_ALLOW_TRAILING_COMMAS) {
    op |= base::JSON_ALLOW_TRAILING_COMMAS;
  }
  return op;
}

int GetJSONWriterOptions(honey_json_writer_options_t options) {
  int op = 0;
  if (options & JSON_WRITER_OMIT_BINARY_VALUES) {
    op |= base::JSONWriter::OPTIONS_OMIT_BINARY_VALUES;
  }
  if (options & JSON_WRITER_OMIT_DOUBLE_TYPE_PRESERVATION) {
    op |= base::JSONWriter::OPTIONS_OMIT_DOUBLE_TYPE_PRESERVATION;
  }
  if (options & JSON_WRITER_PRETTY_PRINT) {
    op |= base::JSONWriter::OPTIONS_PRETTY_PRINT;
  }
  return op;
}

}  // namespace

HoneycombRefPtr<HoneycombValue> HoneycombParseJSON(const HoneycombString& json_string,
                                 honey_json_parser_options_t options) {
  const std::string& json = json_string.ToString();
  return HoneycombParseJSON(json.data(), json.size(), options);
}

HoneycombRefPtr<HoneycombValue> HoneycombParseJSON(const void* json,
                                 size_t json_size,
                                 honey_json_parser_options_t options) {
  if (!json || json_size == 0) {
    return nullptr;
  }
  absl::optional<base::Value> parse_result = base::JSONReader::Read(
      base::StringPiece(static_cast<const char*>(json), json_size),
      GetJSONReaderOptions(options));
  if (parse_result) {
    return new HoneycombValueImpl(std::move(parse_result.value()));
  }
  return nullptr;
}

HoneycombRefPtr<HoneycombValue> HoneycombParseJSONAndReturnError(
    const HoneycombString& json_string,
    honey_json_parser_options_t options,
    HoneycombString& error_msg_out) {
  const std::string& json = json_string.ToString();

  std::string error_msg;
  auto result = base::JSONReader::ReadAndReturnValueWithError(
      json, GetJSONReaderOptions(options));
  if (result.has_value()) {
    return new HoneycombValueImpl(std::move(*result));
  }

  error_msg_out = result.error().message;
  return nullptr;
}

HoneycombString HoneycombWriteJSON(HoneycombRefPtr<HoneycombValue> node,
                       honey_json_writer_options_t options) {
  if (!node.get() || !node->IsValid()) {
    return HoneycombString();
  }

  HoneycombValueImpl* impl = static_cast<HoneycombValueImpl*>(node.get());
  HoneycombValueImpl::ScopedLockedValue scoped_value(impl);

  std::string json_string;
  if (base::JSONWriter::WriteWithOptions(
          *scoped_value.value(), GetJSONWriterOptions(options), &json_string)) {
    return json_string;
  }
  return HoneycombString();
}
