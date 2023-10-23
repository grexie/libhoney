// Copyright 2018 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_COMMON_EXTENSIONS_EXTENSIONS_API_PROVIDER_H_
#define HONEYCOMB_LIBHONEYCOMB_COMMON_EXTENSIONS_EXTENSIONS_API_PROVIDER_H_

#include "extensions/common/extensions_api_provider.h"

namespace extensions {

class HoneycombExtensionsAPIProvider : public ExtensionsAPIProvider {
 public:
  HoneycombExtensionsAPIProvider();

  HoneycombExtensionsAPIProvider(const HoneycombExtensionsAPIProvider&) = delete;
  HoneycombExtensionsAPIProvider& operator=(const HoneycombExtensionsAPIProvider&) = delete;

  // ExtensionsAPIProvider:
  void AddAPIFeatures(FeatureProvider* provider) override;
  void AddManifestFeatures(FeatureProvider* provider) override;
  void AddPermissionFeatures(FeatureProvider* provider) override;
  void AddBehaviorFeatures(FeatureProvider* provider) override;
  void AddAPIJSONSources(JSONFeatureProviderSource* json_source) override;
  bool IsAPISchemaGenerated(const std::string& name) override;
  base::StringPiece GetAPISchema(const std::string& name) override;
  void RegisterPermissions(PermissionsInfo* permissions_info) override;
  void RegisterManifestHandlers() override;
};

}  // namespace extensions

#endif  // HONEYCOMB_LIBHONEYCOMB_COMMON_EXTENSIONS_EXTENSIONS_API_PROVIDER_H_
