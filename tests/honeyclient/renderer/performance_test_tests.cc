// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/honey_v8.h"
#include "tests/honeyclient/renderer/performance_test.h"
#include "tests/honeyclient/renderer/performance_test_setup.h"

namespace client {
namespace performance_test {

namespace {

// Test function implementations.

PERF_TEST_FUNC(V8NullCreate) {
  PERF_ITERATIONS_START()
  HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateNull();
  PERF_ITERATIONS_END()
}

PERF_TEST_FUNC(V8BoolCreate) {
  PERF_ITERATIONS_START()
  HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateBool(true);
  PERF_ITERATIONS_END()
}

PERF_TEST_FUNC(V8IntCreate) {
  PERF_ITERATIONS_START()
  HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateInt(-5);
  PERF_ITERATIONS_END()
}

PERF_TEST_FUNC(V8UIntCreate) {
  PERF_ITERATIONS_START()
  HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateUInt(10);
  PERF_ITERATIONS_END()
}

PERF_TEST_FUNC(V8DoubleCreate) {
  PERF_ITERATIONS_START()
  HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateDouble(12.432);
  PERF_ITERATIONS_END()
}

PERF_TEST_FUNC(V8DateCreate) {
  static honey_time_t time_exploded = {2012, 1, 0, 1};

  honey_basetime_t basetime;
  honey_time_to_basetime(&time_exploded, &basetime);
  HoneycombBaseTime time(basetime);

  PERF_ITERATIONS_START()
  HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateDate(time);
  PERF_ITERATIONS_END()
}

PERF_TEST_FUNC(V8StringCreate) {
  HoneycombString str = "test string";

  PERF_ITERATIONS_START()
  HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateString(str);
  PERF_ITERATIONS_END()
}

PERF_TEST_FUNC(V8ArrayCreate) {
  PERF_ITERATIONS_START()
  HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateArray(1);
  PERF_ITERATIONS_END()
}

PERF_TEST_FUNC(V8ArraySetValue) {
  HoneycombRefPtr<HoneycombV8Value> val = HoneycombV8Value::CreateBool(true);
  HoneycombRefPtr<HoneycombV8Value> array = HoneycombV8Value::CreateArray(1);
  array->SetValue(0, val);

  PERF_ITERATIONS_START()
  array->SetValue(0, val);
  PERF_ITERATIONS_END()
}

PERF_TEST_FUNC(V8ArrayGetValue) {
  HoneycombRefPtr<HoneycombV8Value> val = HoneycombV8Value::CreateBool(true);
  HoneycombRefPtr<HoneycombV8Value> array = HoneycombV8Value::CreateArray(1);
  array->SetValue(0, val);

  PERF_ITERATIONS_START()
  HoneycombRefPtr<HoneycombV8Value> ret = array->GetValue(0);
  PERF_ITERATIONS_END()
}

PERF_TEST_FUNC(V8FunctionCreate) {
  class Handler : public HoneycombV8Handler {
   public:
    Handler() {}
    virtual bool Execute(const HoneycombString& name,
                         HoneycombRefPtr<HoneycombV8Value> object,
                         const HoneycombV8ValueList& arguments,
                         HoneycombRefPtr<HoneycombV8Value>& retval,
                         HoneycombString& exception) override {
      return false;
    }
    IMPLEMENT_REFCOUNTING(Handler);
  };

  HoneycombString name = "name";
  HoneycombRefPtr<HoneycombV8Handler> handler = new Handler();

  PERF_ITERATIONS_START()
  HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateFunction(name, handler);
  PERF_ITERATIONS_END()
}

PERF_TEST_FUNC(V8FunctionExecute) {
  class Handler : public HoneycombV8Handler {
   public:
    Handler() {}
    virtual bool Execute(const HoneycombString& name,
                         HoneycombRefPtr<HoneycombV8Value> object,
                         const HoneycombV8ValueList& arguments,
                         HoneycombRefPtr<HoneycombV8Value>& retval,
                         HoneycombString& exception) override {
      return true;
    }
    IMPLEMENT_REFCOUNTING(Handler);
  };

  HoneycombString name = "name";
  HoneycombRefPtr<HoneycombV8Handler> handler = new Handler();
  HoneycombRefPtr<HoneycombV8Value> func = HoneycombV8Value::CreateFunction(name, handler);
  HoneycombRefPtr<HoneycombV8Value> obj = HoneycombV8Context::GetCurrentContext()->GetGlobal();
  HoneycombV8ValueList args;

  PERF_ITERATIONS_START()
  func->ExecuteFunction(obj, args);
  PERF_ITERATIONS_END()
}

PERF_TEST_FUNC(V8FunctionExecuteWithContext) {
  class Handler : public HoneycombV8Handler {
   public:
    Handler() {}
    virtual bool Execute(const HoneycombString& name,
                         HoneycombRefPtr<HoneycombV8Value> object,
                         const HoneycombV8ValueList& arguments,
                         HoneycombRefPtr<HoneycombV8Value>& retval,
                         HoneycombString& exception) override {
      return true;
    }
    IMPLEMENT_REFCOUNTING(Handler);
  };

  HoneycombString name = "name";
  HoneycombRefPtr<HoneycombV8Handler> handler = new Handler();
  HoneycombRefPtr<HoneycombV8Value> func = HoneycombV8Value::CreateFunction(name, handler);
  HoneycombRefPtr<HoneycombV8Context> context = HoneycombV8Context::GetCurrentContext();
  HoneycombRefPtr<HoneycombV8Value> obj = context->GetGlobal();
  HoneycombV8ValueList args;

  PERF_ITERATIONS_START()
  func->ExecuteFunctionWithContext(context, obj, args);
  PERF_ITERATIONS_END()
}

PERF_TEST_FUNC(V8ObjectCreate) {
  PERF_ITERATIONS_START()
  HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateObject(nullptr, nullptr);
  PERF_ITERATIONS_END()
}

PERF_TEST_FUNC(V8ObjectCreateWithAccessor) {
  class Accessor : public HoneycombV8Accessor {
   public:
    Accessor() {}
    virtual bool Get(const HoneycombString& name,
                     const HoneycombRefPtr<HoneycombV8Value> object,
                     HoneycombRefPtr<HoneycombV8Value>& retval,
                     HoneycombString& exception) override {
      return true;
    }
    virtual bool Set(const HoneycombString& name,
                     const HoneycombRefPtr<HoneycombV8Value> object,
                     const HoneycombRefPtr<HoneycombV8Value> value,
                     HoneycombString& exception) override {
      return true;
    }
    IMPLEMENT_REFCOUNTING(Accessor);
  };

  HoneycombRefPtr<HoneycombV8Accessor> accessor = new Accessor();

  PERF_ITERATIONS_START()
  HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateObject(accessor, nullptr);
  PERF_ITERATIONS_END()
}

PERF_TEST_FUNC(V8ObjectCreateWithInterceptor) {
  class Interceptor : public HoneycombV8Interceptor {
   public:
    Interceptor() {}
    virtual bool Get(const HoneycombString& name,
                     const HoneycombRefPtr<HoneycombV8Value> object,
                     HoneycombRefPtr<HoneycombV8Value>& retval,
                     HoneycombString& exception) override {
      return true;
    }
    virtual bool Get(int index,
                     const HoneycombRefPtr<HoneycombV8Value> object,
                     HoneycombRefPtr<HoneycombV8Value>& retval,
                     HoneycombString& exception) override {
      return true;
    }
    virtual bool Set(const HoneycombString& name,
                     const HoneycombRefPtr<HoneycombV8Value> object,
                     const HoneycombRefPtr<HoneycombV8Value> value,
                     HoneycombString& exception) override {
      return true;
    }
    virtual bool Set(int index,
                     const HoneycombRefPtr<HoneycombV8Value> object,
                     const HoneycombRefPtr<HoneycombV8Value> value,
                     HoneycombString& exception) override {
      return true;
    }
    IMPLEMENT_REFCOUNTING(Interceptor);
  };

  HoneycombRefPtr<HoneycombV8Interceptor> interceptor = new Interceptor();

  PERF_ITERATIONS_START()
  HoneycombRefPtr<HoneycombV8Value> value = HoneycombV8Value::CreateObject(nullptr, interceptor);
  PERF_ITERATIONS_END()
}

PERF_TEST_FUNC(V8ObjectSetValue) {
  HoneycombString name = "name";
  HoneycombRefPtr<HoneycombV8Value> val = HoneycombV8Value::CreateBool(true);
  HoneycombRefPtr<HoneycombV8Value> obj = HoneycombV8Value::CreateObject(nullptr, nullptr);
  obj->SetValue(name, val, V8_PROPERTY_ATTRIBUTE_NONE);

  PERF_ITERATIONS_START()
  obj->SetValue(name, val, V8_PROPERTY_ATTRIBUTE_NONE);
  PERF_ITERATIONS_END()
}

PERF_TEST_FUNC(V8ObjectGetValue) {
  HoneycombString name = "name";
  HoneycombRefPtr<HoneycombV8Value> val = HoneycombV8Value::CreateBool(true);
  HoneycombRefPtr<HoneycombV8Value> obj = HoneycombV8Value::CreateObject(nullptr, nullptr);
  obj->SetValue(name, val, V8_PROPERTY_ATTRIBUTE_NONE);

  PERF_ITERATIONS_START()
  HoneycombRefPtr<HoneycombV8Value> ret = obj->GetValue(name);
  PERF_ITERATIONS_END()
}

PERF_TEST_FUNC(V8ObjectSetValueWithAccessor) {
  class Accessor : public HoneycombV8Accessor {
   public:
    Accessor() {}
    virtual bool Get(const HoneycombString& name,
                     const HoneycombRefPtr<HoneycombV8Value> object,
                     HoneycombRefPtr<HoneycombV8Value>& retval,
                     HoneycombString& exception) override {
      return true;
    }
    virtual bool Set(const HoneycombString& name,
                     const HoneycombRefPtr<HoneycombV8Value> object,
                     const HoneycombRefPtr<HoneycombV8Value> value,
                     HoneycombString& exception) override {
      val_ = value;
      return true;
    }
    HoneycombRefPtr<HoneycombV8Value> val_;
    IMPLEMENT_REFCOUNTING(Accessor);
  };

  HoneycombRefPtr<HoneycombV8Accessor> accessor = new Accessor();

  HoneycombString name = "name";
  HoneycombRefPtr<HoneycombV8Value> val = HoneycombV8Value::CreateBool(true);
  HoneycombRefPtr<HoneycombV8Value> obj = HoneycombV8Value::CreateObject(accessor, nullptr);
  obj->SetValue(name, V8_ACCESS_CONTROL_DEFAULT, V8_PROPERTY_ATTRIBUTE_NONE);
  obj->SetValue(name, val, V8_PROPERTY_ATTRIBUTE_NONE);

  PERF_ITERATIONS_START()
  obj->SetValue(name, val, V8_PROPERTY_ATTRIBUTE_NONE);
  PERF_ITERATIONS_END()
}

PERF_TEST_FUNC(V8ObjectGetValueWithAccessor) {
  class Accessor : public HoneycombV8Accessor {
   public:
    Accessor() : val_(HoneycombV8Value::CreateBool(true)) {}
    virtual bool Get(const HoneycombString& name,
                     const HoneycombRefPtr<HoneycombV8Value> object,
                     HoneycombRefPtr<HoneycombV8Value>& retval,
                     HoneycombString& exception) override {
      retval = val_;
      return true;
    }
    virtual bool Set(const HoneycombString& name,
                     const HoneycombRefPtr<HoneycombV8Value> object,
                     const HoneycombRefPtr<HoneycombV8Value> value,
                     HoneycombString& exception) override {
      return true;
    }
    HoneycombRefPtr<HoneycombV8Value> val_;
    IMPLEMENT_REFCOUNTING(Accessor);
  };

  HoneycombRefPtr<HoneycombV8Accessor> accessor = new Accessor();

  HoneycombString name = "name";
  HoneycombRefPtr<HoneycombV8Value> val = HoneycombV8Value::CreateBool(true);
  HoneycombRefPtr<HoneycombV8Value> obj = HoneycombV8Value::CreateObject(accessor, nullptr);
  obj->SetValue(name, V8_ACCESS_CONTROL_DEFAULT, V8_PROPERTY_ATTRIBUTE_NONE);
  obj->SetValue(name, val, V8_PROPERTY_ATTRIBUTE_NONE);

  PERF_ITERATIONS_START()
  HoneycombRefPtr<HoneycombV8Value> ret = obj->GetValue(name);
  PERF_ITERATIONS_END()
}

PERF_TEST_FUNC(V8ArrayBufferCreate) {
  class ReleaseCallback : public HoneycombV8ArrayBufferReleaseCallback {
   public:
    void ReleaseBuffer(void* buffer) override { std::free(buffer); }
    IMPLEMENT_REFCOUNTING(ReleaseCallback);
  };

  size_t len = 1;
  size_t byte_len = len * sizeof(float);
  HoneycombRefPtr<HoneycombV8ArrayBufferReleaseCallback> callback = new ReleaseCallback();

  PERF_ITERATIONS_START()
  float* buffer = (float*)std::malloc(byte_len);
  HoneycombRefPtr<HoneycombV8Value> ret =
      HoneycombV8Value::CreateArrayBuffer(buffer, byte_len, callback);
  PERF_ITERATIONS_END()
}

PERF_TEST_FUNC(V8ContextEnterExit) {
  HoneycombRefPtr<HoneycombV8Context> context = HoneycombV8Context::GetCurrentContext();

  PERF_ITERATIONS_START()
  context->Enter();
  context->Exit();
  PERF_ITERATIONS_END()
}

PERF_TEST_FUNC(V8ContextEval) {
  HoneycombRefPtr<HoneycombV8Context> context = HoneycombV8Context::GetCurrentContext();
  HoneycombString jsCode = "var i = 0;";
  HoneycombRefPtr<HoneycombV8Value> retval;
  HoneycombRefPtr<HoneycombV8Exception> exception;

  PERF_ITERATIONS_START()
  context->Eval(jsCode, HoneycombString(), 0, retval, exception);
  PERF_ITERATIONS_END()
}

}  // namespace

// Test function entries.

const PerfTestEntry kPerfTests[] = {
    PERF_TEST_ENTRY(V8NullCreate),
    PERF_TEST_ENTRY(V8BoolCreate),
    PERF_TEST_ENTRY(V8IntCreate),
    PERF_TEST_ENTRY(V8UIntCreate),
    PERF_TEST_ENTRY(V8DoubleCreate),
    PERF_TEST_ENTRY(V8DateCreate),
    PERF_TEST_ENTRY(V8StringCreate),
    PERF_TEST_ENTRY(V8ArrayCreate),
    PERF_TEST_ENTRY(V8ArraySetValue),
    PERF_TEST_ENTRY(V8ArrayGetValue),
    PERF_TEST_ENTRY(V8FunctionCreate),
    PERF_TEST_ENTRY(V8FunctionExecute),
    PERF_TEST_ENTRY(V8FunctionExecuteWithContext),
    PERF_TEST_ENTRY(V8ObjectCreate),
    PERF_TEST_ENTRY(V8ObjectCreateWithAccessor),
    PERF_TEST_ENTRY(V8ObjectCreateWithInterceptor),
    PERF_TEST_ENTRY(V8ObjectSetValue),
    PERF_TEST_ENTRY(V8ObjectGetValue),
    PERF_TEST_ENTRY(V8ObjectSetValueWithAccessor),
    PERF_TEST_ENTRY(V8ObjectGetValueWithAccessor),
    PERF_TEST_ENTRY(V8ArrayBufferCreate),
    PERF_TEST_ENTRY(V8ContextEnterExit),
    PERF_TEST_ENTRY(V8ContextEval),
};

const int kPerfTestsCount = (sizeof(kPerfTests) / sizeof(kPerfTests[0]));

}  // namespace performance_test
}  // namespace client
