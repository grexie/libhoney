// Copyright (c) 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_EXTENSIONS_API_TABS_TABS_API_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_EXTENSIONS_API_TABS_TABS_API_H_

#include "libhoneycomb/browser/extensions/extension_function_details.h"

#include "chrome/common/extensions/api/tabs.h"
#include "extensions/browser/api/execute_code_function.h"
#include "extensions/browser/extension_function.h"

// The contents of this file are extracted from
// chrome/browser/extensions/api/tabs/tabs_api.h.

namespace content {
class WebContents;
}

namespace extensions {
namespace honey {

class TabsGetFunction : public ExtensionFunction {
  ~TabsGetFunction() override {}

  ResponseAction Run() override;

  DECLARE_EXTENSION_FUNCTION("tabs.get", TABS_GET)
};

class TabsCreateFunction : public ExtensionFunction {
 public:
  TabsCreateFunction();
  ~TabsCreateFunction() override {}

  ResponseAction Run() override;

  DECLARE_EXTENSION_FUNCTION("tabs.create", TABS_CREATE)

 private:
  const HoneycombExtensionFunctionDetails honey_details_;
};

class BaseAPIFunction : public ExtensionFunction {
 public:
  BaseAPIFunction();

 protected:
  ~BaseAPIFunction() override {}

  // Gets the WebContents for |tab_id| if it is specified. Otherwise get the
  // WebContents for the active tab in the current window. Calling this function
  // may set |error_|.
  content::WebContents* GetWebContents(int tab_id);

  std::string error_;
  const HoneycombExtensionFunctionDetails honey_details_;
};

class TabsUpdateFunction : public BaseAPIFunction {
 private:
  ~TabsUpdateFunction() override {}

  ResponseAction Run() override;

  bool UpdateURL(const std::string& url, int tab_id, std::string* error);
  ResponseValue GetResult();

  DECLARE_EXTENSION_FUNCTION("tabs.update", TABS_UPDATE)

  int tab_id_ = -1;
  content::WebContents* web_contents_ = nullptr;
};

// Implement API calls tabs.executeScript, tabs.insertCSS, and tabs.removeCSS.
class ExecuteCodeInTabFunction : public ExecuteCodeFunction {
 public:
  ExecuteCodeInTabFunction();

 protected:
  ~ExecuteCodeInTabFunction() override;

  // Initializes |execute_tab_id_| and |details_|.
  InitResult Init() override;
  bool ShouldInsertCSS() const override;
  bool ShouldRemoveCSS() const override;
  bool CanExecuteScriptOnPage(std::string* error) override;
  ScriptExecutor* GetScriptExecutor(std::string* error) override;
  bool IsWebView() const override;
  const GURL& GetWebViewSrc() const override;
  bool LoadFile(const std::string& file, std::string* error) override;

 private:
  const HoneycombExtensionFunctionDetails honey_details_;

  void LoadFileComplete(const std::string& file,
                        std::unique_ptr<std::string> data);

  // Id of tab which executes code.
  int execute_tab_id_;
};

class TabsExecuteScriptFunction : public ExecuteCodeInTabFunction {
 private:
  ~TabsExecuteScriptFunction() override {}

  DECLARE_EXTENSION_FUNCTION("tabs.executeScript", TABS_EXECUTESCRIPT)
};

class TabsInsertCSSFunction : public ExecuteCodeInTabFunction {
 private:
  ~TabsInsertCSSFunction() override {}

  bool ShouldInsertCSS() const override;

  DECLARE_EXTENSION_FUNCTION("tabs.insertCSS", TABS_INSERTCSS)
};

class TabsRemoveCSSFunction : public ExecuteCodeInTabFunction {
 private:
  ~TabsRemoveCSSFunction() override {}

  bool ShouldRemoveCSS() const override;

  DECLARE_EXTENSION_FUNCTION("tabs.removeCSS", TABS_INSERTCSS)
};

// Based on ChromeAsyncExtensionFunction.
class ZoomAPIFunction : public ExtensionFunction {
 public:
  ZoomAPIFunction();

 protected:
  ~ZoomAPIFunction() override {}

  // Gets the WebContents for |tab_id| if it is specified. Otherwise get the
  // WebContents for the active tab in the current window. Calling this function
  // may set |error_|.
  content::WebContents* GetWebContents(int tab_id);

  std::string error_;

 private:
  const HoneycombExtensionFunctionDetails honey_details_;
};

class TabsSetZoomFunction : public BaseAPIFunction {
 private:
  ~TabsSetZoomFunction() override {}

  ResponseAction Run() override;

  DECLARE_EXTENSION_FUNCTION("tabs.setZoom", TABS_SETZOOM)
};

class TabsGetZoomFunction : public BaseAPIFunction {
 private:
  ~TabsGetZoomFunction() override {}

  ResponseAction Run() override;

  DECLARE_EXTENSION_FUNCTION("tabs.getZoom", TABS_GETZOOM)
};

class TabsSetZoomSettingsFunction : public BaseAPIFunction {
 private:
  ~TabsSetZoomSettingsFunction() override {}

  ResponseAction Run() override;

  DECLARE_EXTENSION_FUNCTION("tabs.setZoomSettings", TABS_SETZOOMSETTINGS)
};

class TabsGetZoomSettingsFunction : public BaseAPIFunction {
 private:
  ~TabsGetZoomSettingsFunction() override {}

  ResponseAction Run() override;

  DECLARE_EXTENSION_FUNCTION("tabs.getZoomSettings", TABS_GETZOOMSETTINGS)
};

}  // namespace honey
}  // namespace extensions

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_EXTENSIONS_API_TABS_TABS_API_H_
