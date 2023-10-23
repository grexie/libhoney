// Copyright 2018 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_RENDERER_ALLOY_URL_LOADER_THROTTLE_PROVIDER_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_RENDERER_ALLOY_URL_LOADER_THROTTLE_PROVIDER_IMPL_H_

#include <memory>
#include <vector>

#include "base/threading/thread_checker.h"
#include "third_party/blink/public/platform/url_loader_throttle_provider.h"

class AlloyContentRendererClient;

// Instances must be constructed on the render thread, and then used and
// destructed on a single thread, which can be different from the render thread.
class HoneycombURLLoaderThrottleProviderImpl
    : public blink::URLLoaderThrottleProvider {
 public:
  HoneycombURLLoaderThrottleProviderImpl(
      blink::URLLoaderThrottleProviderType type,
      AlloyContentRendererClient* alloy_content_renderer_client);

  HoneycombURLLoaderThrottleProviderImpl& operator=(
      const HoneycombURLLoaderThrottleProviderImpl&) = delete;

  ~HoneycombURLLoaderThrottleProviderImpl() override;

  // blink::URLLoaderThrottleProvider implementation.
  std::unique_ptr<blink::URLLoaderThrottleProvider> Clone() override;
  blink::WebVector<std::unique_ptr<blink::URLLoaderThrottle>> CreateThrottles(
      int render_frame_id,
      const blink::WebURLRequest& request) override;
  void SetOnline(bool is_online) override;

 private:
  // This copy constructor works in conjunction with Clone(), not intended for
  // general use.
  HoneycombURLLoaderThrottleProviderImpl(
      const HoneycombURLLoaderThrottleProviderImpl& other);

  blink::URLLoaderThrottleProviderType type_;
  AlloyContentRendererClient* const alloy_content_renderer_client_;

  THREAD_CHECKER(thread_checker_);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_RENDERER_ALLOY_URL_LOADER_THROTTLE_PROVIDER_IMPL_H_
