// Copyright (c) 2017 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeytests/extensions/extension_test_handler.h"

#include "include/honey_request_context_handler.h"
#include "tests/honeytests/test_suite.h"
#include "tests/honeytests/test_util.h"

ExtensionTestHandler::ExtensionTestHandler(
    RequestContextType request_context_type)
    : request_context_type_(request_context_type), create_main_browser_(true) {
  // Verify supported flag combinations.
  if (request_context_on_disk()) {
    EXPECT_TRUE(request_context_is_custom());
  }
  if (request_context_load_with_handler()) {
    EXPECT_FALSE(request_context_load_without_handler());
  }
  if (request_context_load_without_handler()) {
    EXPECT_TRUE(request_context_with_handler());
    EXPECT_FALSE(request_context_load_with_handler());
  }
}

ExtensionTestHandler::~ExtensionTestHandler() {
  if (!request_context_temp_dir_.IsEmpty()) {
    // Temporary directory will be deleted on shutdown.
    request_context_temp_dir_.Take();
  }
}

void ExtensionTestHandler::RunTest() {
  if (create_main_browser_) {
    OnAddMainBrowserResources();
  }

  HoneycombRefPtr<HoneycombRequestContextHandler> rc_handler;
  if (request_context_with_handler()) {
    class Handler : public HoneycombRequestContextHandler {
     public:
      explicit Handler(ExtensionTestHandler* test_handler)
          : test_handler_(test_handler) {}

      void OnRequestContextInitialized(
          HoneycombRefPtr<HoneycombRequestContext> request_context) override {
        if (test_handler_->create_main_browser()) {
          // Load extensions after the RequestContext has been initialized by
          // creation of the main browser.
          test_handler_->OnLoadExtensions();
        }
      }

     private:
      ExtensionTestHandler* test_handler_;

      IMPLEMENT_REFCOUNTING(Handler);
    };
    rc_handler = new Handler(this);
  }

  if (request_context_is_custom()) {
    HoneycombRequestContextSettings settings;

    if (request_context_on_disk()) {
      // Create a new temporary directory.
      EXPECT_TRUE(request_context_temp_dir_.CreateUniqueTempDirUnderPath(
          HoneycombTestSuite::GetInstance()->root_cache_path()));
      HoneycombString(&settings.cache_path) = request_context_temp_dir_.GetPath();
    }

    request_context_ = HoneycombRequestContext::CreateContext(settings, rc_handler);
  } else {
    request_context_ = HoneycombRequestContext::CreateContext(
        HoneycombRequestContext::GetGlobalContext(), rc_handler);
  }

  if (request_context_load_with_handler()) {
    class Handler : public HoneycombRequestContextHandler {
     public:
      Handler() {}

     private:
      IMPLEMENT_REFCOUNTING(Handler);
    };
    loader_request_context_ =
        HoneycombRequestContext::CreateContext(request_context_, new Handler());
  } else if (request_context_load_without_handler()) {
    loader_request_context_ =
        HoneycombRequestContext::CreateContext(request_context_, nullptr);
  } else {
    loader_request_context_ = request_context_;
  }

  if (create_main_browser_) {
    OnCreateMainBrowser();
  } else {
    // Creation of the extension browser will trigger initialization of the
    // RequestContext, so just load the extensions now.
    OnLoadExtensions();
  }

  // Time out the test after a reasonable period of time.
  SetTestTimeout();
}

void ExtensionTestHandler::DestroyTest() {
  OnDestroyTest();
  ReleaseRequestContexts();
  RoutingTestHandler::DestroyTest();
}

void ExtensionTestHandler::OnAfterCreated(HoneycombRefPtr<HoneycombBrowser> browser) {
  RoutingTestHandler::OnAfterCreated(browser);

  if (create_main_browser() && !request_context_with_handler() &&
      GetBrowserId() == browser->GetIdentifier()) {
    // When the RequestContext doesn't have a handler we won't get a
    // notification for RequestContext initialization. Instead use main browser
    // creation to indicate that the RequestContext has been initialized.
    OnLoadExtensions();
  }
}

void ExtensionTestHandler::OnExtensionLoadFailed(honey_errorcode_t result) {
  EXPECT_TRUE(HoneycombCurrentlyOn(TID_UI));
  EXPECT_TRUE(false);  // Not reached.
}

// HoneycombMessageRouterBrowserSide::Handler methods:
bool ExtensionTestHandler::OnQuery(HoneycombRefPtr<HoneycombBrowser> browser,
                                   HoneycombRefPtr<HoneycombFrame> frame,
                                   int64_t query_id,
                                   const HoneycombString& request,
                                   bool persistent,
                                   HoneycombRefPtr<Callback> callback) {
  if (OnMessage(browser, request)) {
    return true;
  }

  EXPECT_FALSE(true) << "Unexpected message: " << request.ToString();
  return false;
}

// static
HoneycombRefPtr<HoneycombDictionaryValue> ExtensionTestHandler::CreateDefaultManifest(
    const std::vector<std::string>& api_permissions) {
  HoneycombRefPtr<HoneycombDictionaryValue> manifest = HoneycombDictionaryValue::Create();
  manifest->SetString("name", "An extension");
  manifest->SetString("description", "An extension description");
  manifest->SetString("version", "1.0");
  manifest->SetInt("manifest_version", 2);

  HoneycombRefPtr<HoneycombListValue> permissions = HoneycombListValue::Create();
  permissions->SetSize(api_permissions.size() + 2);
  size_t idx = 0;
  for (; idx < api_permissions.size(); ++idx) {
    permissions->SetString(idx, api_permissions[idx]);
  }

  // Allow access to all http/https origins.
  permissions->SetString(idx++, "http://*/*");
  permissions->SetString(idx++, "https://*/*");

  manifest->SetList("permissions", permissions);

  return manifest;
}

// static
std::string ExtensionTestHandler::GetMessageJS(const std::string& message) {
  EXPECT_TRUE(!message.empty());
  return "window.testQuery({request:'" + message + "'});";
}

// static
void ExtensionTestHandler::VerifyExtensionInContext(
    HoneycombRefPtr<HoneycombExtension> extension,
    HoneycombRefPtr<HoneycombRequestContext> context,
    bool has_access,
    bool is_loader) {
  const HoneycombString& extension_id = extension->GetIdentifier();
  EXPECT_FALSE(extension_id.empty());

  if (has_access) {
    EXPECT_TRUE(context->DidLoadExtension(extension_id));
    EXPECT_TRUE(context->HasExtension(extension_id));
  } else {
    EXPECT_FALSE(context->DidLoadExtension(extension_id));
    EXPECT_FALSE(context->HasExtension(extension_id));
  }

  HoneycombRefPtr<HoneycombExtension> extension2 = context->GetExtension(extension_id);
  if (has_access) {
    EXPECT_TRUE(extension2);
    EXPECT_TRUE(extension->IsSame(extension2));
    TestDictionaryEqual(extension->GetManifest(), extension2->GetManifest());
  } else {
    EXPECT_FALSE(extension2);
  }

  std::vector<HoneycombString> extension_ids;
  EXPECT_TRUE(context->GetExtensions(extension_ids));

  // Should be our test extension and possibly the builtin PDF extension if it
  // has finished loading (our extension may load first if the call to
  // LoadExtension initializes the request context).
  bool has_extension = false;
  for (size_t i = 0; i < extension_ids.size(); ++i) {
    if (extension_ids[i] == extension_id) {
      has_extension = true;
      break;
    }
  }
  if (has_access) {
    EXPECT_TRUE(has_extension);
  } else {
    EXPECT_FALSE(has_extension);
  }
}

void ExtensionTestHandler::LoadExtension(
    const std::string& extension_path,
    HoneycombRefPtr<HoneycombDictionaryValue> manifest) {
  EXPECT_TRUE(!extension_path.empty());
  loader_request_context_->LoadExtension(extension_path, manifest, this);
}

void ExtensionTestHandler::UnloadExtension(HoneycombRefPtr<HoneycombExtension> extension) {
  EXPECT_TRUE(extension);
  extension->Unload();
  EXPECT_FALSE(extension->IsLoaded());
  EXPECT_FALSE(extension->GetLoaderContext());
}

void ExtensionTestHandler::ReleaseRequestContexts() {
  request_context_ = nullptr;
  loader_request_context_ = nullptr;
}
