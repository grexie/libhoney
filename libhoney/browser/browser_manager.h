// Copyright 2021 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_BROWSER_MANAGER_H_
#define HONEYCOMB_LIBHONEY_BROWSER_BROWSER_MANAGER_H_

#include "honey/libhoney/common/mojom/honey.mojom.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "services/service_manager/public/cpp/binder_registry.h"

namespace blink {
class AssociatedInterfaceRegistry;
}

namespace content {
class RenderProcessHost;
}

class HoneycombBrowserManager : public honey::mojom::BrowserManager {
 public:
  explicit HoneycombBrowserManager(int render_process_id);

  HoneycombBrowserManager(const HoneycombBrowserManager&) = delete;
  HoneycombBrowserManager& operator=(const HoneycombBrowserManager&) = delete;

  ~HoneycombBrowserManager() override;

  // Called from the ContentBrowserClient method of the same name.
  // |associated_registry| is used for interfaces which must be associated with
  // some IPC::ChannelProxy, meaning that messages on the interface retain FIFO
  // with respect to legacy Chrome IPC messages sent or dispatched on the
  // channel.
  static void ExposeInterfacesToRenderer(
      service_manager::BinderRegistry* registry,
      blink::AssociatedInterfaceRegistry* associated_registry,
      content::RenderProcessHost* host);

  // Connects to HoneycombRenderManager in the render process.
  static mojo::Remote<honey::mojom::RenderManager> GetRenderManagerForProcess(
      content::RenderProcessHost* host);

 private:
  // honey::mojom::BrowserManager methods:
  void GetNewRenderThreadInfo(
      honey::mojom::BrowserManager::GetNewRenderThreadInfoCallback callback)
      override;
  void GetNewBrowserInfo(
      int32_t render_frame_routing_id,
      honey::mojom::BrowserManager::GetNewBrowserInfoCallback callback) override;

  // The process ID of the renderer.
  const int render_process_id_;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_BROWSER_MANAGER_H_
