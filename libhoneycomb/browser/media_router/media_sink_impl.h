// Copyright (c) 2020 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_MEDIA_ROUTER_MEDIA_SINK_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_MEDIA_ROUTER_MEDIA_SINK_IMPL_H_
#pragma once

#include "include/honey_media_router.h"

#include "components/media_router/common/media_sink.h"

// Implementation of the HoneycombMediaSink interface. May be created on any thread.
class HoneycombMediaSinkImpl : public HoneycombMediaSink {
 public:
  explicit HoneycombMediaSinkImpl(const media_router::MediaSink& sink);
  HoneycombMediaSinkImpl(const media_router::MediaSink::Id& sink_id,
                   const std::string& sink_name,
                   media_router::mojom::MediaRouteProviderId provider_id);

  HoneycombMediaSinkImpl(const HoneycombMediaSinkImpl&) = delete;
  HoneycombMediaSinkImpl& operator=(const HoneycombMediaSinkImpl&) = delete;

  // HoneycombMediaSink methods.
  HoneycombString GetId() override;
  HoneycombString GetName() override;
  IconType GetIconType() override;
  void GetDeviceInfo(
      HoneycombRefPtr<HoneycombMediaSinkDeviceInfoCallback> callback) override;
  bool IsCastSink() override;
  bool IsDialSink() override;
  bool IsCompatibleWith(HoneycombRefPtr<HoneycombMediaSource> source) override;

  const media_router::MediaSink& sink() const { return sink_; }

 private:
  // Read-only after creation.
  const media_router::MediaSink sink_;

  IMPLEMENT_REFCOUNTING(HoneycombMediaSinkImpl);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_MEDIA_ROUTER_MEDIA_SINK_IMPL_H_
