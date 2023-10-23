// Copyright (c) 2020 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoneycomb/browser/media_router/media_sink_impl.h"

#include "libhoneycomb/browser/thread_util.h"

#include "base/strings/string_number_conversions.h"
#include "chrome/browser/media/router/discovery/dial/dial_media_sink_service_impl.h"
#include "chrome/browser/media/router/discovery/mdns/cast_media_sink_service_impl.h"
#include "chrome/browser/media/router/providers/cast/dual_media_sink_service.h"
#include "components/media_router/common/discovery/media_sink_internal.h"
#include "components/media_router/common/discovery/media_sink_service_base.h"

namespace {

using SinkServiceVector = std::vector<media_router::MediaSinkServiceBase*>;

SinkServiceVector GetSinkServices() {
  HONEYCOMB_REQUIRE_UIT();
  auto sink_service = media_router::DualMediaSinkService::GetInstance();
  return {sink_service->GetCastMediaSinkServiceImpl(),
          sink_service->GetDialMediaSinkServiceImpl()};
}

void GetSinkInternalAndContinue(
    SinkServiceVector services,
    const media_router::MediaSink::Id& sink_id,
    HoneycombRefPtr<HoneycombMediaSinkDeviceInfoCallback> callback) {
  HONEYCOMB_REQUIRE_IOT();

  HoneycombMediaSinkDeviceInfo device_info;
  const media_router::MediaSinkInternal* sink_internal = nullptr;

  for (auto service : services) {
    sink_internal = service->GetSinkById(sink_id);
    if (sink_internal) {
      break;
    }
  }

  if (sink_internal) {
    if (sink_internal->is_cast_sink()) {
      const auto& cast_data = sink_internal->cast_data();
      HoneycombString(&device_info.ip_address) =
          cast_data.ip_endpoint.ToStringWithoutPort();
      device_info.port = cast_data.ip_endpoint.port();
      HoneycombString(&device_info.model_name) = cast_data.model_name;
    } else if (sink_internal->is_dial_sink()) {
      const auto& dial_data = sink_internal->dial_data();
      HoneycombString(&device_info.ip_address) = dial_data.ip_address.ToString();
      if (dial_data.app_url.is_valid() && dial_data.app_url.has_port()) {
        base::StringToInt(dial_data.app_url.port_piece(), &device_info.port);
      }
      HoneycombString(&device_info.model_name) = dial_data.model_name;
    }
  }

  // Execute the callback on the UI thread.
  HONEYCOMB_POST_TASK(
      HONEYCOMB_UIT,
      base::BindOnce(&HoneycombMediaSinkDeviceInfoCallback::OnMediaSinkDeviceInfo,
                     callback, device_info));
}

void GetDeviceInfo(const media_router::MediaSink::Id& sink_id,
                   HoneycombRefPtr<HoneycombMediaSinkDeviceInfoCallback> callback) {
  auto next_step = base::BindOnce(
      [](const media_router::MediaSink::Id& sink_id,
         HoneycombRefPtr<HoneycombMediaSinkDeviceInfoCallback> callback) {
        HONEYCOMB_POST_TASK(HONEYCOMB_IOT,
                      base::BindOnce(GetSinkInternalAndContinue,
                                     GetSinkServices(), sink_id, callback));
      },
      sink_id, callback);

  if (HONEYCOMB_CURRENTLY_ON(HONEYCOMB_UIT)) {
    std::move(next_step).Run();
  } else {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT, std::move(next_step));
  }
}

}  // namespace

HoneycombMediaSinkImpl::HoneycombMediaSinkImpl(const media_router::MediaSink& sink)
    : sink_(sink) {}

HoneycombMediaSinkImpl::HoneycombMediaSinkImpl(
    const media_router::MediaSink::Id& sink_id,
    const std::string& sink_name,
    media_router::mojom::MediaRouteProviderId provider_id)
    : sink_(sink_id,
            sink_name,
            media_router::SinkIconType::GENERIC,
            provider_id) {}

HoneycombString HoneycombMediaSinkImpl::GetId() {
  return sink_.id();
}

HoneycombString HoneycombMediaSinkImpl::GetName() {
  return sink_.name();
}

HoneycombMediaSink::IconType HoneycombMediaSinkImpl::GetIconType() {
  // Verify that our enum matches Chromium's values.
  static_assert(static_cast<int>(HONEYCOMB_MSIT_TOTAL_COUNT) ==
                    static_cast<int>(media_router::SinkIconType::TOTAL_COUNT),
                "enum mismatch");

  return static_cast<HoneycombMediaSink::IconType>(sink_.icon_type());
}

void HoneycombMediaSinkImpl::GetDeviceInfo(
    HoneycombRefPtr<HoneycombMediaSinkDeviceInfoCallback> callback) {
  ::GetDeviceInfo(sink_.id(), callback);
}

bool HoneycombMediaSinkImpl::IsCastSink() {
  return sink_.provider_id() == media_router::mojom::MediaRouteProviderId::CAST;
}

bool HoneycombMediaSinkImpl::IsDialSink() {
  return sink_.provider_id() == media_router::mojom::MediaRouteProviderId::DIAL;
}

bool HoneycombMediaSinkImpl::IsCompatibleWith(HoneycombRefPtr<HoneycombMediaSource> source) {
  if (source) {
    if (IsCastSink()) {
      return source->IsCastSource();
    }
    if (IsDialSink()) {
      return source->IsDialSource();
    }
  }
  return false;
}
