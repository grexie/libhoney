// Copyright (c) 2020 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoney/browser/media_router/media_source_impl.h"

HoneycombMediaSourceImpl::HoneycombMediaSourceImpl(
    const media_router::MediaSource::Id& source_id)
    : source_(source_id) {}

HoneycombMediaSourceImpl::HoneycombMediaSourceImpl(const GURL& presentation_url)
    : source_(presentation_url) {}

HoneycombString HoneycombMediaSourceImpl::GetId() {
  return source_.id();
}

bool HoneycombMediaSourceImpl::IsCastSource() {
  return !IsDialSource();
}

bool HoneycombMediaSourceImpl::IsDialSource() {
  return source_.IsDialSource();
}
