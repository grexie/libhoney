// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_VIEWS_BROWSER_VIEW_IMPL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_VIEWS_BROWSER_VIEW_IMPL_H_
#pragma once

#include "include/honey_client.h"
#include "include/views/honey_browser_view.h"
#include "include/views/honey_browser_view_delegate.h"
#include "libhoney/browser/browser_host_base.h"
#include "libhoney/browser/views/browser_view_view.h"
#include "libhoney/browser/views/view_impl.h"

#include "base/functional/callback_forward.h"
#include "base/memory/weak_ptr.h"
#include "ui/views/controls/webview/unhandled_keyboard_event_handler.h"

class HoneycombBrowserHostBase;
class HoneycombWindowImpl;
class ChromeBrowserView;

class HoneycombBrowserViewImpl
    : public HoneycombViewImpl<views::View, HoneycombBrowserView, HoneycombBrowserViewDelegate>,
      public HoneycombBrowserViewView::Delegate {
 public:
  using ParentClass =
      HoneycombViewImpl<views::View, HoneycombBrowserView, HoneycombBrowserViewDelegate>;

  HoneycombBrowserViewImpl(const HoneycombBrowserViewImpl&) = delete;
  HoneycombBrowserViewImpl& operator=(const HoneycombBrowserViewImpl&) = delete;

  // Create a new HoneycombBrowserView instance. |delegate| may be nullptr.
  // |window_info| will only be used when creating a Chrome child window.
  static HoneycombRefPtr<HoneycombBrowserViewImpl> Create(
      const HoneycombWindowInfo& window_info,
      HoneycombRefPtr<HoneycombClient> client,
      const HoneycombString& url,
      const HoneycombBrowserSettings& settings,
      HoneycombRefPtr<HoneycombDictionaryValue> extra_info,
      HoneycombRefPtr<HoneycombRequestContext> request_context,
      HoneycombRefPtr<HoneycombBrowserViewDelegate> delegate);

  // Create a new HoneycombBrowserView instance for a popup. |delegate| may be
  // nullptr.
  static HoneycombRefPtr<HoneycombBrowserViewImpl> CreateForPopup(
      const HoneycombBrowserSettings& settings,
      HoneycombRefPtr<HoneycombBrowserViewDelegate> delegate);

  // Called from HoneycombBrowserPlatformDelegateViews.
  void WebContentsCreated(content::WebContents* web_contents);
  void BrowserCreated(HoneycombBrowserHostBase* browser,
                      base::RepeatingClosure on_bounds_changed);
  void BrowserDestroyed(HoneycombBrowserHostBase* browser);

  // Called to handle accelerators when the event is unhandled by the web
  // content and the browser client.
  bool HandleKeyboardEvent(const content::NativeWebKeyboardEvent& event);

  // HoneycombBrowserView methods:
  HoneycombRefPtr<HoneycombBrowser> GetBrowser() override;
  HoneycombRefPtr<HoneycombView> GetChromeToolbar() override;
  void SetPreferAccelerators(bool prefer_accelerators) override;

  // HoneycombView methods:
  HoneycombRefPtr<HoneycombBrowserView> AsBrowserView() override { return this; }
  void RequestFocus() override;
  void SetBackgroundColor(honey_color_t color) override;

  // HoneycombViewAdapter methods:
  void Detach() override;
  std::string GetDebugType() override { return "BrowserView"; }
  void GetDebugInfo(base::Value::Dict* info, bool include_children) override;

  // HoneycombBrowserViewView::Delegate methods:
  void OnBrowserViewAdded() override;
  void OnBoundsChanged() override;
  bool OnGestureEvent(ui::GestureEvent* event) override;

  // Return the WebView representation of this object.
  views::WebView* web_view() const;

  // Return the Honeycomb specialization of BrowserView.
  ChromeBrowserView* chrome_browser_view() const;

  // Return the HoneycombWindowImpl hosting this object.
  HoneycombWindowImpl* honey_window() const;

 private:
  // Create a new implementation object.
  // Always call Initialize() after creation.
  // |delegate| may be nullptr.
  explicit HoneycombBrowserViewImpl(HoneycombRefPtr<HoneycombBrowserViewDelegate> delegate);

  void SetPendingBrowserCreateParams(
      const HoneycombWindowInfo& window_info,
      HoneycombRefPtr<HoneycombClient> client,
      const HoneycombString& url,
      const HoneycombBrowserSettings& settings,
      HoneycombRefPtr<HoneycombDictionaryValue> extra_info,
      HoneycombRefPtr<HoneycombRequestContext> request_context);

  void SetDefaults(const HoneycombBrowserSettings& settings);

  // HoneycombViewImpl methods:
  views::View* CreateRootView() override;
  void InitializeRootView() override;

  // Logic extracted from UnhandledKeyboardEventHandler::HandleKeyboardEvent for
  // the handling of accelerators. Returns true if the event was handled by the
  // accelerator.
  bool HandleAccelerator(const content::NativeWebKeyboardEvent& event,
                         views::FocusManager* focus_manager);

  void RequestFocusInternal();

  std::unique_ptr<HoneycombBrowserCreateParams> pending_browser_create_params_;

  HoneycombRefPtr<HoneycombBrowserHostBase> browser_;

  views::UnhandledKeyboardEventHandler unhandled_keyboard_event_handler_;
  bool ignore_next_char_event_ = false;

  base::RepeatingClosure on_bounds_changed_;

  base::WeakPtrFactory<HoneycombBrowserViewImpl> weak_ptr_factory_;

  IMPLEMENT_REFCOUNTING_DELETE_ON_UIT(HoneycombBrowserViewImpl);
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_VIEWS_BROWSER_VIEW_IMPL_H_
