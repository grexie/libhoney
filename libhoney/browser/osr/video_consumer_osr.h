#ifndef LIBHONEY_BROWSER_OSR_VIDEO_CONSUMER_OSR_H_
#define LIBHONEY_BROWSER_OSR_VIDEO_CONSUMER_OSR_H_

#include "base/functional/callback.h"
#include "components/viz/host/client_frame_sink_video_capturer.h"
#include "media/capture/mojom/video_capture_types.mojom.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

class HoneycombRenderWidgetHostViewOSR;

class HoneycombVideoConsumerOSR : public viz::mojom::FrameSinkVideoConsumer {
 public:
  explicit HoneycombVideoConsumerOSR(HoneycombRenderWidgetHostViewOSR* view);

  HoneycombVideoConsumerOSR(const HoneycombVideoConsumerOSR&) = delete;
  HoneycombVideoConsumerOSR& operator=(const HoneycombVideoConsumerOSR&) = delete;

  ~HoneycombVideoConsumerOSR() override;

  void SetActive(bool active);
  void SetFrameRate(base::TimeDelta frame_rate);
  void SizeChanged(const gfx::Size& size_in_pixels);
  void RequestRefreshFrame(const absl::optional<gfx::Rect>& bounds_in_pixels);

 private:
  // viz::mojom::FrameSinkVideoConsumer implementation.
  void OnFrameCaptured(
      media::mojom::VideoBufferHandlePtr data,
      media::mojom::VideoFrameInfoPtr info,
      const gfx::Rect& content_rect,
      mojo::PendingRemote<viz::mojom::FrameSinkVideoConsumerFrameCallbacks>
          callbacks) override;
  void OnNewCropVersion(uint32_t crop_version) override {}
  void OnFrameWithEmptyRegionCapture() override {}
  void OnStopped() override {}
  void OnLog(const std::string& message) override {}

  HoneycombRenderWidgetHostViewOSR* const view_;
  std::unique_ptr<viz::ClientFrameSinkVideoCapturer> video_capturer_;

  gfx::Size size_in_pixels_;
  absl::optional<gfx::Rect> bounds_in_pixels_;
};

#endif  // LIBHONEY_BROWSER_OSR_VIDEO_CONSUMER_OSR_H_