// Copyright (c) 2020 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_MEDIA_ROUTER_MEDIA_SOURCE_IMPL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_MEDIA_ROUTER_MEDIA_SOURCE_IMPL_H_
#pragma once

#include "include/honey_media_router.h"

#include "components/media_router/common/media_source.h"

// Implementation of the HoneycombMediaSource interface. May be created on any thread.
class HoneycombMediaSourceImpl : public HoneycombMediaSource {
 public:
  explicit HoneycombMediaSourceImpl(const media_router::MediaSource::Id& source_id);
  explicit HoneycombMediaSourceImpl(const GURL& presentation_url);

  HoneycombMediaSourceImpl(const HoneycombMediaSourceImpl&) = delete;
  HoneycombMediaSourceImpl& operator=(const HoneycombMediaSourceImpl&) = delete;

  // HoneycombMediaSource methods.
  HoneycombString GetId() override;
  bool IsCastSource() override;
  bool IsDialSource() override;

  const media_router::MediaSource& source() const { return source_; }

 private:
  // Read-only after creation.
  const media_router::MediaSource source_;

  IMPLEMENT_REFCOUNTING(HoneycombMediaSourceImpl);
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_MEDIA_ROUTER_MEDIA_SOURCE_IMPL_H_
