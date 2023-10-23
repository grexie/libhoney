// Copyright (c) 2019 The Honeycomb Authors.
// Portions copyright (c) 2011 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoney/browser/audio_capturer.h"

#include "libhoney/browser/alloy/alloy_browser_host_impl.h"
#include "libhoney/browser/audio_loopback_stream_creator.h"

#include "components/mirroring/service/captured_audio_input.h"
#include "media/audio/audio_input_device.h"

namespace {

media::ChannelLayoutConfig TranslateChannelLayout(
    honey_channel_layout_t channel_layout) {
  // Verify that our enum matches Chromium's values. The enum values match
  // between those enums and existing values don't ever change, so it's enough
  // to check that there are no new ones added.
  static_assert(
      static_cast<int>(HONEYCOMB_CHANNEL_LAYOUT_MAX) ==
          static_cast<int>(media::CHANNEL_LAYOUT_MAX),
      "honey_channel_layout_t must match the ChannelLayout enum in Chromium");

  const auto layout = static_cast<media::ChannelLayout>(channel_layout);
  return {layout, media::ChannelLayoutToChannelCount(layout)};
}

void StreamCreatorHelper(
    content::WebContents* source_web_contents,
    HoneycombAudioLoopbackStreamCreator* audio_stream_creator,
    mojo::PendingRemote<mirroring::mojom::AudioStreamCreatorClient> client,
    const media::AudioParameters& params,
    uint32_t total_segments) {
  audio_stream_creator->CreateLoopbackStream(
      source_web_contents, params, total_segments,
      base::BindRepeating(
          [](mojo::PendingRemote<mirroring::mojom::AudioStreamCreatorClient>
                 client,
             mojo::PendingRemote<media::mojom::AudioInputStream> stream,
             mojo::PendingReceiver<media::mojom::AudioInputStreamClient>
                 client_receiver,
             media::mojom::ReadOnlyAudioDataPipePtr data_pipe) {
            mojo::Remote<mirroring::mojom::AudioStreamCreatorClient>
                audio_client(std::move(client));
            audio_client->StreamCreated(std::move(stream),
                                        std::move(client_receiver),
                                        std::move(data_pipe));
          },
          base::Passed(&client)));
}

}  // namespace

HoneycombAudioCapturer::HoneycombAudioCapturer(const HoneycombAudioParameters& params,
                                   HoneycombRefPtr<AlloyBrowserHostImpl> browser,
                                   HoneycombRefPtr<HoneycombAudioHandler> audio_handler)
    : params_(params),
      browser_(browser),
      audio_handler_(audio_handler),
      audio_stream_creator_(std::make_unique<HoneycombAudioLoopbackStreamCreator>()) {
  media::AudioParameters audio_params(
      media::AudioParameters::AUDIO_PCM_LINEAR,
      TranslateChannelLayout(params.channel_layout), params.sample_rate,
      params.frames_per_buffer);

  if (!audio_params.IsValid()) {
    LOG(ERROR) << "Invalid audio parameters";
    return;
  }

  DCHECK(browser_);
  DCHECK(audio_handler_);
  DCHECK(browser_->web_contents());

  channels_ = audio_params.channels();
  audio_input_device_ = new media::AudioInputDevice(
      std::make_unique<mirroring::CapturedAudioInput>(base::BindRepeating(
          &StreamCreatorHelper, base::Unretained(browser_->web_contents()),
          base::Unretained(audio_stream_creator_.get()))),
      media::AudioInputDevice::kLoopback,
      media::AudioInputDevice::DeadStreamDetection::kEnabled);

  audio_input_device_->Initialize(audio_params, this);
  audio_input_device_->Start();
}

HoneycombAudioCapturer::~HoneycombAudioCapturer() {
  StopStream();
}

void HoneycombAudioCapturer::OnCaptureStarted() {
  audio_handler_->OnAudioStreamStarted(browser_, params_, channels_);
  DCHECK(!capturing_);
  capturing_ = true;
}

void HoneycombAudioCapturer::Capture(const media::AudioBus* source,
                               base::TimeTicks audio_capture_time,
                               double /*volume*/,
                               bool /*key_pressed*/) {
  const int channels = source->channels();
  std::array<const float*, media::CHANNELS_MAX> data;
  DCHECK(channels == channels_);
  DCHECK(channels <= static_cast<int>(data.size()));
  for (int c = 0; c < channels; ++c) {
    data[c] = source->channel(c);
  }
  base::TimeDelta pts = audio_capture_time - base::TimeTicks::UnixEpoch();
  audio_handler_->OnAudioStreamPacket(browser_, data.data(), source->frames(),
                                      pts.InMilliseconds());
}

void HoneycombAudioCapturer::OnCaptureError(
    media::AudioCapturerSource::ErrorCode code,
    const std::string& message) {
  audio_handler_->OnAudioStreamError(browser_, message);
  StopStream();
}

void HoneycombAudioCapturer::StopStream() {
  if (audio_input_device_) {
    audio_input_device_->Stop();
  }
  if (capturing_) {
    audio_handler_->OnAudioStreamStopped(browser_);
  }

  audio_input_device_ = nullptr;
  capturing_ = false;
}