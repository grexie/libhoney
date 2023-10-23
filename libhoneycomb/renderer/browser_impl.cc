// Copyright (c) 2012 The Honeycomb Authors.
// Portions copyright (c) 2011 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoneycomb/renderer/browser_impl.h"

#include <string>
#include <vector>

#include "libhoneycomb/common/app_manager.h"
#include "libhoneycomb/renderer/blink_glue.h"
#include "libhoneycomb/renderer/render_frame_util.h"
#include "libhoneycomb/renderer/render_manager.h"
#include "libhoneycomb/renderer/thread_util.h"

#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/renderer/render_frame.h"
#include "content/renderer/document_state.h"
#include "content/renderer/navigation_state.h"
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/platform/web_url_error.h"
#include "third_party/blink/public/platform/web_url_response.h"
#include "third_party/blink/public/web/web_document.h"
#include "third_party/blink/public/web/web_frame.h"
#include "third_party/blink/public/web/web_frame_content_dumper.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/public/web/web_security_policy.h"
#include "third_party/blink/public/web/web_view.h"

// HoneycombBrowserImpl static methods.
// -----------------------------------------------------------------------------

// static
HoneycombRefPtr<HoneycombBrowserImpl> HoneycombBrowserImpl::GetBrowserForView(
    blink::WebView* view) {
  return HoneycombRenderManager::Get()->GetBrowserForView(view);
}

// static
HoneycombRefPtr<HoneycombBrowserImpl> HoneycombBrowserImpl::GetBrowserForMainFrame(
    blink::WebFrame* frame) {
  return HoneycombRenderManager::Get()->GetBrowserForMainFrame(frame);
}

// HoneycombBrowser methods.
// -----------------------------------------------------------------------------

bool HoneycombBrowserImpl::IsValid() {
  HONEYCOMB_REQUIRE_RT_RETURN(false);
  return !!GetWebView();
}

HoneycombRefPtr<HoneycombBrowserHost> HoneycombBrowserImpl::GetHost() {
  DCHECK(false) << "GetHost cannot be called from the render process";
  return nullptr;
}

bool HoneycombBrowserImpl::CanGoBack() {
  HONEYCOMB_REQUIRE_RT_RETURN(false);

  return blink_glue::CanGoBack(GetWebView());
}

void HoneycombBrowserImpl::GoBack() {
  HONEYCOMB_REQUIRE_RT_RETURN_VOID();

  blink_glue::GoBack(GetWebView());
}

bool HoneycombBrowserImpl::CanGoForward() {
  HONEYCOMB_REQUIRE_RT_RETURN(false);

  return blink_glue::CanGoForward(GetWebView());
}

void HoneycombBrowserImpl::GoForward() {
  HONEYCOMB_REQUIRE_RT_RETURN_VOID();

  blink_glue::GoForward(GetWebView());
}

bool HoneycombBrowserImpl::IsLoading() {
  HONEYCOMB_REQUIRE_RT_RETURN(false);

  if (GetWebView()) {
    blink::WebFrame* main_frame = GetWebView()->MainFrame();
    if (main_frame) {
      return main_frame->ToWebLocalFrame()->IsLoading();
    }
  }
  return false;
}

void HoneycombBrowserImpl::Reload() {
  HONEYCOMB_REQUIRE_RT_RETURN_VOID();

  if (GetWebView()) {
    blink::WebFrame* main_frame = GetWebView()->MainFrame();
    if (main_frame && main_frame->IsWebLocalFrame()) {
      main_frame->ToWebLocalFrame()->StartReload(
          blink::WebFrameLoadType::kReload);
    }
  }
}

void HoneycombBrowserImpl::ReloadIgnoreCache() {
  HONEYCOMB_REQUIRE_RT_RETURN_VOID();

  if (GetWebView()) {
    blink::WebFrame* main_frame = GetWebView()->MainFrame();
    if (main_frame && main_frame->IsWebLocalFrame()) {
      main_frame->ToWebLocalFrame()->StartReload(
          blink::WebFrameLoadType::kReloadBypassingCache);
    }
  }
}

void HoneycombBrowserImpl::StopLoad() {
  HONEYCOMB_REQUIRE_RT_RETURN_VOID();

  if (GetWebView()) {
    blink::WebFrame* main_frame = GetWebView()->MainFrame();
    if (main_frame && main_frame->IsWebLocalFrame()) {
      main_frame->ToWebLocalFrame()->DeprecatedStopLoading();
    }
  }
}

int HoneycombBrowserImpl::GetIdentifier() {
  HONEYCOMB_REQUIRE_RT_RETURN(0);

  return browser_id();
}

bool HoneycombBrowserImpl::IsSame(HoneycombRefPtr<HoneycombBrowser> that) {
  HONEYCOMB_REQUIRE_RT_RETURN(false);

  HoneycombBrowserImpl* impl = static_cast<HoneycombBrowserImpl*>(that.get());
  return (impl == this);
}

bool HoneycombBrowserImpl::IsPopup() {
  HONEYCOMB_REQUIRE_RT_RETURN(false);

  return is_popup();
}

bool HoneycombBrowserImpl::HasDocument() {
  HONEYCOMB_REQUIRE_RT_RETURN(false);

  if (GetWebView()) {
    blink::WebFrame* main_frame = GetWebView()->MainFrame();
    if (main_frame && main_frame->IsWebLocalFrame()) {
      return !main_frame->ToWebLocalFrame()->GetDocument().IsNull();
    }
  }
  return false;
}

HoneycombRefPtr<HoneycombFrame> HoneycombBrowserImpl::GetMainFrame() {
  HONEYCOMB_REQUIRE_RT_RETURN(nullptr);

  if (GetWebView()) {
    blink::WebFrame* main_frame = GetWebView()->MainFrame();
    if (main_frame && main_frame->IsWebLocalFrame()) {
      return GetWebFrameImpl(main_frame->ToWebLocalFrame()).get();
    }
  }
  return nullptr;
}

HoneycombRefPtr<HoneycombFrame> HoneycombBrowserImpl::GetFocusedFrame() {
  HONEYCOMB_REQUIRE_RT_RETURN(nullptr);

  if (GetWebView() && GetWebView()->FocusedFrame()) {
    return GetWebFrameImpl(GetWebView()->FocusedFrame()).get();
  }
  return nullptr;
}

HoneycombRefPtr<HoneycombFrame> HoneycombBrowserImpl::GetFrame(int64_t identifier) {
  HONEYCOMB_REQUIRE_RT_RETURN(nullptr);

  return GetWebFrameImpl(identifier).get();
}

HoneycombRefPtr<HoneycombFrame> HoneycombBrowserImpl::GetFrame(const HoneycombString& name) {
  HONEYCOMB_REQUIRE_RT_RETURN(nullptr);

  blink::WebView* web_view = GetWebView();
  if (web_view) {
    const blink::WebString& frame_name =
        blink::WebString::FromUTF16(name.ToString16());
    // Search by assigned frame name (Frame::name).
    blink::WebFrame* frame = web_view->MainFrame();
    if (frame && frame->IsWebLocalFrame()) {
      frame = frame->ToWebLocalFrame()->FindFrameByName(frame_name);
    }
    if (!frame) {
      // Search by unique frame name (Frame::uniqueName).
      const std::string& searchname = name;
      for (blink::WebFrame* cur_frame = web_view->MainFrame(); cur_frame;
           cur_frame = cur_frame->TraverseNext()) {
        if (cur_frame->IsWebLocalFrame() &&
            render_frame_util::GetName(cur_frame->ToWebLocalFrame()) ==
                searchname) {
          frame = cur_frame;
          break;
        }
      }
    }
    if (frame && frame->IsWebLocalFrame()) {
      return GetWebFrameImpl(frame->ToWebLocalFrame()).get();
    }
  }

  return nullptr;
}

size_t HoneycombBrowserImpl::GetFrameCount() {
  HONEYCOMB_REQUIRE_RT_RETURN(0);

  int count = 0;

  if (GetWebView()) {
    for (blink::WebFrame* frame = GetWebView()->MainFrame(); frame;
         frame = frame->TraverseNext()) {
      count++;
    }
  }

  return count;
}

void HoneycombBrowserImpl::GetFrameIdentifiers(std::vector<int64_t>& identifiers) {
  HONEYCOMB_REQUIRE_RT_RETURN_VOID();

  if (identifiers.size() > 0) {
    identifiers.clear();
  }

  if (GetWebView()) {
    for (blink::WebFrame* frame = GetWebView()->MainFrame(); frame;
         frame = frame->TraverseNext()) {
      if (frame->IsWebLocalFrame()) {
        identifiers.push_back(
            render_frame_util::GetIdentifier(frame->ToWebLocalFrame()));
      }
    }
  }
}

void HoneycombBrowserImpl::GetFrameNames(std::vector<HoneycombString>& names) {
  HONEYCOMB_REQUIRE_RT_RETURN_VOID();

  if (names.size() > 0) {
    names.clear();
  }

  if (GetWebView()) {
    for (blink::WebFrame* frame = GetWebView()->MainFrame(); frame;
         frame = frame->TraverseNext()) {
      if (frame->IsWebLocalFrame()) {
        names.push_back(render_frame_util::GetName(frame->ToWebLocalFrame()));
      }
    }
  }
}

// HoneycombBrowserImpl public methods.
// -----------------------------------------------------------------------------

HoneycombBrowserImpl::HoneycombBrowserImpl(blink::WebView* web_view,
                               int browser_id,
                               bool is_popup,
                               bool is_windowless)
    : blink::WebViewObserver(web_view),
      browser_id_(browser_id),
      is_popup_(is_popup),
      is_windowless_(is_windowless) {}

HoneycombBrowserImpl::~HoneycombBrowserImpl() {}

HoneycombRefPtr<HoneycombFrameImpl> HoneycombBrowserImpl::GetWebFrameImpl(
    blink::WebLocalFrame* frame) {
  DCHECK(frame);
  int64_t frame_id = render_frame_util::GetIdentifier(frame);

  // Frames are re-used between page loads. Only add the frame to the map once.
  FrameMap::const_iterator it = frames_.find(frame_id);
  if (it != frames_.end()) {
    return it->second;
  }

  HoneycombRefPtr<HoneycombFrameImpl> framePtr(new HoneycombFrameImpl(this, frame, frame_id));
  frames_.insert(std::make_pair(frame_id, framePtr));

  return framePtr;
}

HoneycombRefPtr<HoneycombFrameImpl> HoneycombBrowserImpl::GetWebFrameImpl(int64_t frame_id) {
  if (frame_id == blink_glue::kInvalidFrameId) {
    if (GetWebView()) {
      blink::WebFrame* main_frame = GetWebView()->MainFrame();
      if (main_frame && main_frame->IsWebLocalFrame()) {
        return GetWebFrameImpl(main_frame->ToWebLocalFrame());
      }
    }
    return nullptr;
  }

  // Check if we already know about the frame.
  FrameMap::const_iterator it = frames_.find(frame_id);
  if (it != frames_.end()) {
    return it->second;
  }

  if (GetWebView()) {
    // Check if the frame exists but we don't know about it yet.
    for (blink::WebFrame* frame = GetWebView()->MainFrame(); frame;
         frame = frame->TraverseNext()) {
      if (frame->IsWebLocalFrame() &&
          render_frame_util::GetIdentifier(frame->ToWebLocalFrame()) ==
              frame_id) {
        return GetWebFrameImpl(frame->ToWebLocalFrame());
      }
    }
  }

  return nullptr;
}

void HoneycombBrowserImpl::AddFrameObject(int64_t frame_id,
                                    HoneycombTrackNode* tracked_object) {
  HoneycombRefPtr<HoneycombTrackManager> manager;

  if (!frame_objects_.empty()) {
    FrameObjectMap::const_iterator it = frame_objects_.find(frame_id);
    if (it != frame_objects_.end()) {
      manager = it->second;
    }
  }

  if (!manager.get()) {
    manager = new HoneycombTrackManager();
    frame_objects_.insert(std::make_pair(frame_id, manager));
  }

  manager->Add(tracked_object);
}

// RenderViewObserver methods.
// -----------------------------------------------------------------------------

void HoneycombBrowserImpl::OnDestruct() {
  // Notify that the browser window has been destroyed.
  HoneycombRefPtr<HoneycombApp> app = HoneycombAppManager::Get()->GetApplication();
  if (app.get()) {
    HoneycombRefPtr<HoneycombRenderProcessHandler> handler = app->GetRenderProcessHandler();
    if (handler.get()) {
      handler->OnBrowserDestroyed(this);
    }
  }

  HoneycombRenderManager::Get()->OnBrowserDestroyed(this);
}

void HoneycombBrowserImpl::FrameDetached(int64_t frame_id) {
  if (!frames_.empty()) {
    // Remove the frame from the map.
    FrameMap::iterator it = frames_.find(frame_id);
    if (it != frames_.end()) {
      frames_.erase(it);
    }
  }

  if (!frame_objects_.empty()) {
    // Remove any tracked objects associated with the frame.
    FrameObjectMap::iterator it = frame_objects_.find(frame_id);
    if (it != frame_objects_.end()) {
      frame_objects_.erase(it);
    }
  }
}

void HoneycombBrowserImpl::OnLoadingStateChange(bool isLoading) {
  HoneycombRefPtr<HoneycombApp> app = HoneycombAppManager::Get()->GetApplication();
  if (app.get()) {
    HoneycombRefPtr<HoneycombRenderProcessHandler> handler = app->GetRenderProcessHandler();
    if (handler.get()) {
      HoneycombRefPtr<HoneycombLoadHandler> load_handler = handler->GetLoadHandler();
      if (load_handler.get()) {
        blink::WebView* web_view = GetWebView();
        const bool canGoBack = blink_glue::CanGoBack(web_view);
        const bool canGoForward = blink_glue::CanGoForward(web_view);

        // Don't call OnLoadingStateChange multiple times with the same status.
        // This can occur in cases where there are multiple highest-level
        // LocalFrames in-process for the same browser.
        if (last_loading_state_ &&
            last_loading_state_->IsMatch(isLoading, canGoBack, canGoForward)) {
          return;
        }

        if (was_in_bfcache_) {
          // Send the expected callbacks when exiting the BFCache.
          DCHECK(!isLoading);
          load_handler->OnLoadingStateChange(this, /*isLoading=*/true,
                                             canGoBack, canGoForward);

          auto main_frame = GetMainFrame();
          load_handler->OnLoadStart(this, main_frame, TT_EXPLICIT);
          load_handler->OnLoadEnd(this, main_frame, 0);

          was_in_bfcache_ = false;
        }

        load_handler->OnLoadingStateChange(this, isLoading, canGoBack,
                                           canGoForward);
        last_loading_state_.reset(
            new LoadingState(isLoading, canGoBack, canGoForward));
      }
    }
  }
}

void HoneycombBrowserImpl::OnEnterBFCache() {
  // Reset loading state so that notifications will be resent if/when exiting
  // BFCache.
  was_in_bfcache_ = true;
  last_loading_state_.reset();
}
