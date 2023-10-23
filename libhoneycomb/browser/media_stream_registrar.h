// Copyright 2022 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_MEDIA_STREAM_REGISTRAR_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_MEDIA_STREAM_REGISTRAR_H_
#pragma once

#include <map>
#include <memory>
#include <string>

#include "base/memory/weak_ptr.h"
#include "content/public/browser/media_stream_request.h"

class HoneycombBrowserHostBase;
class HoneycombMediaStreamUI;

class HoneycombMediaStreamRegistrar {
 public:
  explicit HoneycombMediaStreamRegistrar(HoneycombBrowserHostBase* browser);

  HoneycombMediaStreamRegistrar(const HoneycombMediaStreamRegistrar&) = delete;
  HoneycombMediaStreamRegistrar& operator=(const HoneycombMediaStreamRegistrar&) = delete;

  std::unique_ptr<content::MediaStreamUI> MaybeCreateMediaStreamUI(
      bool has_video,
      bool has_audio);

 private:
  friend class HoneycombMediaStreamUI;

  // Called from HoneycombMediaStreamUI.
  void RegisterMediaStream(const std::string& label, bool video, bool audio);
  void UnregisterMediaStream(const std::string& label);

  void NotifyMediaStreamChange();

  // Guaranteed to outlive this object.
  HoneycombBrowserHostBase* const browser_;

  struct MediaStreamInfo {
    bool video;
    bool audio;
  };

  // Current in use media streams.
  std::map<std::string, MediaStreamInfo> registered_streams_;

  // Last notified media stream info.
  MediaStreamInfo last_notified_info_{};

  base::WeakPtrFactory<HoneycombMediaStreamRegistrar> weak_ptr_factory_{this};
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_MEDIA_STREAM_REGISTRAR_H_
