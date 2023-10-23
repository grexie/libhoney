// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <sstream>

#include "include/base/honey_callback.h"
#include "include/honey_task.h"
#include "include/honey_v8.h"
#include "include/wrapper/honey_closure_task.h"
#include "tests/honeytests/test_handler.h"
#include "tests/honeytests/test_util.h"
#include "tests/gtest/include/gtest/gtest.h"
#include "tests/shared/browser/client_app_browser.h"
#include "tests/shared/renderer/client_app_renderer.h"

using client::ClientAppBrowser;
using client::ClientAppRenderer;

// How to add a new test:
// 1. Add a new value to the V8TestMode enumeration.
// 2. Add a method that implements the test in V8RendererTest.
// 3. Add a case for the new enumeration value in V8RendererTest::RunTest.
// 4. Add a line for the test in the "Define the tests" section at the bottom of
//    the file.

namespace {

// Unique values for V8 tests.
const char kV8TestUrl[] = "https://tests/V8Test.Test";
const char kV8BindingTestUrl[] = "https://tests/V8Test.BindingTest";
const char kV8ContextParentTestUrl[] = "https://tests/V8Test.ContextParentTest";
const char kV8ContextChildTestUrl[] = "https://tests/V8Test.ContextChildTest";
const char kV8NavTestUrl[] = "https://tests/V8Test.NavTest";
const char kV8ContextEvalCspBypassUnsafeEval[] =
    "https://tests/V8Test.ContextEvalCspBypassUnsafeEval";
const char kV8ContextEvalCspBypassSandbox[] =
    "https://tests/V8Test.ContextEvalCspBypassSandbox";
const char kV8OnUncaughtExceptionTestUrl[] =
    "https://tests/V8Test.OnUncaughtException";
const char kV8HandlerCallOnReleasedContextUrl[] =
    "https://tests/V8Test.HandlerCallOnReleasedContext/main.html";
const char kV8HandlerCallOnReleasedContextChildUrl[] =
    "https://tests/V8Test.HandlerCallOnReleasedContext/child.html";
const char kV8TestMsg[] = "V8Test.Test";
const char kV8TestCmdKey[] = "v8-test";
const char kV8RunTestMsg[] = "V8Test.RunTest";

enum V8TestMode {
  V8TEST_NONE = 0,
  V8TEST_NULL_CREATE,
  V8TEST_BOOL_CREATE,
  V8TEST_INT_CREATE,
  V8TEST_UINT_CREATE,
  V8TEST_DOUBLE_CREATE,
  V8TEST_DATE_CREATE,
  V8TEST_STRING_CREATE,
  V8TEST_EMPTY_STRING_CREATE,
  V8TEST_ARRAY_CREATE,
  V8TEST_ARRAY_VALUE,
  V8TEST_ARRAY_BUFFER,
  V8TEST_ARRAY_BUFFER_VALUE,
  V8TEST_OBJECT_CREATE,
  V8TEST_OBJECT_USERDATA,
  V8TEST_OBJECT_ACCESSOR,
  V8TEST_OBJECT_ACCESSOR_EXCEPTION,
  V8TEST_OBJECT_ACCESSOR_FAIL,
  V8TEST_OBJECT_ACCESSOR_READONLY,
  V8TEST_OBJECT_INTERCEPTOR,
  V8TEST_OBJECT_INTERCEPTOR_FAIL,
  V8TEST_OBJECT_INTERCEPTOR_EXCEPTION,
  V8TEST_OBJECT_INTERCEPTOR_AND_ACCESSOR,
  V8TEST_OBJECT_VALUE,
  V8TEST_OBJECT_VALUE_READONLY,
  V8TEST_OBJECT_VALUE_ENUM,
  V8TEST_OBJECT_VALUE_DONTENUM,
  V8TEST_OBJECT_VALUE_DELETE,
  V8TEST_OBJECT_VALUE_DONTDELETE,
  V8TEST_OBJECT_VALUE_EMPTYKEY,
  V8TEST_FUNCTION_CREATE,
  V8TEST_FUNCTION_HANDLER,
  V8TEST_FUNCTION_HANDLER_EXCEPTION,
  V8TEST_FUNCTION_HANDLER_FAIL,
  V8TEST_FUNCTION_HANDLER_NO_OBJECT,
  V8TEST_FUNCTION_HANDLER_WITH_CONTEXT,
  V8TEST_FUNCTION_HANDLER_EMPTY_STRING,
  V8TEST_PROMISE_CREATE,
  V8TEST_PROMISE_RESOLVE,
  V8TEST_PROMISE_RESOLVE_NO_ARGUMENT,
  V8TEST_PROMISE_RESOLVE_HANDLER,
  V8TEST_PROMISE_REJECT,
  V8TEST_PROMISE_REJECT_HANDLER,
  V8TEST_CONTEXT_EVAL,
  V8TEST_CONTEXT_EVAL_EXCEPTION,
  V8TEST_CONTEXT_EVAL_CSP_BYPASS_UNSAFE_EVAL,
  V8TEST_CONTEXT_EVAL_CSP_BYPASS_SANDBOX,
  V8TEST_CONTEXT_ENTERED,
  V8TEST_BINDING,
  V8TEST_STACK_TRACE,
  V8TEST_ON_UNCAUGHT_EXCEPTION,
  V8TEST_ON_UNCAUGHT_EXCEPTION_DEV_TOOLS,
  V8TEST_EXTENSION,
  V8TEST_HANDLER_CALL_ON_RELEASED_CONTEXT,
};

// Renderer side.
class V8RendererTest : public ClientAppRenderer::Delegate,
                       public HoneycombLoadHandler {
 public:
  V8RendererTest() : test_mode_(V8TEST_NONE) {}

  // Run a test when the process message is received from the browser.
  void RunTest() {
    switch (test_mode_) {
      case V8TEST_NULL_CREATE:
        RunNullCreateTest();
        break;
      case V8TEST_BOOL_CREATE:
        RunBoolCreateTest();
        break;
      case V8TEST_INT_CREATE:
        RunIntCreateTest();
        break;
      case V8TEST_UINT_CREATE:
        RunUIntCreateTest();
        break;
      case V8TEST_DOUBLE_CREATE:
        RunDoubleCreateTest();
        break;
      case V8TEST_DATE_CREATE:
        RunDateCreateTest();
        break;
      case V8TEST_STRING_CREATE:
        RunStringCreateTest();
        break;
      case V8TEST_EMPTY_STRING_CREATE:
        RunEmptyStringCreateTest();
        break;
      case V8TEST_ARRAY_CREATE:
        RunArrayCreateTest();
        break;
      case V8TEST_ARRAY_VALUE:
        RunArrayValueTest();
        break;
      case V8TEST_ARRAY_BUFFER:
        RunArrayBufferTest();
        break;
      case V8TEST_ARRAY_BUFFER_VALUE:
        RunArrayBufferValueTest();
        break;
      case V8TEST_OBJECT_CREATE:
        RunObjectCreateTest();
        break;
      case V8TEST_OBJECT_USERDATA:
        RunObjectUserDataTest();
        break;
      case V8TEST_OBJECT_ACCESSOR:
        RunObjectAccessorTest();
        break;
      case V8TEST_OBJECT_ACCESSOR_EXCEPTION:
        RunObjectAccessorExceptionTest();
        break;
      case V8TEST_OBJECT_ACCESSOR_FAIL:
        RunObjectAccessorFailTest();
        break;
      case V8TEST_OBJECT_ACCESSOR_READONLY:
        RunObjectAccessorReadOnlyTest();
        break;
      case V8TEST_OBJECT_INTERCEPTOR:
        RunObjectInterceptorTest();
        break;
      case V8TEST_OBJECT_INTERCEPTOR_FAIL:
        RunObjectInterceptorFailTest();
        break;
      case V8TEST_OBJECT_INTERCEPTOR_EXCEPTION:
        RunObjectInterceptorExceptionTest();
        break;
      case V8TEST_OBJECT_INTERCEPTOR_AND_ACCESSOR:
        RunObjectInterceptorAndAccessorTest();
        break;
      case V8TEST_OBJECT_VALUE:
        RunObjectValueTest();
        break;
      case V8TEST_OBJECT_VALUE_READONLY:
        RunObjectValueReadOnlyTest();
        break;
      case V8TEST_OBJECT_VALUE_ENUM:
        RunObjectValueEnumTest();
        break;
      case V8TEST_OBJECT_VALUE_DONTENUM:
        RunObjectValueDontEnumTest();
        break;
      case V8TEST_OBJECT_VALUE_DELETE:
        RunObjectValueDeleteTest();
        break;
      case V8TEST_OBJECT_VALUE_DONTDELETE:
        RunObjectValueDontDeleteTest();
        break;
      case V8TEST_OBJECT_VALUE_EMPTYKEY:
        RunObjectValueEmptyKeyTest();
        break;
      case V8TEST_FUNCTION_CREATE:
        RunFunctionCreateTest();
        break;
      case V8TEST_FUNCTION_HANDLER:
        RunFunctionHandlerTest();
        break;
      case V8TEST_FUNCTION_HANDLER_EXCEPTION:
        RunFunctionHandlerExceptionTest();
        break;
      case V8TEST_FUNCTION_HANDLER_FAIL:
        RunFunctionHandlerFailTest();
        break;
      case V8TEST_FUNCTION_HANDLER_NO_OBJECT:
        RunFunctionHandlerNoObjectTest();
        break;
      case V8TEST_FUNCTION_HANDLER_WITH_CONTEXT:
        RunFunctionHandlerWithContextTest();
        break;
      case V8TEST_FUNCTION_HANDLER_EMPTY_STRING:
        RunFunctionHandlerEmptyStringTest();
        break;
      case V8TEST_PROMISE_CREATE:
        RunPromiseCreateTest();
        break;
      case V8TEST_PROMISE_RESOLVE:
        RunPromiseResolveTest();
        break;
      case V8TEST_PROMISE_RESOLVE_NO_ARGUMENT:
        RunPromiseResolveNoArgumentTest();
        break;
      case V8TEST_PROMISE_RESOLVE_HANDLER:
        RunPromiseResolveHandlerTest();
        break;
      case V8TEST_PROMISE_REJECT:
        RunPromiseRejectTest();
        break;
      case V8TEST_PROMISE_REJECT_HANDLER:
        RunPromiseRejectHandlerTest();
        break;
      case V8TEST_CONTEXT_EVAL:
        RunContextEvalTest();
        break;
      case V8TEST_CONTEXT_EVAL_EXCEPTION:
        RunContextEvalExceptionTest();
        break;
      case V8TEST_CONTEXT_EVAL_CSP_BYPASS_UNSAFE_EVAL:
        RunContextEvalCspBypassUnsafeEval();
        break;
      case V8TEST_CONTEXT_EVAL_CSP_BYPASS_SANDBOX:
        RunContextEvalCspBypassSandbox();
        break;
      case V8TEST_CONTEXT_ENTERED:
        RunContextEnteredTest();
        break;
      case V8TEST_BINDING:
        RunBindingTest();
        break;
      case V8TEST_STACK_TRACE:
        RunStackTraceTest();
        break;
      case V8TEST_ON_UNCAUGHT_EXCEPTION:
        RunOnUncaughtExceptionTest();
        break;
      case V8TEST_HANDLER_CALL_ON_RELEASED_CONTEXT:
        break;
      default:
        // Was a startup test.
        EXPECT_TRUE(startup_test_success_);
        DestroyTest();
        break;
    }
  }

  // Run a test on render process startup.
  void RunStartupTest() {
    switch (test_mode_) {
      case V8TEST_EXTENSION:
        RunExtensionTest();
        break;
      default:
        break;
    }
  }

  void RunNullCreateTest() {
    HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateNull();
    EXPECT_TRUE(value.get());
    EXPECT_TRUE(value->IsNull());

    EXPECT_FALSE(value->IsUndefined());
    EXPECT_FALSE(value->IsArray());
    EXPECT_FALSE(value->IsBool());
    EXPECT_FALSE(value->IsDate());
    EXPECT_FALSE(value->IsDouble());
    EXPECT_FALSE(value->IsFunction());
    EXPECT_FALSE(value->IsInt());
    EXPECT_FALSE(value->IsUInt());
    EXPECT_FALSE(value->IsObject());
    EXPECT_FALSE(value->IsPromise());
    EXPECT_FALSE(value->IsString());

    DestroyTest();
  }

  void RunBoolCreateTest() {
    HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateBool(true);
    EXPECT_TRUE(value.get());
    EXPECT_TRUE(value->IsBool());
    EXPECT_EQ(true, value->GetBoolValue());

    EXPECT_FALSE(value->IsUndefined());
    EXPECT_FALSE(value->IsArray());
    EXPECT_FALSE(value->IsDate());
    EXPECT_FALSE(value->IsDouble());
    EXPECT_FALSE(value->IsFunction());
    EXPECT_FALSE(value->IsInt());
    EXPECT_FALSE(value->IsUInt());
    EXPECT_FALSE(value->IsNull());
    EXPECT_FALSE(value->IsObject());
    EXPECT_FALSE(value->IsPromise());
    EXPECT_FALSE(value->IsString());

    DestroyTest();
  }

  void RunIntCreateTest() {
    HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateInt(12);
    EXPECT_TRUE(value.get());
    EXPECT_TRUE(value->IsInt());
    EXPECT_TRUE(value->IsUInt());
    EXPECT_TRUE(value->IsDouble());
    EXPECT_EQ(12, value->GetIntValue());
    EXPECT_EQ((uint32_t)12, value->GetUIntValue());
    EXPECT_EQ(12, value->GetDoubleValue());

    EXPECT_FALSE(value->IsUndefined());
    EXPECT_FALSE(value->IsArray());
    EXPECT_FALSE(value->IsBool());
    EXPECT_FALSE(value->IsDate());
    EXPECT_FALSE(value->IsFunction());
    EXPECT_FALSE(value->IsNull());
    EXPECT_FALSE(value->IsObject());
    EXPECT_FALSE(value->IsPromise());
    EXPECT_FALSE(value->IsString());

    DestroyTest();
  }

  void RunUIntCreateTest() {
    HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateUInt(12);
    EXPECT_TRUE(value.get());
    EXPECT_TRUE(value->IsInt());
    EXPECT_TRUE(value->IsUInt());
    EXPECT_TRUE(value->IsDouble());
    EXPECT_EQ(12, value->GetIntValue());
    EXPECT_EQ((uint32_t)12, value->GetUIntValue());
    EXPECT_EQ(12, value->GetDoubleValue());

    EXPECT_FALSE(value->IsUndefined());
    EXPECT_FALSE(value->IsArray());
    EXPECT_FALSE(value->IsBool());
    EXPECT_FALSE(value->IsDate());
    EXPECT_FALSE(value->IsFunction());
    EXPECT_FALSE(value->IsNull());
    EXPECT_FALSE(value->IsObject());
    EXPECT_FALSE(value->IsPromise());
    EXPECT_FALSE(value->IsString());

    DestroyTest();
  }

  void RunDoubleCreateTest() {
    HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateDouble(12.1223);
    EXPECT_TRUE(value.get());
    EXPECT_TRUE(value->IsDouble());
    EXPECT_EQ(12.1223, value->GetDoubleValue());

    EXPECT_FALSE(value->IsUndefined());
    EXPECT_FALSE(value->IsArray());
    EXPECT_FALSE(value->IsBool());
    EXPECT_FALSE(value->IsDate());
    EXPECT_FALSE(value->IsFunction());
    EXPECT_FALSE(value->IsInt());
    EXPECT_FALSE(value->IsUInt());
    EXPECT_FALSE(value->IsNull());
    EXPECT_FALSE(value->IsObject());
    EXPECT_FALSE(value->IsPromise());
    EXPECT_FALSE(value->IsString());

    DestroyTest();
  }

  void RunDateCreateTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    HoneycombTime date;
    date.year = 2200;
    date.month = 4;
#if !defined(OS_MAC)
    date.day_of_week = 5;
#endif
    date.day_of_month = 11;
    date.hour = 20;
    date.minute = 15;
    date.second = 42;

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateDate(HoneycombBaseTimeFrom(date));
    EXPECT_TRUE(value.get());
    EXPECT_TRUE(value->IsDate());
    EXPECT_EQ(date.GetTimeT(), HoneycombTimeFrom(value->GetDateValue()).GetTimeT());

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    EXPECT_FALSE(value->IsUndefined());
    EXPECT_FALSE(value->IsArray());
    EXPECT_FALSE(value->IsBool());
    EXPECT_FALSE(value->IsDouble());
    EXPECT_FALSE(value->IsFunction());
    EXPECT_FALSE(value->IsInt());
    EXPECT_FALSE(value->IsUInt());
    EXPECT_FALSE(value->IsObject());
    EXPECT_FALSE(value->IsNull());
    EXPECT_FALSE(value->IsPromise());
    EXPECT_FALSE(value->IsString());

    DestroyTest();
  }

  void RunStringCreateTest() {
    HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateString("My string");
    EXPECT_TRUE(value.get());
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ("My string", value->GetStringValue().ToString().c_str());

    EXPECT_FALSE(value->IsUndefined());
    EXPECT_FALSE(value->IsArray());
    EXPECT_FALSE(value->IsBool());
    EXPECT_FALSE(value->IsDate());
    EXPECT_FALSE(value->IsDouble());
    EXPECT_FALSE(value->IsFunction());
    EXPECT_FALSE(value->IsInt());
    EXPECT_FALSE(value->IsUInt());
    EXPECT_FALSE(value->IsNull());
    EXPECT_FALSE(value->IsPromise());
    EXPECT_FALSE(value->IsObject());

    DestroyTest();
  }

  void RunEmptyStringCreateTest() {
    HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateString(HoneycombString());
    EXPECT_TRUE(value.get());
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ("", value->GetStringValue().ToString().c_str());

    EXPECT_FALSE(value->IsUndefined());
    EXPECT_FALSE(value->IsArray());
    EXPECT_FALSE(value->IsBool());
    EXPECT_FALSE(value->IsDate());
    EXPECT_FALSE(value->IsDouble());
    EXPECT_FALSE(value->IsFunction());
    EXPECT_FALSE(value->IsInt());
    EXPECT_FALSE(value->IsUInt());
    EXPECT_FALSE(value->IsNull());
    EXPECT_FALSE(value->IsObject());
    EXPECT_FALSE(value->IsPromise());

    DestroyTest();
  }

  void RunArrayCreateTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateArray(2);
    EXPECT_TRUE(value.get());
    EXPECT_TRUE(value->IsArray());
    EXPECT_TRUE(value->IsObject());
    EXPECT_EQ(2, value->GetArrayLength());
    EXPECT_FALSE(value->HasValue(0));
    EXPECT_FALSE(value->HasValue(1));

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    EXPECT_FALSE(value->IsUndefined());
    EXPECT_FALSE(value->IsBool());
    EXPECT_FALSE(value->IsDate());
    EXPECT_FALSE(value->IsDouble());
    EXPECT_FALSE(value->IsFunction());
    EXPECT_FALSE(value->IsInt());
    EXPECT_FALSE(value->IsUInt());
    EXPECT_FALSE(value->IsNull());
    EXPECT_FALSE(value->IsPromise());
    EXPECT_FALSE(value->IsString());

    DestroyTest();
  }

  void RunArrayValueTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateArray(0);
    EXPECT_TRUE(value.get());
    EXPECT_TRUE(value->IsArray());
    EXPECT_EQ(0, value->GetArrayLength());

    // Test addng values.
    EXPECT_FALSE(value->HasValue(0));
    EXPECT_FALSE(value->HasValue(1));

    EXPECT_TRUE(value->SetValue(0, HoneycombV8Value::CreateInt(10)));
    EXPECT_FALSE(value->HasException());
    EXPECT_TRUE(value->HasValue(0));
    EXPECT_FALSE(value->HasValue(1));

    EXPECT_TRUE(value->GetValue(0)->IsInt());
    EXPECT_EQ(10, value->GetValue(0)->GetIntValue());
    EXPECT_FALSE(value->HasException());
    EXPECT_EQ(1, value->GetArrayLength());

    EXPECT_TRUE(value->SetValue(1, HoneycombV8Value::CreateInt(43)));
    EXPECT_FALSE(value->HasException());
    EXPECT_TRUE(value->HasValue(0));
    EXPECT_TRUE(value->HasValue(1));

    EXPECT_TRUE(value->GetValue(1)->IsInt());
    EXPECT_EQ(43, value->GetValue(1)->GetIntValue());
    EXPECT_FALSE(value->HasException());
    EXPECT_EQ(2, value->GetArrayLength());

    EXPECT_TRUE(value->DeleteValue(0));
    EXPECT_FALSE(value->HasValue(0));
    EXPECT_TRUE(value->HasValue(1));
    EXPECT_EQ(2, value->GetArrayLength());

    EXPECT_TRUE(value->DeleteValue(1));
    EXPECT_FALSE(value->HasValue(0));
    EXPECT_FALSE(value->HasValue(1));
    EXPECT_EQ(2, value->GetArrayLength());

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunArrayBufferTest() {
    class TestArrayBufferReleaseCallback
        : public HoneycombV8ArrayBufferReleaseCallback {
     public:
      TestArrayBufferReleaseCallback(bool* destructorCalled,
                                     bool* releaseBufferCalled)
          : destructorCalled_(destructorCalled),
            releaseBufferCalled_(releaseBufferCalled) {}

      ~TestArrayBufferReleaseCallback() { *destructorCalled_ = true; }

      void ReleaseBuffer(void* buffer) override {
        *releaseBufferCalled_ = true;
      }

      IMPLEMENT_REFCOUNTING(TestArrayBufferReleaseCallback);

     private:
      bool* destructorCalled_;
      bool* releaseBufferCalled_;
    };

    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    bool destructorCalled = false;
    bool releaseBufferCalled = false;
    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());
    {
      int static_data[16];
      HoneycombRefPtr<TestArrayBufferReleaseCallback> release_callback =
          new TestArrayBufferReleaseCallback(&destructorCalled,
                                             &releaseBufferCalled);
      HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateArrayBuffer(
          static_data, sizeof(static_data), release_callback);
      EXPECT_TRUE(value.get());
      EXPECT_TRUE(value->IsArrayBuffer());
      EXPECT_TRUE(value->IsObject());
      EXPECT_FALSE(value->HasValue(0));
      EXPECT_TRUE(value->GetArrayBufferReleaseCallback().get() != nullptr);
      EXPECT_TRUE(((TestArrayBufferReleaseCallback*)value
                       ->GetArrayBufferReleaseCallback()
                       .get()) == release_callback);

      // |Value| buffer is explicitly freed by NeuterArrayBuffer().
      EXPECT_FALSE(destructorCalled);
      EXPECT_FALSE(releaseBufferCalled);
      EXPECT_TRUE(value->NeuterArrayBuffer());
      EXPECT_TRUE(releaseBufferCalled);
    }
    // Exit the V8 context.
    EXPECT_TRUE(destructorCalled);
    EXPECT_TRUE(context->Exit());
    DestroyTest();
  }

  void RunArrayBufferValueTest() {
    class TestArrayBufferReleaseCallback
        : public HoneycombV8ArrayBufferReleaseCallback {
     public:
      TestArrayBufferReleaseCallback() {}

      ~TestArrayBufferReleaseCallback() {}

      void ReleaseBuffer(void* buffer) override {}

      IMPLEMENT_REFCOUNTING(TestArrayBufferReleaseCallback);
    };

    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    // Enter the V8 context.
    HoneycombRefPtr<HoneycombV8Value> value;

    HoneycombRefPtr<TestArrayBufferReleaseCallback> owner =
        new TestArrayBufferReleaseCallback();
    EXPECT_TRUE(context->Enter());
    int static_data[16];
    static_data[0] = 3;
    value =
        HoneycombV8Value::CreateArrayBuffer(static_data, sizeof(static_data), owner);

    HoneycombRefPtr<HoneycombV8Value> object = context->GetGlobal();
    EXPECT_TRUE(object.get());
    object->SetValue("arr", value, V8_PROPERTY_ATTRIBUTE_NONE);
    std::string test =
        "let data = new Int32Array(window.arr); data[0] += data.length";
    HoneycombRefPtr<HoneycombV8Value> retval;
    HoneycombRefPtr<HoneycombV8Exception> exception;
    EXPECT_TRUE(context->Eval(test, HoneycombString(), 0, retval, exception));
    if (exception.get()) {
      ADD_FAILURE() << exception->GetMessage().c_str();
    }

    EXPECT_TRUE(static_data[0] == 19);
    EXPECT_TRUE(value->GetArrayBufferReleaseCallback().get() != nullptr);
    EXPECT_TRUE(value->NeuterArrayBuffer());

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());
    DestroyTest();
  }

  void RunObjectCreateTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateObject(nullptr, nullptr);

    EXPECT_TRUE(value.get());
    EXPECT_TRUE(value->IsObject());
    EXPECT_FALSE(value->GetUserData().get());

    EXPECT_FALSE(value->IsUndefined());
    EXPECT_FALSE(value->IsArray());
    EXPECT_FALSE(value->IsBool());
    EXPECT_FALSE(value->IsDate());
    EXPECT_FALSE(value->IsDouble());
    EXPECT_FALSE(value->IsFunction());
    EXPECT_FALSE(value->IsInt());
    EXPECT_FALSE(value->IsUInt());
    EXPECT_FALSE(value->IsNull());
    EXPECT_FALSE(value->IsPromise());
    EXPECT_FALSE(value->IsString());

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunObjectUserDataTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    class UserData : public HoneycombBaseRefCounted {
     public:
      explicit UserData(int value) : value_(value) {}
      int value_;
      IMPLEMENT_REFCOUNTING(UserData);
    };

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateObject(nullptr, nullptr);
    EXPECT_TRUE(value.get());

    EXPECT_TRUE(value->SetUserData(new UserData(10)));

    HoneycombRefPtr<HoneycombBaseRefCounted> user_data = value->GetUserData();
    EXPECT_TRUE(user_data.get());
    UserData* user_data_impl = static_cast<UserData*>(user_data.get());
    EXPECT_EQ(10, user_data_impl->value_);

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunObjectAccessorTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    static const char* kName = "val";
    static const int kValue = 20;

    class Accessor : public HoneycombV8Accessor {
     public:
      Accessor() : value_(0) {}
      bool Get(const HoneycombString& name,
               const HoneycombRefPtr<HoneycombV8Value> object,
               HoneycombRefPtr<HoneycombV8Value>& retval,
               HoneycombString& exception) override {
        EXPECT_STREQ(kName, name.ToString().c_str());

        EXPECT_TRUE(object.get());
        EXPECT_TRUE(object->IsSame(object_));

        EXPECT_FALSE(retval.get());
        EXPECT_TRUE(exception.empty());

        got_get_.yes();
        retval = HoneycombV8Value::CreateInt(value_);
        EXPECT_EQ(kValue, retval->GetIntValue());
        return true;
      }

      bool Set(const HoneycombString& name,
               const HoneycombRefPtr<HoneycombV8Value> object,
               const HoneycombRefPtr<HoneycombV8Value> value,
               HoneycombString& exception) override {
        EXPECT_STREQ(kName, name.ToString().c_str());

        EXPECT_TRUE(object.get());
        EXPECT_TRUE(object->IsSame(object_));

        EXPECT_TRUE(value.get());
        EXPECT_TRUE(exception.empty());

        got_set_.yes();
        value_ = value->GetIntValue();
        EXPECT_EQ(kValue, value_);
        return true;
      }

      HoneycombRefPtr<HoneycombV8Value> object_;
      int value_;
      TrackCallback got_get_;
      TrackCallback got_set_;

      IMPLEMENT_REFCOUNTING(Accessor);
    };

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    Accessor* accessor = new Accessor;
    HoneycombRefPtr<HoneycombV8Accessor> accessorPtr(accessor);

    HoneycombRefPtr<HoneycombV8Value> object = HoneycombV8Value::CreateObject(accessor, nullptr);
    EXPECT_TRUE(object.get());
    accessor->object_ = object;

    EXPECT_FALSE(object->HasValue(kName));

    EXPECT_TRUE(object->SetValue(kName, V8_ACCESS_CONTROL_DEFAULT,
                                 V8_PROPERTY_ATTRIBUTE_NONE));
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(object->HasValue(kName));

    EXPECT_TRUE(object->SetValue(kName, HoneycombV8Value::CreateInt(kValue),
                                 V8_PROPERTY_ATTRIBUTE_NONE));
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(accessor->got_set_);
    EXPECT_EQ(kValue, accessor->value_);

    HoneycombRefPtr<HoneycombV8Value> val = object->GetValue(kName);
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(val.get());
    EXPECT_TRUE(accessor->got_get_);
    EXPECT_TRUE(val->IsInt());
    EXPECT_EQ(kValue, val->GetIntValue());

    accessor->object_ = nullptr;

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunObjectAccessorExceptionTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    static const char* kName = "val";
    static const char* kGetException = "My get exception";
    static const char* kSetException = "My set exception";
    static const char* kGetExceptionMsg = "Uncaught Error: My get exception";
    static const char* kSetExceptionMsg = "Uncaught Error: My set exception";

    class Accessor : public HoneycombV8Accessor {
     public:
      Accessor() {}
      bool Get(const HoneycombString& name,
               const HoneycombRefPtr<HoneycombV8Value> object,
               HoneycombRefPtr<HoneycombV8Value>& retval,
               HoneycombString& exception) override {
        got_get_.yes();
        exception = kGetException;
        return true;
      }

      bool Set(const HoneycombString& name,
               const HoneycombRefPtr<HoneycombV8Value> object,
               const HoneycombRefPtr<HoneycombV8Value> value,
               HoneycombString& exception) override {
        got_set_.yes();
        exception = kSetException;
        return true;
      }

      TrackCallback got_get_;
      TrackCallback got_set_;

      IMPLEMENT_REFCOUNTING(Accessor);
    };

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    HoneycombRefPtr<HoneycombV8Exception> exception;
    Accessor* accessor = new Accessor;
    HoneycombRefPtr<HoneycombV8Accessor> accessorPtr(accessor);

    HoneycombRefPtr<HoneycombV8Value> object = HoneycombV8Value::CreateObject(accessor, nullptr);
    EXPECT_TRUE(object.get());

    EXPECT_FALSE(object->HasValue(kName));

    EXPECT_TRUE(object->SetValue(kName, V8_ACCESS_CONTROL_DEFAULT,
                                 V8_PROPERTY_ATTRIBUTE_NONE));
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(object->HasValue(kName));

    EXPECT_FALSE(object->SetValue(kName, HoneycombV8Value::CreateInt(1),
                                  V8_PROPERTY_ATTRIBUTE_NONE));
    EXPECT_TRUE(object->HasException());
    EXPECT_TRUE(accessor->got_set_);
    exception = object->GetException();
    EXPECT_TRUE(exception.get());
    EXPECT_STREQ(kSetExceptionMsg, exception->GetMessage().ToString().c_str());

    EXPECT_TRUE(object->ClearException());
    EXPECT_FALSE(object->HasException());

    HoneycombRefPtr<HoneycombV8Value> val = object->GetValue(kName);
    EXPECT_FALSE(val.get());
    EXPECT_TRUE(object->HasException());
    EXPECT_TRUE(accessor->got_get_);
    exception = object->GetException();
    EXPECT_TRUE(exception.get());
    EXPECT_STREQ(kGetExceptionMsg, exception->GetMessage().ToString().c_str());

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunObjectAccessorFailTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    static const char* kName = "val";

    class Accessor : public HoneycombV8Accessor {
     public:
      Accessor() {}
      bool Get(const HoneycombString& name,
               const HoneycombRefPtr<HoneycombV8Value> object,
               HoneycombRefPtr<HoneycombV8Value>& retval,
               HoneycombString& exception) override {
        got_get_.yes();
        return false;
      }

      bool Set(const HoneycombString& name,
               const HoneycombRefPtr<HoneycombV8Value> object,
               const HoneycombRefPtr<HoneycombV8Value> value,
               HoneycombString& exception) override {
        got_set_.yes();
        return false;
      }

      TrackCallback got_get_;
      TrackCallback got_set_;

      IMPLEMENT_REFCOUNTING(Accessor);
    };

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    HoneycombRefPtr<HoneycombV8Exception> exception;
    Accessor* accessor = new Accessor;
    HoneycombRefPtr<HoneycombV8Accessor> accessorPtr(accessor);

    HoneycombRefPtr<HoneycombV8Value> object = HoneycombV8Value::CreateObject(accessor, nullptr);
    EXPECT_TRUE(object.get());

    EXPECT_FALSE(object->HasValue(kName));

    EXPECT_TRUE(object->SetValue(kName, V8_ACCESS_CONTROL_DEFAULT,
                                 V8_PROPERTY_ATTRIBUTE_NONE));
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(object->HasValue(kName));

    EXPECT_TRUE(object->SetValue(kName, HoneycombV8Value::CreateInt(1),
                                 V8_PROPERTY_ATTRIBUTE_NONE));
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(accessor->got_set_);

    HoneycombRefPtr<HoneycombV8Value> val = object->GetValue(kName);
    EXPECT_TRUE(val.get());
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(accessor->got_get_);
    EXPECT_TRUE(val->IsUndefined());

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunObjectAccessorReadOnlyTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    static const char* kName = "val";

    class Accessor : public HoneycombV8Accessor {
     public:
      Accessor() {}
      bool Get(const HoneycombString& name,
               const HoneycombRefPtr<HoneycombV8Value> object,
               HoneycombRefPtr<HoneycombV8Value>& retval,
               HoneycombString& exception) override {
        got_get_.yes();
        return true;
      }

      bool Set(const HoneycombString& name,
               const HoneycombRefPtr<HoneycombV8Value> object,
               const HoneycombRefPtr<HoneycombV8Value> value,
               HoneycombString& exception) override {
        got_set_.yes();
        return true;
      }

      TrackCallback got_get_;
      TrackCallback got_set_;

      IMPLEMENT_REFCOUNTING(Accessor);
    };

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    HoneycombRefPtr<HoneycombV8Exception> exception;
    Accessor* accessor = new Accessor;
    HoneycombRefPtr<HoneycombV8Accessor> accessorPtr(accessor);

    HoneycombRefPtr<HoneycombV8Value> object = HoneycombV8Value::CreateObject(accessor, nullptr);
    EXPECT_TRUE(object.get());

    EXPECT_FALSE(object->HasValue(kName));

    EXPECT_TRUE(object->SetValue(kName, V8_ACCESS_CONTROL_DEFAULT,
                                 V8_PROPERTY_ATTRIBUTE_READONLY));
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(object->HasValue(kName));

    EXPECT_TRUE(object->SetValue(kName, HoneycombV8Value::CreateInt(1),
                                 V8_PROPERTY_ATTRIBUTE_NONE));
    EXPECT_FALSE(object->HasException());
    EXPECT_FALSE(accessor->got_set_);

    HoneycombRefPtr<HoneycombV8Value> val = object->GetValue(kName);
    EXPECT_TRUE(val.get());
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(accessor->got_get_);
    EXPECT_TRUE(val->IsUndefined());

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunObjectInterceptorTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    static const char* kName1 = "val1";
    static const char* kName2 = "val2";
    static const char* kName3 = "val3";

    static const int kValue1 = 20;
    static const uint32_t kValue2 = 30u;
    static const char* kValue3 = "40";

    static const int kArray[] = {50, 60, 70};

    class Interceptor : public HoneycombV8Interceptor {
     public:
      Interceptor() : value1_(0), value2_(0u), array_() {}
      virtual bool Get(const HoneycombString& name,
                       const HoneycombRefPtr<HoneycombV8Value> object,
                       HoneycombRefPtr<HoneycombV8Value>& retval,
                       HoneycombString& exception) override {
        EXPECT_TRUE(name.ToString() == kName1 || name.ToString() == kName2 ||
                    name.ToString() == kName3);

        EXPECT_TRUE(object.get());
        EXPECT_TRUE(object->IsSame(object_));

        EXPECT_FALSE(retval.get());
        EXPECT_TRUE(exception.empty());

        got_get_byname_.yes();
        if (name.ToString() == kName1) {
          retval = HoneycombV8Value::CreateInt(value1_);
          EXPECT_EQ(kValue1, retval->GetIntValue());
        } else if (name.ToString() == kName2) {
          retval = HoneycombV8Value::CreateUInt(value2_);
          EXPECT_EQ(kValue2, retval->GetUIntValue());
        } else if (name.ToString() == kName3) {
          retval = HoneycombV8Value::CreateString(value3_);
          EXPECT_STREQ(kValue3, retval->GetStringValue().ToString().c_str());
        }
        return true;
      }

      virtual bool Get(int index,
                       const HoneycombRefPtr<HoneycombV8Value> object,
                       HoneycombRefPtr<HoneycombV8Value>& retval,
                       HoneycombString& exception) override {
        EXPECT_TRUE(index >= 0 && index < 3);

        EXPECT_TRUE(object.get());
        EXPECT_TRUE(object->IsSame(object_));

        EXPECT_FALSE(retval.get());
        EXPECT_TRUE(exception.empty());

        got_get_byindex_.yes();
        retval = HoneycombV8Value::CreateInt(array_[index]);
        EXPECT_EQ(kArray[index], retval->GetIntValue());
        return true;
      }

      virtual bool Set(const HoneycombString& name,
                       const HoneycombRefPtr<HoneycombV8Value> object,
                       const HoneycombRefPtr<HoneycombV8Value> value,
                       HoneycombString& exception) override {
        EXPECT_TRUE(name.ToString() == kName1 || name.ToString() == kName2 ||
                    name.ToString() == kName3);

        EXPECT_TRUE(object.get());
        EXPECT_TRUE(object->IsSame(object_));

        EXPECT_TRUE(value.get());
        EXPECT_TRUE(exception.empty());

        got_set_byname_.yes();
        if (name.ToString() == kName1) {
          value1_ = value->GetIntValue();
          EXPECT_EQ(kValue1, value1_);
        } else if (name.ToString() == kName2) {
          value2_ = value->GetUIntValue();
          EXPECT_EQ(kValue2, value2_);
        } else if (name.ToString() == kName3) {
          value3_ = value->GetStringValue();
          EXPECT_STREQ(kValue3, value3_.ToString().c_str());
        }
        return true;
      }

      virtual bool Set(int index,
                       const HoneycombRefPtr<HoneycombV8Value> object,
                       const HoneycombRefPtr<HoneycombV8Value> value,
                       HoneycombString& exception) override {
        EXPECT_TRUE(index >= 0 && index < 3);

        EXPECT_TRUE(object.get());
        EXPECT_TRUE(object->IsSame(object_));

        EXPECT_TRUE(value.get());
        EXPECT_TRUE(exception.empty());

        got_set_byindex_.yes();
        array_[index] = value->GetIntValue();
        EXPECT_EQ(array_[index], kArray[index]);
        return true;
      }

      HoneycombRefPtr<HoneycombV8Value> object_;
      int value1_;
      unsigned int value2_;
      HoneycombString value3_;
      int array_[3];

      TrackCallback got_get_byname_;
      TrackCallback got_get_byindex_;
      TrackCallback got_set_byname_;
      TrackCallback got_set_byindex_;

      IMPLEMENT_REFCOUNTING(Interceptor);
    };

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    Interceptor* interceptor = new Interceptor;
    HoneycombRefPtr<HoneycombV8Interceptor> interceptorPtr(interceptor);

    HoneycombRefPtr<HoneycombV8Value> object =
        HoneycombV8Value::CreateObject(nullptr, interceptor);
    EXPECT_TRUE(object.get());
    interceptor->object_ = object;

    EXPECT_FALSE(object->HasException());

    EXPECT_TRUE(object->SetValue(kName1, HoneycombV8Value::CreateInt(kValue1),
                                 V8_PROPERTY_ATTRIBUTE_NONE));
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(interceptor->got_set_byname_);
    interceptor->got_set_byname_.reset();

    EXPECT_TRUE(object->SetValue(kName2, HoneycombV8Value::CreateUInt(kValue2),
                                 V8_PROPERTY_ATTRIBUTE_NONE));
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(interceptor->got_set_byname_);
    interceptor->got_set_byname_.reset();

    EXPECT_TRUE(object->SetValue(kName3, HoneycombV8Value::CreateString(kValue3),
                                 V8_PROPERTY_ATTRIBUTE_NONE));
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(interceptor->got_set_byname_);
    interceptor->got_set_byname_.reset();

    EXPECT_EQ(kValue1, interceptor->value1_);
    EXPECT_EQ(kValue2, interceptor->value2_);
    EXPECT_STREQ(kValue3, interceptor->value3_.ToString().c_str());

    for (int i = 0; i < 3; ++i) {
      EXPECT_TRUE(object->SetValue(i, HoneycombV8Value::CreateInt(kArray[i])));
      EXPECT_FALSE(object->HasException());
      EXPECT_TRUE(interceptor->got_set_byindex_);
      interceptor->got_set_byindex_.reset();
      EXPECT_EQ(kArray[i], interceptor->array_[i]);
    }

    HoneycombRefPtr<HoneycombV8Value> val1 = object->GetValue(kName1);
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(val1.get());
    EXPECT_TRUE(interceptor->got_get_byname_);
    interceptor->got_get_byname_.reset();
    EXPECT_TRUE(val1->IsInt());
    EXPECT_EQ(kValue1, val1->GetIntValue());

    HoneycombRefPtr<HoneycombV8Value> val2 = object->GetValue(kName2);
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(val2.get());
    EXPECT_TRUE(interceptor->got_get_byname_);
    interceptor->got_get_byname_.reset();
    EXPECT_TRUE(val2->IsUInt());
    EXPECT_EQ(kValue2, val2->GetUIntValue());

    HoneycombRefPtr<HoneycombV8Value> val3 = object->GetValue(kName3);
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(val3.get());
    EXPECT_TRUE(interceptor->got_get_byname_);
    interceptor->got_get_byname_.reset();
    EXPECT_TRUE(val3->IsString());
    EXPECT_STREQ(kValue3, val3->GetStringValue().ToString().c_str());

    for (int i = 0; i < 3; ++i) {
      HoneycombRefPtr<HoneycombV8Value> val = object->GetValue(i);
      EXPECT_FALSE(object->HasException());
      EXPECT_TRUE(val.get());
      EXPECT_TRUE(interceptor->got_get_byindex_);
      interceptor->got_get_byname_.reset();
      EXPECT_EQ(kArray[i], val->GetIntValue());
    }

    interceptor->object_ = nullptr;

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunObjectInterceptorFailTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    static const char* kName = "val";
    static const int kIndex = 0;
    static const int kValue1 = 20;
    static const int kValue2 = 30;

    class Interceptor : public HoneycombV8Interceptor {
      typedef std::map<int, int> IntMap;
      typedef std::map<std::string, int> StringMap;

     public:
      Interceptor() {}
      virtual bool Get(const HoneycombString& name,
                       const HoneycombRefPtr<HoneycombV8Value> object,
                       HoneycombRefPtr<HoneycombV8Value>& retval,
                       HoneycombString& exception) override {
        got_get_byname_.yes();
        StringMap::iterator it = string_map_.find(name.ToString());
        if (it != string_map_.end()) {
          retval = HoneycombV8Value::CreateInt(it->second);
        }
        return true;
      }

      virtual bool Get(int index,
                       const HoneycombRefPtr<HoneycombV8Value> object,
                       HoneycombRefPtr<HoneycombV8Value>& retval,
                       HoneycombString& exception) override {
        got_get_byindex_.yes();
        IntMap::iterator it = int_map_.find(index);
        if (it != int_map_.end()) {
          retval = HoneycombV8Value::CreateInt(it->second);
        }
        return true;
      }

      virtual bool Set(const HoneycombString& name,
                       const HoneycombRefPtr<HoneycombV8Value> object,
                       const HoneycombRefPtr<HoneycombV8Value> value,
                       HoneycombString& exception) override {
        EXPECT_TRUE(value->IsInt());
        got_set_byname_.yes();
        string_map_[name.ToString()] = value->GetIntValue();
        return true;
      }

      virtual bool Set(int index,
                       const HoneycombRefPtr<HoneycombV8Value> object,
                       const HoneycombRefPtr<HoneycombV8Value> value,
                       HoneycombString& exception) override {
        EXPECT_TRUE(value->IsInt());
        got_set_byindex_.yes();
        int_map_[index] = value->GetIntValue();
        return true;
      }

      IntMap int_map_;
      StringMap string_map_;

      TrackCallback got_get_byname_;
      TrackCallback got_get_byindex_;
      TrackCallback got_set_byname_;
      TrackCallback got_set_byindex_;

      IMPLEMENT_REFCOUNTING(Interceptor);
    };

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    Interceptor* interceptor = new Interceptor;
    HoneycombRefPtr<HoneycombV8Interceptor> interceptorPtr(interceptor);

    HoneycombRefPtr<HoneycombV8Value> object =
        HoneycombV8Value::CreateObject(nullptr, interceptor);
    EXPECT_TRUE(object.get());

    EXPECT_FALSE(object->HasValue(kName));
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(interceptor->got_get_byname_);
    interceptor->got_get_byname_.reset();

    HoneycombRefPtr<HoneycombV8Value> val1 = object->GetValue(kName);
    EXPECT_TRUE(val1.get());
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(interceptor->got_get_byname_);
    EXPECT_TRUE(val1->IsUndefined());
    interceptor->got_get_byname_.reset();

    EXPECT_TRUE(object->SetValue(kName, HoneycombV8Value::CreateInt(kValue1),
                                 V8_PROPERTY_ATTRIBUTE_NONE));
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(interceptor->got_set_byname_);

    val1 = object->GetValue(kName);
    EXPECT_TRUE(val1.get());
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(interceptor->got_get_byname_);
    EXPECT_EQ(kValue1, val1->GetIntValue());

    EXPECT_FALSE(object->HasValue(kIndex));
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(interceptor->got_get_byindex_);
    interceptor->got_get_byindex_.reset();

    HoneycombRefPtr<HoneycombV8Value> val2 = object->GetValue(kIndex);
    EXPECT_TRUE(val2.get());
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(interceptor->got_get_byindex_);
    EXPECT_TRUE(val2->IsUndefined());
    interceptor->got_get_byindex_.reset();

    EXPECT_TRUE(object->SetValue(kIndex, HoneycombV8Value::CreateInt(kValue2)));
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(interceptor->got_set_byindex_);

    val2 = object->GetValue(kIndex);
    EXPECT_TRUE(val2.get());
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(interceptor->got_get_byindex_);
    EXPECT_EQ(kValue2, val2->GetIntValue());

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunObjectInterceptorExceptionTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();
    static const char* kName = "val";
    static const int kIndex = 1;

    static const char* kGetByNameException = "My get_byname exception";
    static const char* kGetByIndexException = "My get_byindex exception";
    static const char* kSetByNameException = "My set_byname exception";
    static const char* kSetByIndexException = "My set_byindex exception";

    static const char* kGetByNameExceptionMsg =
        "Uncaught Error: My get_byname exception";
    static const char* kGetByIndexExceptionMsg =
        "Uncaught Error: My get_byindex exception";
    static const char* kSetByNameExceptionMsg =
        "Uncaught Error: My set_byname exception";
    static const char* kSetByIndexExceptionMsg =
        "Uncaught Error: My set_byindex exception";

    class Interceptor : public HoneycombV8Interceptor {
     public:
      Interceptor() {}
      virtual bool Get(const HoneycombString& name,
                       const HoneycombRefPtr<HoneycombV8Value> object,
                       HoneycombRefPtr<HoneycombV8Value>& retval,
                       HoneycombString& exception) override {
        got_get_byname_.yes();
        exception = kGetByNameException;
        return true;
      }

      virtual bool Get(int index,
                       const HoneycombRefPtr<HoneycombV8Value> object,
                       HoneycombRefPtr<HoneycombV8Value>& retval,
                       HoneycombString& exception) override {
        got_get_byindex_.yes();
        exception = kGetByIndexException;
        return true;
      }

      virtual bool Set(const HoneycombString& name,
                       const HoneycombRefPtr<HoneycombV8Value> object,
                       const HoneycombRefPtr<HoneycombV8Value> value,
                       HoneycombString& exception) override {
        got_set_byname_.yes();
        exception = kSetByNameException;
        return true;
      }

      virtual bool Set(int index,
                       const HoneycombRefPtr<HoneycombV8Value> object,
                       const HoneycombRefPtr<HoneycombV8Value> value,
                       HoneycombString& exception) override {
        got_set_byindex_.yes();
        exception = kSetByIndexException;
        return true;
      }

      TrackCallback got_get_byname_;
      TrackCallback got_get_byindex_;
      TrackCallback got_set_byname_;
      TrackCallback got_set_byindex_;

      IMPLEMENT_REFCOUNTING(Interceptor);
    };

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    HoneycombRefPtr<HoneycombV8Exception> exception;
    Interceptor* interceptor = new Interceptor;
    HoneycombRefPtr<HoneycombV8Interceptor> interceptorPtr(interceptor);

    HoneycombRefPtr<HoneycombV8Value> object =
        HoneycombV8Value::CreateObject(nullptr, interceptor);
    EXPECT_TRUE(object.get());

    EXPECT_FALSE(object->SetValue(kName, HoneycombV8Value::CreateInt(1),
                                  V8_PROPERTY_ATTRIBUTE_NONE));
    EXPECT_TRUE(object->HasException());
    EXPECT_TRUE(interceptor->got_set_byname_);
    exception = object->GetException();
    EXPECT_TRUE(exception.get());
    EXPECT_STREQ(kSetByNameExceptionMsg,
                 exception->GetMessage().ToString().c_str());

    EXPECT_TRUE(object->ClearException());
    EXPECT_FALSE(object->HasException());

    HoneycombRefPtr<HoneycombV8Value> val1 = object->GetValue(kName);
    EXPECT_FALSE(val1.get());
    EXPECT_TRUE(object->HasException());
    EXPECT_TRUE(interceptor->got_get_byname_);
    exception = object->GetException();
    EXPECT_TRUE(exception.get());
    EXPECT_STREQ(kGetByNameExceptionMsg,
                 exception->GetMessage().ToString().c_str());

    EXPECT_TRUE(object->ClearException());
    EXPECT_FALSE(object->HasException());

    EXPECT_FALSE(object->SetValue(kIndex, HoneycombV8Value::CreateInt(1)));
    EXPECT_TRUE(object->HasException());
    EXPECT_TRUE(interceptor->got_set_byindex_);
    exception = object->GetException();
    EXPECT_TRUE(exception.get());
    EXPECT_STREQ(kSetByIndexExceptionMsg,
                 exception->GetMessage().ToString().c_str());

    EXPECT_TRUE(object->ClearException());
    EXPECT_FALSE(object->HasException());

    HoneycombRefPtr<HoneycombV8Value> val2 = object->GetValue(kIndex);
    EXPECT_FALSE(val2.get());
    EXPECT_TRUE(object->HasException());
    EXPECT_TRUE(interceptor->got_get_byindex_);
    exception = object->GetException();
    EXPECT_TRUE(exception.get());
    EXPECT_STREQ(kGetByIndexExceptionMsg,
                 exception->GetMessage().ToString().c_str());

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunObjectInterceptorAndAccessorTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();
    static const char* kInterceptorName = "val1";
    static const char* kAccessorName = "val2";

    static const int kInterceptorValue = 20;
    static const int kAccessorValue = 30;

    class Interceptor : public HoneycombV8Interceptor {
     public:
      Interceptor() {}
      virtual bool Get(const HoneycombString& name,
                       const HoneycombRefPtr<HoneycombV8Value> object,
                       HoneycombRefPtr<HoneycombV8Value>& retval,
                       HoneycombString& exception) override {
        EXPECT_FALSE(retval.get());
        got_get_byname_.yes();
        if (name.ToString() == kInterceptorName) {
          retval = HoneycombV8Value::CreateInt(kInterceptorValue);
        }
        return true;
      }

      virtual bool Get(int index,
                       const HoneycombRefPtr<HoneycombV8Value> object,
                       HoneycombRefPtr<HoneycombV8Value>& retval,
                       HoneycombString& exception) override {
        got_get_byindex_.yes();
        return true;
      }

      virtual bool Set(const HoneycombString& name,
                       const HoneycombRefPtr<HoneycombV8Value> object,
                       const HoneycombRefPtr<HoneycombV8Value> value,
                       HoneycombString& exception) override {
        got_set_byname_.yes();
        return true;
      }

      virtual bool Set(int index,
                       const HoneycombRefPtr<HoneycombV8Value> object,
                       const HoneycombRefPtr<HoneycombV8Value> value,
                       HoneycombString& exception) override {
        got_set_byindex_.yes();
        return true;
      }

      TrackCallback got_get_byname_;
      TrackCallback got_get_byindex_;
      TrackCallback got_set_byname_;
      TrackCallback got_set_byindex_;

      IMPLEMENT_REFCOUNTING(Interceptor);
    };

    class Accessor : public HoneycombV8Accessor {
     public:
      Accessor() {}
      virtual bool Get(const HoneycombString& name,
                       const HoneycombRefPtr<HoneycombV8Value> object,
                       HoneycombRefPtr<HoneycombV8Value>& retval,
                       HoneycombString& exception) override {
        got_get_.yes();
        retval = HoneycombV8Value::CreateInt(kAccessorValue);
        return true;
      }

      virtual bool Set(const HoneycombString& name,
                       const HoneycombRefPtr<HoneycombV8Value> object,
                       const HoneycombRefPtr<HoneycombV8Value> value,
                       HoneycombString& exception) override {
        got_set_.yes();
        return true;
      }

      TrackCallback got_get_;
      TrackCallback got_set_;

      IMPLEMENT_REFCOUNTING(Accessor);
    };

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    Interceptor* interceptor = new Interceptor;
    HoneycombRefPtr<HoneycombV8Interceptor> interceptorPtr(interceptor);

    Accessor* accessor = new Accessor;
    HoneycombRefPtr<HoneycombV8Accessor> accessorPtr(accessor);

    HoneycombRefPtr<HoneycombV8Value> object =
        HoneycombV8Value::CreateObject(accessor, interceptor);
    EXPECT_TRUE(object.get());

    // We register both names for accessor.
    EXPECT_TRUE(object->SetValue(kAccessorName, V8_ACCESS_CONTROL_DEFAULT,
                                 V8_PROPERTY_ATTRIBUTE_NONE));
    EXPECT_FALSE(object->HasException());

    EXPECT_TRUE(object->SetValue(kInterceptorName, V8_ACCESS_CONTROL_DEFAULT,
                                 V8_PROPERTY_ATTRIBUTE_NONE));
    EXPECT_FALSE(object->HasException());

    EXPECT_TRUE(object->SetValue(kAccessorName,
                                 HoneycombV8Value::CreateInt(kAccessorValue),
                                 V8_PROPERTY_ATTRIBUTE_NONE));
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(accessor->got_set_);
    accessor->got_set_.reset();
    EXPECT_TRUE(interceptor->got_set_byname_);
    interceptor->got_set_byname_.reset();

    EXPECT_TRUE(object->SetValue(kInterceptorName,
                                 HoneycombV8Value::CreateInt(kInterceptorValue),
                                 V8_PROPERTY_ATTRIBUTE_NONE));
    EXPECT_FALSE(object->HasException());
    EXPECT_TRUE(accessor->got_set_);
    accessor->got_set_.reset();
    EXPECT_TRUE(interceptor->got_set_byname_);
    interceptor->got_set_byname_.reset();

    // When interceptor returns nothing, accessor's getter is called.
    HoneycombRefPtr<HoneycombV8Value> val1 = object->GetValue(kAccessorName);
    EXPECT_TRUE(val1.get());
    EXPECT_TRUE(interceptor->got_get_byname_);
    interceptor->got_get_byname_.reset();
    EXPECT_TRUE(accessor->got_get_);
    accessor->got_get_.reset();
    EXPECT_EQ(kAccessorValue, val1->GetIntValue());

    // When interceptor returns value, accessor's getter is not called.
    HoneycombRefPtr<HoneycombV8Value> val2 = object->GetValue(kInterceptorName);
    EXPECT_TRUE(val2.get());
    EXPECT_TRUE(interceptor->got_get_byname_);
    EXPECT_FALSE(accessor->got_get_);
    EXPECT_EQ(kInterceptorValue, val2->GetIntValue());

    EXPECT_FALSE(interceptor->got_get_byindex_);
    EXPECT_FALSE(interceptor->got_set_byindex_);

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunObjectValueTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    static const char* kName = "test_arg";
    static const int kVal1 = 13;
    static const int kVal2 = 65;

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    HoneycombRefPtr<HoneycombV8Value> object = context->GetGlobal();
    EXPECT_TRUE(object.get());

    object->SetValue(kName, HoneycombV8Value::CreateInt(kVal1),
                     V8_PROPERTY_ATTRIBUTE_NONE);

    std::stringstream test;
    test << "if (window." << kName << " != " << kVal1 << ") throw 'Fail';\n"
         << "window." << kName << " = " << kVal2 << ";";

    HoneycombRefPtr<HoneycombV8Value> retval;
    HoneycombRefPtr<HoneycombV8Exception> exception;

    EXPECT_TRUE(context->Eval(test.str(), HoneycombString(), 0, retval, exception));
    if (exception.get()) {
      ADD_FAILURE() << exception->GetMessage().c_str();
    }

    HoneycombRefPtr<HoneycombV8Value> newval = object->GetValue(kName);
    EXPECT_TRUE(newval.get());
    EXPECT_TRUE(newval->IsInt());
    EXPECT_EQ(kVal2, newval->GetIntValue());

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunObjectValueReadOnlyTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    static const char* kName = "test_arg";
    static const int kVal1 = 13;
    static const int kVal2 = 65;

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    HoneycombRefPtr<HoneycombV8Value> object = context->GetGlobal();
    EXPECT_TRUE(object.get());

    object->SetValue(kName, HoneycombV8Value::CreateInt(kVal1),
                     V8_PROPERTY_ATTRIBUTE_READONLY);

    std::stringstream test;
    test << "if (window." << kName << " != " << kVal1 << ") throw 'Fail';\n"
         << "window." << kName << " = " << kVal2 << ";";

    HoneycombRefPtr<HoneycombV8Value> retval;
    HoneycombRefPtr<HoneycombV8Exception> exception;

    EXPECT_TRUE(context->Eval(test.str(), HoneycombString(), 0, retval, exception));
    if (exception.get()) {
      ADD_FAILURE() << exception->GetMessage().c_str();
    }

    HoneycombRefPtr<HoneycombV8Value> newval = object->GetValue(kName);
    EXPECT_TRUE(newval.get());
    EXPECT_TRUE(newval->IsInt());
    EXPECT_EQ(kVal1, newval->GetIntValue());

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunObjectValueEnumTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    static const char* kObjName = "test_obj";
    static const char* kArgName = "test_arg";

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    HoneycombRefPtr<HoneycombV8Value> object = context->GetGlobal();
    EXPECT_TRUE(object.get());

    HoneycombRefPtr<HoneycombV8Value> obj1 = HoneycombV8Value::CreateObject(nullptr, nullptr);
    object->SetValue(kObjName, obj1, V8_PROPERTY_ATTRIBUTE_NONE);

    obj1->SetValue(kArgName, HoneycombV8Value::CreateInt(0),
                   V8_PROPERTY_ATTRIBUTE_NONE);

    std::stringstream test;
    test << "for (var i in window." << kObjName
         << ") {\n"
            "window."
         << kObjName
         << "[i]++;\n"
            "}";

    HoneycombRefPtr<HoneycombV8Value> retval;
    HoneycombRefPtr<HoneycombV8Exception> exception;

    EXPECT_TRUE(context->Eval(test.str(), HoneycombString(), 0, retval, exception));
    if (exception.get()) {
      ADD_FAILURE() << exception->GetMessage().c_str();
    }

    HoneycombRefPtr<HoneycombV8Value> newval = obj1->GetValue(kArgName);
    EXPECT_TRUE(newval.get());
    EXPECT_TRUE(newval->IsInt());
    EXPECT_EQ(1, newval->GetIntValue());

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunObjectValueDontEnumTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    static const char* kObjName = "test_obj";
    static const char* kArgName = "test_arg";

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    HoneycombRefPtr<HoneycombV8Value> object = context->GetGlobal();
    EXPECT_TRUE(object.get());

    HoneycombRefPtr<HoneycombV8Value> obj1 = HoneycombV8Value::CreateObject(nullptr, nullptr);
    object->SetValue(kObjName, obj1, V8_PROPERTY_ATTRIBUTE_NONE);

    obj1->SetValue(kArgName, HoneycombV8Value::CreateInt(0),
                   V8_PROPERTY_ATTRIBUTE_DONTENUM);

    std::stringstream test;
    test << "for (var i in window." << kObjName
         << ") {\n"
            "window."
         << kObjName
         << "[i]++;\n"
            "}";

    HoneycombRefPtr<HoneycombV8Value> retval;
    HoneycombRefPtr<HoneycombV8Exception> exception;

    EXPECT_TRUE(context->Eval(test.str(), HoneycombString(), 0, retval, exception));
    if (exception.get()) {
      ADD_FAILURE() << exception->GetMessage().c_str();
    }

    HoneycombRefPtr<HoneycombV8Value> newval = obj1->GetValue(kArgName);
    EXPECT_TRUE(newval.get());
    EXPECT_TRUE(newval->IsInt());
    EXPECT_EQ(0, newval->GetIntValue());

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunObjectValueDeleteTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    static const char* kName = "test_arg";
    static const int kVal1 = 13;
    static const int kVal2 = 65;

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    HoneycombRefPtr<HoneycombV8Value> object = context->GetGlobal();
    EXPECT_TRUE(object.get());

    object->SetValue(kName, HoneycombV8Value::CreateInt(kVal1),
                     V8_PROPERTY_ATTRIBUTE_NONE);

    std::stringstream test;
    test << "if (window." << kName << " != " << kVal1 << ") throw 'Fail';\n"
         << "window." << kName << " = " << kVal2
         << ";\n"
            "delete window."
         << kName << ";";

    HoneycombRefPtr<HoneycombV8Value> retval;
    HoneycombRefPtr<HoneycombV8Exception> exception;

    EXPECT_TRUE(context->Eval(test.str(), HoneycombString(), 0, retval, exception));
    if (exception.get()) {
      ADD_FAILURE() << exception->GetMessage().c_str();
    }

    HoneycombRefPtr<HoneycombV8Value> newval = object->GetValue(kName);
    EXPECT_TRUE(newval.get());
    EXPECT_TRUE(newval->IsUndefined());
    EXPECT_FALSE(newval->IsInt());

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunObjectValueDontDeleteTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    static const char* kName = "test_arg";
    static const int kVal1 = 13;
    static const int kVal2 = 65;

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    HoneycombRefPtr<HoneycombV8Value> object = context->GetGlobal();
    EXPECT_TRUE(object.get());

    object->SetValue(kName, HoneycombV8Value::CreateInt(kVal1),
                     V8_PROPERTY_ATTRIBUTE_DONTDELETE);

    std::stringstream test;
    test << "if (window." << kName << " != " << kVal1 << ") throw 'Fail';\n"
         << "window." << kName << " = " << kVal2
         << ";\n"
            "delete window."
         << kName << ";";

    HoneycombRefPtr<HoneycombV8Value> retval;
    HoneycombRefPtr<HoneycombV8Exception> exception;

    EXPECT_TRUE(context->Eval(test.str(), HoneycombString(), 0, retval, exception));
    if (exception.get()) {
      ADD_FAILURE() << exception->GetMessage().c_str();
    }

    HoneycombRefPtr<HoneycombV8Value> newval = object->GetValue(kName);
    EXPECT_TRUE(newval.get());
    EXPECT_TRUE(newval->IsInt());
    EXPECT_EQ(kVal2, newval->GetIntValue());

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunObjectValueEmptyKeyTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    static const char* kName = "";
    static const int kVal = 13;

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    HoneycombRefPtr<HoneycombV8Value> object = context->GetGlobal();
    EXPECT_TRUE(object.get());

    EXPECT_FALSE(object->HasValue(kName));

    object->SetValue(kName, HoneycombV8Value::CreateInt(kVal),
                     V8_PROPERTY_ATTRIBUTE_NONE);
    EXPECT_TRUE(object->HasValue(kName));

    HoneycombRefPtr<HoneycombV8Value> newval = object->GetValue(kName);
    EXPECT_TRUE(newval.get());
    EXPECT_TRUE(newval->IsInt());
    EXPECT_EQ(kVal, newval->GetIntValue());

    EXPECT_TRUE(object->DeleteValue(kName));
    EXPECT_FALSE(object->HasValue(kName));

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunFunctionCreateTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    class Handler : public HoneycombV8Handler {
     public:
      Handler() {}
      bool Execute(const HoneycombString& name,
                   HoneycombRefPtr<HoneycombV8Value> object,
                   const HoneycombV8ValueList& arguments,
                   HoneycombRefPtr<HoneycombV8Value>& retval,
                   HoneycombString& exception) override {
        return false;
      }
      IMPLEMENT_REFCOUNTING(Handler);
    };

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateFunction("f", new Handler);

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    EXPECT_TRUE(value.get());
    EXPECT_TRUE(value->IsFunction());
    EXPECT_TRUE(value->IsObject());

    EXPECT_FALSE(value->IsUndefined());
    EXPECT_FALSE(value->IsArray());
    EXPECT_FALSE(value->IsBool());
    EXPECT_FALSE(value->IsDate());
    EXPECT_FALSE(value->IsDouble());
    EXPECT_FALSE(value->IsInt());
    EXPECT_FALSE(value->IsUInt());
    EXPECT_FALSE(value->IsNull());
    EXPECT_FALSE(value->IsPromise());
    EXPECT_FALSE(value->IsString());

    DestroyTest();
  }

  void RunFunctionHandlerTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    static const char* kFuncName = "myfunc";
    static const int kVal1 = 32;
    static const int kVal2 = 41;
    static const int kRetVal = 8;

    class Handler : public HoneycombV8Handler {
     public:
      Handler() {}
      bool Execute(const HoneycombString& name,
                   HoneycombRefPtr<HoneycombV8Value> object,
                   const HoneycombV8ValueList& arguments,
                   HoneycombRefPtr<HoneycombV8Value>& retval,
                   HoneycombString& exception) override {
        EXPECT_STREQ(kFuncName, name.ToString().c_str());
        EXPECT_TRUE(object->IsSame(object_));

        EXPECT_EQ((size_t)2, arguments.size());
        EXPECT_TRUE(arguments[0]->IsInt());
        EXPECT_EQ(kVal1, arguments[0]->GetIntValue());
        EXPECT_TRUE(arguments[1]->IsInt());
        EXPECT_EQ(kVal2, arguments[1]->GetIntValue());

        EXPECT_TRUE(exception.empty());

        retval = HoneycombV8Value::CreateInt(kRetVal);
        EXPECT_TRUE(retval.get());
        EXPECT_EQ(kRetVal, retval->GetIntValue());

        got_execute_.yes();
        return true;
      }

      HoneycombRefPtr<HoneycombV8Value> object_;
      TrackCallback got_execute_;

      IMPLEMENT_REFCOUNTING(Handler);
    };

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    Handler* handler = new Handler;
    HoneycombRefPtr<HoneycombV8Handler> handlerPtr(handler);

    HoneycombRefPtr<HoneycombV8Value> func = HoneycombV8Value::CreateFunction(kFuncName, handler);
    EXPECT_TRUE(func.get());

    HoneycombRefPtr<HoneycombV8Value> obj = HoneycombV8Value::CreateObject(nullptr, nullptr);
    EXPECT_TRUE(obj.get());
    handler->object_ = obj;

    HoneycombV8ValueList args;
    args.push_back(HoneycombV8Value::CreateInt(kVal1));
    args.push_back(HoneycombV8Value::CreateInt(kVal2));

    HoneycombRefPtr<HoneycombV8Value> retval = func->ExecuteFunction(obj, args);
    EXPECT_TRUE(handler->got_execute_);
    EXPECT_TRUE(retval.get());
    EXPECT_FALSE(func->HasException());
    EXPECT_TRUE(retval->IsInt());
    EXPECT_EQ(kRetVal, retval->GetIntValue());

    handler->object_ = nullptr;

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunFunctionHandlerExceptionTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    static const char* kException = "My error";
    static const char* kExceptionMsg = "Uncaught Error: My error";

    class Handler : public HoneycombV8Handler {
     public:
      Handler() {}
      bool Execute(const HoneycombString& name,
                   HoneycombRefPtr<HoneycombV8Value> object,
                   const HoneycombV8ValueList& arguments,
                   HoneycombRefPtr<HoneycombV8Value>& retval,
                   HoneycombString& exception) override {
        exception = kException;
        got_execute_.yes();
        return true;
      }

      TrackCallback got_execute_;

      IMPLEMENT_REFCOUNTING(Handler);
    };

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    Handler* handler = new Handler;
    HoneycombRefPtr<HoneycombV8Handler> handlerPtr(handler);

    HoneycombRefPtr<HoneycombV8Value> func = HoneycombV8Value::CreateFunction("myfunc", handler);
    EXPECT_TRUE(func.get());

    HoneycombV8ValueList args;

    HoneycombRefPtr<HoneycombV8Value> retval = func->ExecuteFunction(nullptr, args);
    EXPECT_TRUE(handler->got_execute_);
    EXPECT_FALSE(retval.get());
    EXPECT_TRUE(func->HasException());
    HoneycombRefPtr<HoneycombV8Exception> exception = func->GetException();
    EXPECT_TRUE(exception.get());
    EXPECT_STREQ(kExceptionMsg, exception->GetMessage().ToString().c_str());

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunFunctionHandlerFailTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    class Handler : public HoneycombV8Handler {
     public:
      Handler() {}
      bool Execute(const HoneycombString& name,
                   HoneycombRefPtr<HoneycombV8Value> object,
                   const HoneycombV8ValueList& arguments,
                   HoneycombRefPtr<HoneycombV8Value>& retval,
                   HoneycombString& exception) override {
        got_execute_.yes();
        return false;
      }

      TrackCallback got_execute_;

      IMPLEMENT_REFCOUNTING(Handler);
    };

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    Handler* handler = new Handler;
    HoneycombRefPtr<HoneycombV8Handler> handlerPtr(handler);

    HoneycombRefPtr<HoneycombV8Value> func = HoneycombV8Value::CreateFunction("myfunc", handler);
    EXPECT_TRUE(func.get());

    HoneycombV8ValueList args;

    HoneycombRefPtr<HoneycombV8Value> retval = func->ExecuteFunction(nullptr, args);
    EXPECT_TRUE(handler->got_execute_);
    EXPECT_TRUE(retval.get());
    EXPECT_FALSE(func->HasException());
    EXPECT_TRUE(retval->IsUndefined());

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunFunctionHandlerNoObjectTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    class Handler : public HoneycombV8Handler {
     public:
      Handler() {}
      bool Execute(const HoneycombString& name,
                   HoneycombRefPtr<HoneycombV8Value> object,
                   const HoneycombV8ValueList& arguments,
                   HoneycombRefPtr<HoneycombV8Value>& retval,
                   HoneycombString& exception) override {
        EXPECT_TRUE(object.get());
        HoneycombRefPtr<HoneycombV8Context> context = HoneycombV8Context::GetCurrentContext();
        EXPECT_TRUE(context.get());
        HoneycombRefPtr<HoneycombV8Value> global = context->GetGlobal();
        EXPECT_TRUE(global.get());
        EXPECT_TRUE(global->IsSame(object));

        got_execute_.yes();
        return true;
      }

      TrackCallback got_execute_;

      IMPLEMENT_REFCOUNTING(Handler);
    };

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    Handler* handler = new Handler;
    HoneycombRefPtr<HoneycombV8Handler> handlerPtr(handler);

    HoneycombRefPtr<HoneycombV8Value> func = HoneycombV8Value::CreateFunction("myfunc", handler);
    EXPECT_TRUE(func.get());

    HoneycombV8ValueList args;

    HoneycombRefPtr<HoneycombV8Value> retval = func->ExecuteFunction(nullptr, args);
    EXPECT_TRUE(handler->got_execute_);
    EXPECT_TRUE(retval.get());
    EXPECT_FALSE(func->HasException());

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunFunctionHandlerWithContextTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    class Handler : public HoneycombV8Handler {
     public:
      Handler() {}
      bool Execute(const HoneycombString& name,
                   HoneycombRefPtr<HoneycombV8Value> object,
                   const HoneycombV8ValueList& arguments,
                   HoneycombRefPtr<HoneycombV8Value>& retval,
                   HoneycombString& exception) override {
        HoneycombRefPtr<HoneycombV8Context> context = HoneycombV8Context::GetCurrentContext();
        EXPECT_TRUE(context.get());
        EXPECT_TRUE(context->IsSame(context_));
        got_execute_.yes();
        return true;
      }

      HoneycombRefPtr<HoneycombV8Context> context_;
      TrackCallback got_execute_;

      IMPLEMENT_REFCOUNTING(Handler);
    };

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    Handler* handler = new Handler;
    HoneycombRefPtr<HoneycombV8Handler> handlerPtr(handler);
    handler->context_ = context;

    HoneycombRefPtr<HoneycombV8Value> func = HoneycombV8Value::CreateFunction("myfunc", handler);
    EXPECT_TRUE(func.get());

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    HoneycombV8ValueList args;

    HoneycombRefPtr<HoneycombV8Value> retval =
        func->ExecuteFunctionWithContext(context, nullptr, args);
    EXPECT_TRUE(handler->got_execute_);
    EXPECT_TRUE(retval.get());
    EXPECT_FALSE(func->HasException());

    handler->context_ = nullptr;

    DestroyTest();
  }

  void RunFunctionHandlerEmptyStringTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    class Handler : public HoneycombV8Handler {
     public:
      Handler() {}
      bool Execute(const HoneycombString& name,
                   HoneycombRefPtr<HoneycombV8Value> object,
                   const HoneycombV8ValueList& arguments,
                   HoneycombRefPtr<HoneycombV8Value>& retval,
                   HoneycombString& exception) override {
        EXPECT_TRUE(object.get());
        HoneycombRefPtr<HoneycombV8Context> context = HoneycombV8Context::GetCurrentContext();
        EXPECT_TRUE(context.get());
        HoneycombRefPtr<HoneycombV8Value> global = context->GetGlobal();
        EXPECT_TRUE(global.get());
        EXPECT_TRUE(global->IsSame(object));

        EXPECT_EQ((size_t)1, arguments.size());
        EXPECT_TRUE(arguments[0]->IsString());
        EXPECT_STREQ("", arguments[0]->GetStringValue().ToString().c_str());

        retval = HoneycombV8Value::CreateString(HoneycombString());

        got_execute_.yes();
        return true;
      }

      TrackCallback got_execute_;

      IMPLEMENT_REFCOUNTING(Handler);
    };

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    Handler* handler = new Handler;
    HoneycombRefPtr<HoneycombV8Handler> handlerPtr(handler);

    HoneycombRefPtr<HoneycombV8Value> func = HoneycombV8Value::CreateFunction("myfunc", handler);
    EXPECT_TRUE(func.get());

    HoneycombV8ValueList args;
    args.push_back(HoneycombV8Value::CreateString(HoneycombString()));

    HoneycombRefPtr<HoneycombV8Value> retval = func->ExecuteFunction(nullptr, args);
    EXPECT_TRUE(handler->got_execute_);
    EXPECT_TRUE(retval.get());
    EXPECT_FALSE(func->HasException());

    EXPECT_TRUE(retval->IsString());
    EXPECT_STREQ("", retval->GetStringValue().ToString().c_str());

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunPromiseCreateTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreatePromise();

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    EXPECT_TRUE(value.get());
    EXPECT_TRUE(value->IsPromise());
    EXPECT_TRUE(value->IsObject());

    EXPECT_FALSE(value->IsUndefined());
    EXPECT_FALSE(value->IsArray());
    EXPECT_FALSE(value->IsBool());
    EXPECT_FALSE(value->IsDate());
    EXPECT_FALSE(value->IsDouble());
    EXPECT_FALSE(value->IsFunction());
    EXPECT_FALSE(value->IsInt());
    EXPECT_FALSE(value->IsUInt());
    EXPECT_FALSE(value->IsNull());
    EXPECT_FALSE(value->IsString());

    DestroyTest();
  }

  void RunPromiseResolveTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreatePromise();
    HoneycombRefPtr<HoneycombV8Value> obj = HoneycombV8Value::CreateObject(nullptr, nullptr);

    EXPECT_TRUE(value.get());
    EXPECT_TRUE(value->ResolvePromise(obj));

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunPromiseResolveNoArgumentTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreatePromise();

    EXPECT_TRUE(value.get());
    EXPECT_TRUE(value->ResolvePromise(nullptr));

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunPromiseResolveHandlerTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    static const char* kPromiseName = "myprom";
    static const char* kResolveName = "myresolve";
    static const char* kRejectName = "myreject";
    static const int kVal1 = 32;

    class Handler : public HoneycombV8Handler {
     public:
      Handler() {}
      bool Execute(const HoneycombString& name,
                   HoneycombRefPtr<HoneycombV8Value> object,
                   const HoneycombV8ValueList& arguments,
                   HoneycombRefPtr<HoneycombV8Value>& retval,
                   HoneycombString& exception) override {
        EXPECT_STREQ(kResolveName, name.ToString().c_str());
        EXPECT_EQ((size_t)1, arguments.size());
        EXPECT_TRUE(arguments[0]->IsInt());
        EXPECT_EQ(kVal1, arguments[0]->GetIntValue());

        got_execute_.yes();

        return false;
      }

      TrackCallback got_execute_;

      IMPLEMENT_REFCOUNTING(Handler);
    };

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    Handler* resolveHandler = new Handler;
    Handler* rejectHandler = new Handler;

    HoneycombRefPtr<HoneycombV8Handler> resolveHandlerPtr(resolveHandler);
    HoneycombRefPtr<HoneycombV8Handler> rejectHandlerPtr(rejectHandler);

    HoneycombRefPtr<HoneycombV8Value> resolveFunc =
        HoneycombV8Value::CreateFunction(kResolveName, resolveHandler);
    EXPECT_TRUE(resolveFunc.get());
    EXPECT_TRUE(context->GetGlobal()->SetValue(kResolveName, resolveFunc,
                                               V8_PROPERTY_ATTRIBUTE_NONE));

    HoneycombRefPtr<HoneycombV8Value> rejectFunc =
        HoneycombV8Value::CreateFunction(kRejectName, rejectHandler);
    EXPECT_TRUE(rejectFunc.get());
    EXPECT_TRUE(context->GetGlobal()->SetValue(kRejectName, rejectFunc,
                                               V8_PROPERTY_ATTRIBUTE_NONE));

    HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreatePromise();

    EXPECT_TRUE(value.get());
    EXPECT_TRUE(context->GetGlobal()->SetValue(kPromiseName, value,
                                               V8_PROPERTY_ATTRIBUTE_NONE));

    HoneycombRefPtr<HoneycombV8Value> retval;
    HoneycombRefPtr<HoneycombV8Exception> exception;

    std::stringstream test;
    test << "window." << kPromiseName << ".then(" << kResolveName << ").catch("
         << kRejectName << ")";

    EXPECT_TRUE(context->Eval(test.str(), HoneycombString(), 0, retval, exception));
    EXPECT_TRUE(retval.get());
    EXPECT_TRUE(retval->IsPromise());
    EXPECT_FALSE(exception.get());

    HoneycombRefPtr<HoneycombV8Value> arg = HoneycombV8Value::CreateInt(kVal1);
    EXPECT_TRUE(value->ResolvePromise(arg));

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    EXPECT_TRUE(resolveHandler->got_execute_);
    EXPECT_FALSE(rejectHandler->got_execute_);

    DestroyTest();
  }

  void RunPromiseRejectTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreatePromise();

    EXPECT_TRUE(value.get());
    EXPECT_TRUE(value->RejectPromise("Error: Unknown"));

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunPromiseRejectHandlerTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    static const char* kPromiseName = "myprom";
    static const char* kResolveName = "myresolve";
    static const char* kRejectName = "myreject";

    class Handler : public HoneycombV8Handler {
     public:
      Handler() {}
      bool Execute(const HoneycombString& name,
                   HoneycombRefPtr<HoneycombV8Value> object,
                   const HoneycombV8ValueList& arguments,
                   HoneycombRefPtr<HoneycombV8Value>& retval,
                   HoneycombString& exception) override {
        EXPECT_STREQ(kRejectName, name.ToString().c_str());
        EXPECT_EQ((size_t)1, arguments.size());
        EXPECT_TRUE(arguments[0]->IsObject());

        got_execute_.yes();

        return false;
      }

      TrackCallback got_execute_;

      IMPLEMENT_REFCOUNTING(Handler);
    };

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    Handler* resolveHandler = new Handler;
    Handler* rejectHandler = new Handler;

    HoneycombRefPtr<HoneycombV8Handler> resolveHandlerPtr(resolveHandler);
    HoneycombRefPtr<HoneycombV8Handler> rejectHandlerPtr(rejectHandler);

    HoneycombRefPtr<HoneycombV8Value> resolveFunc =
        HoneycombV8Value::CreateFunction(kResolveName, resolveHandler);
    EXPECT_TRUE(resolveFunc.get());
    EXPECT_TRUE(context->GetGlobal()->SetValue(kResolveName, resolveFunc,
                                               V8_PROPERTY_ATTRIBUTE_NONE));

    HoneycombRefPtr<HoneycombV8Value> rejectFunc =
        HoneycombV8Value::CreateFunction(kRejectName, rejectHandler);
    EXPECT_TRUE(rejectFunc.get());
    EXPECT_TRUE(context->GetGlobal()->SetValue(kRejectName, rejectFunc,
                                               V8_PROPERTY_ATTRIBUTE_NONE));

    HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreatePromise();

    EXPECT_TRUE(value.get());
    EXPECT_TRUE(context->GetGlobal()->SetValue(kPromiseName, value,
                                               V8_PROPERTY_ATTRIBUTE_NONE));

    HoneycombRefPtr<HoneycombV8Value> retval;
    HoneycombRefPtr<HoneycombV8Exception> exception;

    std::stringstream test;
    test << "window." << kPromiseName << ".then(" << kResolveName << ").catch("
         << kRejectName << ")";

    EXPECT_TRUE(context->Eval(test.str(), HoneycombString(), 0, retval, exception));
    EXPECT_TRUE(retval.get());
    EXPECT_TRUE(retval->IsPromise());
    EXPECT_FALSE(exception.get());

    EXPECT_TRUE(value->RejectPromise("Error: Unknown"));

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    EXPECT_FALSE(resolveHandler->got_execute_);
    EXPECT_TRUE(rejectHandler->got_execute_);

    DestroyTest();
  }

  void RunContextEvalTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    HoneycombRefPtr<HoneycombV8Value> retval;
    HoneycombRefPtr<HoneycombV8Exception> exception;

    EXPECT_TRUE(context->Eval("1+2", HoneycombString(), 0, retval, exception));
    EXPECT_TRUE(retval.get());
    EXPECT_TRUE(retval->IsInt());
    EXPECT_EQ(3, retval->GetIntValue());
    EXPECT_FALSE(exception.get());

    DestroyTest();
  }

  void RunContextEvalExceptionTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    HoneycombRefPtr<HoneycombV8Value> retval;
    HoneycombRefPtr<HoneycombV8Exception> exception;

    EXPECT_FALSE(
        context->Eval("\n\n\n1+foo", HoneycombString(), 0, retval, exception));
    EXPECT_FALSE(retval.get());
    EXPECT_TRUE(exception.get());
    EXPECT_EQ(4, exception->GetLineNumber());

    DestroyTest();
  }

  void RunContextEvalCspBypassUnsafeEval() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    HoneycombRefPtr<HoneycombV8Value> retval;
    HoneycombRefPtr<HoneycombV8Exception> exception;

    bool success =
        context->Eval("(document.getElementById('result').innerHTML)",
                      HoneycombString(), 0, retval, exception);
    if (exception.get()) {
      ADD_FAILURE() << exception->GetMessage().c_str();
      EXPECT_FALSE(success);
    }

    EXPECT_TRUE(success);
    EXPECT_TRUE(retval.get());
    if (retval.get()) {
      EXPECT_TRUE(retval->IsString());
      EXPECT_EQ(HoneycombString("CSP_BYPASSED"), retval->GetStringValue());
    }

    DestroyTest();
  }

  void RunContextEvalCspBypassSandbox() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    HoneycombRefPtr<HoneycombV8Value> retval;
    HoneycombRefPtr<HoneycombV8Exception> exception;

    bool success =
        context->Eval("(document.getElementById('result').innerHTML)",
                      HoneycombString(), 0, retval, exception);
    if (exception.get()) {
      ADD_FAILURE() << exception->GetMessage().c_str();
      EXPECT_FALSE(success);
    }

    EXPECT_TRUE(success);
    EXPECT_TRUE(retval.get());
    if (retval.get()) {
      EXPECT_TRUE(retval->IsString());
      EXPECT_EQ(HoneycombString("CSP_BYPASSED"), retval->GetStringValue());
    }

    DestroyTest();
  }

  void RunContextEnteredTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    HoneycombRefPtr<HoneycombV8Value> retval;
    HoneycombRefPtr<HoneycombV8Exception> exception;

    // Test value defined in OnContextCreated
    EXPECT_TRUE(context->Eval(
        "document.getElementById('f').contentWindow.v8_context_entered_test()",
        HoneycombString(), 0, retval, exception));
    if (exception.get()) {
      ADD_FAILURE() << exception->GetMessage().c_str();
    }

    EXPECT_TRUE(retval.get());
    EXPECT_TRUE(retval->IsInt());
    EXPECT_EQ(21, retval->GetIntValue());

    DestroyTest();
  }

  void RunBindingTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    HoneycombRefPtr<HoneycombV8Value> object = context->GetGlobal();
    EXPECT_TRUE(object.get());

    // Test value defined in OnContextCreated
    HoneycombRefPtr<HoneycombV8Value> value = object->GetValue("v8_binding_test");
    EXPECT_TRUE(value.get());
    EXPECT_TRUE(value->IsInt());
    EXPECT_EQ(12, value->GetIntValue());

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunStackTraceTest() {
    HoneycombRefPtr<HoneycombV8Context> context = GetContext();

    static const char* kFuncName = "myfunc";

    class Handler : public HoneycombV8Handler {
     public:
      Handler() {}
      bool Execute(const HoneycombString& name,
                   HoneycombRefPtr<HoneycombV8Value> object,
                   const HoneycombV8ValueList& arguments,
                   HoneycombRefPtr<HoneycombV8Value>& retval,
                   HoneycombString& exception) override {
        EXPECT_STREQ(kFuncName, name.ToString().c_str());

        stack_trace_ = HoneycombV8StackTrace::GetCurrent(10);

        retval = HoneycombV8Value::CreateInt(3);
        got_execute_.yes();
        return true;
      }

      TrackCallback got_execute_;
      HoneycombRefPtr<HoneycombV8StackTrace> stack_trace_;

      IMPLEMENT_REFCOUNTING(Handler);
    };

    // Enter the V8 context.
    EXPECT_TRUE(context->Enter());

    Handler* handler = new Handler;
    HoneycombRefPtr<HoneycombV8Handler> handlerPtr(handler);

    HoneycombRefPtr<HoneycombV8Value> func = HoneycombV8Value::CreateFunction(kFuncName, handler);
    EXPECT_TRUE(func.get());
    HoneycombRefPtr<HoneycombV8Value> obj = context->GetGlobal();
    EXPECT_TRUE(obj.get());
    obj->SetValue(kFuncName, func, V8_PROPERTY_ATTRIBUTE_NONE);

    HoneycombRefPtr<HoneycombV8Value> retval;
    HoneycombRefPtr<HoneycombV8Exception> exception;

    EXPECT_TRUE(
        context->Eval("function jsfunc() { return window.myfunc(); }\n"
                      "jsfunc();",
                      HoneycombString(), 0, retval, exception));
    EXPECT_TRUE(retval.get());
    EXPECT_TRUE(retval->IsInt());
    EXPECT_EQ(3, retval->GetIntValue());
    EXPECT_FALSE(exception.get());

    EXPECT_TRUE(handler->stack_trace_.get());
    EXPECT_EQ(2, handler->stack_trace_->GetFrameCount());

    HoneycombRefPtr<HoneycombV8StackFrame> frame;

    frame = handler->stack_trace_->GetFrame(0);
    EXPECT_TRUE(frame->GetScriptName().empty());
    EXPECT_TRUE(frame->GetScriptNameOrSourceURL().empty());
    EXPECT_STREQ("jsfunc", frame->GetFunctionName().ToString().c_str());
    EXPECT_EQ(1, frame->GetLineNumber());
    EXPECT_EQ(35, frame->GetColumn());
    EXPECT_TRUE(frame.get());
    EXPECT_FALSE(frame->IsEval());
    EXPECT_FALSE(frame->IsConstructor());

    frame = handler->stack_trace_->GetFrame(1);
    EXPECT_TRUE(frame->GetScriptName().empty());
    EXPECT_TRUE(frame->GetScriptNameOrSourceURL().empty());
    EXPECT_TRUE(frame->GetFunctionName().empty());
    EXPECT_EQ(2, frame->GetLineNumber());
    EXPECT_EQ(1, frame->GetColumn());
    EXPECT_TRUE(frame.get());
    EXPECT_FALSE(frame->IsEval());
    EXPECT_FALSE(frame->IsConstructor());

    // Exit the V8 context.
    EXPECT_TRUE(context->Exit());

    DestroyTest();
  }

  void RunOnUncaughtExceptionTest() {
    test_context_ = browser_->GetMainFrame()->GetV8Context();
    browser_->GetMainFrame()->ExecuteJavaScript(
        "window.setTimeout(test, 0)", browser_->GetMainFrame()->GetURL(), 0);
  }

  // Test execution of a native function when the extension is loaded.
  void RunExtensionTest() {
    std::string code =
        "native function v8_extension_test();"
        "v8_extension_test();";

    class Handler : public HoneycombV8Handler {
     public:
      Handler(TrackCallback* callback) : callback_(callback) {}

      bool Execute(const HoneycombString& name,
                   HoneycombRefPtr<HoneycombV8Value> object,
                   const HoneycombV8ValueList& arguments,
                   HoneycombRefPtr<HoneycombV8Value>& retval,
                   HoneycombString& exception) override {
        EXPECT_STREQ("v8_extension_test", name.ToString().c_str());
        callback_->yes();
        return true;
      }

      TrackCallback* callback_;

      IMPLEMENT_REFCOUNTING(Handler);
    };

    HoneycombRegisterExtension("v8/test-extension", code,
                         new Handler(&startup_test_success_));
  }

  void OnBrowserCreated(HoneycombRefPtr<ClientAppRenderer> app,
                        HoneycombRefPtr<HoneycombBrowser> browser,
                        HoneycombRefPtr<HoneycombDictionaryValue> extra_info) override {
    if (extra_info && extra_info->HasKey(kV8TestCmdKey)) {
      test_mode_ = static_cast<V8TestMode>(extra_info->GetInt(kV8TestCmdKey));
    }
    if (test_mode_ > V8TEST_NONE) {
      RunStartupTest();
    }
    if (test_mode_ == V8TEST_CONTEXT_EVAL_CSP_BYPASS_UNSAFE_EVAL ||
        test_mode_ == V8TEST_CONTEXT_EVAL_CSP_BYPASS_SANDBOX) {
      browser_ = browser;
    }
  }

  HoneycombRefPtr<HoneycombLoadHandler> GetLoadHandler(
      HoneycombRefPtr<ClientAppRenderer> app) override {
    if (test_mode_ == V8TEST_NONE) {
      return nullptr;
    }

    return this;
  }

  void OnLoadEnd(HoneycombRefPtr<HoneycombBrowser> browser,
                 HoneycombRefPtr<HoneycombFrame> frame,
                 int httpStatusCode) override {
    if (test_mode_ == V8TEST_ON_UNCAUGHT_EXCEPTION_DEV_TOOLS &&
        browser->IsPopup()) {
      DevToolsLoadHook(browser);
    }
  }

  void OnContextCreated(HoneycombRefPtr<ClientAppRenderer> app,
                        HoneycombRefPtr<HoneycombBrowser> browser,
                        HoneycombRefPtr<HoneycombFrame> frame,
                        HoneycombRefPtr<HoneycombV8Context> context) override {
    if (test_mode_ == V8TEST_NONE) {
      return;
    }

    if (test_mode_ == V8TEST_ON_UNCAUGHT_EXCEPTION_DEV_TOOLS) {
      if (!browser->IsPopup()) {
        app_ = app;
        browser_ = browser;
        test_context_ = context;
      }
      return;
    }

    app_ = app;
    browser_ = browser;

    std::string url = frame->GetURL();
    if (url == kV8ContextChildTestUrl) {
      // For V8TEST_CONTEXT_ENTERED
      class Handler : public HoneycombV8Handler {
       public:
        Handler() {}
        bool Execute(const HoneycombString& name,
                     HoneycombRefPtr<HoneycombV8Value> object,
                     const HoneycombV8ValueList& arguments,
                     HoneycombRefPtr<HoneycombV8Value>& retval,
                     HoneycombString& exception) override {
          // context for the sub-frame
          HoneycombRefPtr<HoneycombV8Context> context = HoneycombV8Context::GetCurrentContext();
          EXPECT_TRUE(context.get());

          // entered context should be the same as the main frame context
          HoneycombRefPtr<HoneycombV8Context> entered = HoneycombV8Context::GetEnteredContext();
          EXPECT_TRUE(entered.get());
          EXPECT_TRUE(entered->IsSame(context_));

          context_ = nullptr;
          retval = HoneycombV8Value::CreateInt(21);
          return true;
        }

        HoneycombRefPtr<HoneycombV8Context> context_;
        IMPLEMENT_REFCOUNTING(Handler);
      };

      Handler* handler = new Handler;
      HoneycombRefPtr<HoneycombV8Handler> handlerPtr(handler);

      // main frame context
      handler->context_ = GetContext();

      // Function that will be called from the parent frame context.
      HoneycombRefPtr<HoneycombV8Value> func =
          HoneycombV8Value::CreateFunction("v8_context_entered_test", handler);
      EXPECT_TRUE(func.get());

      HoneycombRefPtr<HoneycombV8Value> object = context->GetGlobal();
      EXPECT_TRUE(object.get());
      EXPECT_TRUE(object->SetValue("v8_context_entered_test", func,
                                   V8_PROPERTY_ATTRIBUTE_NONE));
    } else if (url == kV8ContextParentTestUrl) {
      // For V8TEST_CONTEXT_ENTERED. Do nothing.
      return;
    } else if (url == kV8BindingTestUrl) {
      // For V8TEST_BINDING
      HoneycombRefPtr<HoneycombV8Value> object = context->GetGlobal();
      EXPECT_TRUE(object.get());
      EXPECT_TRUE(object->SetValue("v8_binding_test", HoneycombV8Value::CreateInt(12),
                                   V8_PROPERTY_ATTRIBUTE_NONE));
    } else if (url == kV8HandlerCallOnReleasedContextUrl) {
      // For V8TEST_HANDLER_CALL_ON_RELEASED_CONTEXT
      class Handler : public HoneycombV8Handler {
       public:
        Handler(HoneycombRefPtr<V8RendererTest> renderer_test)
            : renderer_test_(renderer_test) {}

        bool Execute(const HoneycombString& name,
                     HoneycombRefPtr<HoneycombV8Value> object,
                     const HoneycombV8ValueList& arguments,
                     HoneycombRefPtr<HoneycombV8Value>& retval,
                     HoneycombString& exception) override {
          if (name == "notify_test_done") {
            HoneycombPostDelayedTask(TID_RENDERER,
                               base::BindOnce(&V8RendererTest::DestroyTest,
                                              renderer_test_.get()),
                               1000);
            return true;
          }

          return false;
        }

       private:
        HoneycombRefPtr<V8RendererTest> renderer_test_;
        IMPLEMENT_REFCOUNTING(Handler);
      };

      Handler* handler = new Handler(this);
      HoneycombRefPtr<HoneycombV8Handler> handlerPtr(handler);

      // Function that will be called from the parent frame context.
      HoneycombRefPtr<HoneycombV8Value> func =
          HoneycombV8Value::CreateFunction("notify_test_done", handler);
      EXPECT_TRUE(func.get());

      HoneycombRefPtr<HoneycombV8Value> object = context->GetGlobal();
      EXPECT_TRUE(object.get());
      EXPECT_TRUE(object->SetValue("notify_test_done", func,
                                   V8_PROPERTY_ATTRIBUTE_NONE));
    } else if (url == kV8HandlerCallOnReleasedContextChildUrl) {
      // For V8TEST_HANDLER_CALL_ON_RELEASED_CONTEXT
      class Handler : public HoneycombV8Handler {
       public:
        Handler() {}
        bool Execute(const HoneycombString& name,
                     HoneycombRefPtr<HoneycombV8Value> object,
                     const HoneycombV8ValueList& arguments,
                     HoneycombRefPtr<HoneycombV8Value>& retval,
                     HoneycombString& exception) override {
          if (name == "v8_context_is_alive") {
            retval = HoneycombV8Value::CreateBool(true);
            return true;
          }

          return false;
        }

        IMPLEMENT_REFCOUNTING(Handler);
      };

      Handler* handler = new Handler;
      HoneycombRefPtr<HoneycombV8Handler> handlerPtr(handler);

      // Function that will be called from the parent frame context.
      HoneycombRefPtr<HoneycombV8Value> func =
          HoneycombV8Value::CreateFunction("v8_context_is_alive", handler);
      EXPECT_TRUE(func.get());

      HoneycombRefPtr<HoneycombV8Value> object = context->GetGlobal();
      EXPECT_TRUE(object.get());
      EXPECT_TRUE(object->SetValue("v8_context_is_alive", func,
                                   V8_PROPERTY_ATTRIBUTE_NONE));
    }
  }

  void OnUncaughtException(HoneycombRefPtr<ClientAppRenderer> app,
                           HoneycombRefPtr<HoneycombBrowser> browser,
                           HoneycombRefPtr<HoneycombFrame> frame,
                           HoneycombRefPtr<HoneycombV8Context> context,
                           HoneycombRefPtr<HoneycombV8Exception> exception,
                           HoneycombRefPtr<HoneycombV8StackTrace> stackTrace) override {
    if (test_mode_ == V8TEST_NONE) {
      return;
    }

    if (test_mode_ == V8TEST_ON_UNCAUGHT_EXCEPTION ||
        test_mode_ == V8TEST_ON_UNCAUGHT_EXCEPTION_DEV_TOOLS) {
      EXPECT_TRUE(test_context_->IsSame(context));
      EXPECT_STREQ("Uncaught ReferenceError: asd is not defined",
                   exception->GetMessage().ToString().c_str());
      std::ostringstream stackFormatted;
      for (int i = 0; i < stackTrace->GetFrameCount(); ++i) {
        stackFormatted << "at "
                       << stackTrace->GetFrame(i)->GetFunctionName().ToString()
                       << "() in "
                       << stackTrace->GetFrame(i)->GetScriptName().ToString()
                       << " on line "
                       << stackTrace->GetFrame(i)->GetLineNumber() << "\n";
      }
      const char* stackFormattedShouldBe =
          "at test2() in https://tests/V8Test.OnUncaughtException on line 3\n"
          "at test() in https://tests/V8Test.OnUncaughtException on line 2\n";
      EXPECT_STREQ(stackFormattedShouldBe, stackFormatted.str().c_str());
      DestroyTest();
    }
  }

  bool OnProcessMessageReceived(HoneycombRefPtr<ClientAppRenderer> app,
                                HoneycombRefPtr<HoneycombBrowser> browser,
                                HoneycombRefPtr<HoneycombFrame> frame,
                                HoneycombProcessId source_process,
                                HoneycombRefPtr<HoneycombProcessMessage> message) override {
    if (test_mode_ == V8TEST_NONE) {
      return false;
    }

    const std::string& message_name = message->GetName();
    if (message_name == kV8RunTestMsg) {
      // Run the test asynchronously.
      HoneycombPostTask(TID_RENDERER, base::BindOnce(&V8RendererTest::RunTest, this));
      return true;
    }
    return false;
  }

  void DevToolsLoadHook(HoneycombRefPtr<HoneycombBrowser> browser) {
    EXPECT_TRUE(browser->IsPopup());
    HoneycombRefPtr<HoneycombFrame> frame = browser->GetMainFrame();
    HoneycombRefPtr<HoneycombV8Context> context = frame->GetV8Context();
    static const char* kFuncName = "DevToolsLoaded";

    class Handler : public HoneycombV8Handler {
     public:
      Handler() {}
      bool Execute(const HoneycombString& name,
                   HoneycombRefPtr<HoneycombV8Value> object,
                   const HoneycombV8ValueList& arguments,
                   HoneycombRefPtr<HoneycombV8Value>& retval,
                   HoneycombString& exception) override {
        EXPECT_STREQ(kFuncName, name.ToString().c_str());
        if (name == kFuncName) {
          EXPECT_TRUE(exception.empty());
          retval = HoneycombV8Value::CreateNull();
          EXPECT_TRUE(retval.get());
          renderer_test_->DevToolsLoaded(browser_);
          return true;
        }
        return false;
      }
      HoneycombRefPtr<V8RendererTest> renderer_test_;
      HoneycombRefPtr<HoneycombBrowser> browser_;
      IMPLEMENT_REFCOUNTING(Handler);
    };

    EXPECT_TRUE(context->Enter());
    Handler* handler = new Handler;
    handler->renderer_test_ = this;
    handler->browser_ = browser;
    HoneycombRefPtr<HoneycombV8Handler> handlerPtr(handler);
    HoneycombRefPtr<HoneycombV8Value> func = HoneycombV8Value::CreateFunction(kFuncName, handler);
    EXPECT_TRUE(func.get());
    EXPECT_TRUE(context->GetGlobal()->SetValue(kFuncName, func,
                                               V8_PROPERTY_ATTRIBUTE_NONE));
    EXPECT_TRUE(context->Exit());

    // Dismiss the DevTools window after 500ms. It would be better to hook the
    // DevTools JS to receive notification of when loading is complete but that
    // is no longer possible.
    HoneycombPostDelayedTask(
        TID_RENDERER,
        base::BindOnce(&HoneycombFrame::ExecuteJavaScript, frame.get(),
                       "window.DevToolsLoaded()", frame->GetURL(), 0),
        500);
  }

  void DevToolsLoaded(HoneycombRefPtr<HoneycombBrowser> browser) {
    EXPECT_TRUE(browser->IsPopup());
    // |browser_| will be nullptr if the DevTools window is opened in a separate
    // render process.
    const int other_browser_id =
        (browser_.get() ? browser_->GetIdentifier() : -1);
    EXPECT_NE(browser->GetIdentifier(), other_browser_id);

    // Close the DevTools window. This will trigger OnBeforeClose in the browser
    // process.
    HoneycombRefPtr<HoneycombV8Value> retval;
    HoneycombRefPtr<HoneycombV8Exception> exception;
    EXPECT_TRUE(browser->GetMainFrame()->GetV8Context()->Eval(
        "window.close()", HoneycombString(), 0, retval, exception));
  }

 protected:
  // Return from the test.
  void DestroyTest() {
    EXPECT_TRUE(HoneycombCurrentlyOn(TID_RENDERER));

    // Check if the test has failed.
    bool result = !TestFailed();

    // Return the result to the browser process.
    HoneycombRefPtr<HoneycombProcessMessage> return_msg =
        HoneycombProcessMessage::Create(kV8TestMsg);
    EXPECT_TRUE(return_msg->GetArgumentList()->SetBool(0, result));
    browser_->GetMainFrame()->SendProcessMessage(PID_BROWSER, return_msg);

    app_ = nullptr;
    browser_ = nullptr;
    test_context_ = nullptr;
    test_object_ = nullptr;
  }

  // Return the V8 context.
  HoneycombRefPtr<HoneycombV8Context> GetContext() {
    HoneycombRefPtr<HoneycombV8Context> context = browser_->GetMainFrame()->GetV8Context();
    EXPECT_TRUE(context.get());
    return context;
  }

  HoneycombRefPtr<ClientAppRenderer> app_;
  HoneycombRefPtr<HoneycombBrowser> browser_;
  V8TestMode test_mode_;

  HoneycombRefPtr<HoneycombV8Context> test_context_;
  HoneycombRefPtr<HoneycombV8Value> test_object_;

  // Used by startup tests to indicate success.
  TrackCallback startup_test_success_;

  IMPLEMENT_REFCOUNTING(V8RendererTest);
};

// Browser side.
class V8TestHandler : public TestHandler {
 public:
  V8TestHandler(V8TestMode test_mode, const char* test_url)
      : test_mode_(test_mode), test_url_(test_url) {}

  void RunTest() override {
    HoneycombRefPtr<HoneycombDictionaryValue> extra_info = HoneycombDictionaryValue::Create();
    extra_info->SetInt(kV8TestCmdKey, test_mode_);

    // Nested script tag forces creation of the V8 context.
    if (test_mode_ == V8TEST_CONTEXT_EVAL_CSP_BYPASS_UNSAFE_EVAL ||
        test_mode_ == V8TEST_CONTEXT_EVAL_CSP_BYPASS_SANDBOX) {
      std::string url;
      ResourceContent::HeaderMap headers;
      if (test_mode_ == V8TEST_CONTEXT_EVAL_CSP_BYPASS_UNSAFE_EVAL) {
        url = kV8ContextEvalCspBypassUnsafeEval;
        headers.insert(std::pair<std::string, std::string>(
            "Content-Security-Policy", "script-src 'self'"));
      } else if (test_mode_ == V8TEST_CONTEXT_EVAL_CSP_BYPASS_SANDBOX) {
        url = kV8ContextEvalCspBypassSandbox;
        headers.insert(std::pair<std::string, std::string>(
            "Content-Security-Policy", "sandbox"));
      } else {
        NOTREACHED();
      }
      AddResource(url,
                  "<html><body>" + url +
                      "<p id='result' style='display:none'>CSP_BYPASSED</p>"
                      "</body></html>",
                  "text/html", headers);
      CreateBrowser(test_url_, nullptr, extra_info);
    } else if (test_mode_ == V8TEST_CONTEXT_ENTERED) {
      AddResource(kV8ContextParentTestUrl,
                  "<html><body>"
                  "<script>var i = 0;</script><iframe src=\"" +
                      std::string(kV8ContextChildTestUrl) +
                      "\" id=\"f\"></iframe></body>"
                      "</html>",
                  "text/html");
      AddResource(kV8ContextChildTestUrl,
                  "<html><body>"
                  "<script>var i = 0;</script>CHILD</body></html>",
                  "text/html");
      CreateBrowser(kV8ContextParentTestUrl, nullptr, extra_info);
    } else if (test_mode_ == V8TEST_ON_UNCAUGHT_EXCEPTION ||
               test_mode_ == V8TEST_ON_UNCAUGHT_EXCEPTION_DEV_TOOLS) {
      AddResource(kV8OnUncaughtExceptionTestUrl,
                  "<html><body>"
                  "<h1>OnUncaughtException</h1>"
                  "<script>\n"
                  "function test(){ test2(); }\n"
                  "function test2(){ asd(); }\n"
                  "</script>\n"
                  "</body></html>\n",
                  "text/html");
      CreateBrowser(kV8OnUncaughtExceptionTestUrl, nullptr, extra_info);
    } else if (test_mode_ == V8TEST_HANDLER_CALL_ON_RELEASED_CONTEXT) {
      AddResource(kV8HandlerCallOnReleasedContextUrl,
                  "<html><body onload='createFrame()'>"
                  "(main)"
                  "<script>"
                  "function createFrame() {"
                  "  var el = document.createElement('iframe');"
                  "  el.id = 'child';"
                  "  el.src = '" +
                      std::string(kV8HandlerCallOnReleasedContextChildUrl) +
                      "';"
                      "  el.onload = function() {"
                      "    setTimeout(function() {"
                      "      try {"
                      "        el.contentWindow.removeMe();"
                      "        window.notify_test_done();"
                      "      } catch (e) { alert('Unit test error.\\n' + e); }"
                      "    }, 1000);"
                      "  };"
                      "  document.body.appendChild(el);"
                      "}"
                      ""
                      "function removeFrame(id) {"
                      "  var el = document.getElementById(id);"
                      "  if (el) { el.parentElement.removeChild(el); }"
                      "  else { alert('Error in test. No element \"' + id + "
                      "'\" found.'); }"
                      "}"
                      "</script>"
                      "</body></html>",
                  "text/html");
      AddResource(kV8HandlerCallOnReleasedContextChildUrl,
                  "<html><body>"
                  "(child)"
                  "<script>"
                  "try {"
                  "  if (!window.v8_context_is_alive()) {"
                  "    throw 'v8_context_is_alive returns non-true value.';"
                  "  }"
                  "} catch (e) {"
                  "  alert('Unit test error.\\n' + e);"
                  "}"
                  ""
                  "function removeMe() {"
                  "  var w = window;"
                  "  w.parent.removeFrame('child');"
                  "  return w.v8_context_is_alive();"
                  "}"
                  "</script>"
                  "</body></html>",
                  "text/html");
      CreateBrowser(kV8HandlerCallOnReleasedContextUrl, nullptr, extra_info);
    } else {
      EXPECT_TRUE(test_url_ != nullptr);
      AddResource(test_url_,
                  "<html><body>"
                  "<script>var i = 0;</script>TEST</body></html>",
                  "text/html");
      CreateBrowser(test_url_, nullptr, extra_info);
    }

    // Time out the test after a reasonable period of time.
    SetTestTimeout(test_mode_ == V8TEST_ON_UNCAUGHT_EXCEPTION_DEV_TOOLS ? 10000
                                                                        : 5000);
  }

  void OnBeforeClose(HoneycombRefPtr<HoneycombBrowser> browser) override {
    if (test_mode_ == V8TEST_ON_UNCAUGHT_EXCEPTION_DEV_TOOLS &&
        browser->IsPopup()) {
      // Generate the uncaught exception in the main browser. Use a 200ms delay
      // because there's a bit of a lag between destroying the DevToolsAgent and
      // re-registering for uncaught exceptions.
      GetBrowser()->GetMainFrame()->ExecuteJavaScript(
          "window.setTimeout(test, 200);",
          GetBrowser()->GetMainFrame()->GetURL(), 0);
    }

    TestHandler::OnBeforeClose(browser);
  }

  void OnLoadEnd(HoneycombRefPtr<HoneycombBrowser> browser,
                 HoneycombRefPtr<HoneycombFrame> frame,
                 int httpStatusCode) override {
    if (test_mode_ == V8TEST_ON_UNCAUGHT_EXCEPTION_DEV_TOOLS) {
      if (!browser->IsPopup()) {
        // Create the DevTools window.
        HoneycombWindowInfo windowInfo;
        HoneycombBrowserSettings settings;

#if defined(OS_WIN)
        windowInfo.SetAsPopup(browser->GetHost()->GetWindowHandle(),
                              "DevTools");
#endif

        browser->GetHost()->ShowDevTools(windowInfo, this, settings,
                                         HoneycombPoint());
      }
      return;
    }

    const std::string& url = frame->GetURL();
    if (url != kV8NavTestUrl && url != kV8ContextParentTestUrl &&
        url.find("https://tests/") != std::string::npos) {
      // Run the test.
      HoneycombRefPtr<HoneycombProcessMessage> return_msg =
          HoneycombProcessMessage::Create(kV8RunTestMsg);
      frame->SendProcessMessage(PID_RENDERER, return_msg);
    }
  }

  bool OnProcessMessageReceived(HoneycombRefPtr<HoneycombBrowser> browser,
                                HoneycombRefPtr<HoneycombFrame> frame,
                                HoneycombProcessId source_process,
                                HoneycombRefPtr<HoneycombProcessMessage> message) override {
    EXPECT_TRUE(browser.get());
    EXPECT_TRUE(frame.get());
    EXPECT_TRUE(frame->IsMain());
    EXPECT_EQ(PID_RENDERER, source_process);
    EXPECT_TRUE(message.get());
    EXPECT_TRUE(message->IsReadOnly());

    const std::string& message_name = message->GetName();
    EXPECT_STREQ(kV8TestMsg, message_name.c_str());

    got_message_.yes();

    if (message->GetArgumentList()->GetBool(0)) {
      got_success_.yes();
    }

    // Test is complete.
    DestroyTest();

    return true;
  }

  V8TestMode test_mode_;
  const char* test_url_;
  TrackCallback got_message_;
  TrackCallback got_success_;

  IMPLEMENT_REFCOUNTING(V8TestHandler);
};

}  // namespace

// Entry point for creating V8 renderer test objects.
// Called from client_app_delegates.cc.
void CreateV8RendererTests(ClientAppRenderer::DelegateSet& delegates) {
  delegates.insert(new V8RendererTest);
}

// Helpers for defining V8 tests.
#define V8_TEST_EX(name, test_mode, test_url)                                  \
  TEST(V8Test, name) {                                                         \
    HoneycombRefPtr<V8TestHandler> handler = new V8TestHandler(test_mode, test_url); \
    handler->ExecuteTest();                                                    \
    EXPECT_TRUE(handler->got_message_);                                        \
    EXPECT_TRUE(handler->got_success_);                                        \
    ReleaseAndWaitForDestructor(handler);                                      \
  }

#define V8_TEST(name, test_mode) V8_TEST_EX(name, test_mode, kV8TestUrl)

// Define the tests.
V8_TEST(NullCreate, V8TEST_NULL_CREATE)
V8_TEST(BoolCreate, V8TEST_BOOL_CREATE)
V8_TEST(IntCreate, V8TEST_INT_CREATE)
V8_TEST(UIntCreate, V8TEST_UINT_CREATE)
V8_TEST(DoubleCreate, V8TEST_DOUBLE_CREATE)
V8_TEST(DateCreate, V8TEST_DATE_CREATE)
V8_TEST(StringCreate, V8TEST_STRING_CREATE)
V8_TEST(EmptyStringCreate, V8TEST_EMPTY_STRING_CREATE)
V8_TEST(ArrayCreate, V8TEST_ARRAY_CREATE)
V8_TEST(ArrayValue, V8TEST_ARRAY_VALUE)
V8_TEST(ArrayBuffer, V8TEST_ARRAY_BUFFER)
V8_TEST(ArrayBufferValue, V8TEST_ARRAY_BUFFER_VALUE)
V8_TEST(ObjectCreate, V8TEST_OBJECT_CREATE)
V8_TEST(ObjectUserData, V8TEST_OBJECT_USERDATA)
V8_TEST(ObjectAccessor, V8TEST_OBJECT_ACCESSOR)
V8_TEST(ObjectAccessorException, V8TEST_OBJECT_ACCESSOR_EXCEPTION)
V8_TEST(ObjectAccessorFail, V8TEST_OBJECT_ACCESSOR_FAIL)
V8_TEST(ObjectAccessorReadOnly, V8TEST_OBJECT_ACCESSOR_READONLY)
V8_TEST(ObjectInterceptor, V8TEST_OBJECT_INTERCEPTOR)
V8_TEST(ObjectInterceptorFail, V8TEST_OBJECT_INTERCEPTOR_FAIL)
V8_TEST(ObjectInterceptorException, V8TEST_OBJECT_INTERCEPTOR_EXCEPTION)
V8_TEST(ObjectInterceptorAndAccessor, V8TEST_OBJECT_INTERCEPTOR_AND_ACCESSOR)
V8_TEST(ObjectValue, V8TEST_OBJECT_VALUE)
V8_TEST(ObjectValueReadOnly, V8TEST_OBJECT_VALUE_READONLY)
V8_TEST(ObjectValueEnum, V8TEST_OBJECT_VALUE_ENUM)
V8_TEST(ObjectValueDontEnum, V8TEST_OBJECT_VALUE_DONTENUM)
V8_TEST(ObjectValueDelete, V8TEST_OBJECT_VALUE_DELETE)
V8_TEST(ObjectValueDontDelete, V8TEST_OBJECT_VALUE_DONTDELETE)
V8_TEST(ObjectValueEmptyKey, V8TEST_OBJECT_VALUE_EMPTYKEY)
V8_TEST(FunctionCreate, V8TEST_FUNCTION_CREATE)
V8_TEST(FunctionHandler, V8TEST_FUNCTION_HANDLER)
V8_TEST(FunctionHandlerException, V8TEST_FUNCTION_HANDLER_EXCEPTION)
V8_TEST(FunctionHandlerFail, V8TEST_FUNCTION_HANDLER_FAIL)
V8_TEST(FunctionHandlerNoObject, V8TEST_FUNCTION_HANDLER_NO_OBJECT)
V8_TEST(FunctionHandlerWithContext, V8TEST_FUNCTION_HANDLER_WITH_CONTEXT)
V8_TEST(FunctionHandlerEmptyString, V8TEST_FUNCTION_HANDLER_EMPTY_STRING)
V8_TEST(PromiseCreate, V8TEST_PROMISE_CREATE)
V8_TEST(PromiseResolve, V8TEST_PROMISE_RESOLVE)
V8_TEST(PromiseResolveNoArgument, V8TEST_PROMISE_RESOLVE_NO_ARGUMENT)
V8_TEST(PromiseResolveHandler, V8TEST_PROMISE_RESOLVE_HANDLER)
V8_TEST(PromiseReject, V8TEST_PROMISE_REJECT)
V8_TEST(PromiseRejectHandler, V8TEST_PROMISE_REJECT_HANDLER)
V8_TEST(ContextEval, V8TEST_CONTEXT_EVAL)
V8_TEST(ContextEvalException, V8TEST_CONTEXT_EVAL_EXCEPTION)
V8_TEST_EX(ContextEvalCspBypassUnsafeEval,
           V8TEST_CONTEXT_EVAL_CSP_BYPASS_UNSAFE_EVAL,
           kV8ContextEvalCspBypassUnsafeEval)
V8_TEST_EX(ContextEvalCspBypassSandbox,
           V8TEST_CONTEXT_EVAL_CSP_BYPASS_SANDBOX,
           kV8ContextEvalCspBypassSandbox)
V8_TEST_EX(ContextEntered, V8TEST_CONTEXT_ENTERED, nullptr)
V8_TEST_EX(Binding, V8TEST_BINDING, kV8BindingTestUrl)
V8_TEST(StackTrace, V8TEST_STACK_TRACE)
V8_TEST(OnUncaughtException, V8TEST_ON_UNCAUGHT_EXCEPTION)
V8_TEST(OnUncaughtExceptionDevTools, V8TEST_ON_UNCAUGHT_EXCEPTION_DEV_TOOLS)
V8_TEST(Extension, V8TEST_EXTENSION)
V8_TEST_EX(HandlerCallOnReleasedContext,
           V8TEST_HANDLER_CALL_ON_RELEASED_CONTEXT,
           kV8HandlerCallOnReleasedContextUrl)
