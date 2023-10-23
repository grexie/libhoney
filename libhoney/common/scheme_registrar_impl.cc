// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "libhoney/common/scheme_registrar_impl.h"

#include <string>

#include "libhoney/common/app_manager.h"
#include "libhoney/common/net/scheme_info.h"
#include "libhoney/common/net/scheme_registration.h"

#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/strings/string_util.h"

namespace {

void AppendArray(const std::vector<std::string>& source,
                 std::vector<std::string>* target) {
  if (source.empty()) {
    return;
  }
  target->insert(target->end(), source.begin(), source.end());
}
}  // namespace

HoneycombSchemeRegistrarImpl::HoneycombSchemeRegistrarImpl() {}

bool HoneycombSchemeRegistrarImpl::AddCustomScheme(const HoneycombString& scheme_name,
                                             int options) {
  const std::string& scheme = base::ToLowerASCII(scheme_name.ToString());
  if (scheme::IsInternalHandledScheme(scheme) ||
      registered_schemes_.find(scheme) != registered_schemes_.end()) {
    return false;
  }

  registered_schemes_.insert(scheme);

  const bool is_standard = options & HONEYCOMB_SCHEME_OPTION_STANDARD;
  const bool is_local = options & HONEYCOMB_SCHEME_OPTION_LOCAL;
  const bool is_display_isolated = options & HONEYCOMB_SCHEME_OPTION_DISPLAY_ISOLATED;
  const bool is_secure = options & HONEYCOMB_SCHEME_OPTION_SECURE;
  const bool is_cors_enabled = options & HONEYCOMB_SCHEME_OPTION_CORS_ENABLED;
  const bool is_csp_bypassing = options & HONEYCOMB_SCHEME_OPTION_CSP_BYPASSING;
  const bool is_fetch_enabled = options & HONEYCOMB_SCHEME_OPTION_FETCH_ENABLED;

  // The |is_display_isolated| value is excluded here because it's registered
  // with Blink only.
  if (is_standard) {
    schemes_.standard_schemes.push_back(scheme);
  }
  if (is_local) {
    schemes_.local_schemes.push_back(scheme);
  }
  if (is_secure) {
    schemes_.secure_schemes.push_back(scheme);
  }
  if (is_cors_enabled) {
    schemes_.cors_enabled_schemes.push_back(scheme);
  }
  if (is_csp_bypassing) {
    schemes_.csp_bypassing_schemes.push_back(scheme);
  }

  HoneycombSchemeInfo scheme_info = {
      scheme,    is_standard,     is_local,         is_display_isolated,
      is_secure, is_cors_enabled, is_csp_bypassing, is_fetch_enabled};
  HoneycombAppManager::Get()->AddCustomScheme(&scheme_info);

  return true;
}

void HoneycombSchemeRegistrarImpl::GetSchemes(
    content::ContentClient::Schemes* schemes) {
  AppendArray(schemes_.standard_schemes, &schemes->standard_schemes);
  AppendArray(schemes_.local_schemes, &schemes->local_schemes);
  AppendArray(schemes_.secure_schemes, &schemes->secure_schemes);
  AppendArray(schemes_.cors_enabled_schemes, &schemes->cors_enabled_schemes);
  AppendArray(schemes_.csp_bypassing_schemes, &schemes->csp_bypassing_schemes);
}
