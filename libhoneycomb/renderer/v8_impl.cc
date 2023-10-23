// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

// MSVC++ requires this to be set before any other includes to get M_PI.
// Otherwise there will be compile errors in wtf/MathExtras.h.
#define _USE_MATH_DEFINES

#include <map>
#include <string>

#include "base/command_line.h"
#include "build/build_config.h"

// Enable deprecation warnings for MSVC and Clang. See http://crbug.com/585142.
#if BUILDFLAG(IS_WIN)
#if defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic error "-Wdeprecated-declarations"
#else
#pragma warning(push)
#pragma warning(default : 4996)
#endif
#endif

#include "libhoneycomb/renderer/v8_impl.h"

#include "libhoneycomb/common/app_manager.h"
#include "libhoneycomb/common/honey_switches.h"
#include "libhoneycomb/common/task_runner_impl.h"
#include "libhoneycomb/common/tracker.h"
#include "libhoneycomb/renderer/blink_glue.h"
#include "libhoneycomb/renderer/browser_impl.h"
#include "libhoneycomb/renderer/render_frame_util.h"
#include "libhoneycomb/renderer/thread_util.h"

#include "base/auto_reset.h"
#include "base/functional/bind.h"
#include "base/lazy_instance.h"
#include "base/strings/string_number_conversions.h"
#include "third_party/abseil-cpp/absl/base/attributes.h"
#include "third_party/blink/public/web/blink.h"
#include "third_party/blink/public/web/web_frame.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/public/web/web_script_controller.h"
#include "url/gurl.h"

namespace {

static const char kHoneycombTrackObject[] = "Honeycomb::TrackObject";

void MessageListenerCallbackImpl(v8::Handle<v8::Message> message,
                                 v8::Handle<v8::Value> data);

// The following *Private functions are convenience wrappers for methods on
// v8::Object with the corresponding names.
// Based on extensions/renderer/object_backed_native_handler.cc.

void SetPrivate(v8::Local<v8::Context> context,
                v8::Local<v8::Object> obj,
                const char* key,
                v8::Local<v8::Value> value) {
  v8::Isolate* isolate = context->GetIsolate();
  obj->SetPrivate(context,
                  v8::Private::ForApi(
                      isolate, v8::String::NewFromUtf8(
                                   isolate, key, v8::NewStringType::kNormal)
                                   .ToLocalChecked()),
                  value)
      .FromJust();
}

bool GetPrivate(v8::Local<v8::Context> context,
                v8::Local<v8::Object> obj,
                const char* key,
                v8::Local<v8::Value>* result) {
  v8::Isolate* isolate = context->GetIsolate();
  return obj
      ->GetPrivate(context,
                   v8::Private::ForApi(
                       isolate, v8::String::NewFromUtf8(
                                    isolate, key, v8::NewStringType::kNormal)
                                    .ToLocalChecked()))
      .ToLocal(result);
}

// Chromium uses the default Isolate for the main render process thread and a
// new Isolate for each WebWorker thread. Continue this pattern by tracking
// Isolate information on a per-thread basis. This implementation will need to
// be re-worked (perhaps using a map keyed on v8::Isolate::GetCurrent()) if
// in the future Chromium begins using the same Isolate across multiple threads.
class HoneycombV8IsolateManager;
ABSL_CONST_INIT thread_local HoneycombV8IsolateManager* g_isolate_manager = nullptr;

// Manages memory and state information associated with a single Isolate.
class HoneycombV8IsolateManager {
 public:
  HoneycombV8IsolateManager()
      : resetter_(&g_isolate_manager, this),
        isolate_(v8::Isolate::GetCurrent()),
        task_runner_(HONEYCOMB_RENDER_TASK_RUNNER()),
        message_listener_registered_(false),
        worker_id_(0) {
    DCHECK(isolate_);
    DCHECK(task_runner_.get());
  }
  ~HoneycombV8IsolateManager() = default;

  static HoneycombV8IsolateManager* Get() { return g_isolate_manager; }

  void Destroy() {
    DCHECK_EQ(isolate_, v8::Isolate::GetCurrent());
    DCHECK(context_map_.empty());
    delete this;
  }

  scoped_refptr<HoneycombV8ContextState> GetContextState(
      v8::Local<v8::Context> context) {
    DCHECK_EQ(isolate_, v8::Isolate::GetCurrent());
    DCHECK(context.IsEmpty() || isolate_ == context->GetIsolate());

    if (context.IsEmpty()) {
      if (isolate_->InContext()) {
        context = isolate_->GetCurrentContext();
      } else {
        return scoped_refptr<HoneycombV8ContextState>();
      }
    }

    int hash = context->Global()->GetIdentityHash();
    ContextMap::const_iterator it = context_map_.find(hash);
    if (it != context_map_.end()) {
      return it->second;
    }

    scoped_refptr<HoneycombV8ContextState> state = new HoneycombV8ContextState();
    context_map_.insert(std::make_pair(hash, state));

    return state;
  }

  void ReleaseContext(v8::Local<v8::Context> context) {
    DCHECK_EQ(isolate_, v8::Isolate::GetCurrent());
    DCHECK_EQ(isolate_, context->GetIsolate());

    int hash = context->Global()->GetIdentityHash();
    ContextMap::iterator it = context_map_.find(hash);
    if (it != context_map_.end()) {
      it->second->Detach();
      context_map_.erase(it);
    }
  }

  void AddGlobalTrackObject(HoneycombTrackNode* object) {
    DCHECK_EQ(isolate_, v8::Isolate::GetCurrent());
    global_manager_.Add(object);
  }

  void DeleteGlobalTrackObject(HoneycombTrackNode* object) {
    DCHECK_EQ(isolate_, v8::Isolate::GetCurrent());
    global_manager_.Delete(object);
  }

  void SetUncaughtExceptionStackSize(int stack_size) {
    if (stack_size <= 0) {
      return;
    }

    if (!message_listener_registered_) {
      isolate_->AddMessageListener(&MessageListenerCallbackImpl);
      message_listener_registered_ = true;
    }

    isolate_->SetCaptureStackTraceForUncaughtExceptions(
        true, stack_size, v8::StackTrace::kDetailed);
  }

  void SetWorkerAttributes(int worker_id, const GURL& worker_url) {
    worker_id_ = worker_id;
    worker_url_ = worker_url;
  }

  v8::Isolate* isolate() const { return isolate_; }
  scoped_refptr<base::SingleThreadTaskRunner> task_runner() const {
    return task_runner_;
  }

  int worker_id() const { return worker_id_; }

  const GURL& worker_url() const { return worker_url_; }

 private:
  const base::AutoReset<HoneycombV8IsolateManager*> resetter_;

  v8::Isolate* isolate_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;

  using ContextMap = std::map<int, scoped_refptr<HoneycombV8ContextState>>;
  ContextMap context_map_;

  // Used for globally tracked objects that are not associated with a particular
  // context.
  HoneycombTrackManager global_manager_;

  // True if the message listener has been registered.
  bool message_listener_registered_;

  // Attributes associated with WebWorker threads.
  int worker_id_;
  GURL worker_url_;
};

class V8TrackObject : public HoneycombTrackNode {
 public:
  explicit V8TrackObject(v8::Isolate* isolate)
      : isolate_(isolate), external_memory_(0) {
    DCHECK(isolate_);
    isolate_->AdjustAmountOfExternalAllocatedMemory(
        static_cast<int>(sizeof(V8TrackObject)));
  }
  ~V8TrackObject() {
    isolate_->AdjustAmountOfExternalAllocatedMemory(
        -static_cast<int>(sizeof(V8TrackObject)) - external_memory_);
  }

  inline int GetExternallyAllocatedMemory() { return external_memory_; }

  int AdjustExternallyAllocatedMemory(int change_in_bytes) {
    int new_value = external_memory_ + change_in_bytes;
    if (new_value < 0) {
      DCHECK(false) << "External memory usage cannot be less than 0 bytes";
      change_in_bytes = -(external_memory_);
      new_value = 0;
    }

    if (change_in_bytes != 0) {
      isolate_->AdjustAmountOfExternalAllocatedMemory(change_in_bytes);
    }
    external_memory_ = new_value;

    return new_value;
  }

  inline void SetAccessor(HoneycombRefPtr<HoneycombV8Accessor> accessor) {
    accessor_ = accessor;
  }

  inline HoneycombRefPtr<HoneycombV8Accessor> GetAccessor() { return accessor_; }

  inline void SetInterceptor(HoneycombRefPtr<HoneycombV8Interceptor> interceptor) {
    interceptor_ = interceptor;
  }

  inline HoneycombRefPtr<HoneycombV8Interceptor> GetInterceptor() { return interceptor_; }

  inline void SetHandler(HoneycombRefPtr<HoneycombV8Handler> handler) {
    handler_ = handler;
  }

  inline HoneycombRefPtr<HoneycombV8Handler> GetHandler() { return handler_; }

  inline void SetUserData(HoneycombRefPtr<HoneycombBaseRefCounted> user_data) {
    user_data_ = user_data;
  }

  inline HoneycombRefPtr<HoneycombBaseRefCounted> GetUserData() { return user_data_; }

  // Attach this track object to the specified V8 object.
  void AttachTo(v8::Local<v8::Context> context, v8::Local<v8::Object> object) {
    SetPrivate(context, object, kHoneycombTrackObject,
               v8::External::New(isolate_, this));
  }

  // Retrieve the track object for the specified V8 object.
  static V8TrackObject* Unwrap(v8::Local<v8::Context> context,
                               v8::Local<v8::Object> object) {
    v8::Local<v8::Value> value;
    if (GetPrivate(context, object, kHoneycombTrackObject, &value) &&
        value->IsExternal()) {
      return static_cast<V8TrackObject*>(v8::External::Cast(*value)->Value());
    }

    return nullptr;
  }

 private:
  v8::Isolate* isolate_;
  HoneycombRefPtr<HoneycombV8Accessor> accessor_;
  HoneycombRefPtr<HoneycombV8Interceptor> interceptor_;
  HoneycombRefPtr<HoneycombV8Handler> handler_;
  HoneycombRefPtr<HoneycombBaseRefCounted> user_data_;
  int external_memory_;
};

class V8TrackString : public HoneycombTrackNode {
 public:
  explicit V8TrackString(const std::string& str) : string_(str) {}
  const char* GetString() { return string_.c_str(); }

 private:
  std::string string_;
};

class V8TrackArrayBuffer : public HoneycombTrackNode {
 public:
  explicit V8TrackArrayBuffer(
      v8::Isolate* isolate,
      HoneycombRefPtr<HoneycombV8ArrayBufferReleaseCallback> release_callback)
      : isolate_(isolate), release_callback_(release_callback) {
    DCHECK(isolate_);
    isolate_->AdjustAmountOfExternalAllocatedMemory(
        static_cast<int>(sizeof(V8TrackArrayBuffer)));
  }

  ~V8TrackArrayBuffer() {
    isolate_->AdjustAmountOfExternalAllocatedMemory(
        -static_cast<int>(sizeof(V8TrackArrayBuffer)));
  }

  HoneycombRefPtr<HoneycombV8ArrayBufferReleaseCallback> GetReleaseCallback() {
    return release_callback_;
  }

  // Attach this track object to the specified V8 object.
  void AttachTo(v8::Local<v8::Context> context,
                v8::Local<v8::ArrayBuffer> arrayBuffer) {
    SetPrivate(context, arrayBuffer, kHoneycombTrackObject,
               v8::External::New(isolate_, this));
  }

  // Retrieve the track object for the specified V8 object.
  static V8TrackArrayBuffer* Unwrap(v8::Local<v8::Context> context,
                                    v8::Local<v8::Object> object) {
    v8::Local<v8::Value> value;
    if (GetPrivate(context, object, kHoneycombTrackObject, &value) &&
        value->IsExternal()) {
      return static_cast<V8TrackArrayBuffer*>(
          v8::External::Cast(*value)->Value());
    }

    return nullptr;
  }

 private:
  v8::Isolate* isolate_;
  HoneycombRefPtr<HoneycombV8ArrayBufferReleaseCallback> release_callback_;
};

// Object wrapped in a v8::External and passed as the Data argument to
// v8::FunctionTemplate::New.
class V8FunctionData {
 public:
  static v8::Local<v8::External> Create(v8::Isolate* isolate,
                                        const HoneycombString& function_name,
                                        HoneycombRefPtr<HoneycombV8Handler> handler) {
    // |data| will be deleted if/when the returned v8::External is GC'd.
    V8FunctionData* data = new V8FunctionData(isolate, function_name, handler);
    return data->CreateExternal();
  }

  static V8FunctionData* Unwrap(v8::Local<v8::Value> data) {
    DCHECK(data->IsExternal());
    return static_cast<V8FunctionData*>(v8::External::Cast(*data)->Value());
  }

  HoneycombString function_name() const { return function_name_; }

  HoneycombV8Handler* handler() const {
    if (!handler_) {
      return nullptr;
    }
    return handler_.get();
  }

 private:
  V8FunctionData(v8::Isolate* isolate,
                 const HoneycombString& function_name,
                 HoneycombRefPtr<HoneycombV8Handler> handler)
      : isolate_(isolate), function_name_(function_name), handler_(handler) {
    DCHECK(isolate_);
    DCHECK(handler_);
  }

  ~V8FunctionData() {
    isolate_->AdjustAmountOfExternalAllocatedMemory(
        -static_cast<int>(sizeof(V8FunctionData)));
    handler_ = nullptr;
    function_name_ = "FreedFunction";
  }

  v8::Local<v8::External> CreateExternal() {
    v8::Local<v8::External> external = v8::External::New(isolate_, this);

    isolate_->AdjustAmountOfExternalAllocatedMemory(
        static_cast<int>(sizeof(V8FunctionData)));

    handle_.Reset(isolate_, external);
    handle_.SetWeak(this, FirstWeakCallback, v8::WeakCallbackType::kParameter);

    return external;
  }

  static void FirstWeakCallback(
      const v8::WeakCallbackInfo<V8FunctionData>& data) {
    V8FunctionData* wrapper = data.GetParameter();
    wrapper->handle_.Reset();
    data.SetSecondPassCallback(SecondWeakCallback);
  }

  static void SecondWeakCallback(
      const v8::WeakCallbackInfo<V8FunctionData>& data) {
    V8FunctionData* wrapper = data.GetParameter();
    delete wrapper;
  }

  v8::Isolate* isolate_;
  HoneycombString function_name_;
  HoneycombRefPtr<HoneycombV8Handler> handler_;
  v8::Persistent<v8::External> handle_;
};

// Convert a HoneycombString to a V8::String.
v8::Local<v8::String> GetV8String(v8::Isolate* isolate, const HoneycombString& str) {
#if defined(HONEYCOMB_STRING_TYPE_UTF16)
  // Already a UTF16 string.
  return v8::String::NewFromTwoByte(
             isolate,
             reinterpret_cast<uint16_t*>(
                 const_cast<HoneycombString::char_type*>(str.c_str())),
             v8::NewStringType::kNormal, str.length())
      .ToLocalChecked();
#elif defined(HONEYCOMB_STRING_TYPE_UTF8)
  // Already a UTF8 string.
  return v8::String::NewFromUtf8(isolate, const_cast<char*>(str.c_str()),
                                 v8::NewStringType::kNormal, str.length())
      .ToLocalChecked();
#else
  // Convert the string to UTF8.
  std::string tmpStr = str;
  return v8::String::NewFromUtf8(isolate, tmpStr.c_str(),
                                 v8::NewStringType::kNormal, tmpStr.length())
      .ToLocalChecked();
#endif
}

#if defined(HONEYCOMB_STRING_TYPE_UTF16)
void v8impl_string_dtor(char16_t* str) {
  delete[] str;
}
#elif defined(HONEYCOMB_STRING_TYPE_UTF8)
void v8impl_string_dtor(char* str) {
  delete[] str;
}
#endif

// Convert a v8::String to HoneycombString.
void GetHoneycombString(v8::Isolate* isolate,
                  v8::Local<v8::String> str,
                  HoneycombString& out) {
  if (str.IsEmpty()) {
    return;
  }

#if defined(HONEYCOMB_STRING_TYPE_WIDE)
  // Allocate enough space for a worst-case conversion.
  int len = str->Utf8Length();
  if (len == 0) {
    return;
  }
  char* buf = new char[len + 1];
  str->WriteUtf8(isolate, buf, len + 1);

  // Perform conversion to the wide type.
  honey_string_t* retws = out.GetWritableStruct();
  honey_string_utf8_to_wide(buf, len, retws);

  delete[] buf;
#else  // !defined(HONEYCOMB_STRING_TYPE_WIDE)
#if defined(HONEYCOMB_STRING_TYPE_UTF16)
  int len = str->Length();
  if (len == 0) {
    return;
  }
  char16_t* buf = new char16_t[len + 1];
  str->Write(isolate, reinterpret_cast<uint16_t*>(buf), 0, len + 1);
#else
  // Allocate enough space for a worst-case conversion.
  int len = str->Utf8Length();
  if (len == 0) {
    return;
  }
  char* buf = new char[len + 1];
  str->WriteUtf8(isolate, buf, len + 1);
#endif

  // Don't perform an extra string copy.
  out.clear();
  honey_string_t* retws = out.GetWritableStruct();
  retws->str = buf;
  retws->length = len;
  retws->dtor = v8impl_string_dtor;
#endif  // !defined(HONEYCOMB_STRING_TYPE_WIDE)
}

// V8 function callback.
void FunctionCallbackImpl(const v8::FunctionCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  V8FunctionData* data = V8FunctionData::Unwrap(info.Data());
  if (!data->handler()) {
    // handler has gone away, bail!
    info.GetReturnValue().SetUndefined();
    return;
  }
  HoneycombV8ValueList params;
  for (int i = 0; i < info.Length(); i++) {
    params.push_back(new HoneycombV8ValueImpl(isolate, context, info[i]));
  }

  HoneycombRefPtr<HoneycombV8Value> object =
      new HoneycombV8ValueImpl(isolate, context, info.This());
  HoneycombRefPtr<HoneycombV8Value> retval;
  HoneycombString exception;

  if (data->handler()->Execute(data->function_name(), object, params, retval,
                               exception)) {
    if (!exception.empty()) {
      info.GetReturnValue().Set(isolate->ThrowException(
          v8::Exception::Error(GetV8String(isolate, exception))));
      return;
    } else {
      HoneycombV8ValueImpl* rv = static_cast<HoneycombV8ValueImpl*>(retval.get());
      if (rv && rv->IsValid()) {
        info.GetReturnValue().Set(rv->GetV8Value(true));
        return;
      }
    }
  }

  info.GetReturnValue().SetUndefined();
}

// V8 Accessor callbacks
void AccessorNameGetterCallbackImpl(
    v8::Local<v8::Name> property,
    const v8::PropertyCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  v8::Local<v8::Object> obj = info.This();

  HoneycombRefPtr<HoneycombV8Accessor> accessorPtr;

  V8TrackObject* tracker = V8TrackObject::Unwrap(context, obj);
  if (tracker) {
    accessorPtr = tracker->GetAccessor();
  }

  if (accessorPtr.get()) {
    HoneycombRefPtr<HoneycombV8Value> retval;
    HoneycombRefPtr<HoneycombV8Value> object = new HoneycombV8ValueImpl(isolate, context, obj);
    HoneycombString name, exception;
    GetHoneycombString(isolate, v8::Local<v8::String>::Cast(property), name);
    if (accessorPtr->Get(name, object, retval, exception)) {
      if (!exception.empty()) {
        info.GetReturnValue().Set(isolate->ThrowException(
            v8::Exception::Error(GetV8String(isolate, exception))));
        return;
      } else {
        HoneycombV8ValueImpl* rv = static_cast<HoneycombV8ValueImpl*>(retval.get());
        if (rv && rv->IsValid()) {
          info.GetReturnValue().Set(rv->GetV8Value(true));
          return;
        }
      }
    }
  }

  return info.GetReturnValue().SetUndefined();
}

void AccessorNameSetterCallbackImpl(
    v8::Local<v8::Name> property,
    v8::Local<v8::Value> value,
    const v8::PropertyCallbackInfo<void>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  v8::Local<v8::Object> obj = info.This();

  HoneycombRefPtr<HoneycombV8Accessor> accessorPtr;

  V8TrackObject* tracker = V8TrackObject::Unwrap(context, obj);
  if (tracker) {
    accessorPtr = tracker->GetAccessor();
  }

  if (accessorPtr.get()) {
    HoneycombRefPtr<HoneycombV8Value> object = new HoneycombV8ValueImpl(isolate, context, obj);
    HoneycombRefPtr<HoneycombV8Value> honeyValue =
        new HoneycombV8ValueImpl(isolate, context, value);
    HoneycombString name, exception;
    GetHoneycombString(isolate, v8::Local<v8::String>::Cast(property), name);
    accessorPtr->Set(name, object, honeyValue, exception);
    if (!exception.empty()) {
      isolate->ThrowException(
          v8::Exception::Error(GetV8String(isolate, exception)));
      return;
    }
  }
}

// Two helper functions for V8 Interceptor callbacks.
HoneycombString PropertyToIndex(v8::Isolate* isolate, v8::Local<v8::Name> property) {
  HoneycombString name;
  GetHoneycombString(isolate, property.As<v8::String>(), name);
  return name;
}

int PropertyToIndex(v8::Isolate* isolate, uint32_t index) {
  return static_cast<int>(index);
}

// V8 Interceptor callbacks.
// T == v8::Local<v8::Name> for named property handlers and
// T == uint32_t for indexed property handlers
template <typename T>
void InterceptorGetterCallbackImpl(
    T property,
    const v8::PropertyCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  v8::Handle<v8::Object> obj = info.This();
  HoneycombRefPtr<HoneycombV8Interceptor> interceptorPtr;

  V8TrackObject* tracker = V8TrackObject::Unwrap(context, obj);
  if (tracker) {
    interceptorPtr = tracker->GetInterceptor();
  }
  if (!interceptorPtr.get()) {
    return;
  }

  HoneycombRefPtr<HoneycombV8Value> object = new HoneycombV8ValueImpl(isolate, context, obj);
  HoneycombRefPtr<HoneycombV8Value> retval;
  HoneycombString exception;
  interceptorPtr->Get(PropertyToIndex(isolate, property), object, retval,
                      exception);
  if (!exception.empty()) {
    info.GetReturnValue().Set(isolate->ThrowException(
        v8::Exception::Error(GetV8String(isolate, exception))));
  } else {
    HoneycombV8ValueImpl* retval_impl = static_cast<HoneycombV8ValueImpl*>(retval.get());
    if (retval_impl && retval_impl->IsValid()) {
      info.GetReturnValue().Set(retval_impl->GetV8Value(true));
    }
  }
}

template <typename T>
void InterceptorSetterCallbackImpl(
    T property,
    v8::Local<v8::Value> value,
    const v8::PropertyCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  v8::Handle<v8::Object> obj = info.This();
  HoneycombRefPtr<HoneycombV8Interceptor> interceptorPtr;

  V8TrackObject* tracker = V8TrackObject::Unwrap(context, obj);
  if (tracker) {
    interceptorPtr = tracker->GetInterceptor();
  }

  if (!interceptorPtr.get()) {
    return;
  }
  HoneycombRefPtr<HoneycombV8Value> object = new HoneycombV8ValueImpl(isolate, context, obj);
  HoneycombRefPtr<HoneycombV8Value> honeyValue = new HoneycombV8ValueImpl(isolate, context, value);
  HoneycombString exception;
  interceptorPtr->Set(PropertyToIndex(isolate, property), object, honeyValue,
                      exception);
  if (!exception.empty()) {
    isolate->ThrowException(
        v8::Exception::Error(GetV8String(isolate, exception)));
  }
}

// V8 extension registration.

class ExtensionWrapper : public v8::Extension {
 public:
  ExtensionWrapper(const char* extension_name,
                   const char* javascript_code,
                   HoneycombV8Handler* handler)
      : v8::Extension(extension_name, javascript_code), handler_(handler) {}

  v8::Handle<v8::FunctionTemplate> GetNativeFunctionTemplate(
      v8::Isolate* isolate,
      v8::Handle<v8::String> name) override {
    if (!handler_) {
      return v8::Local<v8::FunctionTemplate>();
    }

    HoneycombString func_name;
    GetHoneycombString(isolate, name, func_name);

    v8::Local<v8::External> function_data =
        V8FunctionData::Create(isolate, func_name, handler_);

    return v8::FunctionTemplate::New(isolate, FunctionCallbackImpl,
                                     function_data);
  }

 private:
  HoneycombV8Handler* handler_;
};

class HoneycombV8ExceptionImpl : public HoneycombV8Exception {
 public:
  HoneycombV8ExceptionImpl(v8::Local<v8::Context> context,
                     v8::Local<v8::Message> message)
      : line_number_(0),
        start_position_(0),
        end_position_(0),
        start_column_(0),
        end_column_(0) {
    if (message.IsEmpty()) {
      return;
    }

    v8::Isolate* isolate = context->GetIsolate();
    GetHoneycombString(isolate, message->Get(), message_);
    v8::MaybeLocal<v8::String> source_line = message->GetSourceLine(context);
    if (!source_line.IsEmpty()) {
      GetHoneycombString(isolate, source_line.ToLocalChecked(), source_line_);
    }

    if (!message->GetScriptResourceName().IsEmpty()) {
      GetHoneycombString(
          isolate,
          message->GetScriptResourceName()->ToString(context).ToLocalChecked(),
          script_);
    }

    v8::Maybe<int> line_number = message->GetLineNumber(context);
    if (!line_number.IsNothing()) {
      line_number_ = line_number.ToChecked();
    }
    start_position_ = message->GetStartPosition();
    end_position_ = message->GetEndPosition();
    start_column_ = message->GetStartColumn(context).FromJust();
    end_column_ = message->GetEndColumn(context).FromJust();
  }

  HoneycombString GetMessage() override { return message_; }
  HoneycombString GetSourceLine() override { return source_line_; }
  HoneycombString GetScriptResourceName() override { return script_; }
  int GetLineNumber() override { return line_number_; }
  int GetStartPosition() override { return start_position_; }
  int GetEndPosition() override { return end_position_; }
  int GetStartColumn() override { return start_column_; }
  int GetEndColumn() override { return end_column_; }

 protected:
  HoneycombString message_;
  HoneycombString source_line_;
  HoneycombString script_;
  int line_number_;
  int start_position_;
  int end_position_;
  int start_column_;
  int end_column_;

  IMPLEMENT_REFCOUNTING(HoneycombV8ExceptionImpl);
};

void MessageListenerCallbackImpl(v8::Handle<v8::Message> message,
                                 v8::Handle<v8::Value> data) {
  HoneycombRefPtr<HoneycombApp> application = HoneycombAppManager::Get()->GetApplication();
  if (!application.get()) {
    return;
  }

  HoneycombRefPtr<HoneycombRenderProcessHandler> handler =
      application->GetRenderProcessHandler();
  if (!handler.get()) {
    return;
  }

  v8::Isolate* isolate = HoneycombV8IsolateManager::Get()->isolate();
  HoneycombRefPtr<HoneycombV8Context> context = HoneycombV8Context::GetCurrentContext();
  v8::Local<v8::StackTrace> v8Stack = message->GetStackTrace();
  HoneycombRefPtr<HoneycombV8StackTrace> stackTrace =
      new HoneycombV8StackTraceImpl(isolate, v8Stack);

  HoneycombRefPtr<HoneycombV8Exception> exception = new HoneycombV8ExceptionImpl(
      static_cast<HoneycombV8ContextImpl*>(context.get())->GetV8Context(), message);

  HoneycombRefPtr<HoneycombBrowser> browser = context->GetBrowser();
  if (browser) {
    handler->OnUncaughtException(browser, context->GetFrame(), context,
                                 exception, stackTrace);
  }
}

}  // namespace

// Global functions.

void HoneycombV8IsolateCreated() {
  new HoneycombV8IsolateManager();
}

void HoneycombV8IsolateDestroyed() {
  auto* isolate_manager = HoneycombV8IsolateManager::Get();
  isolate_manager->Destroy();
}

void HoneycombV8ReleaseContext(v8::Local<v8::Context> context) {
  HoneycombV8IsolateManager::Get()->ReleaseContext(context);
}

void HoneycombV8SetUncaughtExceptionStackSize(int stack_size) {
  HoneycombV8IsolateManager::Get()->SetUncaughtExceptionStackSize(stack_size);
}

void HoneycombV8SetWorkerAttributes(int worker_id, const GURL& worker_url) {
  HoneycombV8IsolateManager::Get()->SetWorkerAttributes(worker_id, worker_url);
}

bool HoneycombRegisterExtension(const HoneycombString& extension_name,
                          const HoneycombString& javascript_code,
                          HoneycombRefPtr<HoneycombV8Handler> handler) {
  // Verify that this method was called on the correct thread.
  HONEYCOMB_REQUIRE_RT_RETURN(false);

  auto* isolate_manager = HoneycombV8IsolateManager::Get();

  V8TrackString* name = new V8TrackString(extension_name);
  isolate_manager->AddGlobalTrackObject(name);
  V8TrackString* code = new V8TrackString(javascript_code);
  isolate_manager->AddGlobalTrackObject(code);

  if (handler.get()) {
    // The reference will be released when the process exits.
    V8TrackObject* object = new V8TrackObject(isolate_manager->isolate());
    object->SetHandler(handler);
    isolate_manager->AddGlobalTrackObject(object);
  }

  std::unique_ptr<v8::Extension> wrapper(new ExtensionWrapper(
      name->GetString(), code->GetString(), handler.get()));

  blink::WebScriptController::RegisterExtension(std::move(wrapper));
  return true;
}

// Helper macros

#define HONEYCOMB_V8_HAS_ISOLATE() (!!HoneycombV8IsolateManager::Get())
#define HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(var)      \
  if (!HONEYCOMB_V8_HAS_ISOLATE()) {                  \
    DCHECK(false) << "V8 isolate is not valid"; \
    return var;                                 \
  }

#define HONEYCOMB_V8_CURRENTLY_ON_MLT() \
  (!handle_.get() || handle_->BelongsToCurrentThread())
#define HONEYCOMB_V8_REQUIRE_MLT_RETURN(var)             \
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(var);              \
  if (!HONEYCOMB_V8_CURRENTLY_ON_MLT()) {                \
    DCHECK(false) << "called on incorrect thread"; \
    return var;                                    \
  }

#define HONEYCOMB_V8_HANDLE_IS_VALID() (handle_.get() && handle_->IsValid())
#define HONEYCOMB_V8_REQUIRE_VALID_HANDLE_RETURN(ret) \
  HONEYCOMB_V8_REQUIRE_MLT_RETURN(ret);               \
  if (!HONEYCOMB_V8_HANDLE_IS_VALID()) {              \
    DCHECK(false) << "V8 handle is not valid";  \
    return ret;                                 \
  }

#define HONEYCOMB_V8_IS_VALID()                               \
  (HONEYCOMB_V8_HAS_ISOLATE() && HONEYCOMB_V8_CURRENTLY_ON_MLT() && \
   HONEYCOMB_V8_HANDLE_IS_VALID())

#define HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(ret)         \
  HONEYCOMB_V8_REQUIRE_VALID_HANDLE_RETURN(ret);        \
  if (type_ != TYPE_OBJECT) {                     \
    DCHECK(false) << "V8 value is not an object"; \
    return ret;                                   \
  }

// HoneycombV8HandleBase

HoneycombV8HandleBase::~HoneycombV8HandleBase() {
  DCHECK(BelongsToCurrentThread());
}

bool HoneycombV8HandleBase::BelongsToCurrentThread() const {
  return task_runner_->RunsTasksInCurrentSequence();
}

HoneycombV8HandleBase::HoneycombV8HandleBase(v8::Isolate* isolate,
                                 v8::Local<v8::Context> context)
    : isolate_(isolate) {
  DCHECK(isolate_);

  HoneycombV8IsolateManager* manager = HoneycombV8IsolateManager::Get();
  DCHECK(manager);
  DCHECK_EQ(isolate_, manager->isolate());

  task_runner_ = manager->task_runner();
  context_state_ = manager->GetContextState(context);
}

// HoneycombV8Context

// static
HoneycombRefPtr<HoneycombV8Context> HoneycombV8Context::GetCurrentContext() {
  HoneycombRefPtr<HoneycombV8Context> context;
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(context);
  v8::Isolate* isolate = HoneycombV8IsolateManager::Get()->isolate();
  if (isolate->InContext()) {
    v8::HandleScope handle_scope(isolate);
    context = new HoneycombV8ContextImpl(isolate, isolate->GetCurrentContext());
  }
  return context;
}

// static
HoneycombRefPtr<HoneycombV8Context> HoneycombV8Context::GetEnteredContext() {
  HoneycombRefPtr<HoneycombV8Context> context;
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(context);
  v8::Isolate* isolate = HoneycombV8IsolateManager::Get()->isolate();
  if (isolate->InContext()) {
    v8::HandleScope handle_scope(isolate);
    context =
        new HoneycombV8ContextImpl(isolate, isolate->GetEnteredOrMicrotaskContext());
  }
  return context;
}

// static
bool HoneycombV8Context::InContext() {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(false);
  v8::Isolate* isolate = HoneycombV8IsolateManager::Get()->isolate();
  return isolate->InContext();
}

// HoneycombV8ContextImpl

HoneycombV8ContextImpl::HoneycombV8ContextImpl(v8::Isolate* isolate,
                                   v8::Local<v8::Context> context)
    : handle_(new Handle(isolate, context, context)),
      microtask_queue_(blink_glue::GetMicrotaskQueue(context)) {}

HoneycombV8ContextImpl::~HoneycombV8ContextImpl() {
  DLOG_ASSERT(0 == enter_count_);
}

HoneycombRefPtr<HoneycombTaskRunner> HoneycombV8ContextImpl::GetTaskRunner() {
  return new HoneycombTaskRunnerImpl(handle_->task_runner());
}

bool HoneycombV8ContextImpl::IsValid() {
  return HONEYCOMB_V8_IS_VALID();
}

HoneycombRefPtr<HoneycombBrowser> HoneycombV8ContextImpl::GetBrowser() {
  HoneycombRefPtr<HoneycombBrowser> browser;
  HONEYCOMB_V8_REQUIRE_VALID_HANDLE_RETURN(browser);

  blink::WebLocalFrame* webframe = GetWebFrame();
  if (webframe) {
    browser = HoneycombBrowserImpl::GetBrowserForMainFrame(webframe->Top());
  }

  return browser;
}

HoneycombRefPtr<HoneycombFrame> HoneycombV8ContextImpl::GetFrame() {
  HoneycombRefPtr<HoneycombFrame> frame;
  HONEYCOMB_V8_REQUIRE_VALID_HANDLE_RETURN(frame);

  blink::WebLocalFrame* webframe = GetWebFrame();
  if (webframe) {
    HoneycombRefPtr<HoneycombBrowserImpl> browser =
        HoneycombBrowserImpl::GetBrowserForMainFrame(webframe->Top());
    if (browser) {
      frame = browser->GetFrame(render_frame_util::GetIdentifier(webframe));
    }
  }

  return frame;
}

HoneycombRefPtr<HoneycombV8Value> HoneycombV8ContextImpl::GetGlobal() {
  HONEYCOMB_V8_REQUIRE_VALID_HANDLE_RETURN(nullptr);

  if (blink_glue::IsScriptForbidden()) {
    return nullptr;
  }

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Context> context = GetV8Context();
  v8::Context::Scope context_scope(context);
  return new HoneycombV8ValueImpl(isolate, context, context->Global());
}

bool HoneycombV8ContextImpl::Enter() {
  HONEYCOMB_V8_REQUIRE_VALID_HANDLE_RETURN(false);

  if (blink_glue::IsScriptForbidden()) {
    return false;
  }

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);

  if (!microtasks_scope_) {
    // Increment the MicrotasksScope recursion level.
    microtasks_scope_.reset(new v8::MicrotasksScope(
        isolate, microtask_queue_, v8::MicrotasksScope::kRunMicrotasks));
  }

  ++enter_count_;
  handle_->GetNewV8Handle()->Enter();

  return true;
}

bool HoneycombV8ContextImpl::Exit() {
  HONEYCOMB_V8_REQUIRE_VALID_HANDLE_RETURN(false);

  if (blink_glue::IsScriptForbidden()) {
    return false;
  }

  if (enter_count_ <= 0) {
    LOG(ERROR) << "Call to HoneycombV8Context::Exit() without matching call to "
                  "HoneycombV8Context::Enter()";
    return false;
  }

  v8::HandleScope handle_scope(handle_->isolate());

  handle_->GetNewV8Handle()->Exit();

  if (--enter_count_ == 0) {
    // Decrement the MicrotasksScope recursion level.
    microtasks_scope_.reset(nullptr);
  }

  return true;
}

bool HoneycombV8ContextImpl::IsSame(HoneycombRefPtr<HoneycombV8Context> that) {
  HONEYCOMB_V8_REQUIRE_VALID_HANDLE_RETURN(false);

  HoneycombV8ContextImpl* impl = static_cast<HoneycombV8ContextImpl*>(that.get());
  if (!impl || !impl->IsValid()) {
    return false;
  }

  return (handle_->GetPersistentV8Handle() ==
          impl->handle_->GetPersistentV8Handle());
}

bool HoneycombV8ContextImpl::Eval(const HoneycombString& code,
                            const HoneycombString& script_url,
                            int start_line,
                            HoneycombRefPtr<HoneycombV8Value>& retval,
                            HoneycombRefPtr<HoneycombV8Exception>& exception) {
  retval = nullptr;
  exception = nullptr;

  HONEYCOMB_V8_REQUIRE_VALID_HANDLE_RETURN(false);

  if (blink_glue::IsScriptForbidden()) {
    return false;
  }

  if (code.empty()) {
    DCHECK(false) << "invalid input parameter";
    return false;
  }

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Context> context = GetV8Context();
  v8::Context::Scope context_scope(context);

  const blink::WebString& source =
      blink::WebString::FromUTF16(code.ToString16());
  const blink::WebString& source_url =
      blink::WebString::FromUTF16(script_url.ToString16());

  v8::TryCatch try_catch(isolate);
  try_catch.SetVerbose(true);

  v8::Local<v8::Value> func_rv = blink_glue::ExecuteV8ScriptAndReturnValue(
      source, source_url, start_line, context, try_catch);

  if (try_catch.HasCaught()) {
    exception = new HoneycombV8ExceptionImpl(context, try_catch.Message());
    return false;
  } else if (!func_rv.IsEmpty()) {
    retval = new HoneycombV8ValueImpl(isolate, context, func_rv);
    return true;
  }

  DCHECK(false);
  return false;
}

v8::Local<v8::Context> HoneycombV8ContextImpl::GetV8Context() {
  return handle_->GetNewV8Handle();
}

blink::WebLocalFrame* HoneycombV8ContextImpl::GetWebFrame() {
  HONEYCOMB_REQUIRE_RT();

  if (blink_glue::IsScriptForbidden()) {
    return nullptr;
  }

  v8::HandleScope handle_scope(handle_->isolate());
  v8::Local<v8::Context> context = GetV8Context();
  v8::Context::Scope context_scope(context);
  return blink::WebLocalFrame::FrameForContext(context);
}

// HoneycombV8ValueImpl::Handle

HoneycombV8ValueImpl::Handle::Handle(v8::Isolate* isolate,
                               v8::Local<v8::Context> context,
                               handleType v,
                               HoneycombTrackNode* tracker)
    : HoneycombV8HandleBase(isolate, context),
      handle_(isolate, v),
      tracker_(tracker),
      should_persist_(false),
      is_set_weak_(false) {}

HoneycombV8ValueImpl::Handle::~Handle() {
  DCHECK(BelongsToCurrentThread());

  if (tracker_) {
    if (is_set_weak_) {
      if (context_state_.get()) {
        // If the associated context is still valid then delete |tracker_|.
        // Otherwise, |tracker_| will already have been deleted.
        if (context_state_->IsValid()) {
          context_state_->DeleteTrackObject(tracker_);
        }
      } else {
        HoneycombV8IsolateManager::Get()->DeleteGlobalTrackObject(tracker_);
      }
    } else {
      delete tracker_;
    }
  }

  if (is_set_weak_) {
    isolate_->AdjustAmountOfExternalAllocatedMemory(
        -static_cast<int>(sizeof(Handle)));
  } else {
    // SetWeak was not called so reset now.
    handle_.Reset();
  }
}

HoneycombV8ValueImpl::Handle::handleType HoneycombV8ValueImpl::Handle::GetNewV8Handle(
    bool should_persist) {
  DCHECK(IsValid());
  if (should_persist && !should_persist_) {
    should_persist_ = true;
  }
  return handleType::New(isolate(), handle_);
}

HoneycombV8ValueImpl::Handle::persistentType&
HoneycombV8ValueImpl::Handle::GetPersistentV8Handle() {
  return handle_;
}

void HoneycombV8ValueImpl::Handle::SetWeakIfNecessary() {
  if (!BelongsToCurrentThread()) {
    task_runner()->PostTask(
        FROM_HERE,
        base::BindOnce(&HoneycombV8ValueImpl::Handle::SetWeakIfNecessary, this));
    return;
  }

  // Persist the handle (call SetWeak) if:
  // A. The handle has been passed into a V8 function or used as a return value
  //    from a V8 callback, and
  // B. The associated context, if any, is still valid.
  if (should_persist_ && (!context_state_.get() || context_state_->IsValid())) {
    is_set_weak_ = true;

    if (tracker_) {
      if (context_state_.get()) {
        // |tracker_| will be deleted when:
        // A. The associated context is released, or
        // B. SecondWeakCallback is called for the weak handle.
        DCHECK(context_state_->IsValid());
        context_state_->AddTrackObject(tracker_);
      } else {
        // |tracker_| will be deleted when:
        // A. The process shuts down, or
        // B. SecondWeakCallback is called for the weak handle.
        HoneycombV8IsolateManager::Get()->AddGlobalTrackObject(tracker_);
      }
    }

    isolate_->AdjustAmountOfExternalAllocatedMemory(
        static_cast<int>(sizeof(Handle)));

    // The added reference will be released in SecondWeakCallback.
    AddRef();
    handle_.SetWeak(this, FirstWeakCallback, v8::WeakCallbackType::kParameter);
  }
}

// static
void HoneycombV8ValueImpl::Handle::FirstWeakCallback(
    const v8::WeakCallbackInfo<Handle>& data) {
  Handle* wrapper = data.GetParameter();
  wrapper->handle_.Reset();
  data.SetSecondPassCallback(SecondWeakCallback);
}

// static
void HoneycombV8ValueImpl::Handle::SecondWeakCallback(
    const v8::WeakCallbackInfo<Handle>& data) {
  Handle* wrapper = data.GetParameter();
  wrapper->Release();
}

// HoneycombV8Value

// static
HoneycombRefPtr<HoneycombV8Value> HoneycombV8Value::CreateUndefined() {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(nullptr);
  v8::Isolate* isolate = HoneycombV8IsolateManager::Get()->isolate();
  HoneycombRefPtr<HoneycombV8ValueImpl> impl = new HoneycombV8ValueImpl(isolate);
  impl->InitUndefined();
  return impl.get();
}

// static
HoneycombRefPtr<HoneycombV8Value> HoneycombV8Value::CreateNull() {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(nullptr);
  v8::Isolate* isolate = HoneycombV8IsolateManager::Get()->isolate();
  HoneycombRefPtr<HoneycombV8ValueImpl> impl = new HoneycombV8ValueImpl(isolate);
  impl->InitNull();
  return impl.get();
}

// static
HoneycombRefPtr<HoneycombV8Value> HoneycombV8Value::CreateBool(bool value) {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(nullptr);
  v8::Isolate* isolate = HoneycombV8IsolateManager::Get()->isolate();
  HoneycombRefPtr<HoneycombV8ValueImpl> impl = new HoneycombV8ValueImpl(isolate);
  impl->InitBool(value);
  return impl.get();
}

// static
HoneycombRefPtr<HoneycombV8Value> HoneycombV8Value::CreateInt(int32_t value) {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(nullptr);
  v8::Isolate* isolate = HoneycombV8IsolateManager::Get()->isolate();
  HoneycombRefPtr<HoneycombV8ValueImpl> impl = new HoneycombV8ValueImpl(isolate);
  impl->InitInt(value);
  return impl.get();
}

// static
HoneycombRefPtr<HoneycombV8Value> HoneycombV8Value::CreateUInt(uint32_t value) {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(nullptr);
  v8::Isolate* isolate = HoneycombV8IsolateManager::Get()->isolate();
  HoneycombRefPtr<HoneycombV8ValueImpl> impl = new HoneycombV8ValueImpl(isolate);
  impl->InitUInt(value);
  return impl.get();
}

// static
HoneycombRefPtr<HoneycombV8Value> HoneycombV8Value::CreateDouble(double value) {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(nullptr);
  v8::Isolate* isolate = HoneycombV8IsolateManager::Get()->isolate();
  HoneycombRefPtr<HoneycombV8ValueImpl> impl = new HoneycombV8ValueImpl(isolate);
  impl->InitDouble(value);
  return impl.get();
}

// static
HoneycombRefPtr<HoneycombV8Value> HoneycombV8Value::CreateDate(HoneycombBaseTime value) {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(nullptr);
  v8::Isolate* isolate = HoneycombV8IsolateManager::Get()->isolate();
  HoneycombRefPtr<HoneycombV8ValueImpl> impl = new HoneycombV8ValueImpl(isolate);
  impl->InitDate(value);
  return impl.get();
}

// static
HoneycombRefPtr<HoneycombV8Value> HoneycombV8Value::CreateString(const HoneycombString& value) {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(nullptr);
  v8::Isolate* isolate = HoneycombV8IsolateManager::Get()->isolate();
  HoneycombRefPtr<HoneycombV8ValueImpl> impl = new HoneycombV8ValueImpl(isolate);
  HoneycombString str(value);
  impl->InitString(str);
  return impl.get();
}

// static
HoneycombRefPtr<HoneycombV8Value> HoneycombV8Value::CreateObject(
    HoneycombRefPtr<HoneycombV8Accessor> accessor,
    HoneycombRefPtr<HoneycombV8Interceptor> interceptor) {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(nullptr);

  v8::Isolate* isolate = HoneycombV8IsolateManager::Get()->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    DCHECK(false) << "not currently in a V8 context";
    return nullptr;
  }

  // Create the new V8 object. If an interceptor is passed, create object from
  // template and set property handlers.
  v8::Local<v8::Object> obj;
  if (interceptor.get()) {
    v8::Local<v8::ObjectTemplate> tmpl = v8::ObjectTemplate::New(isolate);
    tmpl->SetHandler(v8::NamedPropertyHandlerConfiguration(
        InterceptorGetterCallbackImpl<v8::Local<v8::Name>>,
        InterceptorSetterCallbackImpl<v8::Local<v8::Name>>, nullptr, nullptr,
        nullptr, v8::Local<v8::Value>(),
        v8::PropertyHandlerFlags::kOnlyInterceptStrings));

    tmpl->SetIndexedPropertyHandler(InterceptorGetterCallbackImpl<uint32_t>,
                                    InterceptorSetterCallbackImpl<uint32_t>);

    v8::MaybeLocal<v8::Object> maybe_object = tmpl->NewInstance(context);
    if (!maybe_object.ToLocal<v8::Object>(&obj)) {
      DCHECK(false) << "Failed to create V8 Object with interceptor";
      return nullptr;
    }
  } else {
    obj = v8::Object::New(isolate);
  }

  // Create a tracker object that will cause the user data and/or accessor
  // and/or interceptor reference to be released when the V8 object is
  // destroyed.
  V8TrackObject* tracker = new V8TrackObject(isolate);
  tracker->SetAccessor(accessor);
  tracker->SetInterceptor(interceptor);

  // Attach the tracker object.
  tracker->AttachTo(context, obj);

  HoneycombRefPtr<HoneycombV8ValueImpl> impl = new HoneycombV8ValueImpl(isolate);
  impl->InitObject(obj, tracker);
  return impl.get();
}

// static
HoneycombRefPtr<HoneycombV8Value> HoneycombV8Value::CreateArray(int length) {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(nullptr);

  v8::Isolate* isolate = HoneycombV8IsolateManager::Get()->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    DCHECK(false) << "not currently in a V8 context";
    return nullptr;
  }

  // Create a tracker object that will cause the user data reference to be
  // released when the V8 object is destroyed.
  V8TrackObject* tracker = new V8TrackObject(isolate);

  // Create the new V8 array.
  v8::Local<v8::Array> arr = v8::Array::New(isolate, length);

  // Attach the tracker object.
  tracker->AttachTo(context, arr);

  HoneycombRefPtr<HoneycombV8ValueImpl> impl = new HoneycombV8ValueImpl(isolate);
  impl->InitObject(arr, tracker);
  return impl.get();
}

// static
HoneycombRefPtr<HoneycombV8Value> HoneycombV8Value::CreateArrayBuffer(
    void* buffer,
    size_t length,
    HoneycombRefPtr<HoneycombV8ArrayBufferReleaseCallback> release_callback) {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(nullptr);

  v8::Isolate* isolate = HoneycombV8IsolateManager::Get()->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    DCHECK(false) << "not currently in a V8 context";
    return nullptr;
  }

  // Create a tracker object that will cause the user data reference to be
  // released when the V8 object is destroyed.
  V8TrackArrayBuffer* tracker =
      new V8TrackArrayBuffer(isolate, release_callback);

  if (release_callback) {
    release_callback->AddRef();
  }

  auto deleter = [](void* data, size_t length, void* deleter_data) {
    auto* release_callback =
        reinterpret_cast<HoneycombV8ArrayBufferReleaseCallback*>(deleter_data);
    if (release_callback) {
      release_callback->ReleaseBuffer(data);
      release_callback->Release();
    }
  };

  std::unique_ptr<v8::BackingStore> backing = v8::ArrayBuffer::NewBackingStore(
      buffer, length, deleter, release_callback.get());
  v8::Local<v8::ArrayBuffer> ab =
      v8::ArrayBuffer::New(isolate, std::move(backing));

  // Attach the tracker object.
  tracker->AttachTo(context, ab);

  HoneycombRefPtr<HoneycombV8ValueImpl> impl = new HoneycombV8ValueImpl(isolate);
  impl->InitObject(ab, tracker);
  return impl.get();
}

// static
HoneycombRefPtr<HoneycombV8Value> HoneycombV8Value::CreateFunction(
    const HoneycombString& name,
    HoneycombRefPtr<HoneycombV8Handler> handler) {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(nullptr);

  if (!handler.get()) {
    DCHECK(false) << "invalid parameter";
    return nullptr;
  }

  v8::Isolate* isolate = HoneycombV8IsolateManager::Get()->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    DCHECK(false) << "not currently in a V8 context";
    return nullptr;
  }

  v8::Local<v8::External> function_data =
      V8FunctionData::Create(isolate, name, handler);

  // Create a new V8 function template.
  v8::Local<v8::FunctionTemplate> tmpl =
      v8::FunctionTemplate::New(isolate, FunctionCallbackImpl, function_data);

  // Retrieve the function object and set the name.
  v8::MaybeLocal<v8::Function> maybe_func = tmpl->GetFunction(context);
  v8::Local<v8::Function> func;
  if (!maybe_func.ToLocal(&func)) {
    DCHECK(false) << "failed to create V8 function";
    return nullptr;
  }

  func->SetName(GetV8String(isolate, name));

  // Create a tracker object that will cause the user data and/or handler
  // reference to be released when the V8 object is destroyed.
  V8TrackObject* tracker = new V8TrackObject(isolate);
  tracker->SetHandler(handler);

  // Attach the tracker object.
  tracker->AttachTo(context, func);

  // Create the HoneycombV8ValueImpl and provide a tracker object that will cause
  // the handler reference to be released when the V8 object is destroyed.
  HoneycombRefPtr<HoneycombV8ValueImpl> impl = new HoneycombV8ValueImpl(isolate);
  impl->InitObject(func, tracker);
  return impl.get();
}

// static
HoneycombRefPtr<HoneycombV8Value> HoneycombV8Value::CreatePromise() {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(nullptr);
  v8::Isolate* isolate = HoneycombV8IsolateManager::Get()->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    DCHECK(false) << "not currently in a V8 context";
    return nullptr;
  }

  v8::Local<v8::Promise::Resolver> promise_resolver =
      v8::Promise::Resolver::New(context).ToLocalChecked();

  // Create a tracker object that will cause the user data reference to be
  // released when the V8 object is destroyed.
  V8TrackObject* tracker = new V8TrackObject(isolate);

  // Attach the tracker object.
  tracker->AttachTo(context, promise_resolver);

  HoneycombRefPtr<HoneycombV8ValueImpl> impl = new HoneycombV8ValueImpl(isolate);
  impl->InitObject(promise_resolver, tracker);
  return impl.get();
}

// HoneycombV8ValueImpl

HoneycombV8ValueImpl::HoneycombV8ValueImpl(v8::Isolate* isolate)
    : isolate_(isolate), type_(TYPE_INVALID), rethrow_exceptions_(false) {
  DCHECK(isolate_);
}

HoneycombV8ValueImpl::HoneycombV8ValueImpl(v8::Isolate* isolate,
                               v8::Local<v8::Context> context,
                               v8::Local<v8::Value> value)
    : isolate_(isolate), type_(TYPE_INVALID), rethrow_exceptions_(false) {
  DCHECK(isolate_);
  InitFromV8Value(context, value);
}

HoneycombV8ValueImpl::~HoneycombV8ValueImpl() {
  if (type_ == TYPE_STRING) {
    honey_string_clear(&string_value_);
  }
  if (handle_.get()) {
    handle_->SetWeakIfNecessary();
  }
}

void HoneycombV8ValueImpl::InitFromV8Value(v8::Local<v8::Context> context,
                                     v8::Local<v8::Value> value) {
  if (value->IsUndefined()) {
    InitUndefined();
  } else if (value->IsNull()) {
    InitNull();
  } else if (value->IsTrue()) {
    InitBool(true);
  } else if (value->IsFalse()) {
    InitBool(false);
  } else if (value->IsBoolean()) {
    InitBool(value->ToBoolean(context->GetIsolate())->Value());
  } else if (value->IsInt32()) {
    InitInt(value->ToInt32(context).ToLocalChecked()->Value());
  } else if (value->IsUint32()) {
    InitUInt(value->ToUint32(context).ToLocalChecked()->Value());
  } else if (value->IsNumber()) {
    InitDouble(value->ToNumber(context).ToLocalChecked()->Value());
  } else if (value->IsDate()) {
    // Convert from milliseconds to seconds.
    InitDate(base::Time::FromJsTime(
        value->ToNumber(context).ToLocalChecked()->Value()));
  } else if (value->IsString()) {
    HoneycombString rv;
    GetHoneycombString(context->GetIsolate(),
                 value->ToString(context).ToLocalChecked(), rv);
    InitString(rv);
  } else if (value->IsObject()) {
    InitObject(value, nullptr);
  }
}

void HoneycombV8ValueImpl::InitUndefined() {
  DCHECK_EQ(type_, TYPE_INVALID);
  type_ = TYPE_UNDEFINED;
}

void HoneycombV8ValueImpl::InitNull() {
  DCHECK_EQ(type_, TYPE_INVALID);
  type_ = TYPE_NULL;
}

void HoneycombV8ValueImpl::InitBool(bool value) {
  DCHECK_EQ(type_, TYPE_INVALID);
  type_ = TYPE_BOOL;
  bool_value_ = value;
}

void HoneycombV8ValueImpl::InitInt(int32_t value) {
  DCHECK_EQ(type_, TYPE_INVALID);
  type_ = TYPE_INT;
  int_value_ = value;
}

void HoneycombV8ValueImpl::InitUInt(uint32_t value) {
  DCHECK_EQ(type_, TYPE_INVALID);
  type_ = TYPE_UINT;
  uint_value_ = value;
}

void HoneycombV8ValueImpl::InitDouble(double value) {
  DCHECK_EQ(type_, TYPE_INVALID);
  type_ = TYPE_DOUBLE;
  double_value_ = value;
}

void HoneycombV8ValueImpl::InitDate(HoneycombBaseTime value) {
  DCHECK_EQ(type_, TYPE_INVALID);
  type_ = TYPE_DATE;
  date_value_ = value;
}

void HoneycombV8ValueImpl::InitString(HoneycombString& value) {
  DCHECK_EQ(type_, TYPE_INVALID);
  type_ = TYPE_STRING;
  // Take ownership of the underling string value.
  const honey_string_t* str = value.GetStruct();
  if (str) {
    string_value_ = *str;
    honey_string_t* writable_struct = value.GetWritableStruct();
    writable_struct->str = nullptr;
    writable_struct->length = 0;
  } else {
    string_value_.str = nullptr;
    string_value_.length = 0;
  }
}

void HoneycombV8ValueImpl::InitObject(v8::Local<v8::Value> value,
                                HoneycombTrackNode* tracker) {
  DCHECK_EQ(type_, TYPE_INVALID);
  type_ = TYPE_OBJECT;
  handle_ = new Handle(isolate_, v8::Local<v8::Context>(), value, tracker);
}

v8::Local<v8::Value> HoneycombV8ValueImpl::GetV8Value(bool should_persist) {
  switch (type_) {
    case TYPE_UNDEFINED:
      return v8::Undefined(isolate_);
    case TYPE_NULL:
      return v8::Null(isolate_);
    case TYPE_BOOL:
      return v8::Boolean::New(isolate_, bool_value_);
    case TYPE_INT:
      return v8::Int32::New(isolate_, int_value_);
    case TYPE_UINT:
      return v8::Uint32::NewFromUnsigned(isolate_, uint_value_);
    case TYPE_DOUBLE:
      return v8::Number::New(isolate_, double_value_);
    case TYPE_DATE:
      // Convert from seconds to milliseconds.
      return v8::Date::New(isolate_->GetCurrentContext(),
                           static_cast<base::Time>(HoneycombBaseTime(date_value_))
                               .ToJsTimeIgnoringNull())
          .ToLocalChecked();
    case TYPE_STRING:
      return GetV8String(isolate_, HoneycombString(&string_value_));
    case TYPE_OBJECT:
      return handle_->GetNewV8Handle(should_persist);
    default:
      break;
  }

  DCHECK(false) << "Invalid type for HoneycombV8ValueImpl";
  return v8::Local<v8::Value>();
}

bool HoneycombV8ValueImpl::IsValid() {
  if (!HONEYCOMB_V8_HAS_ISOLATE() || type_ == TYPE_INVALID ||
      (type_ == TYPE_OBJECT &&
       (!handle_->BelongsToCurrentThread() || !handle_->IsValid()))) {
    return false;
  }
  return true;
}

bool HoneycombV8ValueImpl::IsUndefined() {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(false);
  return (type_ == TYPE_UNDEFINED);
}

bool HoneycombV8ValueImpl::IsNull() {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(false);
  return (type_ == TYPE_NULL);
}

bool HoneycombV8ValueImpl::IsBool() {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(false);
  return (type_ == TYPE_BOOL);
}

bool HoneycombV8ValueImpl::IsInt() {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(false);
  return (type_ == TYPE_INT || type_ == TYPE_UINT);
}

bool HoneycombV8ValueImpl::IsUInt() {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(false);
  return (type_ == TYPE_INT || type_ == TYPE_UINT);
}

bool HoneycombV8ValueImpl::IsDouble() {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(false);
  return (type_ == TYPE_INT || type_ == TYPE_UINT || type_ == TYPE_DOUBLE);
}

bool HoneycombV8ValueImpl::IsDate() {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(false);
  return (type_ == TYPE_DATE);
}

bool HoneycombV8ValueImpl::IsString() {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(false);
  return (type_ == TYPE_STRING);
}

bool HoneycombV8ValueImpl::IsObject() {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(false);
  return (type_ == TYPE_OBJECT);
}

bool HoneycombV8ValueImpl::IsArray() {
  HONEYCOMB_V8_REQUIRE_MLT_RETURN(false);
  if (type_ == TYPE_OBJECT) {
    v8::HandleScope handle_scope(handle_->isolate());
    return handle_->GetNewV8Handle(false)->IsArray();
  } else {
    return false;
  }
}

bool HoneycombV8ValueImpl::IsArrayBuffer() {
  HONEYCOMB_V8_REQUIRE_MLT_RETURN(false);
  if (type_ == TYPE_OBJECT) {
    v8::HandleScope handle_scope(handle_->isolate());
    return handle_->GetNewV8Handle(false)->IsArrayBuffer();
  } else {
    return false;
  }
}

bool HoneycombV8ValueImpl::IsFunction() {
  HONEYCOMB_V8_REQUIRE_MLT_RETURN(false);
  if (type_ == TYPE_OBJECT) {
    v8::HandleScope handle_scope(handle_->isolate());
    return handle_->GetNewV8Handle(false)->IsFunction();
  } else {
    return false;
  }
}

bool HoneycombV8ValueImpl::IsPromise() {
  HONEYCOMB_V8_REQUIRE_MLT_RETURN(false);
  if (type_ == TYPE_OBJECT) {
    v8::HandleScope handle_scope(handle_->isolate());
    return handle_->GetNewV8Handle(false)->IsPromise();
  } else {
    return false;
  }
}

bool HoneycombV8ValueImpl::IsSame(HoneycombRefPtr<HoneycombV8Value> that) {
  HONEYCOMB_V8_REQUIRE_MLT_RETURN(false);

  HoneycombV8ValueImpl* thatValue = static_cast<HoneycombV8ValueImpl*>(that.get());
  if (!thatValue || !thatValue->IsValid() || type_ != thatValue->type_) {
    return false;
  }

  switch (type_) {
    case TYPE_UNDEFINED:
    case TYPE_NULL:
      return true;
    case TYPE_BOOL:
      return (bool_value_ == thatValue->bool_value_);
    case TYPE_INT:
      return (int_value_ == thatValue->int_value_);
    case TYPE_UINT:
      return (uint_value_ == thatValue->uint_value_);
    case TYPE_DOUBLE:
      return (double_value_ == thatValue->double_value_);
    case TYPE_DATE:
      return (date_value_.val == thatValue->date_value_.val);
    case TYPE_STRING:
      return (HoneycombString(&string_value_) ==
              HoneycombString(&thatValue->string_value_));
    case TYPE_OBJECT: {
      return (handle_->GetPersistentV8Handle() ==
              thatValue->handle_->GetPersistentV8Handle());
    }
    default:
      break;
  }

  return false;
}

bool HoneycombV8ValueImpl::GetBoolValue() {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(false);
  if (type_ == TYPE_BOOL) {
    return bool_value_;
  }
  return false;
}

int32_t HoneycombV8ValueImpl::GetIntValue() {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(0);
  if (type_ == TYPE_INT || type_ == TYPE_UINT) {
    return int_value_;
  }
  return 0;
}

uint32_t HoneycombV8ValueImpl::GetUIntValue() {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(0);
  if (type_ == TYPE_INT || type_ == TYPE_UINT) {
    return uint_value_;
  }
  return 0;
}

double HoneycombV8ValueImpl::GetDoubleValue() {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(0.);
  if (type_ == TYPE_DOUBLE) {
    return double_value_;
  } else if (type_ == TYPE_INT) {
    return int_value_;
  } else if (type_ == TYPE_UINT) {
    return uint_value_;
  }
  return 0.;
}

HoneycombBaseTime HoneycombV8ValueImpl::GetDateValue() {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(HoneycombBaseTime());
  if (type_ == TYPE_DATE) {
    return date_value_;
  }
  return HoneycombBaseTime();
}

HoneycombString HoneycombV8ValueImpl::GetStringValue() {
  HoneycombString rv;
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(rv);
  if (type_ == TYPE_STRING) {
    rv = HoneycombString(&string_value_);
  }
  return rv;
}

bool HoneycombV8ValueImpl::IsUserCreated() {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(false);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    DCHECK(false) << "not currently in a V8 context";
    return false;
  }

  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject(context).ToLocalChecked();

  V8TrackObject* tracker = V8TrackObject::Unwrap(context, obj);
  return (tracker != nullptr);
}

bool HoneycombV8ValueImpl::HasException() {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(false);

  return (last_exception_.get() != nullptr);
}

HoneycombRefPtr<HoneycombV8Exception> HoneycombV8ValueImpl::GetException() {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(nullptr);

  return last_exception_;
}

bool HoneycombV8ValueImpl::ClearException() {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(false);

  last_exception_ = nullptr;
  return true;
}

bool HoneycombV8ValueImpl::WillRethrowExceptions() {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(false);

  return rethrow_exceptions_;
}

bool HoneycombV8ValueImpl::SetRethrowExceptions(bool rethrow) {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(false);

  rethrow_exceptions_ = rethrow;
  return true;
}

bool HoneycombV8ValueImpl::HasValue(const HoneycombString& key) {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(false);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    DCHECK(false) << "not currently in a V8 context";
    return false;
  }

  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject(context).ToLocalChecked();
  return obj->Has(context, GetV8String(isolate, key)).FromJust();
}

bool HoneycombV8ValueImpl::HasValue(int index) {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(false);

  if (index < 0) {
    DCHECK(false) << "invalid input parameter";
    return false;
  }

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    DCHECK(false) << "not currently in a V8 context";
    return false;
  }

  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject(context).ToLocalChecked();
  return obj->Has(context, index).FromJust();
}

bool HoneycombV8ValueImpl::DeleteValue(const HoneycombString& key) {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(false);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    DCHECK(false) << "not currently in a V8 context";
    return false;
  }

  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject(context).ToLocalChecked();

  v8::TryCatch try_catch(isolate);
  try_catch.SetVerbose(true);
  v8::Maybe<bool> del = obj->Delete(context, GetV8String(isolate, key));
  return (!HasCaught(context, try_catch) && del.FromJust());
}

bool HoneycombV8ValueImpl::DeleteValue(int index) {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(false);

  if (index < 0) {
    DCHECK(false) << "invalid input parameter";
    return false;
  }

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    DCHECK(false) << "not currently in a V8 context";
    return false;
  }

  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject(context).ToLocalChecked();

  v8::TryCatch try_catch(isolate);
  try_catch.SetVerbose(true);
  v8::Maybe<bool> del = obj->Delete(context, index);
  return (!HasCaught(context, try_catch) && del.FromJust());
}

HoneycombRefPtr<HoneycombV8Value> HoneycombV8ValueImpl::GetValue(const HoneycombString& key) {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(nullptr);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    DCHECK(false) << "not currently in a V8 context";
    return nullptr;
  }

  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject(context).ToLocalChecked();

  v8::TryCatch try_catch(isolate);
  try_catch.SetVerbose(true);
  v8::MaybeLocal<v8::Value> ret_value =
      obj->Get(context, GetV8String(isolate, key));
  if (!HasCaught(context, try_catch) && !ret_value.IsEmpty()) {
    return new HoneycombV8ValueImpl(isolate, context, ret_value.ToLocalChecked());
  }
  return nullptr;
}

HoneycombRefPtr<HoneycombV8Value> HoneycombV8ValueImpl::GetValue(int index) {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(nullptr);

  if (index < 0) {
    DCHECK(false) << "invalid input parameter";
    return nullptr;
  }

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    DCHECK(false) << "not currently in a V8 context";
    return nullptr;
  }

  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject(context).ToLocalChecked();

  v8::TryCatch try_catch(isolate);
  try_catch.SetVerbose(true);
  v8::MaybeLocal<v8::Value> ret_value =
      obj->Get(context, v8::Number::New(isolate, index));
  if (!HasCaught(context, try_catch) && !ret_value.IsEmpty()) {
    return new HoneycombV8ValueImpl(isolate, context, ret_value.ToLocalChecked());
  }
  return nullptr;
}

bool HoneycombV8ValueImpl::SetValue(const HoneycombString& key,
                              HoneycombRefPtr<HoneycombV8Value> value,
                              PropertyAttribute attribute) {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(false);

  HoneycombV8ValueImpl* impl = static_cast<HoneycombV8ValueImpl*>(value.get());
  if (impl && impl->IsValid()) {
    v8::Isolate* isolate = handle_->isolate();
    v8::HandleScope handle_scope(isolate);

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
      DCHECK(false) << "not currently in a V8 context";
      return false;
    }

    v8::Local<v8::Value> v8value = handle_->GetNewV8Handle(false);
    v8::Local<v8::Object> obj = v8value->ToObject(context).ToLocalChecked();

    v8::TryCatch try_catch(isolate);
    try_catch.SetVerbose(true);
    // TODO(honey): This usage may not exactly match the previous implementation.
    // Set will trigger interceptors and/or accessors whereas DefineOwnProperty
    // will not. It might be better to split this functionality into separate
    // methods.
    if (attribute == V8_PROPERTY_ATTRIBUTE_NONE) {
      v8::Maybe<bool> set =
          obj->Set(context, GetV8String(isolate, key), impl->GetV8Value(true));
      return (!HasCaught(context, try_catch) && set.FromJust());
    } else {
      v8::Maybe<bool> set = obj->DefineOwnProperty(
          context, GetV8String(isolate, key), impl->GetV8Value(true),
          static_cast<v8::PropertyAttribute>(attribute));
      return (!HasCaught(context, try_catch) && set.FromJust());
    }
  } else {
    DCHECK(false) << "invalid input parameter";
    return false;
  }
}

bool HoneycombV8ValueImpl::SetValue(int index, HoneycombRefPtr<HoneycombV8Value> value) {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(false);

  if (index < 0) {
    DCHECK(false) << "invalid input parameter";
    return false;
  }

  HoneycombV8ValueImpl* impl = static_cast<HoneycombV8ValueImpl*>(value.get());
  if (impl && impl->IsValid()) {
    v8::Isolate* isolate = handle_->isolate();
    v8::HandleScope handle_scope(isolate);

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
      DCHECK(false) << "not currently in a V8 context";
      return false;
    }

    v8::Local<v8::Value> v8value = handle_->GetNewV8Handle(false);
    v8::Local<v8::Object> obj = v8value->ToObject(context).ToLocalChecked();

    v8::TryCatch try_catch(isolate);
    try_catch.SetVerbose(true);
    v8::Maybe<bool> set = obj->Set(context, index, impl->GetV8Value(true));
    return (!HasCaught(context, try_catch) && set.FromJust());
  } else {
    DCHECK(false) << "invalid input parameter";
    return false;
  }
}

bool HoneycombV8ValueImpl::SetValue(const HoneycombString& key,
                              AccessControl settings,
                              PropertyAttribute attribute) {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(false);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    DCHECK(false) << "not currently in a V8 context";
    return false;
  }

  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject(context).ToLocalChecked();

  HoneycombRefPtr<HoneycombV8Accessor> accessorPtr;

  V8TrackObject* tracker = V8TrackObject::Unwrap(context, obj);
  if (tracker) {
    accessorPtr = tracker->GetAccessor();
  }

  // Verify that an accessor exists for this object.
  if (!accessorPtr.get()) {
    return false;
  }

  v8::AccessorNameGetterCallback getter = AccessorNameGetterCallbackImpl;
  v8::AccessorNameSetterCallback setter =
      (attribute & V8_PROPERTY_ATTRIBUTE_READONLY)
          ? nullptr
          : AccessorNameSetterCallbackImpl;

  v8::TryCatch try_catch(isolate);
  try_catch.SetVerbose(true);
  v8::Maybe<bool> set =
      obj->SetAccessor(context, GetV8String(isolate, key), getter, setter, obj,
                       static_cast<v8::AccessControl>(settings),
                       static_cast<v8::PropertyAttribute>(attribute));
  return (!HasCaught(context, try_catch) && set.FromJust());
}

bool HoneycombV8ValueImpl::GetKeys(std::vector<HoneycombString>& keys) {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(false);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    DCHECK(false) << "not currently in a V8 context";
    return false;
  }

  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject(context).ToLocalChecked();
  v8::Local<v8::Array> arr_keys =
      obj->GetPropertyNames(context).ToLocalChecked();

  uint32_t len = arr_keys->Length();
  for (uint32_t i = 0; i < len; ++i) {
    v8::Local<v8::Value> arr_value =
        arr_keys->Get(context, v8::Integer::New(isolate, i)).ToLocalChecked();
    HoneycombString str;
    GetHoneycombString(isolate, arr_value->ToString(context).ToLocalChecked(), str);
    keys.push_back(str);
  }
  return true;
}

bool HoneycombV8ValueImpl::SetUserData(HoneycombRefPtr<HoneycombBaseRefCounted> user_data) {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(false);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    DCHECK(false) << "not currently in a V8 context";
    return false;
  }

  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject(context).ToLocalChecked();

  V8TrackObject* tracker = V8TrackObject::Unwrap(context, obj);
  if (tracker) {
    tracker->SetUserData(user_data);
    return true;
  }

  return false;
}

HoneycombRefPtr<HoneycombBaseRefCounted> HoneycombV8ValueImpl::GetUserData() {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(nullptr);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    DCHECK(false) << "not currently in a V8 context";
    return nullptr;
  }

  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject(context).ToLocalChecked();

  V8TrackObject* tracker = V8TrackObject::Unwrap(context, obj);
  if (tracker) {
    return tracker->GetUserData();
  }

  return nullptr;
}

int HoneycombV8ValueImpl::GetExternallyAllocatedMemory() {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(0);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    DCHECK(false) << "not currently in a V8 context";
    return 0;
  }

  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject(context).ToLocalChecked();

  V8TrackObject* tracker = V8TrackObject::Unwrap(context, obj);
  if (tracker) {
    return tracker->GetExternallyAllocatedMemory();
  }

  return 0;
}

int HoneycombV8ValueImpl::AdjustExternallyAllocatedMemory(int change_in_bytes) {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(0);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    DCHECK(false) << "not currently in a V8 context";
    return 0;
  }

  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject(context).ToLocalChecked();

  V8TrackObject* tracker = V8TrackObject::Unwrap(context, obj);
  if (tracker) {
    return tracker->AdjustExternallyAllocatedMemory(change_in_bytes);
  }

  return 0;
}

int HoneycombV8ValueImpl::GetArrayLength() {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(0);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    DCHECK(false) << "not currently in a V8 context";
    return 0;
  }

  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  if (!value->IsArray()) {
    DCHECK(false) << "V8 value is not an array";
    return 0;
  }

  v8::Local<v8::Object> obj = value->ToObject(context).ToLocalChecked();
  v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(obj);
  return arr->Length();
}

HoneycombRefPtr<HoneycombV8ArrayBufferReleaseCallback>
HoneycombV8ValueImpl::GetArrayBufferReleaseCallback() {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(nullptr);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    DCHECK(false) << "not currently in a V8 context";
    return nullptr;
  }

  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  if (!value->IsArrayBuffer()) {
    DCHECK(false) << "V8 value is not an array buffer";
    return nullptr;
  }

  v8::Local<v8::Object> obj = value->ToObject(context).ToLocalChecked();

  V8TrackArrayBuffer* tracker = V8TrackArrayBuffer::Unwrap(context, obj);
  if (tracker) {
    return tracker->GetReleaseCallback();
  }

  return nullptr;
}

bool HoneycombV8ValueImpl::NeuterArrayBuffer() {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(0);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    DCHECK(false) << "not currently in a V8 context";
    return false;
  }

  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  if (!value->IsArrayBuffer()) {
    DCHECK(false) << "V8 value is not an array buffer";
    return false;
  }
  v8::Local<v8::Object> obj = value->ToObject(context).ToLocalChecked();
  v8::Local<v8::ArrayBuffer> arr = v8::Local<v8::ArrayBuffer>::Cast(obj);
  if (!arr->IsDetachable()) {
    return false;
  }
  arr->Detach();

  return true;
}

HoneycombString HoneycombV8ValueImpl::GetFunctionName() {
  HoneycombString rv;
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(rv);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    DCHECK(false) << "not currently in a V8 context";
    return rv;
  }

  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  if (!value->IsFunction()) {
    DCHECK(false) << "V8 value is not a function";
    return rv;
  }

  v8::Local<v8::Object> obj = value->ToObject(context).ToLocalChecked();
  v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(obj);
  GetHoneycombString(handle_->isolate(),
               v8::Handle<v8::String>::Cast(func->GetName()), rv);
  return rv;
}

HoneycombRefPtr<HoneycombV8Handler> HoneycombV8ValueImpl::GetFunctionHandler() {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(nullptr);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    DCHECK(false) << "not currently in a V8 context";
    return nullptr;
  }

  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  if (!value->IsFunction()) {
    DCHECK(false) << "V8 value is not a function";
    return nullptr;
  }

  v8::Local<v8::Object> obj = value->ToObject(context).ToLocalChecked();
  V8TrackObject* tracker = V8TrackObject::Unwrap(context, obj);
  if (tracker) {
    return tracker->GetHandler();
  }

  return nullptr;
}

HoneycombRefPtr<HoneycombV8Value> HoneycombV8ValueImpl::ExecuteFunction(
    HoneycombRefPtr<HoneycombV8Value> object,
    const HoneycombV8ValueList& arguments) {
  // An empty context value defaults to the current context.
  HoneycombRefPtr<HoneycombV8Context> context;
  return ExecuteFunctionWithContext(context, object, arguments);
}

HoneycombRefPtr<HoneycombV8Value> HoneycombV8ValueImpl::ExecuteFunctionWithContext(
    HoneycombRefPtr<HoneycombV8Context> context,
    HoneycombRefPtr<HoneycombV8Value> object,
    const HoneycombV8ValueList& arguments) {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(nullptr);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  if (!value->IsFunction()) {
    DCHECK(false) << "V8 value is not a function";
    return nullptr;
  }

  if (context.get() && !context->IsValid()) {
    DCHECK(false) << "invalid V8 context parameter";
    return nullptr;
  }
  if (object.get() && (!object->IsValid() || !object->IsObject())) {
    DCHECK(false) << "invalid V8 object parameter";
    return nullptr;
  }

  int argc = arguments.size();
  if (argc > 0) {
    for (int i = 0; i < argc; ++i) {
      if (!arguments[i].get() || !arguments[i]->IsValid()) {
        DCHECK(false) << "invalid V8 arguments parameter";
        return nullptr;
      }
    }
  }

  v8::Local<v8::Context> context_local;
  if (context.get()) {
    HoneycombV8ContextImpl* context_impl =
        static_cast<HoneycombV8ContextImpl*>(context.get());
    context_local = context_impl->GetV8Context();
  } else {
    context_local = isolate->GetCurrentContext();
  }

  v8::Context::Scope context_scope(context_local);

  v8::Local<v8::Object> obj = value->ToObject(context_local).ToLocalChecked();
  v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(obj);
  v8::Local<v8::Object> recv;

  // Default to the global object if no object was provided.
  if (object.get()) {
    HoneycombV8ValueImpl* recv_impl = static_cast<HoneycombV8ValueImpl*>(object.get());
    recv = v8::Local<v8::Object>::Cast(recv_impl->GetV8Value(true));
  } else {
    recv = context_local->Global();
  }

  v8::Local<v8::Value>* argv = nullptr;
  if (argc > 0) {
    argv = new v8::Local<v8::Value>[argc];
    for (int i = 0; i < argc; ++i) {
      argv[i] =
          static_cast<HoneycombV8ValueImpl*>(arguments[i].get())->GetV8Value(true);
    }
  }

  HoneycombRefPtr<HoneycombV8Value> retval;

  {
    v8::TryCatch try_catch(isolate);
    try_catch.SetVerbose(true);

    v8::MaybeLocal<v8::Value> func_rv = blink_glue::CallV8Function(
        context_local, func, recv, argc, argv, handle_->isolate());

    if (!HasCaught(context_local, try_catch) && !func_rv.IsEmpty()) {
      retval =
          new HoneycombV8ValueImpl(isolate, context_local, func_rv.ToLocalChecked());
    }
  }

  if (argv) {
    delete[] argv;
  }

  return retval;
}

bool HoneycombV8ValueImpl::ResolvePromise(HoneycombRefPtr<HoneycombV8Value> arg) {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(false);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  if (!value->IsPromise()) {
    DCHECK(false) << "V8 value is not a Promise";
    return false;
  }

  if (arg.get() && !arg->IsValid()) {
    DCHECK(false) << "invalid V8 arg parameter";
    return false;
  }

  v8::Local<v8::Context> context_local = isolate->GetCurrentContext();

  v8::Context::Scope context_scope(context_local);

  v8::Local<v8::Object> obj = value->ToObject(context_local).ToLocalChecked();
  v8::Local<v8::Promise::Resolver> promise =
      v8::Local<v8::Promise::Resolver>::Cast(obj);

  v8::TryCatch try_catch(isolate);
  try_catch.SetVerbose(true);

  if (arg.get()) {
    promise
        ->Resolve(context_local,
                  static_cast<HoneycombV8ValueImpl*>(arg.get())->GetV8Value(true))
        .ToChecked();
  } else {
    promise->Resolve(context_local, v8::Undefined(isolate)).ToChecked();
  }

  return !HasCaught(context_local, try_catch);
}

bool HoneycombV8ValueImpl::RejectPromise(const HoneycombString& errorMsg) {
  HONEYCOMB_V8_REQUIRE_OBJECT_RETURN(false);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  if (!value->IsPromise()) {
    DCHECK(false) << "V8 value is not a Promise";
    return false;
  }

  v8::Local<v8::Context> context_local = isolate->GetCurrentContext();

  v8::Context::Scope context_scope(context_local);

  v8::Local<v8::Object> obj = value->ToObject(context_local).ToLocalChecked();
  v8::Local<v8::Promise::Resolver> promise =
      v8::Local<v8::Promise::Resolver>::Cast(obj);

  v8::TryCatch try_catch(isolate);
  try_catch.SetVerbose(true);

  promise
      ->Reject(context_local,
               v8::Exception::Error(GetV8String(isolate, errorMsg)))
      .ToChecked();

  return !HasCaught(context_local, try_catch);
}

bool HoneycombV8ValueImpl::HasCaught(v8::Local<v8::Context> context,
                               v8::TryCatch& try_catch) {
  if (try_catch.HasCaught()) {
    last_exception_ = new HoneycombV8ExceptionImpl(context, try_catch.Message());
    if (rethrow_exceptions_) {
      try_catch.ReThrow();
    }
    return true;
  } else {
    if (last_exception_.get()) {
      last_exception_ = nullptr;
    }
    return false;
  }
}

// HoneycombV8StackTrace

// static
HoneycombRefPtr<HoneycombV8StackTrace> HoneycombV8StackTrace::GetCurrent(int frame_limit) {
  HONEYCOMB_V8_REQUIRE_ISOLATE_RETURN(nullptr);

  v8::Isolate* isolate = HoneycombV8IsolateManager::Get()->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::StackTrace> stackTrace = v8::StackTrace::CurrentStackTrace(
      isolate, frame_limit, v8::StackTrace::kDetailed);
  if (stackTrace.IsEmpty()) {
    return nullptr;
  }
  return new HoneycombV8StackTraceImpl(isolate, stackTrace);
}

// HoneycombV8StackTraceImpl

HoneycombV8StackTraceImpl::HoneycombV8StackTraceImpl(v8::Isolate* isolate,
                                         v8::Local<v8::StackTrace> handle) {
  if (!handle.IsEmpty()) {
    int frame_count = handle->GetFrameCount();
    if (frame_count > 0) {
      frames_.reserve(frame_count);
      for (int i = 0; i < frame_count; ++i) {
        frames_.push_back(
            new HoneycombV8StackFrameImpl(isolate, handle->GetFrame(isolate, i)));
      }
    }
  }
}

HoneycombV8StackTraceImpl::~HoneycombV8StackTraceImpl() {}

bool HoneycombV8StackTraceImpl::IsValid() {
  return true;
}

int HoneycombV8StackTraceImpl::GetFrameCount() {
  return frames_.size();
}

HoneycombRefPtr<HoneycombV8StackFrame> HoneycombV8StackTraceImpl::GetFrame(int index) {
  if (index < 0 || index >= static_cast<int>(frames_.size())) {
    return nullptr;
  }
  return frames_[index];
}

// HoneycombV8StackFrameImpl

HoneycombV8StackFrameImpl::HoneycombV8StackFrameImpl(v8::Isolate* isolate,
                                         v8::Local<v8::StackFrame> handle)
    : line_number_(0), column_(0), is_eval_(false), is_constructor_(false) {
  if (handle.IsEmpty()) {
    return;
  }
  GetHoneycombString(isolate, handle->GetScriptName(), script_name_);
  GetHoneycombString(isolate, handle->GetScriptNameOrSourceURL(),
               script_name_or_source_url_);
  GetHoneycombString(isolate, handle->GetFunctionName(), function_name_);
  line_number_ = handle->GetLineNumber();
  column_ = handle->GetColumn();
  is_eval_ = handle->IsEval();
  is_constructor_ = handle->IsConstructor();
}

HoneycombV8StackFrameImpl::~HoneycombV8StackFrameImpl() {}

bool HoneycombV8StackFrameImpl::IsValid() {
  return true;
}

HoneycombString HoneycombV8StackFrameImpl::GetScriptName() {
  return script_name_;
}

HoneycombString HoneycombV8StackFrameImpl::GetScriptNameOrSourceURL() {
  return script_name_or_source_url_;
}

HoneycombString HoneycombV8StackFrameImpl::GetFunctionName() {
  return function_name_;
}

int HoneycombV8StackFrameImpl::GetLineNumber() {
  return line_number_;
}

int HoneycombV8StackFrameImpl::GetColumn() {
  return column_;
}

bool HoneycombV8StackFrameImpl::IsEval() {
  return is_eval_;
}

bool HoneycombV8StackFrameImpl::IsConstructor() {
  return is_constructor_;
}

// Enable deprecation warnings on Windows. See http://crbug.com/585142.
#if BUILDFLAG(IS_WIN)
#if defined(__clang__)
#pragma GCC diagnostic pop
#else
#pragma warning(pop)
#endif
#endif
