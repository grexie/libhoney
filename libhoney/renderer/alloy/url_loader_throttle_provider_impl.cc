// Copyright 2018 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoney/renderer/alloy/url_loader_throttle_provider_impl.h"

#include "libhoney/common/extensions/extensions_util.h"
#include "libhoney/renderer/alloy/alloy_content_renderer_client.h"
#include "libhoney/renderer/alloy/alloy_render_thread_observer.h"

#include <utility>

#include "base/feature_list.h"
#include "chrome/common/google_url_loader_throttle.h"
#include "components/signin/public/base/signin_buildflags.h"
#include "content/public/common/content_features.h"
#include "content/public/renderer/render_frame.h"
#include "services/network/public/cpp/features.h"
#include "third_party/blink/public/common/loader/resource_type_util.h"
#include "third_party/blink/public/platform/web_url.h"

HoneycombURLLoaderThrottleProviderImpl::HoneycombURLLoaderThrottleProviderImpl(
    blink::URLLoaderThrottleProviderType type,
    AlloyContentRendererClient* alloy_content_renderer_client)
    : type_(type),
      alloy_content_renderer_client_(alloy_content_renderer_client) {
  DETACH_FROM_THREAD(thread_checker_);
}

HoneycombURLLoaderThrottleProviderImpl::~HoneycombURLLoaderThrottleProviderImpl() {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
}

HoneycombURLLoaderThrottleProviderImpl::HoneycombURLLoaderThrottleProviderImpl(
    const HoneycombURLLoaderThrottleProviderImpl& other)
    : type_(other.type_),
      alloy_content_renderer_client_(other.alloy_content_renderer_client_) {
  DETACH_FROM_THREAD(thread_checker_);
}

std::unique_ptr<blink::URLLoaderThrottleProvider>
HoneycombURLLoaderThrottleProviderImpl::Clone() {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  return base::WrapUnique(new HoneycombURLLoaderThrottleProviderImpl(*this));
}

blink::WebVector<std::unique_ptr<blink::URLLoaderThrottle>>
HoneycombURLLoaderThrottleProviderImpl::CreateThrottles(
    int render_frame_id,
    const blink::WebURLRequest& request) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);

  blink::WebVector<std::unique_ptr<blink::URLLoaderThrottle>> throttles;

  const network::mojom::RequestDestination request_destination =
      request.GetRequestDestination();

  // Some throttles have already been added in the browser for frame resources.
  // Don't add them for frame requests.
  bool is_frame_resource =
      blink::IsRequestDestinationFrame(request_destination);

  DCHECK(!is_frame_resource ||
         type_ == blink::URLLoaderThrottleProviderType::kFrame);

  throttles.emplace_back(std::make_unique<GoogleURLLoaderThrottle>(
#if BUILDFLAG(ENABLE_BOUND_SESSION_CREDENTIALS)
      /*bound_session_request_throttled_listener=*/nullptr,
#endif
      alloy_content_renderer_client_->GetAlloyObserver()->GetDynamicParams()));

  return throttles;
}

void HoneycombURLLoaderThrottleProviderImpl::SetOnline(bool is_online) {}
