// Copyright (c) 2012 The Honeycomb Authors.
// Portions copyright (c) 2011 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_OSR_WEB_CONTENTS_VIEW_OSR_H_
#define HONEYCOMB_LIBHONEY_BROWSER_OSR_WEB_CONTENTS_VIEW_OSR_H_

#include "content/browser/renderer_host/render_view_host_delegate_view.h"
#include "content/browser/web_contents/web_contents_view.h"
#include "third_party/skia/include/core/SkColor.h"

namespace content {
class BrowserPluginGuest;
class WebContents;
class WebContentsViewDelegate;
}  // namespace content

class AlloyBrowserHostImpl;
class HoneycombRenderWidgetHostViewOSR;
class HoneycombTouchSelectionControllerClientOSR;

// An implementation of WebContentsView for off-screen rendering.
class HoneycombWebContentsViewOSR : public content::WebContentsView,
                              public content::RenderViewHostDelegateView {
 public:
  explicit HoneycombWebContentsViewOSR(SkColor background_color,
                                 bool use_shared_texture,
                                 bool use_external_begin_frame);

  HoneycombWebContentsViewOSR(const HoneycombWebContentsViewOSR&) = delete;
  HoneycombWebContentsViewOSR& operator=(const HoneycombWebContentsViewOSR&) = delete;

  ~HoneycombWebContentsViewOSR() override;

  void WebContentsCreated(content::WebContents* web_contents);
  content::WebContents* web_contents() const { return web_contents_; }

  void RenderViewCreated();

  // WebContentsView methods.
  gfx::NativeView GetNativeView() const override;
  gfx::NativeView GetContentNativeView() const override;
  gfx::NativeWindow GetTopLevelNativeWindow() const override;
  gfx::Rect GetContainerBounds() const override;
  void Focus() override;
  void SetInitialFocus() override;
  void StoreFocus() override;
  void RestoreFocus() override;
  void FocusThroughTabTraversal(bool reverse) override;
  content::DropData* GetDropData() const override;
  gfx::Rect GetViewBounds() const override;
  void CreateView(gfx::NativeView context) override;
  content::RenderWidgetHostViewBase* CreateViewForWidget(
      content::RenderWidgetHost* render_widget_host) override;
  content::RenderWidgetHostViewBase* CreateViewForChildWidget(
      content::RenderWidgetHost* render_widget_host) override;
  void SetPageTitle(const std::u16string& title) override;
  void RenderViewReady() override;
  void RenderViewHostChanged(content::RenderViewHost* old_host,
                             content::RenderViewHost* new_host) override;
  void SetOverscrollControllerEnabled(bool enabled) override;
  void OnCapturerCountChanged() override;
  void UpdateWindowControlsOverlay(const gfx::Rect& bounding_rect) override;

#if BUILDFLAG(IS_MAC)
  bool CloseTabAfterEventTrackingIfNeeded() override;
#endif

  // RenderViewHostDelegateView methods.
  void ShowContextMenu(content::RenderFrameHost& render_frame_host,
                       const content::ContextMenuParams& params) override;
  void StartDragging(const content::DropData& drop_data,
                     blink::DragOperationsMask allowed_ops,
                     const gfx::ImageSkia& image,
                     const gfx::Vector2d& cursor_offset,
                     const gfx::Rect& drag_obj_rect,
                     const blink::mojom::DragEventSourceInfo& event_info,
                     content::RenderWidgetHostImpl* source_rwh) override;
  void UpdateDragCursor(ui::mojom::DragOperation operation) override;
  virtual void GotFocus(
      content::RenderWidgetHostImpl* render_widget_host) override;
  virtual void LostFocus(
      content::RenderWidgetHostImpl* render_widget_host) override;
  virtual void TakeFocus(bool reverse) override;
  virtual void FullscreenStateChanged(bool is_fullscreen) override;

 private:
  HoneycombRenderWidgetHostViewOSR* GetView() const;
  AlloyBrowserHostImpl* GetBrowser() const;
  HoneycombTouchSelectionControllerClientOSR* GetSelectionControllerClient() const;

  const SkColor background_color_;
  const bool use_shared_texture_;
  const bool use_external_begin_frame_;

  content::WebContents* web_contents_;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_OSR_WEB_CONTENTS_VIEW_OSR_H_