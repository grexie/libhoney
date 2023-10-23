// Copyright 2019 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoney/browser/osr/host_display_client_osr.h"

#include <utility>

#include "libhoney/browser/osr/render_widget_host_view_osr.h"

#include "base/memory/shared_memory_mapping.h"
#include "components/viz/common/resources/resource_sizes.h"
#include "mojo/public/cpp/system/platform_handle.h"
#include "services/viz/privileged/mojom/compositing/layered_window_updater.mojom.h"
#include "skia/ext/platform_canvas.h"
#include "third_party/skia/include/core/SkColor.h"
#include "third_party/skia/include/core/SkRect.h"
#include "third_party/skia/src/core/SkDevice.h"
#include "ui/gfx/skia_util.h"

#if BUILDFLAG(IS_WIN)
#include "skia/ext/skia_utils_win.h"
#endif

class HoneycombLayeredWindowUpdaterOSR : public viz::mojom::LayeredWindowUpdater {
 public:
  HoneycombLayeredWindowUpdaterOSR(
      HoneycombRenderWidgetHostViewOSR* const view,
      mojo::PendingReceiver<viz::mojom::LayeredWindowUpdater> receiver);

  HoneycombLayeredWindowUpdaterOSR(const HoneycombLayeredWindowUpdaterOSR&) = delete;
  HoneycombLayeredWindowUpdaterOSR& operator=(const HoneycombLayeredWindowUpdaterOSR&) =
      delete;

  ~HoneycombLayeredWindowUpdaterOSR() override;

  void SetActive(bool active);
  const void* GetPixelMemory() const;
  gfx::Size GetPixelSize() const;

  // viz::mojom::LayeredWindowUpdater implementation.
  void OnAllocatedSharedMemory(const gfx::Size& pixel_size,
                               base::UnsafeSharedMemoryRegion region) override;
  void Draw(const gfx::Rect& damage_rect, DrawCallback draw_callback) override;

 private:
  HoneycombRenderWidgetHostViewOSR* const view_;
  mojo::Receiver<viz::mojom::LayeredWindowUpdater> receiver_;
  bool active_ = false;
  base::WritableSharedMemoryMapping shared_memory_;
  gfx::Size pixel_size_;
};

HoneycombLayeredWindowUpdaterOSR::HoneycombLayeredWindowUpdaterOSR(
    HoneycombRenderWidgetHostViewOSR* const view,
    mojo::PendingReceiver<viz::mojom::LayeredWindowUpdater> receiver)
    : view_(view), receiver_(this, std::move(receiver)) {}

HoneycombLayeredWindowUpdaterOSR::~HoneycombLayeredWindowUpdaterOSR() = default;

void HoneycombLayeredWindowUpdaterOSR::SetActive(bool active) {
  active_ = active;
}

const void* HoneycombLayeredWindowUpdaterOSR::GetPixelMemory() const {
  return shared_memory_.memory();
}

gfx::Size HoneycombLayeredWindowUpdaterOSR::GetPixelSize() const {
  return pixel_size_;
}

void HoneycombLayeredWindowUpdaterOSR::OnAllocatedSharedMemory(
    const gfx::Size& pixel_size,
    base::UnsafeSharedMemoryRegion region) {
  // Make sure |pixel_size| is sane.
  size_t expected_bytes;
  bool size_result = viz::ResourceSizes::MaybeSizeInBytes(
      pixel_size, viz::SinglePlaneFormat::kRGBA_8888, &expected_bytes);
  if (!size_result) {
    return;
  }

  pixel_size_ = pixel_size;
  shared_memory_ = region.Map();
  DCHECK(shared_memory_.IsValid());
}

void HoneycombLayeredWindowUpdaterOSR::Draw(const gfx::Rect& damage_rect,
                                      DrawCallback draw_callback) {
  if (active_) {
    const void* memory = GetPixelMemory();
    if (memory) {
      view_->OnPaint(damage_rect, pixel_size_, memory);
    } else {
      LOG(WARNING) << "Failed to read pixels";
    }
  }

  std::move(draw_callback).Run();
}

HoneycombHostDisplayClientOSR::HoneycombHostDisplayClientOSR(
    HoneycombRenderWidgetHostViewOSR* const view,
    gfx::AcceleratedWidget widget)
    : viz::HostDisplayClient(widget), view_(view) {}

HoneycombHostDisplayClientOSR::~HoneycombHostDisplayClientOSR() {}

void HoneycombHostDisplayClientOSR::SetActive(bool active) {
  active_ = active;
  if (layered_window_updater_) {
    layered_window_updater_->SetActive(active_);
  }
}

const void* HoneycombHostDisplayClientOSR::GetPixelMemory() const {
  return layered_window_updater_ ? layered_window_updater_->GetPixelMemory()
                                 : nullptr;
}

gfx::Size HoneycombHostDisplayClientOSR::GetPixelSize() const {
  return layered_window_updater_ ? layered_window_updater_->GetPixelSize()
                                 : gfx::Size{};
}

void HoneycombHostDisplayClientOSR::UseProxyOutputDevice(
    UseProxyOutputDeviceCallback callback) {
  std::move(callback).Run(true);
}

void HoneycombHostDisplayClientOSR::CreateLayeredWindowUpdater(
    mojo::PendingReceiver<viz::mojom::LayeredWindowUpdater> receiver) {
  layered_window_updater_ =
      std::make_unique<HoneycombLayeredWindowUpdaterOSR>(view_, std::move(receiver));
  layered_window_updater_->SetActive(active_);
}

#if BUILDFLAG(IS_LINUX)
void HoneycombHostDisplayClientOSR::DidCompleteSwapWithNewSize(
    const gfx::Size& size) {}
#endif
