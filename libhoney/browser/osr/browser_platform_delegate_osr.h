// Copyright 2015 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_OSR_BROWSER_PLATFORM_DELEGATE_OSR_H_
#define HONEYCOMB_LIBHONEY_BROWSER_OSR_BROWSER_PLATFORM_DELEGATE_OSR_H_

#include "libhoney/browser/alloy/browser_platform_delegate_alloy.h"
#include "libhoney/browser/native/browser_platform_delegate_native.h"

class HoneycombRenderWidgetHostViewOSR;
class HoneycombWebContentsViewOSR;

namespace content {
class RenderWidgetHostImpl;
}  // namespace content

// Base implementation of windowless browser functionality.
class HoneycombBrowserPlatformDelegateOsr
    : public HoneycombBrowserPlatformDelegateAlloy,
      public HoneycombBrowserPlatformDelegateNative::WindowlessHandler {
 public:
  // HoneycombBrowserPlatformDelegate methods:
  void CreateViewForWebContents(
      content::WebContentsView** view,
      content::RenderViewHostDelegateView** delegate_view) override;
  void WebContentsCreated(content::WebContents* web_contents,
                          bool owned) override;
  void RenderViewCreated(content::RenderViewHost* render_view_host) override;
  void BrowserCreated(HoneycombBrowserHostBase* browser) override;
  void NotifyBrowserDestroyed() override;
  void BrowserDestroyed(HoneycombBrowserHostBase* browser) override;
  SkColor GetBackgroundColor() const override;
  void WasResized() override;
  void SendKeyEvent(const HoneycombKeyEvent& event) override;
  void SendMouseClickEvent(const HoneycombMouseEvent& event,
                           HoneycombBrowserHost::MouseButtonType type,
                           bool mouseUp,
                           int clickCount) override;
  void SendMouseMoveEvent(const HoneycombMouseEvent& event, bool mouseLeave) override;
  void SendMouseWheelEvent(const HoneycombMouseEvent& event,
                           int deltaX,
                           int deltaY) override;
  void SendTouchEvent(const HoneycombTouchEvent& event) override;
  void SetFocus(bool setFocus) override;
  gfx::Point GetScreenPoint(const gfx::Point& view,
                            bool want_dip_coords) const override;
  void ViewText(const std::string& text) override;
  bool HandleKeyboardEvent(
      const content::NativeWebKeyboardEvent& event) override;
  HoneycombEventHandle GetEventHandle(
      const content::NativeWebKeyboardEvent& event) const override;
  std::unique_ptr<HoneycombJavaScriptDialogRunner> CreateJavaScriptDialogRunner()
      override;
  std::unique_ptr<HoneycombMenuRunner> CreateMenuRunner() override;
  bool IsWindowless() const override;
  void WasHidden(bool hidden) override;
  bool IsHidden() const override;
  void NotifyScreenInfoChanged() override;
  void Invalidate(honey_paint_element_type_t type) override;
  void SendExternalBeginFrame() override;
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
                           honey_drag_operations_mask_t allowed_ops) override;
  void DragTargetDragOver(const HoneycombMouseEvent& event,
                          honey_drag_operations_mask_t allowed_ops) override;
  void DragTargetDragLeave() override;
  void DragTargetDrop(const HoneycombMouseEvent& event) override;
  void StartDragging(const content::DropData& drop_data,
                     blink::DragOperationsMask allowed_ops,
                     const gfx::ImageSkia& image,
                     const gfx::Vector2d& image_offset,
                     const blink::mojom::DragEventSourceInfo& event_info,
                     content::RenderWidgetHostImpl* source_rwh) override;
  void UpdateDragCursor(ui::mojom::DragOperation operation) override;
  void DragSourceEndedAt(int x, int y, honey_drag_operations_mask_t op) override;
  void DragSourceSystemDragEnded() override;
  void AccessibilityEventReceived(
      const content::AXEventNotificationDetails& eventData) override;
  void AccessibilityLocationChangesReceived(
      const std::vector<content::AXLocationChangeNotificationDetails>& locData)
      override;

  // HoneycombBrowserPlatformDelegateNative::WindowlessHandler methods:
  HoneycombWindowHandle GetParentWindowHandle() const override;
  gfx::Point GetParentScreenPoint(const gfx::Point& view,
                                  bool want_dip_coords) const override;

 protected:
  // Platform-specific behaviors will be delegated to |native_delegate|.
  HoneycombBrowserPlatformDelegateOsr(
      std::unique_ptr<HoneycombBrowserPlatformDelegateNative> native_delegate,
      bool use_shared_texture,
      bool use_external_begin_frame);

  // Returns the primary OSR host view for the underlying browser. If a
  // full-screen host view currently exists then it will be returned. Otherwise,
  // the main host view will be returned.
  HoneycombRenderWidgetHostViewOSR* GetOSRHostView() const;

  std::unique_ptr<HoneycombBrowserPlatformDelegateNative> native_delegate_;
  const bool use_shared_texture_;
  const bool use_external_begin_frame_;

  HoneycombWebContentsViewOSR* view_osr_ = nullptr;  // Not owned by this class.

  // Pending drag/drop data.
  HoneycombRefPtr<HoneycombDragData> drag_data_;
  honey_drag_operations_mask_t drag_allowed_ops_;

  // We keep track of the RenderWidgetHost we're dragging over. If it changes
  // during a drag, we need to re-send the DragEnter message.
  base::WeakPtr<content::RenderWidgetHostImpl> current_rwh_for_drag_;

  // We also keep track of the RenderViewHost we're dragging over to avoid
  // sending the drag exited message after leaving the current
  // view. |current_rvh_for_drag_| should not be dereferenced.
  void* current_rvh_for_drag_;

  // We keep track of the RenderWidgetHost from which the current drag started,
  // in order to properly route the drag end message to it.
  base::WeakPtr<content::RenderWidgetHostImpl> drag_start_rwh_;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_OSR_BROWSER_PLATFORM_DELEGATE_OSR_H_
