// Copyright 2013 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_DEVTOOLS_DEVTOOLS_MANAGER_DELEGATE_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_DEVTOOLS_DEVTOOLS_MANAGER_DELEGATE_H_

#include "content/public/browser/devtools_manager_delegate.h"

namespace content {
class BrowserContext;
}

class HoneycombDevToolsManagerDelegate : public content::DevToolsManagerDelegate {
 public:
  static void StartHttpHandler(content::BrowserContext* browser_context);
  static void StopHttpHandler();

  HoneycombDevToolsManagerDelegate();

  HoneycombDevToolsManagerDelegate(const HoneycombDevToolsManagerDelegate&) = delete;
  HoneycombDevToolsManagerDelegate& operator=(const HoneycombDevToolsManagerDelegate&) =
      delete;

  ~HoneycombDevToolsManagerDelegate() override;

  // DevToolsManagerDelegate implementation.
  scoped_refptr<content::DevToolsAgentHost> CreateNewTarget(
      const GURL& url,
      content::DevToolsManagerDelegate::TargetType target_type) override;
  std::string GetDiscoveryPageHTML() override;
  bool HasBundledFrontendResources() override;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_DEVTOOLS_DEVTOOLS_MANAGER_DELEGATE_H_
