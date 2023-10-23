// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_RENDERER_V8_IMPL_H_
#define HONEYCOMB_LIBHONEY_RENDERER_V8_IMPL_H_
#pragma once

#include <vector>

#include "include/honey_v8.h"
#include "libhoney/common/tracker.h"

#include "base/location.h"
#include "base/logging.h"
#include "base/memory/ref_counted.h"
#include "base/task/single_thread_task_runner.h"
#include "v8/include/v8.h"

class HoneycombTrackNode;
class GURL;

namespace blink {
class WebLocalFrame;
}

// Call after a V8 Isolate has been created and entered for the first time.
void HoneycombV8IsolateCreated();

// Call before a V8 Isolate is exited and destroyed.
void HoneycombV8IsolateDestroyed();

// Call to detach all handles associated with the specified context.
void HoneycombV8ReleaseContext(v8::Local<v8::Context> context);

// Set the stack size for uncaught exceptions.
void HoneycombV8SetUncaughtExceptionStackSize(int stack_size);

// Set attributes associated with a WebWorker thread.
void HoneycombV8SetWorkerAttributes(int worker_id, const GURL& worker_url);

// Used to detach handles when the associated context is released.
class HoneycombV8ContextState : public base::RefCounted<HoneycombV8ContextState> {
 public:
  HoneycombV8ContextState() : valid_(true) {}

  bool IsValid() { return valid_; }
  void Detach() {
    DCHECK(valid_);
    valid_ = false;
    track_manager_.DeleteAll();
  }

  void AddTrackObject(HoneycombTrackNode* object) {
    DCHECK(valid_);
    track_manager_.Add(object);
  }

  void DeleteTrackObject(HoneycombTrackNode* object) {
    DCHECK(valid_);
    track_manager_.Delete(object);
  }

 private:
  friend class base::RefCounted<HoneycombV8ContextState>;

  ~HoneycombV8ContextState() {}

  bool valid_;
  HoneycombTrackManager track_manager_;
};

// Use this template in conjuction with RefCountedThreadSafe to ensure that a
// V8 object is deleted on the correct thread.
struct HoneycombV8DeleteOnMessageLoopThread {
  template <typename T>
  static void Destruct(const T* x) {
    if (x->task_runner()->RunsTasksInCurrentSequence()) {
      delete x;
    } else {
      if (!x->task_runner()->DeleteSoon(FROM_HERE, x)) {
#if defined(UNIT_TEST)
        // Only logged under unit testing because leaks at shutdown
        // are acceptable under normal circumstances.
        LOG(ERROR) << "DeleteSoon failed on thread " << thread;
#endif  // UNIT_TEST
      }
    }
  }
};

// Base class for V8 Handle types.
class HoneycombV8HandleBase
    : public base::RefCountedThreadSafe<HoneycombV8HandleBase,
                                        HoneycombV8DeleteOnMessageLoopThread> {
 public:
  // Returns true if there is no underlying context or if the underlying context
  // is valid.
  bool IsValid() const {
    return (!context_state_.get() || context_state_->IsValid());
  }

  bool BelongsToCurrentThread() const;

  v8::Isolate* isolate() const { return isolate_; }
  scoped_refptr<base::SingleThreadTaskRunner> task_runner() const {
    return task_runner_;
  }

 protected:
  friend class base::DeleteHelper<HoneycombV8HandleBase>;
  friend class base::RefCountedThreadSafe<HoneycombV8HandleBase,
                                          HoneycombV8DeleteOnMessageLoopThread>;
  friend struct HoneycombV8DeleteOnMessageLoopThread;

  // |context| is the context that owns this handle. If empty the current
  // context will be used.
  HoneycombV8HandleBase(v8::Isolate* isolate, v8::Local<v8::Context> context);
  virtual ~HoneycombV8HandleBase();

 protected:
  v8::Isolate* isolate_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  scoped_refptr<HoneycombV8ContextState> context_state_;
};

// Template for V8 Handle types. This class is used to ensure that V8 objects
// are only released on the render thread.
template <typename v8class>
class HoneycombV8Handle : public HoneycombV8HandleBase {
 public:
  using handleType = v8::Local<v8class>;
  using persistentType = v8::Persistent<v8class>;

  HoneycombV8Handle(v8::Isolate* isolate,
              v8::Local<v8::Context> context,
              handleType v)
      : HoneycombV8HandleBase(isolate, context), handle_(isolate, v) {}

  HoneycombV8Handle(const HoneycombV8Handle&) = delete;
  HoneycombV8Handle& operator=(const HoneycombV8Handle&) = delete;

  handleType GetNewV8Handle() {
    DCHECK(IsValid());
    return handleType::New(isolate(), handle_);
  }

  persistentType& GetPersistentV8Handle() { return handle_; }

 protected:
  ~HoneycombV8Handle() override { handle_.Reset(); }

  persistentType handle_;
};

// Specialization for v8::Value with empty implementation to avoid incorrect
// usage.
template <>
class HoneycombV8Handle<v8::Value> {};

class HoneycombV8ContextImpl : public HoneycombV8Context {
 public:
  HoneycombV8ContextImpl(v8::Isolate* isolate, v8::Local<v8::Context> context);

  HoneycombV8ContextImpl(const HoneycombV8ContextImpl&) = delete;
  HoneycombV8ContextImpl& operator=(const HoneycombV8ContextImpl&) = delete;

  ~HoneycombV8ContextImpl() override;

  HoneycombRefPtr<HoneycombTaskRunner> GetTaskRunner() override;
  bool IsValid() override;
  HoneycombRefPtr<HoneycombBrowser> GetBrowser() override;
  HoneycombRefPtr<HoneycombFrame> GetFrame() override;
  HoneycombRefPtr<HoneycombV8Value> GetGlobal() override;
  bool Enter() override;
  bool Exit() override;
  bool IsSame(HoneycombRefPtr<HoneycombV8Context> that) override;
  bool Eval(const HoneycombString& code,
            const HoneycombString& script_url,
            int start_line,
            HoneycombRefPtr<HoneycombV8Value>& retval,
            HoneycombRefPtr<HoneycombV8Exception>& exception) override;

  v8::Local<v8::Context> GetV8Context();
  blink::WebLocalFrame* GetWebFrame();

 private:
  using Handle = HoneycombV8Handle<v8::Context>;
  scoped_refptr<Handle> handle_;
  v8::MicrotaskQueue* const microtask_queue_;

  int enter_count_ = 0;
  std::unique_ptr<v8::MicrotasksScope> microtasks_scope_;

  IMPLEMENT_REFCOUNTING(HoneycombV8ContextImpl);
};

class HoneycombV8ValueImpl : public HoneycombV8Value {
 public:
  explicit HoneycombV8ValueImpl(v8::Isolate* isolate);
  HoneycombV8ValueImpl(v8::Isolate* isolate,
                 v8::Local<v8::Context> context,
                 v8::Local<v8::Value> value);

  HoneycombV8ValueImpl(const HoneycombV8ValueImpl&) = delete;
  HoneycombV8ValueImpl& operator=(const HoneycombV8ValueImpl&) = delete;

  ~HoneycombV8ValueImpl() override;

  // Used for initializing the HoneycombV8ValueImpl. Should be called a single time
  // after the HoneycombV8ValueImpl is created.
  void InitFromV8Value(v8::Local<v8::Context> context,
                       v8::Local<v8::Value> value);
  void InitUndefined();
  void InitNull();
  void InitBool(bool value);
  void InitInt(int32_t value);
  void InitUInt(uint32_t value);
  void InitDouble(double value);
  void InitDate(HoneycombBaseTime value);
  void InitString(HoneycombString& value);
  void InitObject(v8::Local<v8::Value> value, HoneycombTrackNode* tracker);

  // Creates a new V8 value for the underlying value or returns the existing
  // object handle.
  v8::Local<v8::Value> GetV8Value(bool should_persist);

  bool IsValid() override;
  bool IsUndefined() override;
  bool IsNull() override;
  bool IsBool() override;
  bool IsInt() override;
  bool IsUInt() override;
  bool IsDouble() override;
  bool IsDate() override;
  bool IsString() override;
  bool IsObject() override;
  bool IsArray() override;
  bool IsArrayBuffer() override;
  bool IsFunction() override;
  bool IsPromise() override;
  bool IsSame(HoneycombRefPtr<HoneycombV8Value> value) override;
  bool GetBoolValue() override;
  int32_t GetIntValue() override;
  uint32_t GetUIntValue() override;
  double GetDoubleValue() override;
  HoneycombBaseTime GetDateValue() override;
  HoneycombString GetStringValue() override;
  bool IsUserCreated() override;
  bool HasException() override;
  HoneycombRefPtr<HoneycombV8Exception> GetException() override;
  bool ClearException() override;
  bool WillRethrowExceptions() override;
  bool SetRethrowExceptions(bool rethrow) override;
  bool HasValue(const HoneycombString& key) override;
  bool HasValue(int index) override;
  bool DeleteValue(const HoneycombString& key) override;
  bool DeleteValue(int index) override;
  HoneycombRefPtr<HoneycombV8Value> GetValue(const HoneycombString& key) override;
  HoneycombRefPtr<HoneycombV8Value> GetValue(int index) override;
  bool SetValue(const HoneycombString& key,
                HoneycombRefPtr<HoneycombV8Value> value,
                PropertyAttribute attribute) override;
  bool SetValue(int index, HoneycombRefPtr<HoneycombV8Value> value) override;
  bool SetValue(const HoneycombString& key,
                AccessControl settings,
                PropertyAttribute attribute) override;
  bool GetKeys(std::vector<HoneycombString>& keys) override;
  bool SetUserData(HoneycombRefPtr<HoneycombBaseRefCounted> user_data) override;
  HoneycombRefPtr<HoneycombBaseRefCounted> GetUserData() override;
  int GetExternallyAllocatedMemory() override;
  int AdjustExternallyAllocatedMemory(int change_in_bytes) override;
  int GetArrayLength() override;
  HoneycombRefPtr<HoneycombV8ArrayBufferReleaseCallback> GetArrayBufferReleaseCallback()
      override;
  bool NeuterArrayBuffer() override;
  HoneycombString GetFunctionName() override;
  HoneycombRefPtr<HoneycombV8Handler> GetFunctionHandler() override;
  HoneycombRefPtr<HoneycombV8Value> ExecuteFunction(
      HoneycombRefPtr<HoneycombV8Value> object,
      const HoneycombV8ValueList& arguments) override;
  HoneycombRefPtr<HoneycombV8Value> ExecuteFunctionWithContext(
      HoneycombRefPtr<HoneycombV8Context> context,
      HoneycombRefPtr<HoneycombV8Value> object,
      const HoneycombV8ValueList& arguments) override;

  bool ResolvePromise(HoneycombRefPtr<HoneycombV8Value> arg) override;
  bool RejectPromise(const HoneycombString& errorMsg) override;

 private:
  // Test for and record any exception.
  bool HasCaught(v8::Local<v8::Context> context, v8::TryCatch& try_catch);

  class Handle : public HoneycombV8HandleBase {
   public:
    using handleType = v8::Local<v8::Value>;
    using persistentType = v8::Persistent<v8::Value>;

    Handle(v8::Isolate* isolate,
           v8::Local<v8::Context> context,
           handleType v,
           HoneycombTrackNode* tracker);

    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;

    handleType GetNewV8Handle(bool should_persist);

    persistentType& GetPersistentV8Handle();

    void SetWeakIfNecessary();

   private:
    ~Handle() override;

   private:
    // Callbacks for weak persistent reference destruction.
    static void FirstWeakCallback(const v8::WeakCallbackInfo<Handle>& data);
    static void SecondWeakCallback(const v8::WeakCallbackInfo<Handle>& data);

    persistentType handle_;

    // For Object and Function types, we need to hold on to a reference to their
    // internal data or function handler objects that are reference counted.
    HoneycombTrackNode* tracker_;

    // True if the handle needs to persist due to it being passed into V8.
    bool should_persist_;

    // True if the handle has been set as weak.
    bool is_set_weak_;
  };

  v8::Isolate* isolate_;

  enum {
    TYPE_INVALID = 0,
    TYPE_UNDEFINED,
    TYPE_NULL,
    TYPE_BOOL,
    TYPE_INT,
    TYPE_UINT,
    TYPE_DOUBLE,
    TYPE_DATE,
    TYPE_STRING,
    TYPE_OBJECT,
  } type_;

  union {
    bool bool_value_;
    int32_t int_value_;
    uint32_t uint_value_;
    double double_value_;
    honey_basetime_t date_value_;
    honey_string_t string_value_;
  };

  // Used with Object, Function and Array types.
  scoped_refptr<Handle> handle_;

  HoneycombRefPtr<HoneycombV8Exception> last_exception_;
  bool rethrow_exceptions_;

  IMPLEMENT_REFCOUNTING(HoneycombV8ValueImpl);
};

class HoneycombV8StackTraceImpl : public HoneycombV8StackTrace {
 public:
  HoneycombV8StackTraceImpl(v8::Isolate* isolate, v8::Local<v8::StackTrace> handle);

  HoneycombV8StackTraceImpl(const HoneycombV8StackTraceImpl&) = delete;
  HoneycombV8StackTraceImpl& operator=(const HoneycombV8StackTraceImpl&) = delete;

  ~HoneycombV8StackTraceImpl() override;

  bool IsValid() override;
  int GetFrameCount() override;
  HoneycombRefPtr<HoneycombV8StackFrame> GetFrame(int index) override;

 private:
  std::vector<HoneycombRefPtr<HoneycombV8StackFrame>> frames_;

  IMPLEMENT_REFCOUNTING(HoneycombV8StackTraceImpl);
};

class HoneycombV8StackFrameImpl : public HoneycombV8StackFrame {
 public:
  HoneycombV8StackFrameImpl(v8::Isolate* isolate, v8::Local<v8::StackFrame> handle);

  HoneycombV8StackFrameImpl(const HoneycombV8StackFrameImpl&) = delete;
  HoneycombV8StackFrameImpl& operator=(const HoneycombV8StackFrameImpl&) = delete;

  ~HoneycombV8StackFrameImpl() override;

  bool IsValid() override;
  HoneycombString GetScriptName() override;
  HoneycombString GetScriptNameOrSourceURL() override;
  HoneycombString GetFunctionName() override;
  int GetLineNumber() override;
  int GetColumn() override;
  bool IsEval() override;
  bool IsConstructor() override;

 private:
  HoneycombString script_name_;
  HoneycombString script_name_or_source_url_;
  HoneycombString function_name_;
  int line_number_;
  int column_;
  bool is_eval_;
  bool is_constructor_;

  IMPLEMENT_REFCOUNTING(HoneycombV8StackFrameImpl);
};

#endif  // HONEYCOMB_LIBHONEY_RENDERER_V8_IMPL_H_
