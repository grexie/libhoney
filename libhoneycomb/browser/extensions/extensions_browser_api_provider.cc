// Copyright 2018 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoneycomb/browser/extensions/extensions_browser_api_provider.h"
#include "libhoneycomb/browser/extensions/chrome_api_registration.h"

// #include "honey/libhoneycomb/browser/extensions/api/generated_api_registration.h"
#include "extensions/browser/api/generated_api_registration.h"

namespace extensions {

HoneycombExtensionsBrowserAPIProvider::HoneycombExtensionsBrowserAPIProvider() = default;
HoneycombExtensionsBrowserAPIProvider::~HoneycombExtensionsBrowserAPIProvider() = default;

void HoneycombExtensionsBrowserAPIProvider::RegisterExtensionFunctions(
    ExtensionFunctionRegistry* registry) {
  // Honeycomb-only APIs.
  // TODO(honey): Enable if/when Honeycomb exposes its own Mojo APIs. See
  // libhoneycomb/common/extensions/api/README.txt for details.
  // api::honey::HoneycombGeneratedFunctionRegistry::RegisterAll(registry);

  // Chrome APIs whitelisted by Honeycomb.
  api::honey::ChromeFunctionRegistry::RegisterAll(registry);
}

}  // namespace extensions
