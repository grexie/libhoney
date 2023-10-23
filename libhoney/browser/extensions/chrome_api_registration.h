// Copyright (c) 2016 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_CHROME_API_REGISTRATION_H_
#define HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_CHROME_API_REGISTRATION_H_

#include <string>

class ExtensionFunctionRegistry;

namespace extensions {
namespace api {
namespace honey {

// Array of currently supported APIs.
extern const char* const kSupportedAPIs[];

class ChromeFunctionRegistry {
 public:
  static bool IsSupported(const std::string& name);
  static void RegisterAll(ExtensionFunctionRegistry* registry);
};

}  // namespace honey
}  // namespace api
}  // namespace extensions

#endif  // HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_CHROME_API_REGISTRATION_H_
