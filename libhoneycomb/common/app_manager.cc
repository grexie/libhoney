// Copyright 2020 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoneycomb/common/app_manager.h"

#include "libhoneycomb/common/net/scheme_info.h"
#include "libhoneycomb/common/net/scheme_registration.h"
#include "libhoneycomb/common/scheme_registrar_impl.h"

#include "base/command_line.h"
#include "base/logging.h"
#include "content/public/browser/child_process_security_policy.h"
#include "content/public/common/content_switches.h"

#if BUILDFLAG(IS_WIN)
#include <windows.h>
#include "base/path_service.h"
#endif

namespace {

HoneycombAppManager* g_manager = nullptr;

}  // namespace

// static
HoneycombAppManager* HoneycombAppManager::Get() {
  return g_manager;
}

HoneycombAppManager::HoneycombAppManager() {
  // Only a single instance should exist.
  DCHECK(!g_manager);
  g_manager = this;
}

HoneycombAppManager::~HoneycombAppManager() {
  g_manager = nullptr;
}

void HoneycombAppManager::AddCustomScheme(HoneycombSchemeInfo* scheme_info) {
  DCHECK(!scheme_info_list_locked_);
  scheme_info_list_.push_back(*scheme_info);

  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  if (!command_line->HasSwitch(switches::kProcessType)) {
    // Register as a Web-safe scheme in the browser process so that requests for
    // the scheme from a render process will be allowed in
    // resource_dispatcher_host_impl.cc ShouldServiceRequest.
    content::ChildProcessSecurityPolicy* policy =
        content::ChildProcessSecurityPolicy::GetInstance();
    if (!policy->IsWebSafeScheme(scheme_info->scheme_name)) {
      policy->RegisterWebSafeScheme(scheme_info->scheme_name);
    }
  }
}

bool HoneycombAppManager::HasCustomScheme(const std::string& scheme_name) {
  DCHECK(scheme_info_list_locked_);
  if (scheme_info_list_.empty()) {
    return false;
  }

  SchemeInfoList::const_iterator it = scheme_info_list_.begin();
  for (; it != scheme_info_list_.end(); ++it) {
    if (it->scheme_name == scheme_name) {
      return true;
    }
  }

  return false;
}

const HoneycombAppManager::SchemeInfoList* HoneycombAppManager::GetCustomSchemes() {
  DCHECK(scheme_info_list_locked_);
  return &scheme_info_list_;
}

void HoneycombAppManager::AddAdditionalSchemes(
    content::ContentClient::Schemes* schemes) {
  DCHECK(!scheme_info_list_locked_);

  auto application = GetApplication();
  if (application) {
    HoneycombSchemeRegistrarImpl schemeRegistrar;
    application->OnRegisterCustomSchemes(&schemeRegistrar);
    schemeRegistrar.GetSchemes(schemes);
  }

  scheme::AddInternalSchemes(schemes);

  scheme_info_list_locked_ = true;
}

#if BUILDFLAG(IS_WIN)
const wchar_t* HoneycombAppManager::GetResourceDllName() {
  static wchar_t file_path[MAX_PATH + 1] = {0};

  if (file_path[0] == 0) {
    // Retrieve the module path (usually libhoneycomb.dll).
    base::FilePath module;
    base::PathService::Get(base::FILE_MODULE, &module);
    const std::wstring wstr = module.value();
    size_t count = std::min(static_cast<size_t>(MAX_PATH), wstr.size());
    wcsncpy(file_path, wstr.c_str(), count);
    file_path[count] = 0;
  }

  return file_path;
}
#endif  // BUILDFLAG(IS_WIN)
