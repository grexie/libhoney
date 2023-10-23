// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBTESTS_TEST_HANDLER_H_
#define HONEYCOMB_TESTS_HONEYCOMBTESTS_TEST_HANDLER_H_
#pragma once

#include <atomic>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "include/base/honey_callback.h"
#include "include/honey_browser.h"
#include "include/honey_client.h"
#include "include/honey_frame.h"
#include "include/honey_task.h"
#include "include/honey_waitable_event.h"
#include "tests/honeytests/thread_helper.h"
#include "tests/honeytests/track_callback.h"
#include "tests/gtest/include/gtest/gtest.h"

class ResourceContent {
 public:
  typedef std::multimap<std::string, std::string> HeaderMap;

  ResourceContent(const std::string& content,
                  const std::string& mime_type,
                  const HeaderMap& header_map)
      : content_(content), mime_type_(mime_type), header_map_(header_map) {}

  const std::string& content() const { return content_; }
  const std::string& mimeType() const { return mime_type_; }
  const HeaderMap& headerMap() const { return header_map_; }

 private:
  std::string content_;
  std::string mime_type_;
  HeaderMap header_map_;
};

// Base implementation of HoneycombClient for unit tests. Add new interfaces as needed
// by test cases.
class TestHandler : public HoneycombClient,
                    public HoneycombDialogHandler,
                    public HoneycombDisplayHandler,
                    public HoneycombDownloadHandler,
                    public HoneycombJSDialogHandler,
                    public HoneycombLifeSpanHandler,
                    public HoneycombLoadHandler,
                    public HoneycombRequestHandler,
                    public HoneycombResourceRequestHandler {
 public:
  // Tracks the completion state of related test runs.
  class CompletionState {
   public:
    // |total| is the number of times that TestComplete() must be called before
    // WaitForTests() will return.
    explicit CompletionState(int total);

    // Call this method to indicate that a test has completed.
    void TestComplete();

    // This method blocks until TestComplete() has been called the required
    // number of times.
    void WaitForTests();

    int total() const { return total_; }
    int count() const { return count_; }

   private:
    int total_;
    int count_;

    // Handle used to notify when the test is complete
    HoneycombRefPtr<HoneycombWaitableEvent> event_;
  };

  // Represents a collection of related tests that need to be run
  // simultaniously.
  class Collection {
   public:
    // The |completion_state| object must outlive this class.
    explicit Collection(CompletionState* completion_state);

    // The |test_handler| object must outlive this class and it must share the
    // same CompletionState object passed to the constructor.
    void AddTestHandler(TestHandler* test_handler);

    // Manages the test run.
    // 1. Calls TestHandler::SetupTest() for all of the test objects.
    // 2. Waits for all TestHandler objects to report that initial setup is
    //    complete by calling TestHandler::SetupComplete().
    // 3. Calls TestHandler::RunTest() for all of the test objects.
    // 4. Waits for all TestHandler objects to report that the test is
    //    complete by calling TestHandler::DestroyTest().
    void ExecuteTests();

   private:
    CompletionState* completion_state_;

    typedef std::list<TestHandler*> TestHandlerList;
    TestHandlerList handler_list_;
  };

  typedef std::map<int, HoneycombRefPtr<HoneycombBrowser>> BrowserMap;

  // Helper for executing methods using WeakPtr references to TestHandler.
  class UIThreadHelper {
   public:
    UIThreadHelper();

    // Pass in a |task| with an unretained reference to TestHandler. |task| will
    // be executed only if TestHandler::DestroyTest has not yet been called.
    // For example:
    //    GetUIThreadHelper()->PostTask(
    //        base::BindOnce(&TestHandler::DoSomething,
    //        base::Unretained(this)));
    void PostTask(base::OnceClosure task);
    void PostDelayedTask(base::OnceClosure task, int delay_ms);

   private:
    void TaskHelper(base::OnceClosure task);

    // Must be the last member.
    base::WeakPtrFactory<UIThreadHelper> weak_ptr_factory_;
  };

  // The |completion_state| object if specified must outlive this class.
  explicit TestHandler(CompletionState* completion_state = nullptr);
  ~TestHandler() override;

  // Implement this method to set up the test. Only used in combination with a
  // Collection. Call SetupComplete() once the setup is complete.
  virtual void SetupTest() {}

  // Implement this method to run the test. Call DestroyTest() once the test is
  // complete.
  virtual void RunTest() = 0;

  // HoneycombClient methods. Add new methods as needed by test cases.
  HoneycombRefPtr<HoneycombDialogHandler> GetDialogHandler() override { return this; }
  HoneycombRefPtr<HoneycombDisplayHandler> GetDisplayHandler() override { return this; }
  HoneycombRefPtr<HoneycombDownloadHandler> GetDownloadHandler() override { return this; }
  HoneycombRefPtr<HoneycombJSDialogHandler> GetJSDialogHandler() override { return this; }
  HoneycombRefPtr<HoneycombLifeSpanHandler> GetLifeSpanHandler() override { return this; }
  HoneycombRefPtr<HoneycombLoadHandler> GetLoadHandler() override { return this; }
  HoneycombRefPtr<HoneycombRequestHandler> GetRequestHandler() override { return this; }

  // HoneycombDownloadHandler methods
  void OnBeforeDownload(
      HoneycombRefPtr<HoneycombBrowser> browser,
      HoneycombRefPtr<HoneycombDownloadItem> download_item,
      const HoneycombString& suggested_name,
      HoneycombRefPtr<HoneycombBeforeDownloadCallback> callback) override {}

  // HoneycombLifeSpanHandler methods
  void OnAfterCreated(HoneycombRefPtr<HoneycombBrowser> browser) override;
  void OnBeforeClose(HoneycombRefPtr<HoneycombBrowser> browser) override;

  // HoneycombRequestHandler methods
  HoneycombRefPtr<HoneycombResourceRequestHandler> GetResourceRequestHandler(
      HoneycombRefPtr<HoneycombBrowser> browser,
      HoneycombRefPtr<HoneycombFrame> frame,
      HoneycombRefPtr<HoneycombRequest> request,
      bool is_navigation,
      bool is_download,
      const HoneycombString& request_initiator,
      bool& disable_default_handling) override {
    return this;
  }

  // HoneycombResourceRequestHandler methods
  HoneycombRefPtr<HoneycombResourceHandler> GetResourceHandler(
      HoneycombRefPtr<HoneycombBrowser> browser,
      HoneycombRefPtr<HoneycombFrame> frame,
      HoneycombRefPtr<HoneycombRequest> request) override;

  void OnRenderProcessTerminated(HoneycombRefPtr<HoneycombBrowser> browser,
                                 TerminationStatus status) override;

  // These methods should only be used if at most one non-popup browser exists.
  HoneycombRefPtr<HoneycombBrowser> GetBrowser() const;
  int GetBrowserId() const;

  // Copies the map of all the currently existing browsers into |map|. Must be
  // called on the UI thread.
  void GetAllBrowsers(BrowserMap* map);

  // Called by the test function to execute the test. This method blocks until
  // the test is complete. Do not reference the object after this method
  // returns. Do not use this method if the CompletionState object is shared by
  // multiple handlers or when using a Collection object.
  void ExecuteTest();

  // Event that will be signaled from the TestHandler destructor.
  // Used by ReleaseAndWaitForDestructor.
  void SetDestroyEvent(HoneycombRefPtr<HoneycombWaitableEvent> event) {
    destroy_event_ = event;
  }

  // If a test will not call DestroyTest() indicate so using this method.
  void SetDestroyTestExpected(bool expected) {
    destroy_test_expected_ = expected;
  }

  // Called from TestWindowDelegate when Views is enabled.
  void OnWindowCreated(int browser_id);
  void OnWindowDestroyed(int browser_id);

  // Returns true if a TestHandler currently exists.
  static bool HasTestHandler() {
    return test_handler_count_.load(std::memory_order_relaxed) > 0U;
  }

  std::string debug_string_prefix() const { return debug_string_prefix_; }

 protected:
  // Indicate that test setup is complete. Only used in combination with a
  // Collection.
  virtual void SetupComplete();

  // Close any remaining browsers. This may result in a call to TestComplete(),
  // depending on the configuration of SetSignalCompletionCount().
  virtual void DestroyTest();

  // Called on the UI thread if the test times out as a result of calling
  // SetTestTimeout(). Calls DestroyTest() by default.
  virtual void OnTestTimeout(int timeout_ms, bool treat_as_error);

  void CreateBrowser(const HoneycombString& url,
                     HoneycombRefPtr<HoneycombRequestContext> request_context = nullptr,
                     HoneycombRefPtr<HoneycombDictionaryValue> extra_info = nullptr);
  static void CloseBrowser(HoneycombRefPtr<HoneycombBrowser> browser, bool force_close);

  void AddResource(const std::string& url,
                   const std::string& content,
                   const std::string& mime_type,
                   const ResourceContent::HeaderMap& header_map = {});

  void AddResourceEx(const std::string& url, const ResourceContent& content);

  void ClearResources();

  // Specify the number of times that SignalTestCompletion() needs to be
  // explicitly called for test completion. Must be configured during test
  // initialization before any browsers are created.
  // - If the test creates browsers and does not explicitly call
  //   SignalTestCompletion() then the default value (0) can be used.
  // - If the test creates browsers and explicitly calls SignalTestCompletion()
  //   then set a value >= 1.
  // - If the test does not create browsers then it must explicitly call
  //   SignalTestCompletion() and set a value >= 1.
  void SetSignalTestCompletionCount(size_t count);

  // Explicitly signal test completion a single time. Used in combination with
  // SetSignalTestCompletionCount(). Results in a call to TestComplete() if
  // all browsers have closed and this method has been called the expected
  // number of times.
  void SignalTestCompletion();

  // Reuturns true if SignalTestCompletion() has been called the necessary
  // number of times (may be 0), in which case TestComplete() will be called
  // automatically when all browsers have closed. Must be called on the
  // UI thread.
  bool AllowTestCompletionWhenAllBrowsersClose() const;

  // Returns true if all browsers have closed. Must be called on the UI
  // thread.
  bool AllBrowsersClosed() const;

  // Call OnTestTimeout() after the specified amount of time.
  void SetTestTimeout(int timeout_ms = 5000, bool treat_as_error = true);

  // Returns the single UIThreadHelper instance, creating it if necessary. Must
  // be called on the UI thread.
  UIThreadHelper* GetUIThreadHelper();

 private:
  void MaybeTestComplete();

  // Complete the test. It is an error to call this method before all browsers
  // have closed.
  void TestComplete();

  enum NotifyType {
    NT_BROWSER,
    NT_WINDOW,
  };
  void OnCreated(int browser_id, NotifyType type);
  void OnClosed(int browser_id, NotifyType type);

  std::string MakeDebugStringPrefix() const;
  const std::string debug_string_prefix_;

  // Used to notify when the test is complete. Can be accessed on any thread.
  CompletionState* completion_state_;
  bool completion_state_owned_;

  // Map of browser ID to browser object. Only accessed on the UI thread.
  BrowserMap browser_map_;

  struct NotifyStatus {
    // Keyed by NotifyType.
    TrackCallback got_created[2];
    TrackCallback got_closed[2];
  };

  // Map of browser ID to current status. Only accessed on the UI thread.
  std::map<int, NotifyStatus> browser_status_map_;

  // Values for the first created browser. Modified on the UI thread but can be
  // accessed on any thread.
  int first_browser_id_ = 0;
  HoneycombRefPtr<HoneycombBrowser> first_browser_;

  // Map of resources that can be automatically loaded. Only accessed on the
  // IO thread.
  typedef std::map<std::string, ResourceContent> ResourceMap;
  ResourceMap resource_map_;

  // Number of times that SignalTestCompletion() must be called.
  size_t signal_completion_count_ = 0U;

  HoneycombRefPtr<HoneycombWaitableEvent> destroy_event_;

  // Tracks whether OnTestTimeout() has been called.
  bool test_timeout_called_ = false;

  // Tracks whether DestroyTest() is expected or has been called.
  bool destroy_test_expected_ = true;
  bool destroy_test_called_ = false;

  std::unique_ptr<UIThreadHelper> ui_thread_helper_;

  // Used to track the number of currently existing TestHandlers.
  static std::atomic<size_t> test_handler_count_;

  DISALLOW_COPY_AND_ASSIGN(TestHandler);
};

// Release |handler| and wait for the destructor to be called.
// This function is used to avoid test state leakage and to verify that
// all Handler references have been released on test completion.
template <typename T>
void ReleaseAndWaitForDestructor(HoneycombRefPtr<T>& handler, int delay_ms = 2000) {
  HoneycombRefPtr<HoneycombWaitableEvent> event =
      HoneycombWaitableEvent::CreateWaitableEvent(true, false);
  handler->SetDestroyEvent(event);
  T* _handler_ptr = handler.get();
  handler = nullptr;
  bool handler_destructed = event->TimedWait(delay_ms);
  EXPECT_TRUE(handler_destructed);
  if (!handler_destructed) {
    // |event| is a stack variable so clear the reference before returning.
    _handler_ptr->SetDestroyEvent(nullptr);
  }
}

// Returns true if the currently running test has failed.
bool TestFailed();

// Helper macros for executing checks in a method with a boolean return value.
// For example:
//
// bool VerifyVals(bool a, bool b) {
//   V_DECLARE();
//   V_EXPECT_TRUE(a);
//   V_EXPECT_FALSE(b);
//   V_RETURN();
// }
//
// EXPECT_TRUE(VerifyVals(true, false));

#define V_DECLARE()     \
  bool __verify = true; \
  bool __result

#define V_RETURN() return __verify

#define V_EXPECT_TRUE(condition)                         \
  __result = !!(condition);                              \
  __verify &= __result;                                  \
  GTEST_TEST_BOOLEAN_(__result, #condition, false, true, \
                      GTEST_NONFATAL_FAILURE_)

#define V_EXPECT_FALSE(condition)                           \
  __result = !!(condition);                                 \
  __verify &= !__result;                                    \
  GTEST_TEST_BOOLEAN_(!(__result), #condition, true, false, \
                      GTEST_NONFATAL_FAILURE_)

#endif  // HONEYCOMB_TESTS_HONEYCOMBTESTS_TEST_HANDLER_H_
