// Copyright 2017 the Honeycomb Authors. Portions copyright
// 2013 The Chromium Authors and 2016 the CEF Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_EXTENSION_BACKGROUND_HOST_H_
#define HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_EXTENSION_BACKGROUND_HOST_H_

#include <memory>

#include "base/functional/callback_forward.h"
#include "extensions/browser/extension_host.h"

class AlloyBrowserHostImpl;

namespace content {
class WebContents;
}  // namespace content

namespace extensions {

// The ExtensionHost for a background page. This is a thin wrapper around the
// ExtensionHost base class to support Honeycomb-specific constructor. Object lifespan
// is managed by ProcessManager.
class HoneycombExtensionBackgroundHost : public ExtensionHost {
 public:
  HoneycombExtensionBackgroundHost(AlloyBrowserHostImpl* browser,
                             base::OnceClosure deleted_callback,
                             const Extension* extension,
                             content::WebContents* host_contents,
                             const GURL& url,
                             mojom::ViewType host_type);

  HoneycombExtensionBackgroundHost(const HoneycombExtensionBackgroundHost&) = delete;
  HoneycombExtensionBackgroundHost& operator=(const HoneycombExtensionBackgroundHost&) =
      delete;

  ~HoneycombExtensionBackgroundHost() override;

  // content::WebContentsDelegate methods:
  bool ShouldAllowRendererInitiatedCrossProcessNavigation(
      bool is_main_frame_navigation) override;

 private:
  // Callback that will be executed on host deletion.
  base::OnceClosure deleted_callback_;
};

}  // namespace extensions

#endif  // HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_EXTENSION_BACKGROUND_HOST_H_
