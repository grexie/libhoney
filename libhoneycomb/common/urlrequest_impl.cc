// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "include/honey_urlrequest.h"
#include "libhoneycomb/browser/net_service/browser_urlrequest_impl.h"
#include "libhoneycomb/common/app_manager.h"
#include "libhoneycomb/common/task_runner_impl.h"
#include "libhoneycomb/features/runtime_checks.h"

#include "base/logging.h"
#include "base/notreached.h"
#include "content/public/common/content_client.h"

// static
HoneycombRefPtr<HoneycombURLRequest> HoneycombURLRequest::Create(
    HoneycombRefPtr<HoneycombRequest> request,
    HoneycombRefPtr<HoneycombURLRequestClient> client,
    HoneycombRefPtr<HoneycombRequestContext> request_context) {
  if (!request.get() || !client.get()) {
    DCHECK(false) << "called with invalid parameters";
    return nullptr;
  }

  if (!HoneycombTaskRunnerImpl::GetCurrentTaskRunner()) {
    DCHECK(false) << "called on invalid thread";
    return nullptr;
  }

  auto content_client = HoneycombAppManager::Get()->GetContentClient();
  if (content_client->browser()) {
    // In the browser process.
    HoneycombRefPtr<HoneycombBrowserURLRequest> impl =
        new HoneycombBrowserURLRequest(nullptr, request, client, request_context);
    if (impl->Start()) {
      return impl.get();
    }
    return nullptr;
  } else {
    DCHECK(false) << "called in unsupported process";
    return nullptr;
  }
}
