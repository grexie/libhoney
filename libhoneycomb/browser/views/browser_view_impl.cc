// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoneycomb/browser/views/browser_view_impl.h"

#include "libhoneycomb/browser/browser_host_base.h"
#include "libhoneycomb/browser/browser_util.h"
#include "libhoneycomb/browser/chrome/views/chrome_browser_view.h"
#include "libhoneycomb/browser/context.h"
#include "libhoneycomb/browser/request_context_impl.h"
#include "libhoneycomb/browser/thread_util.h"
#include "libhoneycomb/browser/views/window_impl.h"

#include "content/public/common/input/native_web_keyboard_event.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
#include "ui/content_accelerators/accelerator_util.h"

namespace {

absl::optional<honey_gesture_command_t> GetGestureCommand(
    ui::GestureEvent* event) {
#if BUILDFLAG(IS_MAC)
  if (event->details().type() == ui::ET_GESTURE_SWIPE) {
    if (event->details().swipe_left()) {
      return HONEYCOMB_GESTURE_COMMAND_BACK;
    } else if (event->details().swipe_right()) {
      return HONEYCOMB_GESTURE_COMMAND_FORWARD;
    }
  }
#endif
  return absl::nullopt;
}

}  // namespace

// static
HoneycombRefPtr<HoneycombBrowserView> HoneycombBrowserView::CreateBrowserView(
    HoneycombRefPtr<HoneycombClient> client,
    const HoneycombString& url,
    const HoneycombBrowserSettings& settings,
    HoneycombRefPtr<HoneycombDictionaryValue> extra_info,
    HoneycombRefPtr<HoneycombRequestContext> request_context,
    HoneycombRefPtr<HoneycombBrowserViewDelegate> delegate) {
  return HoneycombBrowserViewImpl::Create(HoneycombWindowInfo(), client, url, settings,
                                    extra_info, request_context, delegate);
}

// static
HoneycombRefPtr<HoneycombBrowserView> HoneycombBrowserView::GetForBrowser(
    HoneycombRefPtr<HoneycombBrowser> browser) {
  HONEYCOMB_REQUIRE_UIT_RETURN(nullptr);

  HoneycombBrowserHostBase* browser_impl =
      static_cast<HoneycombBrowserHostBase*>(browser.get());
  if (browser_impl && browser_impl->is_views_hosted()) {
    return browser_impl->GetBrowserView();
  }
  return nullptr;
}

// static
HoneycombRefPtr<HoneycombBrowserViewImpl> HoneycombBrowserViewImpl::Create(
    const HoneycombWindowInfo& window_info,
    HoneycombRefPtr<HoneycombClient> client,
    const HoneycombString& url,
    const HoneycombBrowserSettings& settings,
    HoneycombRefPtr<HoneycombDictionaryValue> extra_info,
    HoneycombRefPtr<HoneycombRequestContext> request_context,
    HoneycombRefPtr<HoneycombBrowserViewDelegate> delegate) {
  HONEYCOMB_REQUIRE_UIT_RETURN(nullptr);

  if (!request_context) {
    request_context = HoneycombRequestContext::GetGlobalContext();
  }

  // Verify that the browser context is valid. Do this here instead of risking
  // potential browser creation failure when this view is added to the window.
  auto request_context_impl =
      static_cast<HoneycombRequestContextImpl*>(request_context.get());
  if (!request_context_impl->VerifyBrowserContext()) {
    return nullptr;
  }

  HoneycombRefPtr<HoneycombBrowserViewImpl> browser_view = new HoneycombBrowserViewImpl(delegate);
  browser_view->SetPendingBrowserCreateParams(
      window_info, client, url, settings, extra_info, request_context);
  browser_view->Initialize();
  browser_view->SetDefaults(settings);
  return browser_view;
}

// static
HoneycombRefPtr<HoneycombBrowserViewImpl> HoneycombBrowserViewImpl::CreateForPopup(
    const HoneycombBrowserSettings& settings,
    HoneycombRefPtr<HoneycombBrowserViewDelegate> delegate) {
  HONEYCOMB_REQUIRE_UIT_RETURN(nullptr);

  HoneycombRefPtr<HoneycombBrowserViewImpl> browser_view = new HoneycombBrowserViewImpl(delegate);
  browser_view->Initialize();
  browser_view->SetDefaults(settings);
  return browser_view;
}

void HoneycombBrowserViewImpl::WebContentsCreated(
    content::WebContents* web_contents) {
  if (web_view()) {
    web_view()->SetWebContents(web_contents);
  }
}

void HoneycombBrowserViewImpl::BrowserCreated(
    HoneycombBrowserHostBase* browser,
    base::RepeatingClosure on_bounds_changed) {
  browser_ = browser;
  on_bounds_changed_ = on_bounds_changed;
}

void HoneycombBrowserViewImpl::BrowserDestroyed(HoneycombBrowserHostBase* browser) {
  DCHECK_EQ(browser, browser_);
  browser_ = nullptr;

  if (web_view()) {
    web_view()->SetWebContents(nullptr);
  }
}

bool HoneycombBrowserViewImpl::HandleKeyboardEvent(
    const content::NativeWebKeyboardEvent& event) {
  if (!root_view()) {
    return false;
  }

  views::FocusManager* focus_manager = root_view()->GetFocusManager();
  if (!focus_manager) {
    return false;
  }

  if (HandleAccelerator(event, focus_manager)) {
    return true;
  }

  // Give the HoneycombWindowDelegate a chance to handle the event.
  if (auto* window_impl = honey_window()) {
    HoneycombKeyEvent honey_event;
    if (browser_util::GetHoneycombKeyEvent(event, honey_event) &&
        window_impl->OnKeyEvent(honey_event)) {
      return true;
    }
  }

  // Proceed with default native handling.
  return unhandled_keyboard_event_handler_.HandleKeyboardEvent(event,
                                                               focus_manager);
}

HoneycombRefPtr<HoneycombBrowser> HoneycombBrowserViewImpl::GetBrowser() {
  HONEYCOMB_REQUIRE_VALID_RETURN(nullptr);
  return browser_;
}

HoneycombRefPtr<HoneycombView> HoneycombBrowserViewImpl::GetChromeToolbar() {
  HONEYCOMB_REQUIRE_VALID_RETURN(nullptr);
  if (honey::IsChromeRuntimeEnabled()) {
    return chrome_browser_view()->honey_toolbar();
  }

  return nullptr;
}

void HoneycombBrowserViewImpl::SetPreferAccelerators(bool prefer_accelerators) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (web_view()) {
    web_view()->set_allow_accelerators(prefer_accelerators);
  }
}

void HoneycombBrowserViewImpl::RequestFocus() {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  // Always execute asynchronously to work around issue #3040.
  HONEYCOMB_POST_TASK(HONEYCOMB_UIT,
                base::BindOnce(&HoneycombBrowserViewImpl::RequestFocusInternal,
                               weak_ptr_factory_.GetWeakPtr()));
}

void HoneycombBrowserViewImpl::SetBackgroundColor(honey_color_t color) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  ParentClass::SetBackgroundColor(color);
  if (web_view()) {
    web_view()->SetResizeBackgroundColor(color);
  }
}

void HoneycombBrowserViewImpl::Detach() {
  ParentClass::Detach();

  // root_view() will be nullptr now.
  DCHECK(!root_view());

  if (browser_) {
    // With the Alloy runtime |browser_| will disappear when WindowDestroyed()
    // indirectly calls BrowserDestroyed() so keep a reference.
    HoneycombRefPtr<HoneycombBrowserHostBase> browser = browser_;

    // Force the browser to be destroyed.
    browser->WindowDestroyed();
  }
}

void HoneycombBrowserViewImpl::GetDebugInfo(base::Value::Dict* info,
                                      bool include_children) {
  ParentClass::GetDebugInfo(info, include_children);
  if (browser_) {
    info->Set("url", browser_->GetMainFrame()->GetURL().ToString());
  }
}

void HoneycombBrowserViewImpl::OnBrowserViewAdded() {
  if (!browser_ && pending_browser_create_params_) {
    // Top-level browsers will be created when this view is added to the views
    // hierarchy.
    pending_browser_create_params_->browser_view = this;

    HoneycombBrowserHostBase::Create(*pending_browser_create_params_);
    DCHECK(browser_);

    pending_browser_create_params_.reset(nullptr);
  }
}

void HoneycombBrowserViewImpl::OnBoundsChanged() {
  if (!on_bounds_changed_.is_null()) {
    on_bounds_changed_.Run();
  }
}

bool HoneycombBrowserViewImpl::OnGestureEvent(ui::GestureEvent* event) {
  if (auto command = GetGestureCommand(event)) {
    if (delegate() && delegate()->OnGestureCommand(this, *command)) {
      return true;
    }

    if (!honey::IsChromeRuntimeEnabled() && browser_) {
      // Default handling for the Alloy runtime.
      switch (*command) {
        case HONEYCOMB_GESTURE_COMMAND_BACK:
          browser_->GoBack();
          break;
        case HONEYCOMB_GESTURE_COMMAND_FORWARD:
          browser_->GoForward();
          break;
      }
      return true;
    }
  }

  return false;
}

HoneycombBrowserViewImpl::HoneycombBrowserViewImpl(
    HoneycombRefPtr<HoneycombBrowserViewDelegate> delegate)
    : ParentClass(delegate), weak_ptr_factory_(this) {}

void HoneycombBrowserViewImpl::SetPendingBrowserCreateParams(
    const HoneycombWindowInfo& window_info,
    HoneycombRefPtr<HoneycombClient> client,
    const HoneycombString& url,
    const HoneycombBrowserSettings& settings,
    HoneycombRefPtr<HoneycombDictionaryValue> extra_info,
    HoneycombRefPtr<HoneycombRequestContext> request_context) {
  DCHECK(!pending_browser_create_params_);
  pending_browser_create_params_.reset(new HoneycombBrowserCreateParams());
  pending_browser_create_params_->MaybeSetWindowInfo(window_info);
  pending_browser_create_params_->client = client;
  pending_browser_create_params_->url = url;
  pending_browser_create_params_->settings = settings;
  pending_browser_create_params_->extra_info = extra_info;
  pending_browser_create_params_->request_context = request_context;
}

void HoneycombBrowserViewImpl::SetDefaults(const HoneycombBrowserSettings& settings) {
  SetBackgroundColor(
      HoneycombContext::Get()->GetBackgroundColor(&settings, STATE_DISABLED));
}

views::View* HoneycombBrowserViewImpl::CreateRootView() {
  if (honey::IsChromeRuntimeEnabled()) {
    return new ChromeBrowserView(this);
  }

  return new HoneycombBrowserViewView(delegate(), this);
}

void HoneycombBrowserViewImpl::InitializeRootView() {
  if (honey::IsChromeRuntimeEnabled()) {
    chrome_browser_view()->Initialize();
  } else {
    static_cast<HoneycombBrowserViewView*>(root_view())->Initialize();
  }
}

views::WebView* HoneycombBrowserViewImpl::web_view() const {
  if (!root_view()) {
    return nullptr;
  }

  if (honey::IsChromeRuntimeEnabled()) {
    return chrome_browser_view()->contents_web_view();
  }

  return static_cast<HoneycombBrowserViewView*>(root_view());
}

ChromeBrowserView* HoneycombBrowserViewImpl::chrome_browser_view() const {
  CHECK(honey::IsChromeRuntimeEnabled());
  return static_cast<ChromeBrowserView*>(root_view());
}

HoneycombWindowImpl* HoneycombBrowserViewImpl::honey_window() const {
  HoneycombRefPtr<HoneycombWindow> window =
      view_util::GetWindowFor(root_view()->GetWidget());
  return static_cast<HoneycombWindowImpl*>(window.get());
}

bool HoneycombBrowserViewImpl::HandleAccelerator(
    const content::NativeWebKeyboardEvent& event,
    views::FocusManager* focus_manager) {
  // Previous calls to TranslateMessage can generate Char events as well as
  // RawKeyDown events, even if the latter triggered an accelerator.  In these
  // cases, we discard the Char events.
  if (event.GetType() == blink::WebInputEvent::Type::kChar &&
      ignore_next_char_event_) {
    ignore_next_char_event_ = false;
    return true;
  }

  // It's necessary to reset this flag, because a RawKeyDown event may not
  // always generate a Char event.
  ignore_next_char_event_ = false;

  if (event.GetType() == blink::WebInputEvent::Type::kRawKeyDown) {
    ui::Accelerator accelerator =
        ui::GetAcceleratorFromNativeWebKeyboardEvent(event);

    // This is tricky: we want to set ignore_next_char_event_ if
    // ProcessAccelerator returns true. But ProcessAccelerator might delete
    // |this| if the accelerator is a "close tab" one. So we speculatively
    // set the flag and fix it if no event was handled.
    ignore_next_char_event_ = true;

    if (focus_manager->ProcessAccelerator(accelerator)) {
      return true;
    }

    // ProcessAccelerator didn't handle the accelerator, so we know both
    // that |this| is still valid, and that we didn't want to set the flag.
    ignore_next_char_event_ = false;
  }

  return false;
}

void HoneycombBrowserViewImpl::RequestFocusInternal() {
  ParentClass::RequestFocus();
}
