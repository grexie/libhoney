// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeyclient/renderer/performance_test.h"

#include <algorithm>
#include <string>

#include "include/base/honey_logging.h"
#include "include/wrapper/honey_stream_resource_handler.h"
#include "tests/honeyclient/renderer/performance_test_setup.h"

namespace client {
namespace performance_test {

// Use more interations for a Release build.
#if DCHECK_IS_ON()
const int kDefaultIterations = 100000;
#else
const int kDefaultIterations = 10000;
#endif

namespace {

const char kGetPerfTests[] = "GetPerfTests";
const char kRunPerfTest[] = "RunPerfTest";
const char kPerfTestReturnValue[] = "PerfTestReturnValue";

class V8Handler : public HoneycombV8Handler {
 public:
  V8Handler() {}

  virtual bool Execute(const HoneycombString& name,
                       HoneycombRefPtr<HoneycombV8Value> object,
                       const HoneycombV8ValueList& arguments,
                       HoneycombRefPtr<HoneycombV8Value>& retval,
                       HoneycombString& exception) override {
    if (name == kRunPerfTest) {
      if (arguments.size() == 1 && arguments[0]->IsString()) {
        // Run the specified perf test.
        bool found = false;

        std::string test = arguments[0]->GetStringValue();
        for (int i = 0; i < kPerfTestsCount; ++i) {
          if (test == kPerfTests[i].name) {
            // Execute the test.
            int64_t delta = kPerfTests[i].test(kPerfTests[i].iterations);

            retval = HoneycombV8Value::CreateInt(static_cast<int32_t>(delta));
            found = true;
            break;
          }
        }

        if (!found) {
          std::string msg = "Unknown test: ";
          msg.append(test);
          exception = msg;
        }
      } else {
        exception = "Invalid function parameters";
      }
    } else if (name == kGetPerfTests) {
      // Retrieve the list of perf tests.
      retval = HoneycombV8Value::CreateArray(kPerfTestsCount);
      for (int i = 0; i < kPerfTestsCount; ++i) {
        HoneycombRefPtr<HoneycombV8Value> val = HoneycombV8Value::CreateArray(2);
        val->SetValue(0, HoneycombV8Value::CreateString(kPerfTests[i].name));
        val->SetValue(1, HoneycombV8Value::CreateUInt(kPerfTests[i].iterations));
        retval->SetValue(i, val);
      }
    } else if (name == kPerfTestReturnValue) {
      if (arguments.size() == 0) {
        retval = HoneycombV8Value::CreateInt(1);
      } else if (arguments.size() == 1 && arguments[0]->IsInt()) {
        int32_t type = arguments[0]->GetIntValue();
        switch (type) {
          case 0:
            retval = HoneycombV8Value::CreateUndefined();
            break;
          case 1:
            retval = HoneycombV8Value::CreateNull();
            break;
          case 2:
            retval = HoneycombV8Value::CreateBool(true);
            break;
          case 3:
            retval = HoneycombV8Value::CreateInt(1);
            break;
          case 4:
            retval = HoneycombV8Value::CreateUInt(1);
            break;
          case 5:
            retval = HoneycombV8Value::CreateDouble(1.234);
            break;
          case 6:
            retval = HoneycombV8Value::CreateDate(HoneycombBaseTime::Now());
            break;
          case 7:
            retval = HoneycombV8Value::CreateString("Hello, world!");
            break;
          case 8:
            retval = HoneycombV8Value::CreateObject(nullptr, nullptr);
            break;
          case 9:
            retval = HoneycombV8Value::CreateArray(8);
            break;
          case 10:
            // retval = HoneycombV8Value::CreateFunction(...);
            exception = "Not implemented";
            break;
          default:
            exception = "Not supported";
        }
      }
    }

    return true;
  }

 private:
  IMPLEMENT_REFCOUNTING(V8Handler);
};

// Handle bindings in the render process.
class RenderDelegate : public ClientAppRenderer::Delegate {
 public:
  RenderDelegate() {}

  virtual void OnContextCreated(HoneycombRefPtr<ClientAppRenderer> app,
                                HoneycombRefPtr<HoneycombBrowser> browser,
                                HoneycombRefPtr<HoneycombFrame> frame,
                                HoneycombRefPtr<HoneycombV8Context> context) override {
    HoneycombRefPtr<HoneycombV8Value> object = context->GetGlobal();

    HoneycombRefPtr<HoneycombV8Handler> handler = new V8Handler();

    // Bind test functions.
    object->SetValue(kGetPerfTests,
                     HoneycombV8Value::CreateFunction(kGetPerfTests, handler),
                     V8_PROPERTY_ATTRIBUTE_READONLY);
    object->SetValue(kRunPerfTest,
                     HoneycombV8Value::CreateFunction(kRunPerfTest, handler),
                     V8_PROPERTY_ATTRIBUTE_READONLY);
    object->SetValue(kPerfTestReturnValue,
                     HoneycombV8Value::CreateFunction(kPerfTestReturnValue, handler),
                     V8_PROPERTY_ATTRIBUTE_READONLY);
  }

 private:
  IMPLEMENT_REFCOUNTING(RenderDelegate);
};

}  // namespace

void CreateDelegates(ClientAppRenderer::DelegateSet& delegates) {
  delegates.insert(new RenderDelegate);
}

}  // namespace performance_test
}  // namespace client
