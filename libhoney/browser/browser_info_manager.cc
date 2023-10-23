// Copyright 2015 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "libhoney/browser/browser_info_manager.h"

#include <utility>

#include "libhoney/browser/browser_host_base.h"
#include "libhoney/browser/browser_platform_delegate.h"
#include "libhoney/browser/extensions/browser_extensions_util.h"
#include "libhoney/browser/thread_util.h"
#include "libhoney/common/honey_switches.h"
#include "libhoney/common/extensions/extensions_util.h"
#include "libhoney/common/frame_util.h"
#include "libhoney/common/values_impl.h"
#include "libhoney/features/runtime_checks.h"

#include "base/command_line.h"
#include "base/logging.h"
#include "base/task/sequenced_task_runner.h"
#include "content/public/browser/child_process_host.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/url_constants.h"

namespace {

// Timeout delay for new browser info responses.
const int64_t kNewBrowserInfoResponseTimeoutMs = 2000;

void TranslatePopupFeatures(const blink::mojom::WindowFeatures& webKitFeatures,
                            HoneycombPopupFeatures& features) {
  features.x = static_cast<int>(webKitFeatures.bounds.x());
  features.xSet = webKitFeatures.has_x;
  features.y = static_cast<int>(webKitFeatures.bounds.y());
  features.ySet = webKitFeatures.has_y;
  features.width = static_cast<int>(webKitFeatures.bounds.width());
  features.widthSet = webKitFeatures.has_width;
  features.height = static_cast<int>(webKitFeatures.bounds.height());
  features.heightSet = webKitFeatures.has_height;

  features.isPopup = webKitFeatures.is_popup;
}

HoneycombBrowserInfoManager* g_info_manager = nullptr;

}  // namespace

HoneycombBrowserInfoManager::HoneycombBrowserInfoManager() {
  DCHECK(!g_info_manager);
  g_info_manager = this;
}

HoneycombBrowserInfoManager::~HoneycombBrowserInfoManager() {
  DCHECK(browser_info_list_.empty());
  g_info_manager = nullptr;
}

// static
HoneycombBrowserInfoManager* HoneycombBrowserInfoManager::GetInstance() {
  return g_info_manager;
}

scoped_refptr<HoneycombBrowserInfo> HoneycombBrowserInfoManager::CreateBrowserInfo(
    bool is_popup,
    bool is_windowless,
    HoneycombRefPtr<HoneycombDictionaryValue> extra_info) {
  base::AutoLock lock_scope(browser_info_lock_);

  scoped_refptr<HoneycombBrowserInfo> browser_info = new HoneycombBrowserInfo(
      ++next_browser_id_, is_popup, is_windowless, extra_info);
  browser_info_list_.push_back(browser_info);

  return browser_info;
}

scoped_refptr<HoneycombBrowserInfo> HoneycombBrowserInfoManager::CreatePopupBrowserInfo(
    content::WebContents* new_contents,
    bool is_windowless,
    HoneycombRefPtr<HoneycombDictionaryValue> extra_info) {
  base::AutoLock lock_scope(browser_info_lock_);

  auto frame_host = new_contents->GetPrimaryMainFrame();

  scoped_refptr<HoneycombBrowserInfo> browser_info =
      new HoneycombBrowserInfo(++next_browser_id_, true, is_windowless, extra_info);
  browser_info_list_.push_back(browser_info);

  // Continue any pending NewBrowserInfo request.
  auto it = pending_new_browser_info_map_.find(frame_host->GetGlobalId());
  if (it != pending_new_browser_info_map_.end()) {
    SendNewBrowserInfoResponse(browser_info, /*is_guest_view=*/false,
                               std::move(it->second->callback),
                               it->second->callback_runner);
    pending_new_browser_info_map_.erase(it);
  }

  return browser_info;
}

bool HoneycombBrowserInfoManager::CanCreateWindow(
    content::RenderFrameHost* opener,
    const GURL& target_url,
    const content::Referrer& referrer,
    const std::string& frame_name,
    WindowOpenDisposition disposition,
    const blink::mojom::WindowFeatures& features,
    bool user_gesture,
    bool opener_suppressed,
    bool* no_javascript_access) {
  HONEYCOMB_REQUIRE_UIT();

  content::OpenURLParams params(target_url, referrer, disposition,
                                ui::PAGE_TRANSITION_LINK,
                                /*is_renderer_initiated=*/true);
  params.user_gesture = user_gesture;

  HoneycombRefPtr<HoneycombBrowserHostBase> browser;
  if (!MaybeAllowNavigation(opener, params, browser) || !browser) {
    // Cancel the popup.
    return false;
  }

  HoneycombRefPtr<HoneycombClient> client = browser->GetClient();
  bool allow = true;
  bool handled = false;

  HoneycombWindowInfo window_info;

#if BUILDFLAG(IS_WIN)
  window_info.SetAsPopup(nullptr, HoneycombString());
#endif

  auto pending_popup = std::make_unique<HoneycombBrowserInfoManager::PendingPopup>();
  pending_popup->step = PendingPopup::CAN_CREATE_WINDOW;
  pending_popup->opener_global_id = opener->GetGlobalId();
  pending_popup->target_url = target_url;
  pending_popup->target_frame_name = frame_name;

  // Start with the current browser's settings.
  pending_popup->client = client;
  pending_popup->settings = browser->settings();

  // With the Chrome runtime, we want to use default popup Browser creation
  // for document picture-in-picture.
  pending_popup->use_default_browser_creation =
      disposition == WindowOpenDisposition::NEW_PICTURE_IN_PICTURE;

  if (client.get()) {
    HoneycombRefPtr<HoneycombLifeSpanHandler> handler = client->GetLifeSpanHandler();
    if (handler.get()) {
      HoneycombRefPtr<HoneycombFrame> opener_frame = browser->GetFrameForHost(opener);
      DCHECK(opener_frame);

      HoneycombPopupFeatures honey_features;
      TranslatePopupFeatures(features, honey_features);

      // Default to the size from the popup features.
      if (honey_features.xSet) {
        window_info.bounds.x = honey_features.x;
      }
      if (honey_features.ySet) {
        window_info.bounds.y = honey_features.y;
      }
      if (honey_features.widthSet) {
        window_info.bounds.width = honey_features.width;
      }
      if (honey_features.heightSet) {
        window_info.bounds.height = honey_features.height;
      }

      allow = !handler->OnBeforePopup(
          browser.get(), opener_frame, pending_popup->target_url.spec(),
          pending_popup->target_frame_name,
          static_cast<honey_window_open_disposition_t>(disposition), user_gesture,
          honey_features, window_info, pending_popup->client,
          pending_popup->settings, pending_popup->extra_info,
          no_javascript_access);
      handled = true;
    }
  }

  if (allow) {
    HoneycombBrowserCreateParams create_params;
    create_params.MaybeSetWindowInfo(window_info);

    if (!handled) {
      // Use default Browser creation if OnBeforePopup was unhandled.
      // TODO(chrome): Expose a mechanism for the client to choose default
      // creation.
      pending_popup->use_default_browser_creation = true;
    }

    // In most cases, Views-hosted browsers should create Views-hosted popups
    // and native browsers should use default popup handling. With the Chrome
    // runtime, we should additionally use default handling (a) when using an
    // external parent and (b) when using default Browser creation.
    create_params.popup_with_views_hosted_opener =
        browser->HasView() &&
        !browser->platform_delegate()->HasExternalParent() &&
        !pending_popup->use_default_browser_creation;

    create_params.settings = pending_popup->settings;
    create_params.client = pending_popup->client;
    create_params.extra_info = pending_popup->extra_info;

    pending_popup->platform_delegate =
        HoneycombBrowserPlatformDelegate::Create(create_params);
    CHECK(pending_popup->platform_delegate.get());

    // Between the calls to CanCreateWindow and GetCustomWebContentsView
    // RenderViewHostImpl::CreateNewWindow() will call
    // RenderProcessHostImpl::FilterURL() which, in the case of "javascript:"
    // URIs, rewrites the URL to "about:blank". We need to apply the same filter
    // otherwise GetCustomWebContentsView will fail to retrieve the PopupInfo.
    opener->GetProcess()->FilterURL(false, &pending_popup->target_url);

    PushPendingPopup(std::move(pending_popup));
  }

  return allow;
}

void HoneycombBrowserInfoManager::GetCustomWebContentsView(
    const GURL& target_url,
    const content::GlobalRenderFrameHostId& opener_global_id,
    content::WebContentsView** view,
    content::RenderViewHostDelegateView** delegate_view) {
  HONEYCOMB_REQUIRE_UIT();
  REQUIRE_ALLOY_RUNTIME();

  auto pending_popup = PopPendingPopup(PendingPopup::CAN_CREATE_WINDOW,
                                       opener_global_id, target_url);
  DCHECK(pending_popup.get());
  DCHECK(pending_popup->platform_delegate.get());

  if (pending_popup->platform_delegate->IsWindowless()) {
    pending_popup->platform_delegate->CreateViewForWebContents(view,
                                                               delegate_view);
  }

  pending_popup->step = PendingPopup::GET_CUSTOM_WEB_CONTENTS_VIEW;
  PushPendingPopup(std::move(pending_popup));
}

void HoneycombBrowserInfoManager::WebContentsCreated(
    const GURL& target_url,
    const content::GlobalRenderFrameHostId& opener_global_id,
    HoneycombBrowserSettings& settings,
    HoneycombRefPtr<HoneycombClient>& client,
    std::unique_ptr<HoneycombBrowserPlatformDelegate>& platform_delegate,
    HoneycombRefPtr<HoneycombDictionaryValue>& extra_info,
    content::WebContents* new_contents) {
  HONEYCOMB_REQUIRE_UIT();

  // GET_CUSTOM_WEB_CONTENTS_VIEW is only used with the alloy runtime.
  const auto previous_step = honey::IsAlloyRuntimeEnabled()
                                 ? PendingPopup::GET_CUSTOM_WEB_CONTENTS_VIEW
                                 : PendingPopup::CAN_CREATE_WINDOW;

  auto pending_popup =
      PopPendingPopup(previous_step, opener_global_id, target_url);
  DCHECK(pending_popup.get());
  DCHECK(pending_popup->platform_delegate.get());

  settings = pending_popup->settings;
  client = pending_popup->client;
  platform_delegate = std::move(pending_popup->platform_delegate);
  extra_info = pending_popup->extra_info;

  // AddWebContents (the next step) is only used with the Chrome runtime.
  if (honey::IsChromeRuntimeEnabled()) {
    pending_popup->step = PendingPopup::WEB_CONTENTS_CREATED;
    pending_popup->new_contents = new_contents;
    PushPendingPopup(std::move(pending_popup));
  }
}

bool HoneycombBrowserInfoManager::AddWebContents(content::WebContents* new_contents) {
  HONEYCOMB_REQUIRE_UIT();
  DCHECK(honey::IsChromeRuntimeEnabled());

  // Pending popup information may be missing in cases where
  // chrome::AddWebContents is called directly from the Chrome UI (profile
  // settings, etc).
  auto pending_popup =
      PopPendingPopup(PendingPopup::WEB_CONTENTS_CREATED, new_contents);
  if (pending_popup) {
    return !pending_popup->use_default_browser_creation;
  }

  // Proceed with default handling.
  return false;
}

void HoneycombBrowserInfoManager::OnGetNewBrowserInfo(
    const content::GlobalRenderFrameHostId& global_id,
    honey::mojom::BrowserManager::GetNewBrowserInfoCallback callback) {
  DCHECK(frame_util::IsValidGlobalId(global_id));
  DCHECK(callback);

  auto callback_runner = base::SequencedTaskRunner::GetCurrentDefault();

  base::AutoLock lock_scope(browser_info_lock_);

  bool is_guest_view = false;

  scoped_refptr<HoneycombBrowserInfo> browser_info =
      GetBrowserInfoInternal(global_id, &is_guest_view);

  if (browser_info) {
    // Send the response immediately.
    SendNewBrowserInfoResponse(browser_info, is_guest_view, std::move(callback),
                               callback_runner);
    return;
  }

  // Verify that no request for the same route is currently queued.
  DCHECK(pending_new_browser_info_map_.find(global_id) ==
         pending_new_browser_info_map_.end());

  const int timeout_id = ++next_timeout_id_;

  // Queue the request.
  std::unique_ptr<PendingNewBrowserInfo> pending(new PendingNewBrowserInfo());
  pending->global_id = global_id;
  pending->timeout_id = timeout_id;
  pending->callback = std::move(callback);
  pending->callback_runner = callback_runner;
  pending_new_browser_info_map_.insert(
      std::make_pair(global_id, std::move(pending)));

  // Register a timeout for the pending response so that the renderer process
  // doesn't hang forever. With the Chrome runtime, timeouts may occur in cases
  // where chrome::AddWebContents or WebContents::Create are called directly
  // from the Chrome UI (profile settings, etc).
  if (!base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kDisableNewBrowserInfoTimeout)) {
    HONEYCOMB_POST_DELAYED_TASK(
        HONEYCOMB_UIT,
        base::BindOnce(&HoneycombBrowserInfoManager::TimeoutNewBrowserInfoResponse,
                       global_id, timeout_id),
        kNewBrowserInfoResponseTimeoutMs);
  }
}

void HoneycombBrowserInfoManager::RemoveBrowserInfo(
    scoped_refptr<HoneycombBrowserInfo> browser_info) {
  base::AutoLock lock_scope(browser_info_lock_);

  BrowserInfoList::iterator it = browser_info_list_.begin();
  for (; it != browser_info_list_.end(); ++it) {
    if (*it == browser_info) {
      browser_info_list_.erase(it);
      return;
    }
  }

  DCHECK(false);
}

void HoneycombBrowserInfoManager::DestroyAllBrowsers() {
  BrowserInfoList list;

  {
    base::AutoLock lock_scope(browser_info_lock_);
    list = browser_info_list_;
  }

  // Destroy any remaining browser windows.
  if (!list.empty()) {
    BrowserInfoList::iterator it = list.begin();
    for (; it != list.end(); ++it) {
      HoneycombRefPtr<HoneycombBrowserHostBase> browser = (*it)->browser();
      DCHECK(browser.get());
      if (browser.get()) {
        // DestroyBrowser will call RemoveBrowserInfo.
        browser->DestroyBrowser();
      }
    }
  }

#if DCHECK_IS_ON()
  {
    // Verify that all browser windows have been destroyed.
    base::AutoLock lock_scope(browser_info_lock_);
    DCHECK(browser_info_list_.empty());
  }
#endif
}

scoped_refptr<HoneycombBrowserInfo> HoneycombBrowserInfoManager::GetBrowserInfo(
    const content::GlobalRenderFrameHostId& global_id,
    bool* is_guest_view) {
  base::AutoLock lock_scope(browser_info_lock_);
  return GetBrowserInfoInternal(global_id, is_guest_view);
}

bool HoneycombBrowserInfoManager::MaybeAllowNavigation(
    content::RenderFrameHost* opener,
    const content::OpenURLParams& params,
    HoneycombRefPtr<HoneycombBrowserHostBase>& browser_out) const {
  HONEYCOMB_REQUIRE_UIT();

  bool is_guest_view = false;
  auto browser = extensions::GetOwnerBrowserForHost(opener, &is_guest_view);
  if (!browser) {
    // Print preview uses a modal dialog where we don't own the WebContents.
    // Allow that navigation to proceed.
    return true;
  }

  if (!browser->MaybeAllowNavigation(opener, is_guest_view, params)) {
    return false;
  }

  browser_out = browser;
  return true;
}

HoneycombBrowserInfoManager::BrowserInfoList
HoneycombBrowserInfoManager::GetBrowserInfoList() {
  base::AutoLock lock_scope(browser_info_lock_);
  BrowserInfoList copy;
  copy.assign(browser_info_list_.begin(), browser_info_list_.end());
  return copy;
}

void HoneycombBrowserInfoManager::RenderProcessHostDestroyed(
    content::RenderProcessHost* host) {
  HONEYCOMB_REQUIRE_UIT();

  host->RemoveObserver(this);

  const int render_process_id = host->GetID();
  DCHECK_GT(render_process_id, 0);

  // Remove all pending requests that reference the destroyed host.
  {
    base::AutoLock lock_scope(browser_info_lock_);

    PendingNewBrowserInfoMap::iterator it =
        pending_new_browser_info_map_.begin();
    while (it != pending_new_browser_info_map_.end()) {
      const auto& info = it->second;
      if (info->global_id.child_id == render_process_id) {
        CancelNewBrowserInfoResponse(info.get());
        it = pending_new_browser_info_map_.erase(it);
      } else {
        ++it;
      }
    }
  }

  // Remove all pending popups that reference the destroyed host as the opener.
  {
    PendingPopupList::iterator it = pending_popup_list_.begin();
    while (it != pending_popup_list_.end()) {
      PendingPopup* popup = it->get();
      if (popup->opener_global_id.child_id == render_process_id) {
        it = pending_popup_list_.erase(it);
      } else {
        ++it;
      }
    }
  }
}

void HoneycombBrowserInfoManager::PushPendingPopup(
    std::unique_ptr<PendingPopup> popup) {
  HONEYCOMB_REQUIRE_UIT();
  pending_popup_list_.push_back(std::move(popup));
}

std::unique_ptr<HoneycombBrowserInfoManager::PendingPopup>
HoneycombBrowserInfoManager::PopPendingPopup(
    PendingPopup::Step previous_step,
    const content::GlobalRenderFrameHostId& opener_global_id,
    const GURL& target_url) {
  HONEYCOMB_REQUIRE_UIT();
  DCHECK(frame_util::IsValidGlobalId(opener_global_id));
  DCHECK_LE(previous_step, PendingPopup::GET_CUSTOM_WEB_CONTENTS_VIEW);

  PendingPopupList::iterator it = pending_popup_list_.begin();
  for (; it != pending_popup_list_.end(); ++it) {
    PendingPopup* popup = it->get();
    if (popup->step == previous_step &&
        popup->opener_global_id == opener_global_id &&
        popup->target_url == target_url) {
      // Transfer ownership of the pointer.
      it->release();
      pending_popup_list_.erase(it);
      return base::WrapUnique(popup);
    }
  }

  return nullptr;
}

std::unique_ptr<HoneycombBrowserInfoManager::PendingPopup>
HoneycombBrowserInfoManager::PopPendingPopup(PendingPopup::Step previous_step,
                                       content::WebContents* new_contents) {
  HONEYCOMB_REQUIRE_UIT();
  DCHECK_GE(previous_step, PendingPopup::WEB_CONTENTS_CREATED);

  PendingPopupList::iterator it = pending_popup_list_.begin();
  for (; it != pending_popup_list_.end(); ++it) {
    PendingPopup* popup = it->get();
    if (popup->step == previous_step && popup->new_contents == new_contents) {
      // Transfer ownership of the pointer.
      it->release();
      pending_popup_list_.erase(it);
      return base::WrapUnique(popup);
    }
  }

  return nullptr;
}

scoped_refptr<HoneycombBrowserInfo> HoneycombBrowserInfoManager::GetBrowserInfoInternal(
    const content::GlobalRenderFrameHostId& global_id,
    bool* is_guest_view) {
  browser_info_lock_.AssertAcquired();

  if (is_guest_view) {
    *is_guest_view = false;
  }

  if (!frame_util::IsValidGlobalId(global_id)) {
    return nullptr;
  }

  for (const auto& browser_info : browser_info_list_) {
    bool is_guest_view_tmp;
    auto frame =
        browser_info->GetFrameForGlobalId(global_id, &is_guest_view_tmp);
    if (frame || is_guest_view_tmp) {
      if (is_guest_view) {
        *is_guest_view = is_guest_view_tmp;
      }
      return browser_info;
    }
  }

  return nullptr;
}

// static
void HoneycombBrowserInfoManager::SendNewBrowserInfoResponse(
    scoped_refptr<HoneycombBrowserInfo> browser_info,
    bool is_guest_view,
    honey::mojom::BrowserManager::GetNewBrowserInfoCallback callback,
    scoped_refptr<base::SequencedTaskRunner> callback_runner) {
  if (!callback_runner->RunsTasksInCurrentSequence()) {
    callback_runner->PostTask(
        FROM_HERE,
        base::BindOnce(&HoneycombBrowserInfoManager::SendNewBrowserInfoResponse,
                       browser_info, is_guest_view, std::move(callback),
                       callback_runner));
    return;
  }

  auto params = honey::mojom::NewBrowserInfo::New();
  params->is_guest_view = is_guest_view;

  if (browser_info) {
    params->browser_id = browser_info->browser_id();
    params->is_windowless = browser_info->is_windowless();
    params->is_popup = browser_info->is_popup();

    auto extra_info = browser_info->extra_info();
    if (extra_info) {
      auto extra_info_impl =
          static_cast<HoneycombDictionaryValueImpl*>(extra_info.get());
      auto extra_info_value = extra_info_impl->CopyValue();
      params->extra_info = std::move(extra_info_value.GetDict());
    }
  } else {
    // The new browser info response has timed out.
    params->browser_id = -1;
  }

  std::move(callback).Run(std::move(params));
}

// static
void HoneycombBrowserInfoManager::CancelNewBrowserInfoResponse(
    PendingNewBrowserInfo* pending_info) {
  SendNewBrowserInfoResponse(/*browser_info=*/nullptr, /*is_guest_view=*/false,
                             std::move(pending_info->callback),
                             pending_info->callback_runner);
}

// static
void HoneycombBrowserInfoManager::TimeoutNewBrowserInfoResponse(
    const content::GlobalRenderFrameHostId& global_id,
    int timeout_id) {
  HONEYCOMB_REQUIRE_UIT();
  if (!g_info_manager) {
    return;
  }

  base::AutoLock lock_scope(g_info_manager->browser_info_lock_);

  // Continue the NewBrowserInfo request if it's still pending.
  auto it = g_info_manager->pending_new_browser_info_map_.find(global_id);
  if (it != g_info_manager->pending_new_browser_info_map_.end()) {
    const auto& pending_info = it->second;
    // Don't accidentally timeout a new request for the same frame.
    if (pending_info->timeout_id != timeout_id) {
      return;
    }

#if DCHECK_IS_ON()
    // This method should never be called for a PDF renderer.
    content::RenderProcessHost* process =
        content::RenderProcessHost::FromID(global_id.child_id);
    DCHECK(!process || !process->IsPdf());
#endif

    LOG(ERROR) << "Timeout of new browser info response for frame "
               << frame_util::GetFrameDebugString(global_id);

    CancelNewBrowserInfoResponse(pending_info.get());
    g_info_manager->pending_new_browser_info_map_.erase(it);
  }
}
