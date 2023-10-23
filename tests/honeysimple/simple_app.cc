// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeysimple/simple_app.h"

#include <string>

#include "include/honey_browser.h"
#include "include/honey_command_line.h"
#include "include/views/honey_browser_view.h"
#include "include/views/honey_window.h"
#include "include/wrapper/honey_helpers.h"
#include "tests/honeysimple/simple_handler.h"

namespace {

// When using the Views framework this object provides the delegate
// implementation for the HoneycombWindow that hosts the Views-based browser.
class SimpleWindowDelegate : public HoneycombWindowDelegate {
 public:
  explicit SimpleWindowDelegate(HoneycombRefPtr<HoneycombBrowserView> browser_view)
      : browser_view_(browser_view) {}

  void OnWindowCreated(HoneycombRefPtr<HoneycombWindow> window) override {
    // Add the browser view and show the window.
    window->AddChildView(browser_view_);
    window->Show();

    // Give keyboard focus to the browser view.
    browser_view_->RequestFocus();
  }

  void OnWindowDestroyed(HoneycombRefPtr<HoneycombWindow> window) override {
    browser_view_ = nullptr;
  }

  bool CanClose(HoneycombRefPtr<HoneycombWindow> window) override {
    // Allow the window to close if the browser says it's OK.
    HoneycombRefPtr<HoneycombBrowser> browser = browser_view_->GetBrowser();
    if (browser) {
      return browser->GetHost()->TryCloseBrowser();
    }
    return true;
  }

  HoneycombSize GetPreferredSize(HoneycombRefPtr<HoneycombView> view) override {
    return HoneycombSize(800, 600);
  }

 private:
  HoneycombRefPtr<HoneycombBrowserView> browser_view_;

  IMPLEMENT_REFCOUNTING(SimpleWindowDelegate);
  DISALLOW_COPY_AND_ASSIGN(SimpleWindowDelegate);
};

class SimpleBrowserViewDelegate : public HoneycombBrowserViewDelegate {
 public:
  SimpleBrowserViewDelegate() {}

  bool OnPopupBrowserViewCreated(HoneycombRefPtr<HoneycombBrowserView> browser_view,
                                 HoneycombRefPtr<HoneycombBrowserView> popup_browser_view,
                                 bool is_devtools) override {
    // Create a new top-level Window for the popup. It will show itself after
    // creation.
    HoneycombWindow::CreateTopLevelWindow(
        new SimpleWindowDelegate(popup_browser_view));

    // We created the Window.
    return true;
  }

 private:
  IMPLEMENT_REFCOUNTING(SimpleBrowserViewDelegate);
  DISALLOW_COPY_AND_ASSIGN(SimpleBrowserViewDelegate);
};

}  // namespace

SimpleApp::SimpleApp() {}

void SimpleApp::OnContextInitialized() {
  HONEYCOMB_REQUIRE_UI_THREAD();

  HoneycombRefPtr<HoneycombCommandLine> command_line =
      HoneycombCommandLine::GetGlobalCommandLine();

  // Create the browser using the Views framework if "--use-views" is specified
  // via the command-line. Otherwise, create the browser using the native
  // platform framework.
  const bool use_views = command_line->HasSwitch("use-views");

  // SimpleHandler implements browser-level callbacks.
  HoneycombRefPtr<SimpleHandler> handler(new SimpleHandler(use_views));

  // Specify Honeycomb browser settings here.
  HoneycombBrowserSettings browser_settings;

  std::string url;

  // Check if a "--url=" value was provided via the command-line. If so, use
  // that instead of the default URL.
  url = command_line->GetSwitchValue("url");
  if (url.empty()) {
    url = "http://www.google.com";
  }

  if (use_views) {
    // Create the BrowserView.
    HoneycombRefPtr<HoneycombBrowserView> browser_view = HoneycombBrowserView::CreateBrowserView(
        handler, url, browser_settings, nullptr, nullptr,
        new SimpleBrowserViewDelegate());

    // Create the Window. It will show itself after creation.
    HoneycombWindow::CreateTopLevelWindow(new SimpleWindowDelegate(browser_view));
  } else {
    // Information used when creating the native window.
    HoneycombWindowInfo window_info;

#if defined(OS_WIN)
    // On Windows we need to specify certain flags that will be passed to
    // CreateWindowEx().
    window_info.SetAsPopup(nullptr, "honeysimple");
#endif

    // Create the first browser window.
    HoneycombBrowserHost::CreateBrowser(window_info, handler, url, browser_settings,
                                  nullptr, nullptr);
  }
}

HoneycombRefPtr<HoneycombClient> SimpleApp::GetDefaultClient() {
  // Called when a new browser window is created via the Chrome runtime UI.
  return SimpleHandler::GetInstance();
}
