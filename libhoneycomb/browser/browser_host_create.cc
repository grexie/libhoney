// Copyright (c) 2012 The Honeycomb Authors.
// Portions copyright (c) 2011 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "include/honey_browser.h"
#include "libhoneycomb/browser/alloy/alloy_browser_host_impl.h"
#include "libhoneycomb/browser/chrome/chrome_browser_host_impl.h"
#include "libhoneycomb/browser/chrome/views/chrome_child_window.h"
#include "libhoneycomb/browser/context.h"
#include "libhoneycomb/browser/thread_util.h"
#include "libhoneycomb/features/runtime.h"

namespace {

class CreateBrowserHelper {
 public:
  CreateBrowserHelper(const HoneycombWindowInfo& windowInfo,
                      HoneycombRefPtr<HoneycombClient> client,
                      const HoneycombString& url,
                      const HoneycombBrowserSettings& settings,
                      HoneycombRefPtr<HoneycombDictionaryValue> extra_info,
                      HoneycombRefPtr<HoneycombRequestContext> request_context)
      : window_info_(windowInfo),
        client_(client),
        url_(url),
        settings_(settings),
        extra_info_(extra_info),
        request_context_(request_context) {}

  void Run() {
    HoneycombBrowserHost::CreateBrowserSync(window_info_, client_, url_, settings_,
                                      extra_info_, request_context_);
  }

  HoneycombWindowInfo window_info_;
  HoneycombRefPtr<HoneycombClient> client_;
  HoneycombString url_;
  HoneycombBrowserSettings settings_;
  HoneycombRefPtr<HoneycombDictionaryValue> extra_info_;
  HoneycombRefPtr<HoneycombRequestContext> request_context_;
};

}  // namespace

// static
bool HoneycombBrowserHost::CreateBrowser(
    const HoneycombWindowInfo& windowInfo,
    HoneycombRefPtr<HoneycombClient> client,
    const HoneycombString& url,
    const HoneycombBrowserSettings& settings,
    HoneycombRefPtr<HoneycombDictionaryValue> extra_info,
    HoneycombRefPtr<HoneycombRequestContext> request_context) {
  // Verify that the context is in a valid state.
  if (!CONTEXT_STATE_VALID()) {
    DCHECK(false) << "context not valid";
    return false;
  }

  // Verify that the settings structure is a valid size.
  if (settings.size != sizeof(honey_browser_settings_t)) {
    DCHECK(false) << "invalid HoneycombBrowserSettings structure size";
    return false;
  }

  // Verify windowless rendering requirements.
  if (windowInfo.windowless_rendering_enabled &&
      !client->GetRenderHandler().get()) {
    DCHECK(false) << "HoneycombRenderHandler implementation is required";
    return false;
  }

  if (windowInfo.windowless_rendering_enabled &&
      !HoneycombContext::Get()->settings().windowless_rendering_enabled) {
    LOG(ERROR) << "Creating a windowless browser without setting "
                  "HoneycombSettings.windowless_rendering_enabled may result in "
                  "reduced performance or runtime errors.";
  }

  if (!request_context) {
    request_context = HoneycombRequestContext::GetGlobalContext();
  }

  auto helper = std::make_unique<CreateBrowserHelper>(
      windowInfo, client, url, settings, extra_info, request_context);

  auto request_context_impl =
      static_cast<HoneycombRequestContextImpl*>(request_context.get());

  // Wait for the browser context to be initialized before creating the browser.
  request_context_impl->ExecuteWhenBrowserContextInitialized(base::BindOnce(
      [](std::unique_ptr<CreateBrowserHelper> helper) {
        // Always execute asynchronously to avoid potential issues if we're
        // being called synchronously during app initialization.
        HONEYCOMB_POST_TASK(HONEYCOMB_UIT, base::BindOnce(&CreateBrowserHelper::Run,
                                              std::move(helper)));
      },
      std::move(helper)));

  return true;
}

// static
HoneycombRefPtr<HoneycombBrowser> HoneycombBrowserHost::CreateBrowserSync(
    const HoneycombWindowInfo& windowInfo,
    HoneycombRefPtr<HoneycombClient> client,
    const HoneycombString& url,
    const HoneycombBrowserSettings& settings,
    HoneycombRefPtr<HoneycombDictionaryValue> extra_info,
    HoneycombRefPtr<HoneycombRequestContext> request_context) {
  // Verify that the context is in a valid state.
  if (!CONTEXT_STATE_VALID()) {
    DCHECK(false) << "context not valid";
    return nullptr;
  }

  // Verify that the settings structure is a valid size.
  if (settings.size != sizeof(honey_browser_settings_t)) {
    DCHECK(false) << "invalid HoneycombBrowserSettings structure size";
    return nullptr;
  }

  if (!request_context) {
    request_context = HoneycombRequestContext::GetGlobalContext();
  }

  // Verify that the browser context is valid.
  auto request_context_impl =
      static_cast<HoneycombRequestContextImpl*>(request_context.get());
  if (!request_context_impl->VerifyBrowserContext()) {
    return nullptr;
  }

  // Verify windowless rendering requirements.
  if (windowInfo.windowless_rendering_enabled &&
      !client->GetRenderHandler().get()) {
    DCHECK(false) << "HoneycombRenderHandler implementation is required";
    return nullptr;
  }

  HoneycombBrowserCreateParams create_params;
  create_params.MaybeSetWindowInfo(windowInfo);
  create_params.client = client;
  create_params.url = url;
  create_params.settings = settings;
  create_params.extra_info = extra_info;
  create_params.request_context = request_context;

  return HoneycombBrowserHostBase::Create(create_params);
}

void HoneycombBrowserCreateParams::MaybeSetWindowInfo(
    const HoneycombWindowInfo& new_window_info) {
  if (!honey::IsChromeRuntimeEnabled() ||
      chrome_child_window::HasParentHandle(new_window_info)) {
    window_info = std::make_unique<HoneycombWindowInfo>(new_window_info);
  }
}

// static
HoneycombRefPtr<HoneycombBrowserHostBase> HoneycombBrowserHostBase::Create(
    HoneycombBrowserCreateParams& create_params) {
  if (honey::IsChromeRuntimeEnabled()) {
    if (auto browser =
            chrome_child_window::MaybeCreateChildBrowser(create_params)) {
      return browser.get();
    }
    auto browser = ChromeBrowserHostImpl::Create(create_params);
    return browser.get();
  }

  auto browser = AlloyBrowserHostImpl::Create(create_params);
  return browser.get();
}
