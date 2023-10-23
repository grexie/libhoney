// Copyright (c) 2019 The Honeycomb Authors.
// Portions copyright (c) 2011 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_AUDIO_CAPTURER_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_AUDIO_CAPTURER_H_
#pragma once

#include "include/internal/honey_ptr.h"
#include "include/internal/honey_types_wrappers.h"

#include "media/base/audio_capturer_source.h"

namespace media {
class AudioInputDevice;
}  // namespace media

class HoneycombAudioHandler;
class HoneycombAudioLoopbackStreamCreator;
class AlloyBrowserHostImpl;

class HoneycombAudioCapturer : public media::AudioCapturerSource::CaptureCallback {
 public:
  HoneycombAudioCapturer(const HoneycombAudioParameters& params,
                   HoneycombRefPtr<AlloyBrowserHostImpl> browser,
                   HoneycombRefPtr<HoneycombAudioHandler> audio_handler);
  ~HoneycombAudioCapturer() override;

 private:
  void OnCaptureStarted() override;
  void Capture(const media::AudioBus* audio_source,
               base::TimeTicks audio_capture_time,
               double volume,
               bool key_pressed) override;
  void OnCaptureError(media::AudioCapturerSource::ErrorCode code,
                      const std::string& message) override;
  void OnCaptureMuted(bool is_muted) override {}

  void StopStream();

  HoneycombAudioParameters params_;
  HoneycombRefPtr<AlloyBrowserHostImpl> browser_;
  HoneycombRefPtr<HoneycombAudioHandler> audio_handler_;
  std::unique_ptr<HoneycombAudioLoopbackStreamCreator> audio_stream_creator_;
  scoped_refptr<media::AudioInputDevice> audio_input_device_;
  bool capturing_ = false;
  int channels_ = 0;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_AUDIO_CAPTURER_H_