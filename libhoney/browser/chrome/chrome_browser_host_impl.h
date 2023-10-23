// Copyright 2020 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_CHROME_CHROME_BROWSER_HOST_IMPL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_CHROME_CHROME_BROWSER_HOST_IMPL_H_
#pragma once

#include <memory>

#include "libhoney/browser/browser_host_base.h"
#include "libhoney/browser/chrome/browser_delegate.h"

#include "base/memory/weak_ptr.h"

class Browser;
class ChromeBrowserDelegate;
class ChromeBrowserView;

// HoneycombBrowser implementation for the chrome runtime. Method calls are delegated
// to the chrome Browser object or the WebContents as appropriate. See the
// ChromeBrowserDelegate documentation for additional details. All methods are
// thread-safe unless otherwise indicated.
class ChromeBrowserHostImpl : public HoneycombBrowserHostBase {
 public:
  // Honeycomb-specific parameters passed via Browser::CreateParams::honey_params and
  // possibly shared by multiple Browser instances.
  class DelegateCreateParams : public honey::BrowserDelegate::CreateParams {
   public:
    DelegateCreateParams(const HoneycombBrowserCreateParams& create_params)
        : create_params_(create_params) {}

    HoneycombBrowserCreateParams create_params_;
  };

  // Create a new Browser with a single tab (WebContents) and associated
  // ChromeBrowserHostImpl instance.
  static HoneycombRefPtr<ChromeBrowserHostImpl> Create(
      const HoneycombBrowserCreateParams& params);

  // Returns the browser associated with the specified RenderViewHost.
  static HoneycombRefPtr<ChromeBrowserHostImpl> GetBrowserForHost(
      const content::RenderViewHost* host);
  // Returns the browser associated with the specified RenderFrameHost.
  static HoneycombRefPtr<ChromeBrowserHostImpl> GetBrowserForHost(
      const content::RenderFrameHost* host);
  // Returns the browser associated with the specified WebContents.
  static HoneycombRefPtr<ChromeBrowserHostImpl> GetBrowserForContents(
      const content::WebContents* contents);
  // Returns the browser associated with the specified global ID.
  static HoneycombRefPtr<ChromeBrowserHostImpl> GetBrowserForGlobalId(
      const content::GlobalRenderFrameHostId& global_id);
  // Returns the browser associated with the specified Browser.
  static HoneycombRefPtr<ChromeBrowserHostImpl> GetBrowserForBrowser(
      const Browser* browser);

  ~ChromeBrowserHostImpl() override;

  // HoneycombBrowserContentsDelegate::Observer methods:
  void OnWebContentsDestroyed(content::WebContents* web_contents) override;

  // HoneycombBrowserHostBase methods called from HoneycombFrameHostImpl:
  void OnSetFocus(honey_focus_source_t source) override;

  // HoneycombBrowserHost methods:
  void CloseBrowser(bool force_close) override;
  bool TryCloseBrowser() override;
  HoneycombWindowHandle GetWindowHandle() override;
  HoneycombWindowHandle GetOpenerWindowHandle() override;
  void Find(const HoneycombString& searchText,
            bool forward,
            bool matchCase,
            bool findNext) override;
  void StopFinding(bool clearSelection) override;
  void ShowDevTools(const HoneycombWindowInfo& windowInfo,
                    HoneycombRefPtr<HoneycombClient> client,
                    const HoneycombBrowserSettings& settings,
                    const HoneycombPoint& inspect_element_at) override;
  void CloseDevTools() override;
  bool HasDevTools() override;
  bool IsWindowRenderingDisabled() override;
  void WasResized() override;
  void WasHidden(bool hidden) override;
  void NotifyScreenInfoChanged() override;
  void Invalidate(PaintElementType type) override;
  void SendExternalBeginFrame() override;
  void SendTouchEvent(const HoneycombTouchEvent& event) override;
  void SendCaptureLostEvent() override;
  int GetWindowlessFrameRate() override;
  void SetWindowlessFrameRate(int frame_rate) override;
  void ImeSetComposition(const HoneycombString& text,
                         const std::vector<HoneycombCompositionUnderline>& underlines,
                         const HoneycombRange& replacement_range,
                         const HoneycombRange& selection_range) override;
  void ImeCommitText(const HoneycombString& text,
                     const HoneycombRange& replacement_range,
                     int relative_cursor_pos) override;
  void ImeFinishComposingText(bool keep_selection) override;
  void ImeCancelComposition() override;
  void DragTargetDragEnter(HoneycombRefPtr<HoneycombDragData> drag_data,
                           const HoneycombMouseEvent& event,
                           DragOperationsMask allowed_ops) override;
  void DragTargetDragOver(const HoneycombMouseEvent& event,
                          DragOperationsMask allowed_ops) override;
  void DragTargetDragLeave() override;
  void DragTargetDrop(const HoneycombMouseEvent& event) override;
  void DragSourceSystemDragEnded() override;
  void DragSourceEndedAt(int x, int y, DragOperationsMask op) override;
  void SetAudioMuted(bool mute) override;
  bool IsAudioMuted() override;
  void SetAccessibilityState(honey_state_t accessibility_state) override;
  void SetAutoResizeEnabled(bool enabled,
                            const HoneycombSize& min_size,
                            const HoneycombSize& max_size) override;
  HoneycombRefPtr<HoneycombExtension> GetExtension() override;
  bool IsBackgroundHost() override;

  Browser* browser() const { return browser_; }

  // Return the Honeycomb specialization of BrowserView.
  ChromeBrowserView* chrome_browser_view() const;

  base::WeakPtr<ChromeBrowserHostImpl> GetWeakPtr() {
    return weak_ptr_factory_.GetWeakPtr();
  }

 protected:
  bool Navigate(const content::OpenURLParams& params) override;

 private:
  friend class ChromeBrowserDelegate;

  ChromeBrowserHostImpl(
      const HoneycombBrowserSettings& settings,
      HoneycombRefPtr<HoneycombClient> client,
      std::unique_ptr<HoneycombBrowserPlatformDelegate> platform_delegate,
      scoped_refptr<HoneycombBrowserInfo> browser_info,
      HoneycombRefPtr<HoneycombRequestContextImpl> request_context);

  // Create a new Browser without initializing the WebContents.
  static Browser* CreateBrowser(const HoneycombBrowserCreateParams& params);

  // Called from ChromeBrowserDelegate::CreateBrowser when this object is first
  // created. Must be called on the UI thread.
  void Attach(content::WebContents* web_contents,
              HoneycombRefPtr<ChromeBrowserHostImpl> opener);

  // Called from ChromeBrowserDelegate::AddNewContents to take ownership of a
  // popup WebContents.
  void AddNewContents(std::unique_ptr<content::WebContents> contents);

  // Called when this object changes Browser ownership (e.g. initially created,
  // dragging between windows, etc). The old Browser, if any, will be cleared
  // before the new Browser is added. Must be called on the UI thread.
  void SetBrowser(Browser* browser);

  // HoneycombBrowserHostBase methods:
  void WindowDestroyed() override;
  bool WillBeDestroyed() const override;
  void DestroyBrowser() override;

  void DoCloseBrowser(bool force_close);

  // Returns the current tab index for the associated WebContents, or
  // TabStripModel::kNoTab if not found.
  int GetCurrentTabIndex() const;

  Browser* browser_ = nullptr;
  HoneycombWindowHandle host_window_handle_ = kNullWindowHandle;

  base::WeakPtrFactory<ChromeBrowserHostImpl> weak_ptr_factory_{this};
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_CHROME_CHROME_BROWSER_HOST_IMPL_H_
