// Copyright (c) 2017 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeytests/extensions/extension_test_handler.h"

#include "tests/honeytests/test_util.h"
#include "tests/shared/browser/client_app_browser.h"
#include "tests/shared/browser/extension_util.h"

using client::ClientAppBrowser;

namespace {

const char kExtensionPath[] = "background-extension";
const char kBackgroundScript[] = "background.js";
// HTML file created internally to load the background script.
const char kGeneratedBackgroundPage[] = "_generated_background_page.html";

// Test load/unload of an extension with a background script.
class BackgroundLoadUnloadTestHandler : public ExtensionTestHandler {
 public:
  explicit BackgroundLoadUnloadTestHandler(
      RequestContextType request_context_type)
      : ExtensionTestHandler(request_context_type) {
    // Only creating the extension browser.
    set_create_main_browser(false);
  }

  // HoneycombExtensionHandler methods:
  void OnExtensionLoaded(HoneycombRefPtr<HoneycombExtension> extension) override {
    EXPECT_TRUE(HoneycombCurrentlyOn(TID_UI));
    EXPECT_TRUE(extension);
    EXPECT_TRUE(extension->IsLoaded());
    EXPECT_TRUE(extension->GetLoaderContext());
    EXPECT_TRUE(
        loader_request_context()->IsSame(extension->GetLoaderContext()));
    VerifyExtension(extension);

    EXPECT_FALSE(got_loaded_);
    got_loaded_.yes();

    EXPECT_FALSE(extension_);
    extension_ = extension;

    background_page_url_ = GetExtensionURL(extension, kGeneratedBackgroundPage);

    // Add extension resources.
    script_url_ = GetExtensionURL(extension, kBackgroundScript);
    AddResource(script_url_, GetMessageJS("extension_onload"),
                "text/javascript");
  }

  void OnExtensionUnloaded(HoneycombRefPtr<HoneycombExtension> extension) override {
    EXPECT_TRUE(HoneycombCurrentlyOn(TID_UI));
    EXPECT_TRUE(extension);
    EXPECT_FALSE(extension->IsLoaded());
    EXPECT_FALSE(extension->GetLoaderContext());

    EXPECT_FALSE(got_unloaded_);
    got_unloaded_.yes();

    EXPECT_TRUE(extension_);
    EXPECT_TRUE(extension_->IsSame(extension));

    // The extension should no longer be registered with the context.
    if (loader_request_context()) {
      VerifyExtensionInContext(extension, loader_request_context(), false,
                               true);
    }
    if (request_context() && !request_context_same_loader()) {
      VerifyExtensionInContext(extension, request_context(), false, false);
    }

    extension_ = nullptr;

    // Execute asynchronously so call stacks have a chance to unwind.
    // Will close the browser windows.
    HoneycombPostTask(
        TID_UI,
        base::BindOnce(&BackgroundLoadUnloadTestHandler::DestroyTest, this));
  }

  bool OnBeforeBackgroundBrowser(HoneycombRefPtr<HoneycombExtension> extension,
                                 const HoneycombString& url,
                                 HoneycombRefPtr<HoneycombClient>& client,
                                 HoneycombBrowserSettings& settings) override {
    EXPECT_TRUE(HoneycombCurrentlyOn(TID_UI));
    EXPECT_TRUE(extension);
    EXPECT_TRUE(extension->IsLoaded());
    EXPECT_TRUE(extension->GetLoaderContext());
    EXPECT_TRUE(
        loader_request_context()->IsSame(extension->GetLoaderContext()));
    VerifyExtension(extension);

    const std::string& background_page_url =
        GetExtensionURL(extension, kGeneratedBackgroundPage);
    EXPECT_STREQ(background_page_url.c_str(), url.ToString().c_str());

    EXPECT_FALSE(client);
    client = this;

    // Allow the browser creation.
    return false;
  }

  // HoneycombLoadHandler methods:
  void OnLoadingStateChange(HoneycombRefPtr<HoneycombBrowser> browser,
                            bool isLoading,
                            bool canGoBack,
                            bool canGoForward) override {
    EXPECT_TRUE(browser->GetHost()->IsBackgroundHost());

    HoneycombRefPtr<HoneycombExtension> extension = browser->GetHost()->GetExtension();
    EXPECT_TRUE(extension);
    EXPECT_TRUE(extension_->IsSame(extension));

    if (isLoading) {
      EXPECT_FALSE(extension_browser_);
      extension_browser_ = browser;
    } else {
      EXPECT_TRUE(browser->IsSame(extension_browser_));

      const std::string& url = browser->GetMainFrame()->GetURL();
      EXPECT_STREQ(background_page_url_.c_str(), url.c_str());

      EXPECT_FALSE(got_load_done_);
      got_load_done_.yes();

      TriggerDestroyTestIfDone();
    }
  }

  // HoneycombResourceRequestHandler methods:
  HoneycombRefPtr<HoneycombResourceHandler> GetResourceHandler(
      HoneycombRefPtr<HoneycombBrowser> browser,
      HoneycombRefPtr<HoneycombFrame> frame,
      HoneycombRefPtr<HoneycombRequest> request) override {
    EXPECT_TRUE(browser->GetHost()->IsBackgroundHost());
    EXPECT_TRUE(browser->IsSame(extension_browser_));

    HoneycombRefPtr<HoneycombExtension> extension = browser->GetHost()->GetExtension();
    EXPECT_TRUE(extension);
    EXPECT_TRUE(extension_->IsSame(extension));

    const std::string& url = request->GetURL();
    if (url == background_page_url_) {
      EXPECT_FALSE(got_background_page_url_request_);
      got_background_page_url_request_.yes();
    } else if (url == script_url_) {
      EXPECT_FALSE(got_script_url_request_);
      got_script_url_request_.yes();
    } else {
      EXPECT_TRUE(false);  // Not reached.
    }

    // Handle the resource request.
    return RoutingTestHandler::GetResourceHandler(browser, frame, request);
  }

  HoneycombRefPtr<HoneycombExtension> extension() const { return extension_; }

  // Verify |extension| contents.
  void VerifyExtension(HoneycombRefPtr<HoneycombExtension> extension) const {
    EXPECT_STREQ(("extensions/" + std::string(kExtensionPath)).c_str(),
                 client::extension_util::GetInternalExtensionResourcePath(
                     extension->GetPath())
                     .c_str());

    HoneycombRefPtr<HoneycombDictionaryValue> expected_manifest = CreateManifest();
    TestDictionaryEqual(expected_manifest, extension->GetManifest());

    VerifyExtensionInContext(extension, loader_request_context(), true, true);
    if (!request_context_same_loader()) {
      VerifyExtensionInContext(extension, request_context(), true, false);
    }
  }

  std::string GetExtensionURL(HoneycombRefPtr<HoneycombExtension> extension,
                              const std::string& resource_path) const {
    const std::string& identifier = extension->GetIdentifier();
    const std::string& origin =
        client::extension_util::GetExtensionOrigin(identifier);
    EXPECT_FALSE(origin.empty());
    return origin + resource_path;
  }

 protected:
  void OnLoadExtensions() override {
    LoadExtension(kExtensionPath, CreateManifest());
  }

  bool OnMessage(HoneycombRefPtr<HoneycombBrowser> browser,
                 const std::string& message) override {
    EXPECT_STREQ("extension_onload", message.c_str());
    EXPECT_TRUE(browser->GetHost()->IsBackgroundHost());

    HoneycombRefPtr<HoneycombExtension> extension = browser->GetHost()->GetExtension();
    EXPECT_TRUE(extension);
    EXPECT_TRUE(extension_->IsSame(extension));

    EXPECT_TRUE(browser->IsSame(extension_browser_));
    EXPECT_TRUE(browser->GetHost()->IsBackgroundHost());

    EXPECT_FALSE(got_body_onload_);
    got_body_onload_.yes();

    TriggerDestroyTestIfDone();
    return true;
  }

  void OnDestroyTest() override {
    extension_browser_ = nullptr;

    EXPECT_TRUE(got_loaded_);
    EXPECT_TRUE(got_background_page_url_request_);
    EXPECT_TRUE(got_script_url_request_);
    EXPECT_TRUE(got_body_onload_);
    EXPECT_TRUE(got_load_done_);
    EXPECT_TRUE(got_unloaded_);
  }

  // Create a manifest with background script.
  HoneycombRefPtr<HoneycombDictionaryValue> CreateManifest() const {
    HoneycombRefPtr<HoneycombDictionaryValue> manifest =
        CreateDefaultManifest(ApiPermissionsList());

    HoneycombRefPtr<HoneycombDictionaryValue> background = HoneycombDictionaryValue::Create();
    HoneycombRefPtr<HoneycombListValue> scripts = HoneycombListValue::Create();
    scripts->SetString(0, kBackgroundScript);
    background->SetList("scripts", scripts);
    manifest->SetDictionary("background", background);

    return manifest;
  }

  void TriggerDestroyTestIfDone() {
    if (got_body_onload_ && got_load_done_) {
      TriggerDestroyTest();
    }
  }

  virtual void TriggerDestroyTest() {
    // Execute asynchronously so call stacks have a chance to unwind.
    HoneycombPostTask(TID_UI, base::BindOnce(
                            &BackgroundLoadUnloadTestHandler::UnloadExtension,
                            this, extension_));
  }

  HoneycombRefPtr<HoneycombExtension> extension_;
  std::string script_url_;
  std::string background_page_url_;
  HoneycombRefPtr<HoneycombBrowser> extension_browser_;

  TrackCallback got_loaded_;
  TrackCallback got_background_page_url_request_;
  TrackCallback got_script_url_request_;
  TrackCallback got_body_onload_;
  TrackCallback got_load_done_;
  TrackCallback got_unloaded_;

  IMPLEMENT_REFCOUNTING(BackgroundLoadUnloadTestHandler);
  DISALLOW_COPY_AND_ASSIGN(BackgroundLoadUnloadTestHandler);
};

}  // namespace

EXTENSION_TEST_GROUP_ALL(BackgroundLoadUnload, BackgroundLoadUnloadTestHandler)

namespace {

// Same as above but without the unload. Only do this with a custom context to
// avoid poluting the global context.
class BackgroundLoadNoUnloadTestHandler
    : public BackgroundLoadUnloadTestHandler {
 public:
  explicit BackgroundLoadNoUnloadTestHandler(
      RequestContextType request_context_type)
      : BackgroundLoadUnloadTestHandler(request_context_type) {}

 protected:
  void TriggerDestroyTest() override {
    // Release everything that references the request context. This should
    // trigger unload of the extension.
    CloseBrowser(extension_browser_, false);
    extension_browser_ = nullptr;
    ReleaseRequestContexts();
  }
};

}  // namespace

EXTENSION_TEST_GROUP_MINIMAL_CUSTOM(BackgroundLoadNoUnload,
                                    BackgroundLoadNoUnloadTestHandler)
