// Copyright 2015 The Honeycomb Authors.
// Portions copyright 2014 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoney/common/extensions/extensions_client.h"

#include <utility>

#include "libhoney/common/honey_switches.h"
#include "libhoney/common/extensions/extensions_api_provider.h"

#include "base/logging.h"
#include "extensions/common/core_extensions_api_provider.h"
#include "extensions/common/extension_urls.h"
#include "extensions/common/features/simple_feature.h"
#include "extensions/common/permissions/permission_message_provider.h"
#include "extensions/common/url_pattern_set.h"

namespace extensions {

namespace {

template <class FeatureClass>
SimpleFeature* CreateFeature() {
  return new FeatureClass;
}

}  // namespace

HoneycombExtensionsClient::HoneycombExtensionsClient()
    : webstore_base_url_(extension_urls::kChromeWebstoreBaseURL),
      new_webstore_base_url_(extension_urls::kNewChromeWebstoreBaseURL),
      webstore_update_url_(extension_urls::kChromeWebstoreUpdateURL) {
  AddAPIProvider(std::make_unique<CoreExtensionsAPIProvider>());
  AddAPIProvider(std::make_unique<HoneycombExtensionsAPIProvider>());
}

HoneycombExtensionsClient::~HoneycombExtensionsClient() {}

void HoneycombExtensionsClient::Initialize() {}

void HoneycombExtensionsClient::InitializeWebStoreUrls(
    base::CommandLine* command_line) {}

const PermissionMessageProvider&
HoneycombExtensionsClient::GetPermissionMessageProvider() const {
  return permission_message_provider_;
}

const std::string HoneycombExtensionsClient::GetProductName() {
  return "honey";
}

void HoneycombExtensionsClient::FilterHostPermissions(
    const URLPatternSet& hosts,
    URLPatternSet* new_hosts,
    PermissionIDSet* permissions) const {
  NOTIMPLEMENTED();
}

void HoneycombExtensionsClient::SetScriptingAllowlist(
    const ScriptingAllowlist& allowlist) {
  scripting_allowlist_ = allowlist;
}

const ExtensionsClient::ScriptingAllowlist&
HoneycombExtensionsClient::GetScriptingAllowlist() const {
  // TODO(jamescook): Real allowlist.
  return scripting_allowlist_;
}

URLPatternSet HoneycombExtensionsClient::GetPermittedChromeSchemeHosts(
    const Extension* extension,
    const APIPermissionSet& api_permissions) const {
  return URLPatternSet();
}

bool HoneycombExtensionsClient::IsScriptableURL(const GURL& url,
                                          std::string* error) const {
  return true;
}

const GURL& HoneycombExtensionsClient::GetWebstoreBaseURL() const {
  return webstore_base_url_;
}

const GURL& HoneycombExtensionsClient::GetNewWebstoreBaseURL() const {
  return new_webstore_base_url_;
}

const GURL& HoneycombExtensionsClient::GetWebstoreUpdateURL() const {
  return webstore_update_url_;
}

bool HoneycombExtensionsClient::IsBlocklistUpdateURL(const GURL& url) const {
  // TODO(rockot): Maybe we want to do something else here. For now we accept
  // any URL as a blocklist URL because we don't really care.
  return true;
}

}  // namespace extensions
