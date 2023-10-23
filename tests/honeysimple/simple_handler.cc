// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeysimple/simple_handler.h"

#include <sstream>
#include <string>

#include "include/base/honey_callback.h"
#include "include/honey_app.h"
#include "include/honey_parser.h"
#include "include/views/honey_browser_view.h"
#include "include/views/honey_window.h"
#include "include/wrapper/honey_closure_task.h"
#include "include/wrapper/honey_helpers.h"

namespace {

SimpleHandler* g_instance = nullptr;

// Returns a data: URI with the specified contents.
std::string GetDataURI(const std::string& data, const std::string& mime_type) {
  return "data:" + mime_type + ";base64," +
         HoneycombURIEncode(HoneycombBase64Encode(data.data(), data.size()), false)
             .ToString();
}

}  // namespace

SimpleHandler::SimpleHandler(bool use_views)
    : use_views_(use_views), is_closing_(false) {
  DCHECK(!g_instance);
  g_instance = this;
}

SimpleHandler::~SimpleHandler() {
  g_instance = nullptr;
}

// static
SimpleHandler* SimpleHandler::GetInstance() {
  return g_instance;
}

void SimpleHandler::OnTitleChange(HoneycombRefPtr<HoneycombBrowser> browser,
                                  const HoneycombString& title) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  if (use_views_) {
    // Set the title of the window using the Views framework.
    HoneycombRefPtr<HoneycombBrowserView> browser_view =
        HoneycombBrowserView::GetForBrowser(browser);
    if (browser_view) {
      HoneycombRefPtr<HoneycombWindow> window = browser_view->GetWindow();
      if (window) {
        window->SetTitle(title);
      }
    }
  } else if (!IsChromeRuntimeEnabled()) {
    // Set the title of the window using platform APIs.
    PlatformTitleChange(browser, title);
  }
}

void SimpleHandler::OnAfterCreated(HoneycombRefPtr<HoneycombBrowser> browser) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  // Add to the list of existing browsers.
  browser_list_.push_back(browser);
}

bool SimpleHandler::DoClose(HoneycombRefPtr<HoneycombBrowser> browser) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  // Closing the main window requires special handling. See the DoClose()
  // documentation in the Honeycomb header for a detailed destription of this
  // process.
  if (browser_list_.size() == 1) {
    // Set a flag to indicate that the window close should be allowed.
    is_closing_ = true;
  }

  // Allow the close. For windowed browsers this will result in the OS close
  // event being sent.
  return false;
}

void SimpleHandler::OnBeforeClose(HoneycombRefPtr<HoneycombBrowser> browser) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  // Remove from the list of existing browsers.
  BrowserList::iterator bit = browser_list_.begin();
  for (; bit != browser_list_.end(); ++bit) {
    if ((*bit)->IsSame(browser)) {
      browser_list_.erase(bit);
      break;
    }
  }

  if (browser_list_.empty()) {
    // All browser windows have closed. Quit the application message loop.
    HoneycombQuitMessageLoop();
  }
}

void SimpleHandler::OnLoadError(HoneycombRefPtr<HoneycombBrowser> browser,
                                HoneycombRefPtr<HoneycombFrame> frame,
                                ErrorCode errorCode,
                                const HoneycombString& errorText,
                                const HoneycombString& failedUrl) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  // Allow Chrome to show the error page.
  if (IsChromeRuntimeEnabled()) {
    return;
  }

  // Don't display an error for downloaded files.
  if (errorCode == ERR_ABORTED) {
    return;
  }

  // Display a load error message using a data: URI.
  std::stringstream ss;
  ss << "<html><body bgcolor=\"white\">"
        "<h2>Failed to load URL "
     << std::string(failedUrl) << " with error " << std::string(errorText)
     << " (" << errorCode << ").</h2></body></html>";

  frame->LoadURL(GetDataURI(ss.str(), "text/html"));
}

void SimpleHandler::CloseAllBrowsers(bool force_close) {
  if (!HoneycombCurrentlyOn(TID_UI)) {
    // Execute on the UI thread.
    HoneycombPostTask(TID_UI, base::BindOnce(&SimpleHandler::CloseAllBrowsers, this,
                                       force_close));
    return;
  }

  if (browser_list_.empty()) {
    return;
  }

  BrowserList::const_iterator it = browser_list_.begin();
  for (; it != browser_list_.end(); ++it) {
    (*it)->GetHost()->CloseBrowser(force_close);
  }
}

// static
bool SimpleHandler::IsChromeRuntimeEnabled() {
  static int value = -1;
  if (value == -1) {
    HoneycombRefPtr<HoneycombCommandLine> command_line =
        HoneycombCommandLine::GetGlobalCommandLine();
    value = command_line->HasSwitch("enable-chrome-runtime") ? 1 : 0;
  }
  return value == 1;
}
