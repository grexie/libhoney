// Copyright 2017 the Honeycomb Authors. Portions copyright
// 2013 The Chromium Authors and 2016 the CEF Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_EXTENSION_VIEW_HOST_H_
#define HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_EXTENSION_VIEW_HOST_H_

#include <memory>

#include "base/scoped_observation.h"
#include "extensions/browser/extension_host.h"
#include "extensions/browser/extension_host_registry.h"

class AlloyBrowserHostImpl;

namespace content {
class WebContents;
}  // namespace content

namespace extensions {

// The ExtensionHost for an extension that backs a view in the browser UI. For
// example, this could be an extension popup or dialog, but not a background
// page. Object lifespan is managed by AlloyBrowserHostImpl. Based on
// chrome/browser/extensions/extension_view_host.h.
class HoneycombExtensionViewHost : public ExtensionHost,
                             public ExtensionHostRegistry::Observer {
 public:
  HoneycombExtensionViewHost(AlloyBrowserHostImpl* browser,
                       const Extension* extension,
                       content::WebContents* host_contents,
                       const GURL& url,
                       mojom::ViewType host_type);

  HoneycombExtensionViewHost(const HoneycombExtensionViewHost&) = delete;
  HoneycombExtensionViewHost& operator=(const HoneycombExtensionViewHost&) = delete;

  ~HoneycombExtensionViewHost() override;

  // ExtensionHost methods:
  void OnDidStopFirstLoad() override;
  void LoadInitialURL() override;
  bool IsBackgroundPage() const override;

  // content::WebContentsDelegate methods:
  bool ShouldAllowRendererInitiatedCrossProcessNavigation(
      bool is_main_frame_navigation) override;
  bool PreHandleGestureEvent(content::WebContents* source,
                             const blink::WebGestureEvent& event) override;

  // extensions::ExtensionFunctionDispatcher::Delegate methods:
  content::WebContents* GetVisibleWebContents() const override;

  // ExtensionHostRegistry::Observer methods:
  void OnExtensionHostDocumentElementAvailable(
      content::BrowserContext* browser_context,
      ExtensionHost* extension_host) override;

 private:
  base::ScopedObservation<ExtensionHostRegistry,
                          ExtensionHostRegistry::Observer>
      host_registry_observation_{this};
};

}  // namespace extensions

#endif  // HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_EXTENSION_VIEW_HOST_H_
