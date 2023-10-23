// Copyright 2020 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_COMMON_APP_MANAGER_H_
#define HONEYCOMB_LIBHONEYCOMB_COMMON_APP_MANAGER_H_
#pragma once

#include <list>

#include "include/honey_app.h"
#include "include/honey_request_context.h"

#include "base/functional/callback.h"
#include "build/build_config.h"
#include "content/public/common/content_client.h"

class HoneycombBrowserContext;
struct HoneycombSchemeInfo;

// Exposes global application state in the main and render processes.
class HoneycombAppManager {
 public:
  HoneycombAppManager(const HoneycombAppManager&) = delete;
  HoneycombAppManager& operator=(const HoneycombAppManager&) = delete;

  // Returns the singleton instance that is scoped to Honeycomb lifespan.
  static HoneycombAppManager* Get();

  // The following methods are available in both processes.

  virtual HoneycombRefPtr<HoneycombApp> GetApplication() = 0;
  virtual content::ContentClient* GetContentClient() = 0;

  // Custom scheme information will be registered first with all processes
  // (url/url_util.h) via ContentClient::AddAdditionalSchemes which calls
  // AddCustomScheme, and second with Blink (SchemeRegistry) via
  // ContentRendererClient::WebKitInitialized which calls GetCustomSchemes.
  void AddCustomScheme(HoneycombSchemeInfo* scheme_info);
  bool HasCustomScheme(const std::string& scheme_name);

  using SchemeInfoList = std::list<HoneycombSchemeInfo>;
  const SchemeInfoList* GetCustomSchemes();

  // Called from ContentClient::AddAdditionalSchemes.
  void AddAdditionalSchemes(content::ContentClient::Schemes* schemes);

  // The following methods are only available in the main (browser) process.

  // Called from HoneycombRequestContextImpl. |initialized_cb| may be executed
  // synchronously or asynchronously.
  virtual HoneycombRefPtr<HoneycombRequestContext> GetGlobalRequestContext() = 0;
  virtual HoneycombBrowserContext* CreateNewBrowserContext(
      const HoneycombRequestContextSettings& settings,
      base::OnceClosure initialized_cb) = 0;

#if BUILDFLAG(IS_WIN)
  // Returns the module name (usually libhoneycomb.dll).
  const wchar_t* GetResourceDllName();
#endif

 protected:
  HoneycombAppManager();
  virtual ~HoneycombAppManager();

 private:
  // Custom schemes handled by the client.
  SchemeInfoList scheme_info_list_;
  bool scheme_info_list_locked_ = false;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_COMMON_APP_MANAGER_H_
