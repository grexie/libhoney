// Copyright 2018 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_EXTENSIONS_BROWSER_API_PROVIDER_H_
#define HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_EXTENSIONS_BROWSER_API_PROVIDER_H_

#include "extensions/browser/extensions_browser_api_provider.h"

namespace extensions {

class HoneycombExtensionsBrowserAPIProvider : public ExtensionsBrowserAPIProvider {
 public:
  HoneycombExtensionsBrowserAPIProvider();

  HoneycombExtensionsBrowserAPIProvider(const HoneycombExtensionsBrowserAPIProvider&) =
      delete;
  HoneycombExtensionsBrowserAPIProvider& operator=(
      const HoneycombExtensionsBrowserAPIProvider&) = delete;

  ~HoneycombExtensionsBrowserAPIProvider() override;

  void RegisterExtensionFunctions(ExtensionFunctionRegistry* registry) override;
};

}  // namespace extensions

#endif  // HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_EXTENSIONS_BROWSER_API_PROVIDER_H_
