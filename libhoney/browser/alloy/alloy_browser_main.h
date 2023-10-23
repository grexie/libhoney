// Copyright (c) 2011 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_ALLOY_ALLOY_BROWSER_MAIN_H_
#define HONEYCOMB_LIBHONEY_BROWSER_ALLOY_ALLOY_BROWSER_MAIN_H_
#pragma once

#include "libhoney/browser/request_context_impl.h"

#include "base/command_line.h"
#include "base/strings/string_piece.h"
#include "build/build_config.h"
#include "components/prefs/pref_service.h"
#include "content/public/browser/browser_main_parts.h"
#include "ui/display/screen.h"

#if defined(USE_AURA)
namespace wm {
class WMState;
}
#endif

namespace views {
class ViewsDelegate;
#if BUILDFLAG(IS_MAC)
class LayoutProvider;
#endif
}  // namespace views

#if BUILDFLAG(IS_LINUX)
namespace ui {
class LinuxUiGetter;
}
#endif

class HoneycombDevToolsDelegate;

class AlloyBrowserMainParts : public content::BrowserMainParts {
 public:
  AlloyBrowserMainParts();

  AlloyBrowserMainParts(const AlloyBrowserMainParts&) = delete;
  AlloyBrowserMainParts& operator=(const AlloyBrowserMainParts&) = delete;

  ~AlloyBrowserMainParts() override;

  void ToolkitInitialized() override;
  void PreCreateMainMessageLoop() override;
  void PostCreateMainMessageLoop() override;
  int PreCreateThreads() override;
  int PreMainMessageLoopRun() override;
  void PostMainMessageLoopRun() override;
  void PostDestroyThreads() override;

  HoneycombRefPtr<HoneycombRequestContextImpl> request_context() const {
    return global_request_context_;
  }
  HoneycombDevToolsDelegate* devtools_delegate() const { return devtools_delegate_; }

  scoped_refptr<base::SingleThreadTaskRunner> background_task_runner() const {
    return background_task_runner_;
  }
  scoped_refptr<base::SingleThreadTaskRunner> user_visible_task_runner() const {
    return user_visible_task_runner_;
  }
  scoped_refptr<base::SingleThreadTaskRunner> user_blocking_task_runner()
      const {
    return user_blocking_task_runner_;
  }

 private:
#if BUILDFLAG(IS_WIN)
  void PlatformInitialize();
#endif  // BUILDFLAG(IS_WIN)

  HoneycombRefPtr<HoneycombRequestContextImpl> global_request_context_;
  HoneycombDevToolsDelegate* devtools_delegate_ = nullptr;  // Deletes itself.

  // Blocking task runners exposed via HoneycombTaskRunner. For consistency with
  // previous named thread behavior always execute all pending tasks before
  // shutdown (e.g. to make sure critical data is saved to disk).
  // |background_task_runner_| is also passed to SQLitePersistentCookieStore.
  scoped_refptr<base::SingleThreadTaskRunner> background_task_runner_;
  scoped_refptr<base::SingleThreadTaskRunner> user_visible_task_runner_;
  scoped_refptr<base::SingleThreadTaskRunner> user_blocking_task_runner_;

#if defined(USE_AURA)
  std::unique_ptr<display::Screen> screen_;
  std::unique_ptr<wm::WMState> wm_state_;
#endif

  std::unique_ptr<views::ViewsDelegate> views_delegate_;
#if BUILDFLAG(IS_MAC)
  std::unique_ptr<display::ScopedNativeScreen> screen_;
  std::unique_ptr<views::LayoutProvider> layout_provider_;
#endif

#if BUILDFLAG(IS_LINUX)
  std::unique_ptr<ui::LinuxUiGetter> linux_ui_getter_;
#endif
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_ALLOY_ALLOY_BROWSER_MAIN_H_
