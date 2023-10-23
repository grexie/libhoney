// Copyright (c) 2012 The Honeycomb Authors.
// Portions copyright (c) 2011 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_RENDERER_BROWSER_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_RENDERER_BROWSER_IMPL_H_
#pragma once

#include <stdint.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "include/honey_browser.h"
#include "include/honey_client.h"
#include "libhoneycomb/common/tracker.h"
#include "libhoneycomb/renderer/frame_impl.h"

#include "third_party/blink/public/web/web_view_observer.h"

namespace blink {
class WebFrame;
class WebNode;
class WebView;
}  // namespace blink

// Renderer plumbing for Honeycomb features. There is a one-to-one relationship
// between RenderView on the renderer side and RenderViewHost on the browser
// side.
//
// RenderViewObserver: Interface for observing RenderView notifications.
class HoneycombBrowserImpl : public HoneycombBrowser, public blink::WebViewObserver {
 public:
  // Returns the browser associated with the specified RenderView.
  static HoneycombRefPtr<HoneycombBrowserImpl> GetBrowserForView(blink::WebView* view);
  // Returns the browser associated with the specified main WebFrame.
  static HoneycombRefPtr<HoneycombBrowserImpl> GetBrowserForMainFrame(
      blink::WebFrame* frame);

  // HoneycombBrowser methods.
  bool IsValid() override;
  HoneycombRefPtr<HoneycombBrowserHost> GetHost() override;
  bool CanGoBack() override;
  void GoBack() override;
  bool CanGoForward() override;
  void GoForward() override;
  bool IsLoading() override;
  void Reload() override;
  void ReloadIgnoreCache() override;
  void StopLoad() override;
  int GetIdentifier() override;
  bool IsSame(HoneycombRefPtr<HoneycombBrowser> that) override;
  bool IsPopup() override;
  bool HasDocument() override;
  HoneycombRefPtr<HoneycombFrame> GetMainFrame() override;
  HoneycombRefPtr<HoneycombFrame> GetFocusedFrame() override;
  HoneycombRefPtr<HoneycombFrame> GetFrame(int64_t identifier) override;
  HoneycombRefPtr<HoneycombFrame> GetFrame(const HoneycombString& name) override;
  size_t GetFrameCount() override;
  void GetFrameIdentifiers(std::vector<int64_t>& identifiers) override;
  void GetFrameNames(std::vector<HoneycombString>& names) override;

  HoneycombBrowserImpl(blink::WebView* web_view,
                 int browser_id,
                 bool is_popup,
                 bool is_windowless);

  HoneycombBrowserImpl(const HoneycombBrowserImpl&) = delete;
  HoneycombBrowserImpl& operator=(const HoneycombBrowserImpl&) = delete;

  ~HoneycombBrowserImpl() override;

  // Returns the matching HoneycombFrameImpl reference or creates a new one.
  HoneycombRefPtr<HoneycombFrameImpl> GetWebFrameImpl(blink::WebLocalFrame* frame);
  HoneycombRefPtr<HoneycombFrameImpl> GetWebFrameImpl(int64_t frame_id);

  // Frame objects will be deleted immediately before the frame is closed.
  void AddFrameObject(int64_t frame_id, HoneycombTrackNode* tracked_object);

  int browser_id() const { return browser_id_; }
  bool is_popup() const { return is_popup_; }
  bool is_windowless() const { return is_windowless_; }

  // blink::WebViewObserver methods.
  void OnDestruct() override;
  void FrameDetached(int64_t frame_id);

  void OnLoadingStateChange(bool isLoading);
  void OnEnterBFCache();

 private:
  // ID of the browser that this RenderView is associated with. During loading
  // of cross-origin requests multiple RenderViews may be associated with the
  // same browser ID.
  int browser_id_;
  bool is_popup_;
  bool is_windowless_;

  // Map of unique frame ids to HoneycombFrameImpl references.
  using FrameMap = std::map<int64_t, HoneycombRefPtr<HoneycombFrameImpl>>;
  FrameMap frames_;

  // True if the browser was in the BFCache.
  bool was_in_bfcache_ = false;

  // Map of unique frame ids to HoneycombTrackManager objects that need to be cleaned
  // up when the frame is deleted.
  using FrameObjectMap = std::map<int64_t, HoneycombRefPtr<HoneycombTrackManager>>;
  FrameObjectMap frame_objects_;

  struct LoadingState {
    LoadingState(bool is_loading, bool can_go_back, bool can_go_forward)
        : is_loading_(is_loading),
          can_go_back_(can_go_back),
          can_go_forward_(can_go_forward) {}

    bool IsMatch(bool is_loading, bool can_go_back, bool can_go_forward) const {
      return is_loading_ == is_loading && can_go_back_ == can_go_back &&
             can_go_forward_ == can_go_forward;
    }

    bool is_loading_;
    bool can_go_back_;
    bool can_go_forward_;
  };
  std::unique_ptr<LoadingState> last_loading_state_;

  IMPLEMENT_REFCOUNTING(HoneycombBrowserImpl);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_RENDERER_BROWSER_IMPL_H_
