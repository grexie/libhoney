// Copyright 2018 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoneycomb/common/extensions/extensions_api_provider.h"

#include "libhoneycomb/common/extensions/chrome_generated_schemas.h"

#include "honey/grit/honey_resources.h"
// #include "honey/libhoneycomb/common/extensions/api/generated_schemas.h"
#include "honey/libhoneycomb/common/extensions/api/honey_api_features.h"
#include "honey/libhoneycomb/common/extensions/api/honey_manifest_features.h"
#include "honey/libhoneycomb/common/extensions/api/honey_permission_features.h"
#include "chrome/common/extensions/chrome_manifest_handlers.h"
#include "chrome/common/extensions/permissions/chrome_api_permissions.h"
#include "extensions/common/features/json_feature_provider_source.h"
#include "extensions/common/permissions/permissions_info.h"

namespace extensions
{

  HoneycombExtensionsAPIProvider::HoneycombExtensionsAPIProvider() {}

  void HoneycombExtensionsAPIProvider::AddAPIFeatures(FeatureProvider *provider)
  {
    AddHoneycombAPIFeatures(provider);
  }

  void HoneycombExtensionsAPIProvider::AddManifestFeatures(FeatureProvider *provider)
  {
    AddHoneycombManifestFeatures(provider);
  }

  void HoneycombExtensionsAPIProvider::AddPermissionFeatures(
      FeatureProvider *provider)
  {
    AddHoneycombPermissionFeatures(provider);
  }

  void HoneycombExtensionsAPIProvider::AddBehaviorFeatures(FeatureProvider *provider)
  {
    // No Honeycomb-specific behavior features.
  }

  void HoneycombExtensionsAPIProvider::AddAPIJSONSources(
      JSONFeatureProviderSource *json_source)
  {
    // Extension API features specific to Honeycomb. See
    // libhoneycomb/common/extensions/api/README.txt for additional details.
    json_source->LoadJSON(IDR_HONEYCOMB_EXTENSION_API_FEATURES);
  }

  bool HoneycombExtensionsAPIProvider::IsAPISchemaGenerated(const std::string &name)
  {
    // Schema for Honeycomb-only APIs.
    // TODO(honey): Enable if/when Honeycomb exposes its own Mojo APIs. See
    // libhoneycomb/common/extensions/api/README.txt for details.
    // if (api::honey::HoneycombGeneratedSchemas::IsGenerated(name))
    //   return true;

    // Chrome APIs whitelisted by Honeycomb.
    if (api::honey::ChromeGeneratedSchemas::IsGenerated(name))
    {
      return true;
    }

    return false;
  }

  base::StringPiece HoneycombExtensionsAPIProvider::GetAPISchema(
      const std::string &name)
  {
    // Schema for Honeycomb-only APIs.
    // TODO(honey): Enable if/when Honeycomb exposes its own Mojo APIs. See
    // libhoneycomb/common/extensions/api/README.txt for details.
    // if (api::honey::HoneycombGeneratedSchemas::IsGenerated(name))
    //   return api::honey::HoneycombGeneratedSchemas::Get(name);

    // Chrome APIs whitelisted by Honeycomb.
    if (api::honey::ChromeGeneratedSchemas::IsGenerated(name))
    {
      return api::honey::ChromeGeneratedSchemas::Get(name);
    }

    return base::StringPiece();
  }

  void HoneycombExtensionsAPIProvider::RegisterPermissions(
      PermissionsInfo *permissions_info)
  {
    permissions_info->RegisterPermissions(
        chrome_api_permissions::GetPermissionInfos(),
        chrome_api_permissions::GetPermissionAliases());
  }

  void HoneycombExtensionsAPIProvider::RegisterManifestHandlers()
  {
    RegisterChromeManifestHandlers();
  }

} // namespace extensions
