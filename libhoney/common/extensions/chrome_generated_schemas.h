// Copyright (c) 2016 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// GENERATED FROM THE API DEFINITIONS IN
//   chrome\common\extensions\api
// DO NOT EDIT.

#ifndef HONEYCOMB_LIBHONEY_COMMON_EXTENSIONS_CHROME_GENERATED_SCHEMAS_H_
#define HONEYCOMB_LIBHONEY_COMMON_EXTENSIONS_CHROME_GENERATED_SCHEMAS_H_

#include <map>
#include <string>

#include "base/strings/string_piece.h"

namespace extensions {
namespace api {
namespace honey {

class ChromeGeneratedSchemas {
 public:
  // Determines if schema named |name| is generated.
  static bool IsGenerated(std::string name);

  // Gets the API schema named |name|.
  static base::StringPiece Get(const std::string& name);
};

}  // namespace honey
}  // namespace api
}  // namespace extensions

#endif  // HONEYCOMB_LIBHONEY_COMMON_EXTENSIONS_CHROME_GENERATED_SCHEMAS_H_
