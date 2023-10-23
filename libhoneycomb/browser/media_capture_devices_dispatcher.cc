// Copyright (c) 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoneycomb/browser/media_capture_devices_dispatcher.h"

#include "chrome/common/pref_names.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/pref_service.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/media_capture_devices.h"

using blink::MediaStreamDevices;
using content::BrowserThread;

namespace {

const blink::MediaStreamDevice* FindDefaultDeviceWithId(
    const blink::MediaStreamDevices& devices,
    const std::string& device_id) {
  if (devices.empty()) {
    return nullptr;
  }

  blink::MediaStreamDevices::const_iterator iter = devices.begin();
  for (; iter != devices.end(); ++iter) {
    if (iter->id == device_id) {
      return &(*iter);
    }
  }

  return &(*devices.begin());
}

}  // namespace

HoneycombMediaCaptureDevicesDispatcher*
HoneycombMediaCaptureDevicesDispatcher::GetInstance() {
  return base::Singleton<HoneycombMediaCaptureDevicesDispatcher>::get();
}

HoneycombMediaCaptureDevicesDispatcher::HoneycombMediaCaptureDevicesDispatcher() {}

HoneycombMediaCaptureDevicesDispatcher::~HoneycombMediaCaptureDevicesDispatcher() {}

void HoneycombMediaCaptureDevicesDispatcher::RegisterPrefs(
    PrefRegistrySimple* registry) {
  registry->RegisterStringPref(prefs::kDefaultAudioCaptureDevice,
                               std::string());
  registry->RegisterStringPref(prefs::kDefaultVideoCaptureDevice,
                               std::string());
}

void HoneycombMediaCaptureDevicesDispatcher::GetDefaultDevices(
    PrefService* prefs,
    bool audio,
    bool video,
    blink::MediaStreamDevices* devices) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DCHECK(audio || video);

  std::string default_device;
  if (audio) {
    default_device = prefs->GetString(prefs::kDefaultAudioCaptureDevice);
    GetRequestedDevice(default_device, true, false, devices);
  }

  if (video) {
    default_device = prefs->GetString(prefs::kDefaultVideoCaptureDevice);
    GetRequestedDevice(default_device, false, true, devices);
  }
}

void HoneycombMediaCaptureDevicesDispatcher::GetRequestedDevice(
    const std::string& requested_device_id,
    bool audio,
    bool video,
    blink::MediaStreamDevices* devices) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DCHECK(audio || video);

  if (audio) {
    const blink::MediaStreamDevices& audio_devices = GetAudioCaptureDevices();
    const blink::MediaStreamDevice* const device =
        FindDefaultDeviceWithId(audio_devices, requested_device_id);
    if (device) {
      devices->push_back(*device);
    }
  }
  if (video) {
    const blink::MediaStreamDevices& video_devices = GetVideoCaptureDevices();
    const blink::MediaStreamDevice* const device =
        FindDefaultDeviceWithId(video_devices, requested_device_id);
    if (device) {
      devices->push_back(*device);
    }
  }
}

void HoneycombMediaCaptureDevicesDispatcher::OnAudioCaptureDevicesChanged() {}

void HoneycombMediaCaptureDevicesDispatcher::OnVideoCaptureDevicesChanged() {}

void HoneycombMediaCaptureDevicesDispatcher::OnMediaRequestStateChanged(
    int render_process_id,
    int render_frame_id,
    int page_request_id,
    const GURL& security_origin,
    blink::mojom::MediaStreamType stream_type,
    content::MediaRequestState state) {}

void HoneycombMediaCaptureDevicesDispatcher::OnCreatingAudioStream(
    int render_process_id,
    int render_view_id) {}

void HoneycombMediaCaptureDevicesDispatcher::OnSetCapturingLinkSecured(
    int render_process_id,
    int render_frame_id,
    int page_request_id,
    blink::mojom::MediaStreamType stream_type,
    bool is_secure) {}

const MediaStreamDevices&
HoneycombMediaCaptureDevicesDispatcher::GetAudioCaptureDevices() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  return content::MediaCaptureDevices::GetInstance()->GetAudioCaptureDevices();
}

const MediaStreamDevices&
HoneycombMediaCaptureDevicesDispatcher::GetVideoCaptureDevices() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  return content::MediaCaptureDevices::GetInstance()->GetVideoCaptureDevices();
}
