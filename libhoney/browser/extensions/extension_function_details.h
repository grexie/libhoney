// Copyright 2017 the Honeycomb Authors. Portions copyright
// 2014 The Chromium Authors and 2016 the CEF Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_EXTENSION_FUNCTION_DETAILS_H_
#define HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_EXTENSION_FUNCTION_DETAILS_H_

#include "libhoney/browser/alloy/alloy_browser_host_impl.h"

#include "include/honey_extension.h"

#include "base/functional/callback_forward.h"
#include "chrome/common/extensions/api/tabs.h"
#include "ui/gfx/native_widget_types.h"

class Profile;
class ExtensionFunction;

namespace content {
class WebContents;
}

namespace extensions {

// Provides Honeycomb-specific details to ExtensionFunction implementations.
// Based on chrome/browser/extensions/chrome_extension_function_details.h.
class HoneycombExtensionFunctionDetails {
 public:
  // Constructs a new ChromeExtensionFunctionDetails instance for |function|.
  // This instance does not own |function| and must outlive it.
  explicit HoneycombExtensionFunctionDetails(ExtensionFunction* function);

  HoneycombExtensionFunctionDetails(const HoneycombExtensionFunctionDetails&) = delete;
  HoneycombExtensionFunctionDetails& operator=(const HoneycombExtensionFunctionDetails&) =
      delete;

  ~HoneycombExtensionFunctionDetails();

  Profile* GetProfile() const;

  // Get the "sender" browser that is hosting the extension. May return NULL
  // during startup/shutdown.
  HoneycombRefPtr<AlloyBrowserHostImpl> GetSenderBrowser() const;

  // Get the "current" browser that will be acted on by this extension function,
  // if any. When mapping from a tabId use the GetBrowserForTabId* methods
  // instead of calling this method directly.
  //
  // Many extension APIs operate relative to the browser that the calling code
  // is running inside of. For example, popups and tabs all have a containing
  // browser, but background pages and notification bubbles do not. Other APIs,
  // like chrome.tabs.*, can act on either a specific browser (specified via the
  // tabId parameter) or should allow the client to determine the most
  // appropriate browser (for example, the browser that representing the
  // foreground window).
  //
  // Incognito browsers should not be considered unless the calling extension
  // has incognito access enabled. Honeycomb does not internally enforce incognito
  // status so we pass this flag to client callbacks for consideration.
  //
  // This method can return NULL if there is no matching browser, which can
  // happen if only incognito windows are open, or early in startup or shutdown
  // shutdown when there are no active windows.
  HoneycombRefPtr<AlloyBrowserHostImpl> GetCurrentBrowser() const;

  // Returns true if the sender browser can access |target|. When mapping from a
  // tabId use the GetBrowserForTabId* methods instead of calling this method
  // directly.
  bool CanAccessBrowser(HoneycombRefPtr<AlloyBrowserHostImpl> target) const;

  // Returns the browser matching |tab_id| or NULL if the browser cannot be
  // found or does not have a WebContents. If |tab_id| is < 0 the "current"
  // browser will be returned. |error_message| can optionally be passed in and
  // will be set with an appropriate message on error. This method should only
  // be called one time per extension function and will check all necessary
  // client permissions.
  HoneycombRefPtr<AlloyBrowserHostImpl> GetBrowserForTabIdFirstTime(
      int tab_id,
      std::string* error_message) const;

  // Returns the browser matching |tab_id| or NULL if the browser cannot be
  // found or does not have a WebContents. |tab_id| must be >= 0.
  // |error_message| can optionally be passed in and will be set with an
  // appropriate message on error. This method should be called only after
  // GetBrowserForTabIdFirstTime() has succeeded for the same |tab_id|.
  HoneycombRefPtr<AlloyBrowserHostImpl> GetBrowserForTabIdAgain(
      int tab_id,
      std::string* error_message) const;

  // Give the client a chance to handle |file|. |callback| will be executed
  // once the file contents have been loaded. Returns false if the file is
  // unhandled.
  using LoadFileCallback =
      base::OnceCallback<void(std::unique_ptr<std::string>)>;
  bool LoadFile(const std::string& file, LoadFileCallback callback) const;

  struct OpenTabParams {
    OpenTabParams();
    ~OpenTabParams();

    bool create_browser_if_needed = false;
    absl::optional<int> window_id;
    absl::optional<int> opener_tab_id;
    absl::optional<std::string> url;
    absl::optional<bool> active;
    absl::optional<bool> pinned;
    absl::optional<int> index;
    absl::optional<int> bookmark_id;
  };

  // Opens a new tab given creation parameters |params|. Returns a Tab object
  // if successful, or NULL and optionally sets |error_message| if an error
  // occurs.
  std::unique_ptr<api::tabs::Tab> OpenTab(const OpenTabParams& params,
                                          bool user_gesture,
                                          std::string* error_message) const;

  // Creates a Tab object (see chrome/common/extensions/api/tabs.json) with
  // information about the state of a browser tab. Depending on the
  // permissions of the extension, the object may or may not include sensitive
  // data such as the tab's URL.
  api::tabs::Tab CreateTabObject(HoneycombRefPtr<AlloyBrowserHostImpl> new_browser,
                                 int opener_browser_id,
                                 bool active,
                                 int index) const;

  // Creates a tab MutedInfo object (see chrome/common/extensions/api/tabs.json)
  // with information about the mute state of a browser tab.
  static api::tabs::MutedInfo CreateMutedInfo(content::WebContents* contents);

  // Returns a pointer to the associated ExtensionFunction
  ExtensionFunction* function() { return function_; }
  const ExtensionFunction* function() const { return function_; }

 protected:
  HoneycombRefPtr<HoneycombExtension> GetHoneycombExtension() const;

 private:
  // The function for which these details have been created. Must outlive the
  // HoneycombExtensionFunctionDetails instance.
  ExtensionFunction* function_;

  mutable HoneycombRefPtr<HoneycombExtension> honey_extension_;

  // Verifies correct usage of GetBrowserForTabId* methods.
  mutable bool get_browser_called_first_time_ = false;
};

}  // namespace extensions

#endif  // HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_EXTENSION_FUNCTION_DETAILS_H_
