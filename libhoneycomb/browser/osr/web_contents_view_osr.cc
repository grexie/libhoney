// Copyright (c) 2014 The Honeycomb Authors.
// Portions copyright (c) 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoneycomb/browser/osr/web_contents_view_osr.h"

#include "libhoneycomb/browser/alloy/alloy_browser_host_impl.h"
#include "libhoneycomb/browser/osr/render_widget_host_view_osr.h"
#include "libhoneycomb/browser/osr/touch_selection_controller_client_osr.h"
#include "libhoneycomb/common/drag_data_impl.h"

#include "content/browser/browser_plugin/browser_plugin_embedder.h"
#include "content/browser/browser_plugin/browser_plugin_guest.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/browser/render_widget_host.h"

HoneycombWebContentsViewOSR::HoneycombWebContentsViewOSR(SkColor background_color,
                                             bool use_shared_texture,
                                             bool use_external_begin_frame)
    : background_color_(background_color),
      use_shared_texture_(use_shared_texture),
      use_external_begin_frame_(use_external_begin_frame),
      web_contents_(nullptr) {}

HoneycombWebContentsViewOSR::~HoneycombWebContentsViewOSR() {}

void HoneycombWebContentsViewOSR::WebContentsCreated(
    content::WebContents* web_contents) {
  DCHECK(!web_contents_);
  web_contents_ = web_contents;

  RenderViewCreated();
}

void HoneycombWebContentsViewOSR::RenderViewCreated() {
  if (web_contents_) {
    auto host = web_contents_->GetRenderViewHost();
    HoneycombRenderWidgetHostViewOSR* view =
        static_cast<HoneycombRenderWidgetHostViewOSR*>(host->GetWidget()->GetView());
    if (view) {
      view->InstallTransparency();
    }
  }
}

gfx::NativeView HoneycombWebContentsViewOSR::GetNativeView() const {
  return gfx::NativeView();
}

gfx::NativeView HoneycombWebContentsViewOSR::GetContentNativeView() const {
  return gfx::NativeView();
}

gfx::NativeWindow HoneycombWebContentsViewOSR::GetTopLevelNativeWindow() const {
  return gfx::NativeWindow();
}

gfx::Rect HoneycombWebContentsViewOSR::GetContainerBounds() const {
  return GetViewBounds();
}

void HoneycombWebContentsViewOSR::Focus() {}

void HoneycombWebContentsViewOSR::SetInitialFocus() {}

void HoneycombWebContentsViewOSR::StoreFocus() {}

void HoneycombWebContentsViewOSR::RestoreFocus() {}

void HoneycombWebContentsViewOSR::FocusThroughTabTraversal(bool reverse) {}

void HoneycombWebContentsViewOSR::GotFocus(
    content::RenderWidgetHostImpl* render_widget_host) {
  if (web_contents_) {
    content::WebContentsImpl* web_contents_impl =
        static_cast<content::WebContentsImpl*>(web_contents_);
    if (web_contents_impl) {
      web_contents_impl->NotifyWebContentsFocused(render_widget_host);
    }
  }
}

void HoneycombWebContentsViewOSR::LostFocus(
    content::RenderWidgetHostImpl* render_widget_host) {
  if (web_contents_) {
    content::WebContentsImpl* web_contents_impl =
        static_cast<content::WebContentsImpl*>(web_contents_);
    if (web_contents_impl) {
      web_contents_impl->NotifyWebContentsLostFocus(render_widget_host);
    }
  }
}

void HoneycombWebContentsViewOSR::TakeFocus(bool reverse) {
  if (web_contents_->GetDelegate()) {
    web_contents_->GetDelegate()->TakeFocus(web_contents_, reverse);
  }
}

void HoneycombWebContentsViewOSR::FullscreenStateChanged(bool is_fullscreen) {}

content::DropData* HoneycombWebContentsViewOSR::GetDropData() const {
  return nullptr;
}

gfx::Rect HoneycombWebContentsViewOSR::GetViewBounds() const {
  HoneycombRenderWidgetHostViewOSR* view = GetView();
  return view ? view->GetViewBounds() : gfx::Rect();
}

void HoneycombWebContentsViewOSR::CreateView(gfx::NativeView context) {}

content::RenderWidgetHostViewBase* HoneycombWebContentsViewOSR::CreateViewForWidget(
    content::RenderWidgetHost* render_widget_host) {
  if (render_widget_host->GetView()) {
    return static_cast<content::RenderWidgetHostViewBase*>(
        render_widget_host->GetView());
  }

  return new HoneycombRenderWidgetHostViewOSR(background_color_, use_shared_texture_,
                                        use_external_begin_frame_,
                                        render_widget_host, nullptr);
}

// Called for popup and fullscreen widgets.
content::RenderWidgetHostViewBase*
HoneycombWebContentsViewOSR::CreateViewForChildWidget(
    content::RenderWidgetHost* render_widget_host) {
  HoneycombRenderWidgetHostViewOSR* view = GetView();
  CHECK(view);

  return new HoneycombRenderWidgetHostViewOSR(background_color_, use_shared_texture_,
                                        use_external_begin_frame_,
                                        render_widget_host, view);
}

void HoneycombWebContentsViewOSR::SetPageTitle(const std::u16string& title) {}

void HoneycombWebContentsViewOSR::RenderViewReady() {
  RenderViewCreated();
}

void HoneycombWebContentsViewOSR::RenderViewHostChanged(
    content::RenderViewHost* old_host,
    content::RenderViewHost* new_host) {}

void HoneycombWebContentsViewOSR::SetOverscrollControllerEnabled(bool enabled) {}

void HoneycombWebContentsViewOSR::OnCapturerCountChanged() {}

void HoneycombWebContentsViewOSR::UpdateWindowControlsOverlay(
    const gfx::Rect& bounding_rect) {}

#if BUILDFLAG(IS_MAC)
bool HoneycombWebContentsViewOSR::CloseTabAfterEventTrackingIfNeeded() {
  return false;
}
#endif  // BUILDFLAG(IS_MAC)

void HoneycombWebContentsViewOSR::ShowContextMenu(
    content::RenderFrameHost& render_frame_host,
    const content::ContextMenuParams& params) {
  auto selection_controller_client = GetSelectionControllerClient();
  if (selection_controller_client &&
      selection_controller_client->HandleContextMenu(params)) {
    // Context menu display, if any, will be handled via
    // AlloyWebContentsViewDelegate::ShowContextMenu.
    return;
  }

  if (auto browser = GetBrowser()) {
    browser->ShowContextMenu(params);
  }
}

void HoneycombWebContentsViewOSR::StartDragging(
    const content::DropData& drop_data,
    blink::DragOperationsMask allowed_ops,
    const gfx::ImageSkia& image,
    const gfx::Vector2d& cursor_offset,
    const gfx::Rect& drag_obj_rect,
    const blink::mojom::DragEventSourceInfo& event_info,
    content::RenderWidgetHostImpl* source_rwh) {
  HoneycombRefPtr<AlloyBrowserHostImpl> browser = GetBrowser();
  if (browser.get()) {
    browser->StartDragging(drop_data, allowed_ops, image, cursor_offset,
                           event_info, source_rwh);
  } else if (web_contents_) {
    static_cast<content::WebContentsImpl*>(web_contents_)
        ->SystemDragEnded(source_rwh);
  }
}

void HoneycombWebContentsViewOSR::UpdateDragCursor(
    ui::mojom::DragOperation operation) {
  HoneycombRefPtr<AlloyBrowserHostImpl> browser = GetBrowser();
  if (browser.get()) {
    browser->UpdateDragCursor(operation);
  }
}

HoneycombRenderWidgetHostViewOSR* HoneycombWebContentsViewOSR::GetView() const {
  if (web_contents_) {
    return static_cast<HoneycombRenderWidgetHostViewOSR*>(
        web_contents_->GetRenderViewHost()->GetWidget()->GetView());
  }
  return nullptr;
}

AlloyBrowserHostImpl* HoneycombWebContentsViewOSR::GetBrowser() const {
  HoneycombRenderWidgetHostViewOSR* view = GetView();
  if (view) {
    return view->browser_impl().get();
  }
  return nullptr;
}

HoneycombTouchSelectionControllerClientOSR*
HoneycombWebContentsViewOSR::GetSelectionControllerClient() const {
  HoneycombRenderWidgetHostViewOSR* view = GetView();
  return view ? view->selection_controller_client() : nullptr;
}
