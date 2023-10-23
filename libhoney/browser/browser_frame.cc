// Copyright 2021 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoney/browser/browser_frame.h"

#include "libhoney/browser/browser_host_base.h"
#include "libhoney/browser/thread_util.h"

#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_observer.h"
#include "mojo/public/cpp/bindings/self_owned_receiver.h"

HoneycombBrowserFrame::HoneycombBrowserFrame(
    content::RenderFrameHost* render_frame_host,
    mojo::PendingReceiver<honey::mojom::BrowserFrame> receiver)
    : FrameServiceBase(render_frame_host, std::move(receiver)) {}

HoneycombBrowserFrame::~HoneycombBrowserFrame() = default;

// static
void HoneycombBrowserFrame::RegisterBrowserInterfaceBindersForFrame(
    content::RenderFrameHost* render_frame_host,
    mojo::BinderMapWithContext<content::RenderFrameHost*>* map) {
  map->Add<honey::mojom::BrowserFrame>(base::BindRepeating(
      [](content::RenderFrameHost* frame_host,
         mojo::PendingReceiver<honey::mojom::BrowserFrame> receiver) {
        // This object is bound to the lifetime of |frame_host| and the mojo
        // connection. See DocumentServiceBase for details.
        new HoneycombBrowserFrame(frame_host, std::move(receiver));
      }));
}

void HoneycombBrowserFrame::SendMessage(const std::string& name,
                                  base::Value::List arguments) {
  // Always send to the newly created RFH, which may be speculative when
  // navigating cross-origin.
  if (auto host = GetFrameHost(/*prefer_speculative=*/true)) {
    host->SendMessage(name, std::move(arguments));
  }
}

void HoneycombBrowserFrame::SendSharedMemoryRegion(
    const std::string& name,
    base::WritableSharedMemoryRegion region) {
  // Always send to the newly created RFH, which may be speculative when
  // navigating cross-origin.
  if (auto host = GetFrameHost(/*prefer_speculative=*/true)) {
    host->SendSharedMemoryRegion(name, std::move(region));
  }
}

void HoneycombBrowserFrame::FrameAttached(
    mojo::PendingRemote<honey::mojom::RenderFrame> render_frame,
    bool reattached) {
  // Always send to the newly created RFH, which may be speculative when
  // navigating cross-origin.
  if (auto host = GetFrameHost(/*prefer_speculative=*/true)) {
    host->FrameAttached(std::move(render_frame), reattached);
  }
}

void HoneycombBrowserFrame::UpdateDraggableRegions(
    absl::optional<std::vector<honey::mojom::DraggableRegionEntryPtr>> regions) {
  if (auto host = GetFrameHost()) {
    host->UpdateDraggableRegions(std::move(regions));
  }
}

HoneycombRefPtr<HoneycombFrameHostImpl> HoneycombBrowserFrame::GetFrameHost(
    bool prefer_speculative) const {
  HONEYCOMB_REQUIRE_UIT();
  auto rfh = render_frame_host();
  if (auto browser = HoneycombBrowserHostBase::GetBrowserForHost(rfh)) {
    return browser->browser_info()->GetFrameForHost(rfh, nullptr,
                                                    prefer_speculative);
  }
  DCHECK(false);
  return nullptr;
}
