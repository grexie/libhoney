// Copyright 2021 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_BROWSER_FRAME_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_BROWSER_FRAME_H_
#pragma once

#include "libhoneycomb/browser/frame_host_impl.h"
#include "libhoneycomb/browser/frame_service_base.h"

#include "honey/libhoneycomb/common/mojom/honey.mojom.h"
#include "mojo/public/cpp/bindings/binder_map.h"

// Implementation of the BrowserFrame mojo interface.
// This is implemented separately from HoneycombFrameHostImpl to better manage the
// association with the RenderFrameHost (which may be speculative, etc.), and so
// that messages are always routed to the most appropriate HoneycombFrameHostImpl
// instance. Lifespan is tied to the RFH via FrameServiceBase.
class HoneycombBrowserFrame
    : public content::FrameServiceBase<honey::mojom::BrowserFrame> {
 public:
  HoneycombBrowserFrame(content::RenderFrameHost* render_frame_host,
                  mojo::PendingReceiver<honey::mojom::BrowserFrame> receiver);

  HoneycombBrowserFrame(const HoneycombBrowserFrame&) = delete;
  HoneycombBrowserFrame& operator=(const HoneycombBrowserFrame&) = delete;

  ~HoneycombBrowserFrame() override;

  // Called from the ContentBrowserClient method of the same name.
  static void RegisterBrowserInterfaceBindersForFrame(
      content::RenderFrameHost* render_frame_host,
      mojo::BinderMapWithContext<content::RenderFrameHost*>* map);

 private:
  // honey::mojom::BrowserFrame methods:
  void SendMessage(const std::string& name,
                   base::Value::List arguments) override;
  void SendSharedMemoryRegion(const std::string& name,
                              base::WritableSharedMemoryRegion region) override;
  void FrameAttached(mojo::PendingRemote<honey::mojom::RenderFrame> render_frame,
                     bool reattached) override;
  void UpdateDraggableRegions(
      absl::optional<std::vector<honey::mojom::DraggableRegionEntryPtr>> regions)
      override;

  // FrameServiceBase methods:
  bool ShouldCloseOnFinishNavigation() const override { return false; }

  HoneycombRefPtr<HoneycombFrameHostImpl> GetFrameHost(
      bool prefer_speculative = false) const;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_BROWSER_FRAME_H_
