// Copyright 2015 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoneycomb/browser/browser_platform_delegate.h"

#include "libhoneycomb/browser/alloy/alloy_browser_host_impl.h"
#include "libhoneycomb/browser/thread_util.h"

#include "base/logging.h"
#include "chrome/browser/platform_util.h"
#include "chrome/browser/shell_integration.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host.h"
#include "content/public/browser/render_widget_host_view.h"

namespace {

void ExecuteExternalProtocol(const GURL& url) {
  HONEYCOMB_REQUIRE_BLOCKING();

  // Check that an application is associated with the scheme.
  if (shell_integration::GetApplicationNameForScheme(url).empty()) {
    return;
  }

  HONEYCOMB_POST_TASK(TID_UI, base::BindOnce(&platform_util::OpenExternal, url));
}

}  // namespace

HoneycombBrowserPlatformDelegate::HoneycombBrowserPlatformDelegate() = default;

HoneycombBrowserPlatformDelegate::~HoneycombBrowserPlatformDelegate() {
  DCHECK(!browser_);
}

content::WebContents* HoneycombBrowserPlatformDelegate::CreateWebContents(
    HoneycombBrowserCreateParams& create_params,
    bool& own_web_contents) {
  DCHECK(false);
  return nullptr;
}

void HoneycombBrowserPlatformDelegate::CreateViewForWebContents(
    content::WebContentsView** view,
    content::RenderViewHostDelegateView** delegate_view) {
  DCHECK(false);
}

void HoneycombBrowserPlatformDelegate::WebContentsCreated(
    content::WebContents* web_contents,
    bool owned) {
  // We should not have a browser at this point.
  DCHECK(!browser_);

  DCHECK(!web_contents_);
  web_contents_ = web_contents;
}

void HoneycombBrowserPlatformDelegate::AddNewContents(
    content::WebContents* source,
    std::unique_ptr<content::WebContents> new_contents,
    const GURL& target_url,
    WindowOpenDisposition disposition,
    const blink::mojom::WindowFeatures& window_features,
    bool user_gesture,
    bool* was_blocked) {
  DCHECK(false);
}

void HoneycombBrowserPlatformDelegate::WebContentsDestroyed(
    content::WebContents* web_contents) {
  DCHECK(web_contents_ && web_contents_ == web_contents);
  web_contents_ = nullptr;
}

bool HoneycombBrowserPlatformDelegate::
    ShouldAllowRendererInitiatedCrossProcessNavigation(
        bool is_main_frame_navigation) {
  return true;
}

void HoneycombBrowserPlatformDelegate::RenderViewCreated(
    content::RenderViewHost* render_view_host) {
  // Indicate that the view has an external parent (namely us). This setting is
  // required for proper focus handling on Windows and Linux.
  if (HasExternalParent() && render_view_host->GetWidget()->GetView()) {
    render_view_host->GetWidget()->GetView()->SetHasExternalParent(true);
  }
}

void HoneycombBrowserPlatformDelegate::RenderViewReady() {}

void HoneycombBrowserPlatformDelegate::BrowserCreated(HoneycombBrowserHostBase* browser) {
  // We should have an associated WebContents at this point.
  DCHECK(web_contents_);

  DCHECK(!browser_);
  DCHECK(browser);
  browser_ = browser;
}

void HoneycombBrowserPlatformDelegate::CreateExtensionHost(
    const extensions::Extension* extension,
    const GURL& url,
    extensions::mojom::ViewType host_type) {
  DCHECK(false);
}

extensions::ExtensionHost* HoneycombBrowserPlatformDelegate::GetExtensionHost()
    const {
  DCHECK(false);
  return nullptr;
}

void HoneycombBrowserPlatformDelegate::NotifyBrowserCreated() {}

void HoneycombBrowserPlatformDelegate::NotifyBrowserDestroyed() {}

void HoneycombBrowserPlatformDelegate::BrowserDestroyed(HoneycombBrowserHostBase* browser) {
  // WebContentsDestroyed should already be called.
  DCHECK(!web_contents_);

  DCHECK(browser_ && browser_ == browser);
  browser_ = nullptr;
}

bool HoneycombBrowserPlatformDelegate::CreateHostWindow() {
  DCHECK(false);
  return true;
}

void HoneycombBrowserPlatformDelegate::CloseHostWindow() {
  DCHECK(false);
}

HoneycombWindowHandle HoneycombBrowserPlatformDelegate::GetHostWindowHandle() const {
  DCHECK(false);
  return kNullWindowHandle;
}

views::Widget* HoneycombBrowserPlatformDelegate::GetWindowWidget() const {
  DCHECK(false);
  return nullptr;
}

HoneycombRefPtr<HoneycombBrowserView> HoneycombBrowserPlatformDelegate::GetBrowserView() const {
  DCHECK(false);
  return nullptr;
}

web_modal::WebContentsModalDialogHost*
HoneycombBrowserPlatformDelegate::GetWebContentsModalDialogHost() const {
  DCHECK(false);
  return nullptr;
}

void HoneycombBrowserPlatformDelegate::PopupWebContentsCreated(
    const HoneycombBrowserSettings& settings,
    HoneycombRefPtr<HoneycombClient> client,
    content::WebContents* new_web_contents,
    HoneycombBrowserPlatformDelegate* new_platform_delegate,
    bool is_devtools) {}

void HoneycombBrowserPlatformDelegate::PopupBrowserCreated(
    HoneycombBrowserHostBase* new_browser,
    bool is_devtools) {}

SkColor HoneycombBrowserPlatformDelegate::GetBackgroundColor() const {
  DCHECK(false);
  return SkColor();
}

void HoneycombBrowserPlatformDelegate::WasResized() {
  DCHECK(false);
}

void HoneycombBrowserPlatformDelegate::SendKeyEvent(const HoneycombKeyEvent& event) {
  NOTIMPLEMENTED();
}

void HoneycombBrowserPlatformDelegate::SendMouseClickEvent(
    const HoneycombMouseEvent& event,
    HoneycombBrowserHost::MouseButtonType type,
    bool mouseUp,
    int clickCount) {
  NOTIMPLEMENTED();
}

void HoneycombBrowserPlatformDelegate::SendMouseMoveEvent(const HoneycombMouseEvent& event,
                                                    bool mouseLeave) {
  NOTIMPLEMENTED();
}

void HoneycombBrowserPlatformDelegate::SendMouseWheelEvent(const HoneycombMouseEvent& event,
                                                     int deltaX,
                                                     int deltaY) {
  NOTIMPLEMENTED();
}

void HoneycombBrowserPlatformDelegate::SendTouchEvent(const HoneycombTouchEvent& event) {
  NOTIMPLEMENTED();
}

void HoneycombBrowserPlatformDelegate::SetFocus(bool setFocus) {}

void HoneycombBrowserPlatformDelegate::SendCaptureLostEvent() {
  NOTIMPLEMENTED();
}

#if BUILDFLAG(IS_WIN) || (BUILDFLAG(IS_POSIX) && !BUILDFLAG(IS_MAC))
void HoneycombBrowserPlatformDelegate::NotifyMoveOrResizeStarted() {}

void HoneycombBrowserPlatformDelegate::SizeTo(int width, int height) {}
#endif

gfx::Point HoneycombBrowserPlatformDelegate::GetScreenPoint(
    const gfx::Point& view,
    bool want_dip_coords) const {
  DCHECK(false);
  return gfx::Point();
}

void HoneycombBrowserPlatformDelegate::ViewText(const std::string& text) {
  NOTIMPLEMENTED();
}

bool HoneycombBrowserPlatformDelegate::HandleKeyboardEvent(
    const content::NativeWebKeyboardEvent& event) {
  DCHECK(false);
  return false;
}

bool HoneycombBrowserPlatformDelegate::PreHandleGestureEvent(
    content::WebContents* source,
    const blink::WebGestureEvent& event) {
  return false;
}

bool HoneycombBrowserPlatformDelegate::IsNeverComposited(
    content::WebContents* web_contents) {
  return false;
}

// static
void HoneycombBrowserPlatformDelegate::HandleExternalProtocol(const GURL& url) {
  HONEYCOMB_POST_USER_VISIBLE_TASK(base::BindOnce(ExecuteExternalProtocol, url));
}

HoneycombEventHandle HoneycombBrowserPlatformDelegate::GetEventHandle(
    const content::NativeWebKeyboardEvent& event) const {
  DCHECK(false);
  return kNullEventHandle;
}

std::unique_ptr<HoneycombJavaScriptDialogRunner>
HoneycombBrowserPlatformDelegate::CreateJavaScriptDialogRunner() {
  return nullptr;
}

std::unique_ptr<HoneycombMenuRunner> HoneycombBrowserPlatformDelegate::CreateMenuRunner() {
  NOTIMPLEMENTED();
  return nullptr;
}

void HoneycombBrowserPlatformDelegate::UpdateFindBarBoundingBox(
    gfx::Rect* bounds) const {}

bool HoneycombBrowserPlatformDelegate::IsWindowless() const {
  return false;
}

bool HoneycombBrowserPlatformDelegate::IsViewsHosted() const {
  return false;
}

bool HoneycombBrowserPlatformDelegate::HasExternalParent() const {
  // In the majority of cases a Views-hosted browser will not have an external
  // parent, and visa-versa.
  return !IsViewsHosted();
}

void HoneycombBrowserPlatformDelegate::WasHidden(bool hidden) {
  DCHECK(false);
}

bool HoneycombBrowserPlatformDelegate::IsHidden() const {
  DCHECK(false);
  return false;
}

void HoneycombBrowserPlatformDelegate::NotifyScreenInfoChanged() {
  DCHECK(false);
}

void HoneycombBrowserPlatformDelegate::Invalidate(honey_paint_element_type_t type) {
  DCHECK(false);
}

void HoneycombBrowserPlatformDelegate::SendExternalBeginFrame() {
  DCHECK(false);
}

void HoneycombBrowserPlatformDelegate::SetWindowlessFrameRate(int frame_rate) {
  DCHECK(false);
}

void HoneycombBrowserPlatformDelegate::ImeSetComposition(
    const HoneycombString& text,
    const std::vector<HoneycombCompositionUnderline>& underlines,
    const HoneycombRange& replacement_range,
    const HoneycombRange& selection_range) {
  DCHECK(false);
}

void HoneycombBrowserPlatformDelegate::ImeCommitText(
    const HoneycombString& text,
    const HoneycombRange& replacement_range,
    int relative_cursor_pos) {
  DCHECK(false);
}

void HoneycombBrowserPlatformDelegate::ImeFinishComposingText(bool keep_selection) {
  DCHECK(false);
}

void HoneycombBrowserPlatformDelegate::ImeCancelComposition() {
  DCHECK(false);
}

void HoneycombBrowserPlatformDelegate::DragTargetDragEnter(
    HoneycombRefPtr<HoneycombDragData> drag_data,
    const HoneycombMouseEvent& event,
    honey_drag_operations_mask_t allowed_ops) {
  DCHECK(false);
}

void HoneycombBrowserPlatformDelegate::DragTargetDragOver(
    const HoneycombMouseEvent& event,
    honey_drag_operations_mask_t allowed_ops) {
  DCHECK(false);
}

void HoneycombBrowserPlatformDelegate::DragTargetDragLeave() {
  DCHECK(false);
}

void HoneycombBrowserPlatformDelegate::DragTargetDrop(const HoneycombMouseEvent& event) {
  DCHECK(false);
}

void HoneycombBrowserPlatformDelegate::StartDragging(
    const content::DropData& drop_data,
    blink::DragOperationsMask allowed_ops,
    const gfx::ImageSkia& image,
    const gfx::Vector2d& image_offset,
    const blink::mojom::DragEventSourceInfo& event_info,
    content::RenderWidgetHostImpl* source_rwh) {
  DCHECK(false);
}

void HoneycombBrowserPlatformDelegate::UpdateDragCursor(
    ui::mojom::DragOperation operation) {
  DCHECK(false);
}

void HoneycombBrowserPlatformDelegate::DragSourceEndedAt(
    int x,
    int y,
    honey_drag_operations_mask_t op) {
  DCHECK(false);
}

void HoneycombBrowserPlatformDelegate::DragSourceSystemDragEnded() {
  DCHECK(false);
}

void HoneycombBrowserPlatformDelegate::AccessibilityEventReceived(
    const content::AXEventNotificationDetails& eventData) {
  DCHECK(false);
}

void HoneycombBrowserPlatformDelegate::AccessibilityLocationChangesReceived(
    const std::vector<content::AXLocationChangeNotificationDetails>& locData) {
  DCHECK(false);
}

gfx::Point HoneycombBrowserPlatformDelegate::GetDialogPosition(
    const gfx::Size& size) {
  const gfx::Size& max_size = GetMaximumDialogSize();
  return gfx::Point((max_size.width() - size.width()) / 2,
                    (max_size.height() - size.height()) / 2);
}

gfx::Size HoneycombBrowserPlatformDelegate::GetMaximumDialogSize() {
  if (!web_contents_) {
    return gfx::Size();
  }

  // The dialog should try to fit within the overlay for the web contents.
  // Note that, for things like print preview, this is just a suggested maximum.
  return web_contents_->GetContainerBounds().size();
}

void HoneycombBrowserPlatformDelegate::SetAutoResizeEnabled(bool enabled,
                                                      const HoneycombSize& min_size,
                                                      const HoneycombSize& max_size) {
  NOTIMPLEMENTED();
}

void HoneycombBrowserPlatformDelegate::SetAccessibilityState(
    honey_state_t accessibility_state) {
  NOTIMPLEMENTED();
}

bool HoneycombBrowserPlatformDelegate::IsPrintPreviewSupported() const {
  return true;
}

void HoneycombBrowserPlatformDelegate::Find(const HoneycombString& searchText,
                                      bool forward,
                                      bool matchCase,
                                      bool findNext) {
  NOTIMPLEMENTED();
}

void HoneycombBrowserPlatformDelegate::StopFinding(bool clearSelection) {
  NOTIMPLEMENTED();
}

// static
int HoneycombBrowserPlatformDelegate::TranslateWebEventModifiers(
    uint32_t honey_modifiers) {
  int result = 0;
  // Set modifiers based on key state.
  if (honey_modifiers & EVENTFLAG_CAPS_LOCK_ON) {
    result |= blink::WebInputEvent::kCapsLockOn;
  }
  if (honey_modifiers & EVENTFLAG_SHIFT_DOWN) {
    result |= blink::WebInputEvent::kShiftKey;
  }
  if (honey_modifiers & EVENTFLAG_CONTROL_DOWN) {
    result |= blink::WebInputEvent::kControlKey;
  }
  if (honey_modifiers & EVENTFLAG_ALT_DOWN) {
    result |= blink::WebInputEvent::kAltKey;
  }
  if (honey_modifiers & EVENTFLAG_LEFT_MOUSE_BUTTON) {
    result |= blink::WebInputEvent::kLeftButtonDown;
  }
  if (honey_modifiers & EVENTFLAG_MIDDLE_MOUSE_BUTTON) {
    result |= blink::WebInputEvent::kMiddleButtonDown;
  }
  if (honey_modifiers & EVENTFLAG_RIGHT_MOUSE_BUTTON) {
    result |= blink::WebInputEvent::kRightButtonDown;
  }
  if (honey_modifiers & EVENTFLAG_COMMAND_DOWN) {
    result |= blink::WebInputEvent::kMetaKey;
  }
  if (honey_modifiers & EVENTFLAG_NUM_LOCK_ON) {
    result |= blink::WebInputEvent::kNumLockOn;
  }
  if (honey_modifiers & EVENTFLAG_IS_KEY_PAD) {
    result |= blink::WebInputEvent::kIsKeyPad;
  }
  if (honey_modifiers & EVENTFLAG_IS_LEFT) {
    result |= blink::WebInputEvent::kIsLeft;
  }
  if (honey_modifiers & EVENTFLAG_IS_RIGHT) {
    result |= blink::WebInputEvent::kIsRight;
  }
  if (honey_modifiers & EVENTFLAG_ALTGR_DOWN) {
    result |= blink::WebInputEvent::kAltGrKey;
  }
  if (honey_modifiers & EVENTFLAG_IS_REPEAT) {
    result |= blink::WebInputEvent::kIsAutoRepeat;
  }
  return result;
}
