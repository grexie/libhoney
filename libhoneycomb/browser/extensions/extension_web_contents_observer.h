// Copyright 2014 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_EXTENSIONS_EXTENSION_WEB_CONTENTS_OBSERVER_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_EXTENSIONS_EXTENSION_WEB_CONTENTS_OBSERVER_H_

#include <memory>

#include "base/observer_list.h"
#include "content/public/browser/web_contents_user_data.h"
#include "extensions/browser/extension_web_contents_observer.h"
#include "extensions/browser/script_executor.h"

namespace extensions {

// The Honeycomb version of ExtensionWebContentsObserver.
class HoneycombExtensionWebContentsObserver
    : public ExtensionWebContentsObserver,
      public content::WebContentsUserData<HoneycombExtensionWebContentsObserver> {
 public:
  HoneycombExtensionWebContentsObserver(const HoneycombExtensionWebContentsObserver&) =
      delete;
  HoneycombExtensionWebContentsObserver& operator=(
      const HoneycombExtensionWebContentsObserver&) = delete;

  ~HoneycombExtensionWebContentsObserver() override;

  // Creates and initializes an instance of this class for the given
  // |web_contents|, if it doesn't already exist.
  static void CreateForWebContents(content::WebContents* web_contents);

  ScriptExecutor* script_executor() { return script_executor_.get(); }

 private:
  friend class content::WebContentsUserData<HoneycombExtensionWebContentsObserver>;

  explicit HoneycombExtensionWebContentsObserver(content::WebContents* web_contents);

  // content::WebContentsObserver overrides.
  void RenderFrameCreated(content::RenderFrameHost* render_frame_host) override;

  std::unique_ptr<ScriptExecutor> script_executor_;

  WEB_CONTENTS_USER_DATA_KEY_DECL();
};

}  // namespace extensions

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_EXTENSIONS_EXTENSION_WEB_CONTENTS_OBSERVER_H_
