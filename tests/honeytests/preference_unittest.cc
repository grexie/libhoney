// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/base/honey_callback.h"
#include "include/honey_request_context_handler.h"
#include "include/honey_waitable_event.h"
#include "include/wrapper/honey_closure_task.h"
#include "tests/honeytests/test_handler.h"
#include "tests/honeytests/test_util.h"
#include "tests/gtest/include/gtest/gtest.h"
#include "tests/shared/browser/client_app_browser.h"

namespace {

// Fully qualified preference names.
const char kPrefTest[] = "test";
const char kPrefTestBool[] = "test.bool";
const char kPrefTestInt[] = "test.int";
const char kPrefTestDouble[] = "test.double";
const char kPrefTestString[] = "test.string";
const char kPrefTestList[] = "test.list";
const char kPrefTestDict[] = "test.dict";
const char kPrefTestNoExist[] = "test.noexist";

// Unqualified preference names.
const char kPrefBool[] = "bool";
const char kPrefInt[] = "int";
const char kPrefDouble[] = "double";
const char kPrefString[] = "string";
const char kPrefList[] = "list";
const char kPrefDict[] = "dict";

std::string* PendingAction() {
  static std::string str;
  return &str;
}

HoneycombRefPtr<HoneycombValue> CreateBoolValue(bool value) {
  auto val = HoneycombValue::Create();
  val->SetBool(value);
  return val;
}

HoneycombRefPtr<HoneycombValue> CreateIntValue(int value) {
  auto val = HoneycombValue::Create();
  val->SetInt(value);
  return val;
}

HoneycombRefPtr<HoneycombValue> CreateDoubleValue(double value) {
  auto val = HoneycombValue::Create();
  val->SetDouble(value);
  return val;
}

HoneycombRefPtr<HoneycombValue> CreateStringValue(const std::string& value) {
  auto val = HoneycombValue::Create();
  val->SetString(value);
  return val;
}

HoneycombRefPtr<HoneycombValue> CreateListValue(HoneycombRefPtr<HoneycombListValue> value) {
  auto val = HoneycombValue::Create();
  val->SetList(value);
  return val;
}

HoneycombRefPtr<HoneycombValue> CreateDictionaryValue(HoneycombRefPtr<HoneycombDictionaryValue> value) {
  auto val = HoneycombValue::Create();
  val->SetDictionary(value);
  return val;
}

// Browser-side app delegate.
class PreferenceBrowserTest : public client::ClientAppBrowser::Delegate {
 public:
  PreferenceBrowserTest() {}

  void OnRegisterCustomPreferences(
      HoneycombRefPtr<client::ClientAppBrowser> app,
      honey_preferences_type_t type,
      HoneycombRawPtr<HoneycombPreferenceRegistrar> registrar) override {
    // Register test preferences.
    registrar->AddPreference(kPrefTestBool, CreateBoolValue(true));
    registrar->AddPreference(kPrefTestInt, CreateIntValue(2));
    registrar->AddPreference(kPrefTestDouble, CreateDoubleValue(5.0));
    registrar->AddPreference(kPrefTestString, CreateStringValue("default"));
    registrar->AddPreference(kPrefTestList,
                             CreateListValue(HoneycombListValue::Create()));
    registrar->AddPreference(
        kPrefTestDict, CreateDictionaryValue(HoneycombDictionaryValue::Create()));
  }

 private:
  IMPLEMENT_REFCOUNTING(PreferenceBrowserTest);
};

void ValidateReset(HoneycombRefPtr<HoneycombPreferenceManager> context, const char* name) {
  EXPECT_TRUE(context->HasPreference(name));
  EXPECT_TRUE(context->CanSetPreference(name));

  HoneycombString error;
  EXPECT_TRUE(context->SetPreference(name, nullptr, error));
  EXPECT_TRUE(error.empty());
}

void ValidateBool(HoneycombRefPtr<HoneycombPreferenceManager> context,
                  bool set,
                  bool expected,
                  const char* name = kPrefTestBool) {
  EXPECT_TRUE(context->HasPreference(name));
  EXPECT_TRUE(context->CanSetPreference(name));

  if (set) {
    HoneycombString error;
    EXPECT_TRUE(context->SetPreference(name, CreateBoolValue(expected), error));
    EXPECT_TRUE(error.empty());
  }

  auto value = context->GetPreference(name);
  EXPECT_TRUE(value.get());
  EXPECT_EQ(VTYPE_BOOL, value->GetType());
  EXPECT_EQ(expected, value->GetBool()) << *PendingAction();
}

void ValidateInt(HoneycombRefPtr<HoneycombPreferenceManager> context,
                 bool set,
                 int expected,
                 const char* name = kPrefTestInt) {
  EXPECT_TRUE(context->HasPreference(name));
  EXPECT_TRUE(context->CanSetPreference(name));

  if (set) {
    HoneycombString error;
    EXPECT_TRUE(context->SetPreference(name, CreateIntValue(expected), error));
    EXPECT_TRUE(error.empty());
  }

  auto value = context->GetPreference(name);
  EXPECT_TRUE(value.get());
  EXPECT_EQ(VTYPE_INT, value->GetType());
  EXPECT_EQ(expected, value->GetInt()) << *PendingAction();
}

void ValidateDouble(HoneycombRefPtr<HoneycombPreferenceManager> context,
                    bool set,
                    double expected,
                    const char* name = kPrefTestDouble) {
  EXPECT_TRUE(context->HasPreference(name));
  EXPECT_TRUE(context->CanSetPreference(name));

  if (set) {
    HoneycombString error;
    EXPECT_TRUE(
        context->SetPreference(name, CreateDoubleValue(expected), error));
    EXPECT_TRUE(error.empty());
  }

  auto value = context->GetPreference(name);
  EXPECT_TRUE(value.get());
  EXPECT_EQ(VTYPE_DOUBLE, value->GetType());
  EXPECT_EQ(expected, value->GetDouble()) << *PendingAction();
}

void ValidateString(HoneycombRefPtr<HoneycombPreferenceManager> context,
                    bool set,
                    const std::string& expected,
                    const char* name = kPrefTestString) {
  EXPECT_TRUE(context->HasPreference(name));
  EXPECT_TRUE(context->CanSetPreference(name));

  if (set) {
    HoneycombString error;
    EXPECT_TRUE(
        context->SetPreference(name, CreateStringValue(expected), error));
    EXPECT_TRUE(error.empty());
  }

  auto value = context->GetPreference(name);
  EXPECT_TRUE(value.get());
  EXPECT_EQ(VTYPE_STRING, value->GetType());
  EXPECT_STREQ(expected.c_str(), value->GetString().ToString().c_str())
      << *PendingAction();
}

void ValidateList(HoneycombRefPtr<HoneycombPreferenceManager> context,
                  bool set,
                  HoneycombRefPtr<HoneycombListValue> expected,
                  const char* name = kPrefTestList) {
  EXPECT_TRUE(context->HasPreference(name));
  EXPECT_TRUE(context->CanSetPreference(name));

  if (set) {
    HoneycombString error;
    EXPECT_TRUE(context->SetPreference(name, CreateListValue(expected), error));
    EXPECT_TRUE(error.empty());
  }

  auto value = context->GetPreference(name);
  EXPECT_TRUE(value.get());
  EXPECT_EQ(VTYPE_LIST, value->GetType());
  HoneycombRefPtr<HoneycombListValue> list_val = value->GetList();
  EXPECT_TRUE(list_val);
  TestListEqual(expected, list_val);
}

void ValidateDict(HoneycombRefPtr<HoneycombPreferenceManager> context,
                  bool set,
                  HoneycombRefPtr<HoneycombDictionaryValue> expected,
                  const char* name = kPrefTestDict) {
  EXPECT_TRUE(context->HasPreference(name));
  EXPECT_TRUE(context->CanSetPreference(name));

  if (set) {
    HoneycombString error;
    EXPECT_TRUE(
        context->SetPreference(name, CreateDictionaryValue(expected), error));
    EXPECT_TRUE(error.empty());
  }

  auto value = context->GetPreference(name);
  EXPECT_TRUE(value.get());
  EXPECT_EQ(VTYPE_DICTIONARY, value->GetType());
  HoneycombRefPtr<HoneycombDictionaryValue> dict_val = value->GetDictionary();
  EXPECT_TRUE(dict_val);
  TestDictionaryEqual(expected, dict_val);
}

void ValidateNoExist(HoneycombRefPtr<HoneycombPreferenceManager> context,
                     bool set,
                     const char* name = kPrefTestNoExist) {
  EXPECT_FALSE(context->HasPreference(name));
  EXPECT_FALSE(context->CanSetPreference(name));

  if (set) {
    HoneycombString error;
    EXPECT_FALSE(context->SetPreference(name, CreateBoolValue(false), error));
    EXPECT_FALSE(error.empty());
  }

  auto value = context->GetPreference(name);
  EXPECT_FALSE(value.get()) << *PendingAction();
}

void PopulateRootDefaults(HoneycombRefPtr<HoneycombDictionaryValue> val) {
  // Should match the values in OnRegisterCustomPreferences.
  val->SetBool(kPrefBool, true);
  val->SetInt(kPrefInt, 2);
  val->SetDouble(kPrefDouble, 5.0);
  val->SetString(kPrefString, "default");
  val->SetList(kPrefList, HoneycombListValue::Create());
  val->SetDictionary(kPrefDict, HoneycombDictionaryValue::Create());
}

void ValidateRoot(HoneycombRefPtr<HoneycombDictionaryValue> root,
                  HoneycombRefPtr<HoneycombDictionaryValue> expected,
                  const char* name = kPrefTest) {
  EXPECT_TRUE(root->HasKey(kPrefTest));
  EXPECT_EQ(VTYPE_DICTIONARY, root->GetType(kPrefTest));

  HoneycombRefPtr<HoneycombDictionaryValue> actual = root->GetDictionary(kPrefTest);
  TestDictionaryEqual(expected, actual);
}

// Validate getting default values.
void ValidateDefaults(HoneycombRefPtr<HoneycombPreferenceManager> context,
                      bool reset,
                      HoneycombRefPtr<HoneycombWaitableEvent> event) {
  if (!HoneycombCurrentlyOn(TID_UI)) {
    HoneycombPostTask(TID_UI,
                base::BindOnce(ValidateDefaults, context, reset, event));
    return;
  }

  if (reset) {
    // Reset default values.
    ValidateReset(context, kPrefTestBool);
    ValidateReset(context, kPrefTestInt);
    ValidateReset(context, kPrefTestDouble);
    ValidateReset(context, kPrefTestString);
    ValidateReset(context, kPrefTestList);
    ValidateReset(context, kPrefTestDict);
  }

  // Test default values.
  // Should match the values in HoneycombBrowserPrefStore::CreateService.
  ValidateBool(context, false, true);
  ValidateInt(context, false, 2);
  ValidateDouble(context, false, 5.0);
  ValidateString(context, false, "default");
  ValidateList(context, false, HoneycombListValue::Create());
  ValidateDict(context, false, HoneycombDictionaryValue::Create());
  ValidateNoExist(context, false);

  // Expected value of the tests root.
  HoneycombRefPtr<HoneycombDictionaryValue> expected = HoneycombDictionaryValue::Create();
  PopulateRootDefaults(expected);

  // Test all preferences including defaults.
  ValidateRoot(context->GetAllPreferences(true), expected);

  // Test all preferences excluding defaults.
  EXPECT_FALSE(context->GetAllPreferences(false)->HasKey(kPrefTest));

  event->Signal();
}

void PopulateListValue(HoneycombRefPtr<HoneycombListValue> val) {
  // Test list values.
  val->SetInt(0, 54);
  val->SetString(1, "foobar");
  val->SetDouble(2, 99.7643);
}

void PopulateDictValue(HoneycombRefPtr<HoneycombDictionaryValue> val) {
  // Test dictionary values.
  val->SetString("key1", "some string");
  val->SetBool("key2", false);

  HoneycombRefPtr<HoneycombListValue> list_val = HoneycombListValue::Create();
  PopulateListValue(list_val);
  val->SetList("key3", list_val);
}

void PopulateRootSet(HoneycombRefPtr<HoneycombDictionaryValue> val) {
  HoneycombRefPtr<HoneycombListValue> list_val = HoneycombListValue::Create();
  HoneycombRefPtr<HoneycombDictionaryValue> dict_val = HoneycombDictionaryValue::Create();

  PopulateListValue(list_val);
  PopulateDictValue(dict_val);

  // Should match the values in ValidateSetGet and ValidateGet.
  val->SetBool(kPrefBool, true);
  val->SetInt(kPrefInt, 65);
  val->SetDouble(kPrefDouble, 54.5443);
  val->SetString(kPrefString, "My test string");
  val->SetList(kPrefList, list_val);
  val->SetDictionary(kPrefDict, dict_val);
}

// Validate getting and setting values.
void ValidateSetGet(HoneycombRefPtr<HoneycombPreferenceManager> context,
                    HoneycombRefPtr<HoneycombWaitableEvent> event) {
  if (!HoneycombCurrentlyOn(TID_UI)) {
    HoneycombPostTask(TID_UI, base::BindOnce(ValidateSetGet, context, event));
    return;
  }

  HoneycombRefPtr<HoneycombListValue> list_val = HoneycombListValue::Create();
  HoneycombRefPtr<HoneycombDictionaryValue> dict_val = HoneycombDictionaryValue::Create();

  PopulateListValue(list_val);
  PopulateDictValue(dict_val);

  // Test setting/getting values.
  // Should match the values in PopulateRootSet and ValidateGet.
  ValidateBool(context, true, true);
  ValidateInt(context, true, 65);
  ValidateDouble(context, true, 54.5443);
  ValidateString(context, true, "My test string");
  ValidateList(context, true, list_val);
  ValidateDict(context, true, dict_val);
  ValidateNoExist(context, true);

  // Expected value of the tests root.
  HoneycombRefPtr<HoneycombDictionaryValue> expected = HoneycombDictionaryValue::Create();
  PopulateRootSet(expected);

  // Validate all preferences including defaults.
  ValidateRoot(context->GetAllPreferences(true), expected);

  // Validate all preferences excluding defaults.
  ValidateRoot(context->GetAllPreferences(false), expected);

  event->Signal();
}

// Validate getting values.
void ValidateGet(HoneycombRefPtr<HoneycombPreferenceManager> context,
                 HoneycombRefPtr<HoneycombWaitableEvent> event) {
  if (!HoneycombCurrentlyOn(TID_UI)) {
    HoneycombPostTask(TID_UI, base::BindOnce(ValidateGet, context, event));
    return;
  }

  HoneycombRefPtr<HoneycombListValue> list_val = HoneycombListValue::Create();
  HoneycombRefPtr<HoneycombDictionaryValue> dict_val = HoneycombDictionaryValue::Create();

  PopulateListValue(list_val);
  PopulateDictValue(dict_val);

  // Test getting values.
  // Should match the values in PopulateRootSet and ValidateSetGet.
  ValidateBool(context, false, true);
  ValidateInt(context, false, 65);
  ValidateDouble(context, false, 54.5443);
  ValidateString(context, false, "My test string");
  ValidateList(context, false, list_val);
  ValidateDict(context, false, dict_val);
  ValidateNoExist(context, false);

  // Expected value of the tests root.
  HoneycombRefPtr<HoneycombDictionaryValue> expected = HoneycombDictionaryValue::Create();
  PopulateRootSet(expected);

  // Validate all preferences including defaults.
  ValidateRoot(context->GetAllPreferences(true), expected);

  // Validate all preferences excluding defaults.
  ValidateRoot(context->GetAllPreferences(false), expected);

  event->Signal();
}

// No-op implementation.
class TestRequestContextHandler : public HoneycombRequestContextHandler {
 public:
  TestRequestContextHandler() {}
  explicit TestRequestContextHandler(HoneycombRefPtr<HoneycombWaitableEvent> event)
      : event_(event) {}

  void OnRequestContextInitialized(
      HoneycombRefPtr<HoneycombRequestContext> context) override {
    if (event_) {
      event_->Signal();
      event_ = nullptr;
    }
  }

 private:
  HoneycombRefPtr<HoneycombWaitableEvent> event_;

  IMPLEMENT_REFCOUNTING(TestRequestContextHandler);
};

}  // namespace

// Verify default preference values on the global state.
TEST(PreferenceTest, GlobalDefaults) {
  HoneycombRefPtr<HoneycombWaitableEvent> event =
      HoneycombWaitableEvent::CreateWaitableEvent(true, false);

  auto context = HoneycombPreferenceManager::GetGlobalPreferenceManager();
  EXPECT_TRUE(context.get());

  ValidateDefaults(context, false, event);
  event->Wait();
}

// Verify setting/getting preference values on the global state.
TEST(PreferenceTest, GlobalSetGet) {
  HoneycombRefPtr<HoneycombWaitableEvent> event =
      HoneycombWaitableEvent::CreateWaitableEvent(true, false);

  auto context = HoneycombPreferenceManager::GetGlobalPreferenceManager();
  EXPECT_TRUE(context.get());

  ValidateSetGet(context, event);
  event->Wait();

  // Reset to the default values.
  ValidateDefaults(context, true, event);
  event->Wait();
}

// Verify default preference values on the global request context.
TEST(PreferenceTest, RequestContextGlobalDefaults) {
  HoneycombRefPtr<HoneycombWaitableEvent> event =
      HoneycombWaitableEvent::CreateWaitableEvent(true, false);

  HoneycombRefPtr<HoneycombRequestContext> context = HoneycombRequestContext::GetGlobalContext();
  EXPECT_TRUE(context.get());

  ValidateDefaults(context, false, event);
  event->Wait();
}

// Verify setting/getting preference values on the global request context.
TEST(PreferenceTest, RequestContextGlobalSetGet) {
  HoneycombRefPtr<HoneycombWaitableEvent> event =
      HoneycombWaitableEvent::CreateWaitableEvent(true, false);

  HoneycombRefPtr<HoneycombRequestContext> context = HoneycombRequestContext::GetGlobalContext();
  EXPECT_TRUE(context.get());

  ValidateSetGet(context, event);
  event->Wait();

  // Reset to the default values.
  ValidateDefaults(context, true, event);
  event->Wait();
}

// Verify setting/getting preference values on shared global request contexts.
TEST(PreferenceTest, RequestContextGlobalSetGetShared) {
  HoneycombRefPtr<HoneycombWaitableEvent> event =
      HoneycombWaitableEvent::CreateWaitableEvent(true, false);

  HoneycombRefPtr<HoneycombRequestContext> context = HoneycombRequestContext::GetGlobalContext();
  EXPECT_TRUE(context.get());

  // Sharing storage.
  HoneycombRefPtr<HoneycombRequestContext> context2 =
      HoneycombRequestContext::CreateContext(context, nullptr);
  EXPECT_TRUE(context2.get());

  // Sharing storage.
  HoneycombRefPtr<HoneycombRequestContext> context3 =
      HoneycombRequestContext::CreateContext(context, new TestRequestContextHandler);
  EXPECT_TRUE(context3.get());

  // Unassociated context.
  HoneycombRequestContextSettings settings;
  HoneycombRefPtr<HoneycombRequestContext> context4 = HoneycombRequestContext::CreateContext(
      settings, new TestRequestContextHandler(event));
  EXPECT_TRUE(context4.get());
  // Wait for the context to be fully initialized.
  event->Wait();

  // Set/get the values on the first context.
  *PendingAction() = "Set/get the values on the first context";
  ValidateSetGet(context, event);
  event->Wait();

  // Get the values from the 2nd and 3rd contexts. They should be the same.
  *PendingAction() = "Get the values from the 2nd context.";
  ValidateGet(context2, event);
  event->Wait();
  *PendingAction() = "Get the values from the 3rd context.";
  ValidateGet(context3, event);
  event->Wait();

  // Get the values from the 4th context.
  *PendingAction() = "Get the values from the 4th context.";
  if (IsChromeRuntimeEnabled()) {
    // With the Chrome runtime, prefs set via an incognito profile will become
    // an overlay on top of the global (parent) profile. The incognito profile
    // shares the prefs in this case because they were set via the global
    // profile.
    ValidateGet(context4, event);
  } else {
    // They should be at the default.
    ValidateDefaults(context4, false, event);
  }
  event->Wait();

  // Reset to the default values.
  *PendingAction() = "Reset to the default values.";
  ValidateDefaults(context, true, event);
  event->Wait();
}

// Verify default preference values on a custom request context.
TEST(PreferenceTest, RequestContextCustomDefaults) {
  HoneycombRefPtr<HoneycombWaitableEvent> event =
      HoneycombWaitableEvent::CreateWaitableEvent(true, false);

  HoneycombRequestContextSettings settings;
  HoneycombRefPtr<HoneycombRequestContext> context = HoneycombRequestContext::CreateContext(
      settings, new TestRequestContextHandler(event));
  EXPECT_TRUE(context.get());
  // Wait for the context to be fully initialized.
  event->Wait();

  ValidateDefaults(context, false, event);
  event->Wait();
}

// Verify setting/getting preference values on a custom request context.
TEST(PreferenceTest, RequestContextCustomSetGet) {
  HoneycombRefPtr<HoneycombWaitableEvent> event =
      HoneycombWaitableEvent::CreateWaitableEvent(true, false);

  HoneycombRequestContextSettings settings;
  HoneycombRefPtr<HoneycombRequestContext> context = HoneycombRequestContext::CreateContext(
      settings, new TestRequestContextHandler(event));
  EXPECT_TRUE(context.get());
  // Wait for the context to be fully initialized.
  event->Wait();

  ValidateSetGet(context, event);
  event->Wait();

  // Reset to the default values.
  ValidateDefaults(context, true, event);
  event->Wait();
}

// Verify setting/getting preference values on shared custom request contexts.
TEST(PreferenceTest, RequestContextCustomSetGetShared) {
  HoneycombRefPtr<HoneycombWaitableEvent> event =
      HoneycombWaitableEvent::CreateWaitableEvent(true, false);

  HoneycombRequestContextSettings settings;
  HoneycombRefPtr<HoneycombRequestContext> context = HoneycombRequestContext::CreateContext(
      settings, new TestRequestContextHandler(event));
  EXPECT_TRUE(context.get());
  // Wait for the context to be fully initialized.
  event->Wait();

  // Sharing storage.
  HoneycombRefPtr<HoneycombRequestContext> context2 =
      HoneycombRequestContext::CreateContext(context, nullptr);
  EXPECT_TRUE(context2.get());

  // Sharing storage.
  HoneycombRefPtr<HoneycombRequestContext> context3 =
      HoneycombRequestContext::CreateContext(context, new TestRequestContextHandler);
  EXPECT_TRUE(context3.get());

  // Unassociated context.
  HoneycombRefPtr<HoneycombRequestContext> context4 = HoneycombRequestContext::CreateContext(
      settings, new TestRequestContextHandler(event));
  EXPECT_TRUE(context4.get());
  // Wait for the context to be fully initialized.
  event->Wait();

  // Set/get the values on the first context.
  *PendingAction() = "Set/get the values on the first context";
  ValidateSetGet(context, event);
  event->Wait();

  // Get the values from the 2nd and 3d contexts. They should be the same.
  *PendingAction() = "Get the values from the 2nd context.";
  ValidateGet(context2, event);
  event->Wait();
  *PendingAction() = "Get the values from the 3rd context.";
  ValidateGet(context3, event);
  event->Wait();

  // Get the values from the 4th context. They should be at the default.
  // This works with the Chrome runtime because the preference changes only
  // exist in the other incognito profile's overlay.
  *PendingAction() = "Get the values from the 4th context.";
  ValidateDefaults(context4, false, event);
  event->Wait();

  // Reset to the default values.
  *PendingAction() = "Reset to the default values.";
  ValidateDefaults(context, true, event);
  event->Wait();
}

// Entry point for creating preference browser test objects.
// Called from client_app_delegates.cc.
void CreatePreferenceBrowserTests(
    client::ClientAppBrowser::DelegateSet& delegates) {
  delegates.insert(new PreferenceBrowserTest);
}
