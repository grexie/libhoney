// Copyright 2014 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_RENDERER_EXTENSIONS_EXTENSIONS_RENDERER_CLIENT_H_
#define HONEYCOMB_LIBHONEY_RENDERER_EXTENSIONS_EXTENSIONS_RENDERER_CLIENT_H_

#include <memory>
#include <string>

#include "extensions/renderer/extensions_renderer_client.h"
#include "services/service_manager/public/cpp/binder_registry.h"
#include "ui/base/page_transition_types.h"

class AlloyContentRendererClient;
class GURL;

namespace blink {
class WebFrame;
class WebLocalFrame;
struct WebPluginParams;
class WebURL;
}  // namespace blink

namespace net {
class SiteForCookies;
}

namespace content {
class BrowserPluginDelegate;
class RenderFrame;
struct WebPluginInfo;
}  // namespace content

namespace url {
class Origin;
}

namespace extensions {

class Dispatcher;
class DispatcherDelegate;
class ResourceRequestPolicy;

class HoneycombExtensionsRendererClient : public ExtensionsRendererClient {
 public:
  explicit HoneycombExtensionsRendererClient(
      AlloyContentRendererClient* alloy_content_renderer_client);

  HoneycombExtensionsRendererClient(const HoneycombExtensionsRendererClient&) = delete;
  HoneycombExtensionsRendererClient& operator=(const HoneycombExtensionsRendererClient&) =
      delete;

  ~HoneycombExtensionsRendererClient() override;

  // ExtensionsRendererClient implementation.
  bool IsIncognitoProcess() const override;
  int GetLowestIsolatedWorldId() const override;
  extensions::Dispatcher* GetDispatcher() override;
  void OnExtensionLoaded(const extensions::Extension& extension) override;
  void OnExtensionUnloaded(
      const extensions::ExtensionId& extension_id) override;
  bool ExtensionAPIEnabledForServiceWorkerScript(
      const GURL& scope,
      const GURL& script_url) const override;

  // See AlloyContentRendererClient methods with the same names.
  void RenderThreadStarted();
  void RenderFrameCreated(content::RenderFrame* render_frame,
                          service_manager::BinderRegistry* registry);
  bool OverrideCreatePlugin(content::RenderFrame* render_frame,
                            const blink::WebPluginParams& params);
  void WillSendRequest(blink::WebLocalFrame* frame,
                       ui::PageTransition transition_type,
                       const blink::WebURL& url,
                       const net::SiteForCookies& site_for_cookies,
                       const url::Origin* initiator_origin,
                       GURL* new_url);
  void RunScriptsAtDocumentStart(content::RenderFrame* render_frame);
  void RunScriptsAtDocumentEnd(content::RenderFrame* render_frame);
  void RunScriptsAtDocumentIdle(content::RenderFrame* render_frame);

 private:
  AlloyContentRendererClient* const alloy_content_renderer_client_;

  std::unique_ptr<extensions::Dispatcher> extension_dispatcher_;
  std::unique_ptr<extensions::ResourceRequestPolicy> resource_request_policy_;
};

}  // namespace extensions

#endif  // HONEYCOMB_LIBHONEY_RENDERER_EXTENSIONS_EXTENSIONS_RENDERER_CLIENT_H_
