// Copyright 2021 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoney/browser/browser_manager.h"

#include "libhoney/browser/browser_info_manager.h"
#include "libhoney/browser/origin_whitelist_impl.h"
#include "libhoney/common/frame_util.h"

#include "content/public/browser/render_process_host.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/self_owned_receiver.h"
#include "services/service_manager/public/cpp/binder_registry.h"

HoneycombBrowserManager::HoneycombBrowserManager(int render_process_id)
    : render_process_id_(render_process_id) {}

HoneycombBrowserManager::~HoneycombBrowserManager() = default;

// static
void HoneycombBrowserManager::ExposeInterfacesToRenderer(
    service_manager::BinderRegistry* registry,
    blink::AssociatedInterfaceRegistry* associated_registry,
    content::RenderProcessHost* host) {
  registry->AddInterface<honey::mojom::BrowserManager>(base::BindRepeating(
      [](int render_process_id,
         mojo::PendingReceiver<honey::mojom::BrowserManager> receiver) {
        mojo::MakeSelfOwnedReceiver(
            std::make_unique<HoneycombBrowserManager>(render_process_id),
            std::move(receiver));
      },
      host->GetID()));
}

// static
mojo::Remote<honey::mojom::RenderManager>
HoneycombBrowserManager::GetRenderManagerForProcess(
    content::RenderProcessHost* host) {
  mojo::Remote<honey::mojom::RenderManager> client;
  host->BindReceiver(client.BindNewPipeAndPassReceiver());
  return client;
}

void HoneycombBrowserManager::GetNewRenderThreadInfo(
    honey::mojom::BrowserManager::GetNewRenderThreadInfoCallback callback) {
  auto info = honey::mojom::NewRenderThreadInfo::New();
  GetCrossOriginWhitelistEntries(&info->cross_origin_whitelist_entries);
  std::move(callback).Run(std::move(info));
}

void HoneycombBrowserManager::GetNewBrowserInfo(
    int32_t render_frame_routing_id,
    honey::mojom::BrowserManager::GetNewBrowserInfoCallback callback) {
  HoneycombBrowserInfoManager::GetInstance()->OnGetNewBrowserInfo(
      frame_util::MakeGlobalId(render_process_id_, render_frame_routing_id),
      std::move(callback));
}
