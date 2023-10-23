// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeyclient/browser/window_test_runner_views.h"

#include "include/views/honey_browser_view.h"
#include "include/views/honey_display.h"
#include "include/views/honey_window.h"
#include "include/wrapper/honey_helpers.h"

#include "tests/honeyclient/browser/root_window_views.h"
#include "tests/honeyclient/browser/views_window.h"

namespace client {
namespace window_test {

namespace {

HoneycombRefPtr<HoneycombWindow> GetWindow(const HoneycombRefPtr<HoneycombBrowser>& browser) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  DCHECK(browser->GetHost()->HasView());

  HoneycombRefPtr<HoneycombBrowserView> browser_view =
      HoneycombBrowserView::GetForBrowser(browser);
  DCHECK(browser_view.get());

  HoneycombRefPtr<HoneycombWindow> window = browser_view->GetWindow();
  DCHECK(window.get());
  return window;
}

void SetTitlebarHeight(const HoneycombRefPtr<HoneycombBrowser>& browser,
                       const std::optional<float>& height) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  auto root_window = RootWindow::GetForBrowser(browser->GetIdentifier());
  DCHECK(root_window.get());

  auto root_window_views = static_cast<RootWindowViews*>(root_window.get());
  root_window_views->SetTitlebarHeight(height);
}

}  // namespace

WindowTestRunnerViews::WindowTestRunnerViews() {}

void WindowTestRunnerViews::SetPos(HoneycombRefPtr<HoneycombBrowser> browser,
                                   int x,
                                   int y,
                                   int width,
                                   int height) {
  HoneycombRefPtr<HoneycombWindow> window = GetWindow(browser);

  HoneycombRect window_bounds(x, y, width, height);
  ModifyBounds(window->GetDisplay()->GetWorkArea(), window_bounds);

  window->SetBounds(window_bounds);
}

void WindowTestRunnerViews::Minimize(HoneycombRefPtr<HoneycombBrowser> browser) {
  GetWindow(browser)->Minimize();
}

void WindowTestRunnerViews::Maximize(HoneycombRefPtr<HoneycombBrowser> browser) {
  GetWindow(browser)->Maximize();
}

void WindowTestRunnerViews::Restore(HoneycombRefPtr<HoneycombBrowser> browser) {
  GetWindow(browser)->Restore();
}

void WindowTestRunnerViews::Fullscreen(HoneycombRefPtr<HoneycombBrowser> browser) {
  auto window = GetWindow(browser);

  // Results in a call to ViewsWindow::OnWindowFullscreenTransition().
  if (window->IsFullscreen()) {
    window->SetFullscreen(false);
  } else {
    window->SetFullscreen(true);
  }
}

void WindowTestRunnerViews::SetTitleBarHeight(
    HoneycombRefPtr<HoneycombBrowser> browser,
    const std::optional<float>& height) {
  SetTitlebarHeight(browser, height);
}

}  // namespace window_test
}  // namespace client
