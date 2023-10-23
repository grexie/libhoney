// Copyright 2019 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_OSR_HOST_DISPLAY_CLIENT_OSR_H_
#define HONEYCOMB_LIBHONEY_BROWSER_OSR_HOST_DISPLAY_CLIENT_OSR_H_

#include <memory>

#include "base/functional/callback.h"
#include "base/memory/shared_memory_mapping.h"
#include "components/viz/host/host_display_client.h"
#include "ui/gfx/native_widget_types.h"

class HoneycombLayeredWindowUpdaterOSR;
class HoneycombRenderWidgetHostViewOSR;

class HoneycombHostDisplayClientOSR : public viz::HostDisplayClient {
 public:
  HoneycombHostDisplayClientOSR(HoneycombRenderWidgetHostViewOSR* const view,
                          gfx::AcceleratedWidget widget);

  HoneycombHostDisplayClientOSR(const HoneycombHostDisplayClientOSR&) = delete;
  HoneycombHostDisplayClientOSR& operator=(const HoneycombHostDisplayClientOSR&) = delete;

  ~HoneycombHostDisplayClientOSR() override;

  void SetActive(bool active);
  const void* GetPixelMemory() const;
  gfx::Size GetPixelSize() const;

 private:
  // mojom::DisplayClient implementation.
  void UseProxyOutputDevice(UseProxyOutputDeviceCallback callback) override;

  void CreateLayeredWindowUpdater(
      mojo::PendingReceiver<viz::mojom::LayeredWindowUpdater> receiver)
      override;

#if BUILDFLAG(IS_LINUX)
  void DidCompleteSwapWithNewSize(const gfx::Size& size) override;
#endif

  HoneycombRenderWidgetHostViewOSR* const view_;
  std::unique_ptr<HoneycombLayeredWindowUpdaterOSR> layered_window_updater_;
  bool active_ = false;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_OSR_HOST_DISPLAY_CLIENT_OSR_H_
