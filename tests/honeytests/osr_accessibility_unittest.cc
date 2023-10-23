// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/base/honey_callback.h"
#include "include/honey_accessibility_handler.h"
#include "include/honey_parser.h"
#include "include/honey_waitable_event.h"
#include "include/wrapper/honey_closure_task.h"
#include "tests/honeytests/test_handler.h"
#include "tests/honeytests/test_util.h"
#include "tests/gtest/include/gtest/gtest.h"

namespace {

const char kTestUrl[] = "https://tests/AccessibilityTestHandler";
const char kTipText[] = "Also known as User ID";

// Default OSR widget size.
const int kOsrWidth = 600;
const int kOsrHeight = 400;

// Test type.
enum AccessibilityTestType {
  // Enabling Accessibility should trigger the AccessibilityHandler callback
  // with Accessibility tree details
  TEST_ENABLE,
  // Disabling Accessibility should disable accessibility notification changes
  TEST_DISABLE,
  // Focus change on element should trigger Accessibility focus event
  TEST_FOCUS_CHANGE,
  // Hide/Show etc should trigger Location Change callbacks
  TEST_LOCATION_CHANGE
};

class AccessibilityTestHandler : public TestHandler,
                                 public HoneycombRenderHandler,
                                 public HoneycombAccessibilityHandler {
 public:
  AccessibilityTestHandler(const AccessibilityTestType& type)
      : test_type_(type), edit_box_id_(-1), accessibility_disabled_(false) {}

  HoneycombRefPtr<HoneycombAccessibilityHandler> GetAccessibilityHandler() override {
    return this;
  }

  HoneycombRefPtr<HoneycombRenderHandler> GetRenderHandler() override { return this; }

  // Honeycomb Renderer Handler Methods
  void GetViewRect(HoneycombRefPtr<HoneycombBrowser> browser, HoneycombRect& rect) override {
    rect = HoneycombRect(0, 0, kOsrWidth, kOsrHeight);
  }

  bool GetScreenInfo(HoneycombRefPtr<HoneycombBrowser> browser,
                     HoneycombScreenInfo& screen_info) override {
    screen_info.rect = HoneycombRect(0, 0, kOsrWidth, kOsrHeight);
    screen_info.available_rect = screen_info.rect;
    return true;
  }

  void OnPaint(HoneycombRefPtr<HoneycombBrowser> browser,
               HoneycombRenderHandler::PaintElementType type,
               const HoneycombRenderHandler::RectList& dirtyRects,
               const void* buffer,
               int width,
               int height) override {
    // Do nothing.
  }

  void RunTest() override {
    std::string html =
        "<html><head><title>AccessibilityTest</title></head>"
        "<body><span id='tipspan' role='tooltip' style='color:red;"
        "margin:20px'>";
    html += kTipText;
    html +=
        "</span>"
        "<input id='editbox' type='text' aria-describedby='tipspan' "
        "value='editbox' size='25px'/><input id='button' type='button' "
        "value='button' style='margin:20px'/></body></html>";
    AddResource(kTestUrl, html, "text/html");

    // Create the browser
    CreateOSRBrowser(kTestUrl);

    // Time out the test after a reasonable period of time.
    SetTestTimeout(5000);
  }

  void OnLoadEnd(HoneycombRefPtr<HoneycombBrowser> browser,
                 HoneycombRefPtr<HoneycombFrame> frame,
                 int httpStatusCode) override {
    // Enable Accessibility
    browser->GetHost()->SetAccessibilityState(STATE_ENABLED);
    switch (test_type_) {
      case TEST_ENABLE: {
        // This should trigger OnAccessibilityTreeChange
        // And update will be validated
      } break;
      case TEST_DISABLE: {
        // Post a delayed task to disable Accessibility
        HoneycombPostDelayedTask(
            TID_UI,
            base::BindOnce(&AccessibilityTestHandler::DisableAccessibility,
                           this, browser),
            200);
      } break;
      // Delayed task will posted later after we have initial details
      case TEST_FOCUS_CHANGE: {
      } break;
      case TEST_LOCATION_CHANGE: {
      } break;
    }
  }

  void OnAccessibilityTreeChange(HoneycombRefPtr<HoneycombValue> value) override {
    switch (test_type_) {
      case TEST_ENABLE: {
        TestEnableAccessibilityUpdate(value);
      } break;
      case TEST_DISABLE: {
        // Once Accessibility is disabled in the delayed Task
        // We should not reach here
        EXPECT_FALSE(accessibility_disabled_);
      } break;
      case TEST_LOCATION_CHANGE: {
        // find accessibility id of the edit box, before setting focus
        if (edit_box_id_ == -1) {
          HoneycombRefPtr<HoneycombDictionaryValue> update, event;
          GetFirstUpdateAndEvent(value, update, event);
          EXPECT_TRUE(update.get());

          // Ignore other events.
          if (!event.get() ||
              event->GetString("event_type") != "loadComplete") {
            break;
          }

          SetEditBoxIdAndRect(update);
          EXPECT_NE(edit_box_id_, -1);
          // Post a delayed task to hide the span and trigger location change
          HoneycombPostDelayedTask(
              TID_UI,
              base::BindOnce(&AccessibilityTestHandler::HideEditBox, this,
                             GetBrowser()),
              200);
        }
      } break;
      case TEST_FOCUS_CHANGE: {
        // find accessibility id of the edit box, before setting focus
        if (edit_box_id_ == -1) {
          HoneycombRefPtr<HoneycombDictionaryValue> update, event;
          GetFirstUpdateAndEvent(value, update, event);
          EXPECT_TRUE(update.get());

          // Ignore other events.
          if (!event.get() ||
              event->GetString("event_type") != "loadComplete") {
            break;
          }

          // Now post a delayed task to trigger focus to edit box
          SetEditBoxIdAndRect(update);
          EXPECT_NE(edit_box_id_, -1);

          HoneycombPostDelayedTask(
              TID_UI,
              base::BindOnce(&AccessibilityTestHandler::SetFocusOnEditBox, this,
                             GetBrowser()),
              200);
        } else {
          // Retrieve the "focus" event.
          HoneycombRefPtr<HoneycombDictionaryValue> event;
          if (!GetFirstMatchingEvent(value, "focus", event)) {
            return;
          }
          EXPECT_TRUE(event.get());

          // Verify that focus is set to expected element edit_box.
          EXPECT_EQ(edit_box_id_, event->GetInt("id"));

          // Now Post a delayed task to destroy the test giving
          // sufficient time for any accessibility updates to come through
          HoneycombPostDelayedTask(
              TID_UI,
              base::BindOnce(&AccessibilityTestHandler::DestroyTest, this),
              500);
        }
      } break;
    }
  }

  void OnAccessibilityLocationChange(HoneycombRefPtr<HoneycombValue> value) override {
    if (test_type_ == TEST_LOCATION_CHANGE) {
      EXPECT_NE(edit_box_id_, -1);
      EXPECT_TRUE(value.get());

      // Change has a valid list
      EXPECT_EQ(VTYPE_LIST, value->GetType());
      HoneycombRefPtr<HoneycombListValue> list = value->GetList();
      EXPECT_TRUE(list.get());

      got_accessibility_location_change_.yes();
    }

    if (got_hide_edit_box_) {
      // Now destroy the test.
      HoneycombPostTask(TID_UI,
                  base::BindOnce(&AccessibilityTestHandler::DestroyTest, this));
    }
  }

 private:
  void CreateOSRBrowser(const HoneycombString& url) {
    HoneycombWindowInfo windowInfo;
    HoneycombBrowserSettings settings;

#if defined(OS_WIN)
    windowInfo.SetAsWindowless(GetDesktopWindow());
#elif defined(OS_MAC)
    windowInfo.SetAsWindowless(kNullWindowHandle);
#elif defined(OS_LINUX)
    windowInfo.SetAsWindowless(kNullWindowHandle);
#else
#error "Unsupported platform"
#endif
    HoneycombBrowserHost::CreateBrowser(windowInfo, this, url, settings, nullptr,
                                  nullptr);
  }

  void HideEditBox(HoneycombRefPtr<HoneycombBrowser> browser) {
    // Hide the edit box.
    // This should trigger Location update if enabled
    browser->GetMainFrame()->ExecuteJavaScript(
        "document.getElementById('editbox').style.display = 'none';", kTestUrl,
        0);

    got_hide_edit_box_.yes();
  }

  void SetFocusOnEditBox(HoneycombRefPtr<HoneycombBrowser> browser) {
    // Set focus on edit box
    // This should trigger accessibility update if enabled
    browser->GetMainFrame()->ExecuteJavaScript(
        "document.getElementById('editbox').focus();", kTestUrl, 0);
  }

  void DisableAccessibility(HoneycombRefPtr<HoneycombBrowser> browser) {
    browser->GetHost()->SetAccessibilityState(STATE_DISABLED);
    accessibility_disabled_ = true;
    // Set focus on edit box
    SetFocusOnEditBox(browser);

    // Now Post a delayed task to destroy the test
    // giving sufficient time for any accessibility updates to come through
    HoneycombPostDelayedTask(
        TID_UI, base::BindOnce(&AccessibilityTestHandler::DestroyTest, this),
        500);
  }

  static HoneycombRefPtr<HoneycombListValue> GetUpdateList(HoneycombRefPtr<HoneycombValue> value) {
    EXPECT_TRUE(value.get());
    EXPECT_EQ(value->GetType(), VTYPE_DICTIONARY);
    HoneycombRefPtr<HoneycombDictionaryValue> topLevel = value->GetDictionary();
    EXPECT_TRUE(topLevel.get());

    return topLevel->GetList("updates");
  }

  static size_t GetUpdateListSize(HoneycombRefPtr<HoneycombValue> value) {
    HoneycombRefPtr<HoneycombListValue> updates = GetUpdateList(value);
    if (updates) {
      return updates->GetSize();
    }
    return 0U;
  }

  static HoneycombRefPtr<HoneycombDictionaryValue> GetUpdateValue(HoneycombRefPtr<HoneycombValue> value,
                                                      size_t index) {
    HoneycombRefPtr<HoneycombListValue> updates = GetUpdateList(value);
    if (!updates) {
      return nullptr;
    }
    EXPECT_LT(index, updates->GetSize());
    HoneycombRefPtr<HoneycombDictionaryValue> update = updates->GetDictionary(index);
    EXPECT_TRUE(update);
    return update;
  }

  static HoneycombRefPtr<HoneycombListValue> GetEventList(HoneycombRefPtr<HoneycombValue> value) {
    EXPECT_TRUE(value.get());
    EXPECT_EQ(value->GetType(), VTYPE_DICTIONARY);
    HoneycombRefPtr<HoneycombDictionaryValue> topLevel = value->GetDictionary();
    EXPECT_TRUE(topLevel.get());

    return topLevel->GetList("events");
  }

  static size_t GetEventListSize(HoneycombRefPtr<HoneycombValue> value) {
    HoneycombRefPtr<HoneycombListValue> events = GetEventList(value);
    if (events) {
      return events->GetSize();
    }
    return 0U;
  }

  static HoneycombRefPtr<HoneycombDictionaryValue> GetEventValue(HoneycombRefPtr<HoneycombValue> value,
                                                     size_t index) {
    HoneycombRefPtr<HoneycombListValue> events = GetEventList(value);
    if (!events) {
      return nullptr;
    }
    EXPECT_LT(index, events->GetSize());
    HoneycombRefPtr<HoneycombDictionaryValue> event = events->GetDictionary(index);
    EXPECT_TRUE(event);
    return event;
  }

  static void GetFirstUpdateAndEvent(HoneycombRefPtr<HoneycombValue> value,
                                     HoneycombRefPtr<HoneycombDictionaryValue>& update,
                                     HoneycombRefPtr<HoneycombDictionaryValue>& event) {
    if (GetUpdateListSize(value) > 0U) {
      update = GetUpdateValue(value, 0U);
    }
    if (GetEventListSize(value) > 0U) {
      event = GetEventValue(value, 0U);
    }
  }

  static bool GetFirstMatchingEvent(HoneycombRefPtr<HoneycombValue> value,
                                    const std::string& event_type,
                                    HoneycombRefPtr<HoneycombDictionaryValue>& event) {
    // Return the first event that matches the requested |event_type|.
    const size_t event_size = GetEventListSize(value);
    for (size_t i = 0; i < event_size; ++i) {
      HoneycombRefPtr<HoneycombDictionaryValue> cur_event = GetEventValue(value, i);
      const std::string& cur_event_type = cur_event->GetString("event_type");
      if (cur_event_type == event_type) {
        event = cur_event;
        return true;
      }
    }
    return false;
  }

  void TestEnableAccessibilityUpdate(HoneycombRefPtr<HoneycombValue> value) {
    HoneycombRefPtr<HoneycombDictionaryValue> update, event;
    GetFirstUpdateAndEvent(value, update, event);
    EXPECT_TRUE(update.get());

    // Ignore other events.
    if (!event.get() || event->GetString("event_type") != "loadComplete") {
      return;
    }

    // Get update and validate it has tree data
    EXPECT_TRUE(update->GetBool("has_tree_data"));
    HoneycombRefPtr<HoneycombDictionaryValue> treeData = update->GetDictionary("tree_data");

    // Validate title and Url
    EXPECT_STREQ("AccessibilityTest",
                 treeData->GetString("title").ToString().c_str());
    EXPECT_STREQ(kTestUrl, treeData->GetString("url").ToString().c_str());

    // Validate node data
    HoneycombRefPtr<HoneycombListValue> nodes = update->GetList("nodes");
    EXPECT_TRUE(nodes.get());
    EXPECT_GT(nodes->GetSize(), 0U);

    // Update has a valid root
    HoneycombRefPtr<HoneycombDictionaryValue> root;
    for (size_t index = 0; index < nodes->GetSize(); index++) {
      HoneycombRefPtr<HoneycombDictionaryValue> node = nodes->GetDictionary(index);
      if (node->GetString("role").ToString() == "rootWebArea") {
        root = node;
        break;
      }
    }
    EXPECT_TRUE(root.get());

    // One div containing the tree elements.
    HoneycombRefPtr<HoneycombListValue> childIDs = root->GetList("child_ids");
    EXPECT_TRUE(childIDs.get());
    EXPECT_EQ(1U, childIDs->GetSize());

    // Now Post a delayed task to destroy the test
    // giving sufficient time for any accessibility updates to come through
    HoneycombPostDelayedTask(
        TID_UI, base::BindOnce(&AccessibilityTestHandler::DestroyTest, this),
        500);
  }

  // Find Edit box Id in accessibility tree.
  void SetEditBoxIdAndRect(HoneycombRefPtr<HoneycombDictionaryValue> value) {
    EXPECT_TRUE(value.get());
    // Validate node data.
    HoneycombRefPtr<HoneycombListValue> nodes = value->GetList("nodes");
    EXPECT_TRUE(nodes.get());
    EXPECT_GT(nodes->GetSize(), 0U);

    // Find accessibility id for the text field.
    for (size_t index = 0; index < nodes->GetSize(); index++) {
      HoneycombRefPtr<HoneycombDictionaryValue> node = nodes->GetDictionary(index);
      if (node->GetString("role").ToString() == "textField") {
        edit_box_id_ = node->GetInt("id");
        HoneycombRefPtr<HoneycombDictionaryValue> loc = node->GetDictionary("location");
        EXPECT_TRUE(loc.get());
        EXPECT_GT(loc->GetDouble("x"), 0);
        EXPECT_GT(loc->GetDouble("y"), 0);
        EXPECT_GT(loc->GetDouble("width"), 0);
        EXPECT_GT(loc->GetDouble("height"), 0);
        break;
      }
    }
  }

  void DestroyTest() override {
    if (test_type_ == TEST_LOCATION_CHANGE) {
      EXPECT_GT(edit_box_id_, 0);
      EXPECT_TRUE(got_hide_edit_box_);
      EXPECT_TRUE(got_accessibility_location_change_);
    }
    TestHandler::DestroyTest();
  }

  AccessibilityTestType test_type_;
  int edit_box_id_;
  bool accessibility_disabled_;
  TrackCallback got_hide_edit_box_;
  TrackCallback got_accessibility_location_change_;

  IMPLEMENT_REFCOUNTING(AccessibilityTestHandler);
};

}  // namespace

TEST(OSRTest, AccessibilityEnable) {
  HoneycombRefPtr<AccessibilityTestHandler> handler =
      new AccessibilityTestHandler(TEST_ENABLE);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(OSRTest, AccessibilityDisable) {
  HoneycombRefPtr<AccessibilityTestHandler> handler =
      new AccessibilityTestHandler(TEST_DISABLE);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(OSRTest, AccessibilityFocusChange) {
  HoneycombRefPtr<AccessibilityTestHandler> handler =
      new AccessibilityTestHandler(TEST_FOCUS_CHANGE);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(OSRTest, AccessibilityLocationChange) {
  HoneycombRefPtr<AccessibilityTestHandler> handler =
      new AccessibilityTestHandler(TEST_LOCATION_CHANGE);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}
