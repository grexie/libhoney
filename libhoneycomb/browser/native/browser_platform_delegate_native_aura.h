// Copyright 2020 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_NATIVE_BROWSER_PLATFORM_DELEGATE_NATIVE_AURA_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_NATIVE_BROWSER_PLATFORM_DELEGATE_NATIVE_AURA_H_

#include "libhoneycomb/browser/native/browser_platform_delegate_native.h"

#include "base/memory/weak_ptr.h"
#include "ui/events/event.h"

namespace content {
class RenderWidgetHostViewAura;
}

namespace gfx {
class Vector2d;
}

// Windowed browser implementation for Aura platforms.
class HoneycombBrowserPlatformDelegateNativeAura
    : public HoneycombBrowserPlatformDelegateNative {
 public:
  HoneycombBrowserPlatformDelegateNativeAura(const HoneycombWindowInfo& window_info,
                                       SkColor background_color);

  // HoneycombBrowserPlatformDelegate methods:
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
  std::unique_ptr<HoneycombMenuRunner> CreateMenuRunner() override;
  gfx::Point GetScreenPoint(const gfx::Point& view,
                            bool want_dip_coords) const override;

  // HoneycombBrowserPlatformDelegateNative methods:
  content::NativeWebKeyboardEvent TranslateWebKeyEvent(
      const HoneycombKeyEvent& key_event) const override;
  blink::WebMouseEvent TranslateWebClickEvent(
      const HoneycombMouseEvent& mouse_event,
      HoneycombBrowserHost::MouseButtonType type,
      bool mouseUp,
      int clickCount) const override;
  blink::WebMouseEvent TranslateWebMoveEvent(const HoneycombMouseEvent& mouse_event,
                                             bool mouseLeave) const override;
  blink::WebMouseWheelEvent TranslateWebWheelEvent(
      const HoneycombMouseEvent& mouse_event,
      int deltaX,
      int deltaY) const override;

  // Translate Honeycomb events to Chromium UI events.
  virtual ui::KeyEvent TranslateUiKeyEvent(
      const HoneycombKeyEvent& key_event) const = 0;
  virtual ui::MouseEvent TranslateUiClickEvent(
      const HoneycombMouseEvent& mouse_event,
      HoneycombBrowserHost::MouseButtonType type,
      bool mouseUp,
      int clickCount) const;
  virtual ui::MouseEvent TranslateUiMoveEvent(const HoneycombMouseEvent& mouse_event,
                                              bool mouseLeave) const;
  virtual ui::MouseWheelEvent TranslateUiWheelEvent(
      const HoneycombMouseEvent& mouse_event,
      int deltaX,
      int deltaY) const;
  virtual gfx::Vector2d GetUiWheelEventOffset(int deltaX, int deltaY) const;

 protected:
  base::OnceClosure GetWidgetDeleteCallback();

  static base::TimeTicks GetEventTimeStamp();
  static int TranslateUiEventModifiers(uint32_t honey_modifiers);
  static int TranslateUiChangedButtonFlags(uint32_t honey_modifiers);

  // Widget hosting the web contents. It will be deleted automatically when the
  // associated root window is destroyed.
  views::Widget* window_widget_ = nullptr;

 private:
  // Will only be called if the Widget is deleted before
  // HoneycombBrowserHostBase::DestroyBrowser() is called.
  void WidgetDeleted();

  content::RenderWidgetHostViewAura* GetHostView() const;

  base::WeakPtrFactory<HoneycombBrowserPlatformDelegateNativeAura> weak_ptr_factory_{
      this};
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_NATIVE_BROWSER_PLATFORM_DELEGATE_NATIVE_AURA_H_
