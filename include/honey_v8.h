// Copyright (c) 2013 Marshall A. Greenblatt. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the name Chromium Embedded
// Framework nor the names of its contributors may be used to endorse
// or promote products derived from this software without specific prior
// written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ---------------------------------------------------------------------------
//
// The contents of this file must follow a specific format in order to
// support the Honeycomb translator tool. See the translator.README.txt file in the
// tools directory for more information.
//

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_V8_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_V8_H_
#pragma once

#include <vector>
#include "include/honey_base.h"
#include "include/honey_browser.h"
#include "include/honey_frame.h"
#include "include/honey_task.h"

class HoneycombV8Exception;
class HoneycombV8Handler;
class HoneycombV8StackFrame;
class HoneycombV8Value;

///
/// Register a new V8 extension with the specified JavaScript extension code and
/// handler. Functions implemented by the handler are prototyped using the
/// keyword 'native'. The calling of a native function is restricted to the
/// scope in which the prototype of the native function is defined. This
/// function may only be called on the render process main thread.
///
/// Example JavaScript extension code:
/// <pre>
///   // create the 'example' global object if it doesn't already exist.
///   if (!example)
///     example = {};
///   // create the 'example.test' global object if it doesn't already exist.
///   if (!example.test)
///     example.test = {};
///   (function() {
///     // Define the function 'example.test.myfunction'.
///     example.test.myfunction = function() {
///       // Call HoneycombV8Handler::Execute() with the function name 'MyFunction'
///       // and no arguments.
///       native function MyFunction();
///       return MyFunction();
///     };
///     // Define the getter function for parameter 'example.test.myparam'.
///     example.test.__defineGetter__('myparam', function() {
///       // Call HoneycombV8Handler::Execute() with the function name 'GetMyParam'
///       // and no arguments.
///       native function GetMyParam();
///       return GetMyParam();
///     });
///     // Define the setter function for parameter 'example.test.myparam'.
///     example.test.__defineSetter__('myparam', function(b) {
///       // Call HoneycombV8Handler::Execute() with the function name 'SetMyParam'
///       // and a single argument.
///       native function SetMyParam();
///       if(b) SetMyParam(b);
///     });
///
///     // Extension definitions can also contain normal JavaScript variables
///     // and functions.
///     var myint = 0;
///     example.test.increment = function() {
///       myint += 1;
///       return myint;
///     };
///   })();
/// </pre>
///
/// Example usage in the page:
/// <pre>
///   // Call the function.
///   example.test.myfunction();
///   // Set the parameter.
///   example.test.myparam = value;
///   // Get the parameter.
///   value = example.test.myparam;
///   // Call another function.
///   example.test.increment();
/// </pre>
///
/*--honey(optional_param=handler)--*/
bool HoneycombRegisterExtension(const HoneycombString& extension_name,
                          const HoneycombString& javascript_code,
                          HoneycombRefPtr<HoneycombV8Handler> handler);

///
/// Class representing a V8 context handle. V8 handles can only be accessed from
/// the thread on which they are created. Valid threads for creating a V8 handle
/// include the render process main thread (TID_RENDERER) and WebWorker threads.
/// A task runner for posting tasks on the associated thread can be retrieved
/// via the HoneycombV8Context::GetTaskRunner() method.
///
/*--honey(source=library,no_debugct_check)--*/
class HoneycombV8Context : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Returns the current (top) context object in the V8 context stack.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombV8Context> GetCurrentContext();

  ///
  /// Returns the entered (bottom) context object in the V8 context stack.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombV8Context> GetEnteredContext();

  ///
  /// Returns true if V8 is currently inside a context.
  ///
  /*--honey()--*/
  static bool InContext();

  ///
  /// Returns the task runner associated with this context. V8 handles can only
  /// be accessed from the thread on which they are created. This method can be
  /// called on any render process thread.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombTaskRunner> GetTaskRunner() = 0;

  ///
  /// Returns true if the underlying handle is valid and it can be accessed on
  /// the current thread. Do not call any other methods if this method returns
  /// false.
  ///
  /*--honey()--*/
  virtual bool IsValid() = 0;

  ///
  /// Returns the browser for this context. This method will return an empty
  /// reference for WebWorker contexts.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombBrowser> GetBrowser() = 0;

  ///
  /// Returns the frame for this context. This method will return an empty
  /// reference for WebWorker contexts.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombFrame> GetFrame() = 0;

  ///
  /// Returns the global object for this context. The context must be entered
  /// before calling this method.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombV8Value> GetGlobal() = 0;

  ///
  /// Enter this context. A context must be explicitly entered before creating a
  /// V8 Object, Array, Function or Date asynchronously. Exit() must be called
  /// the same number of times as Enter() before releasing this context. V8
  /// objects belong to the context in which they are created. Returns true if
  /// the scope was entered successfully.
  ///
  /*--honey()--*/
  virtual bool Enter() = 0;

  ///
  /// Exit this context. Call this method only after calling Enter(). Returns
  /// true if the scope was exited successfully.
  ///
  /*--honey()--*/
  virtual bool Exit() = 0;

  ///
  /// Returns true if this object is pointing to the same handle as |that|
  /// object.
  ///
  /*--honey()--*/
  virtual bool IsSame(HoneycombRefPtr<HoneycombV8Context> that) = 0;

  ///
  /// Execute a string of JavaScript code in this V8 context. The |script_url|
  /// parameter is the URL where the script in question can be found, if any.
  /// The |start_line| parameter is the base line number to use for error
  /// reporting. On success |retval| will be set to the return value, if any,
  /// and the function will return true. On failure |exception| will be set to
  /// the exception, if any, and the function will return false.
  ///
  /*--honey(optional_param=script_url)--*/
  virtual bool Eval(const HoneycombString& code,
                    const HoneycombString& script_url,
                    int start_line,
                    HoneycombRefPtr<HoneycombV8Value>& retval,
                    HoneycombRefPtr<HoneycombV8Exception>& exception) = 0;
};

typedef std::vector<HoneycombRefPtr<HoneycombV8Value>> HoneycombV8ValueList;

///
/// Interface that should be implemented to handle V8 function calls. The
/// methods of this class will be called on the thread associated with the V8
/// function.
///
/*--honey(source=client,no_debugct_check)--*/
class HoneycombV8Handler : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Handle execution of the function identified by |name|. |object| is the
  /// receiver ('this' object) of the function. |arguments| is the list of
  /// arguments passed to the function. If execution succeeds set |retval| to
  /// the function return value. If execution fails set |exception| to the
  /// exception that will be thrown. Return true if execution was handled.
  ///
  /*--honey()--*/
  virtual bool Execute(const HoneycombString& name,
                       HoneycombRefPtr<HoneycombV8Value> object,
                       const HoneycombV8ValueList& arguments,
                       HoneycombRefPtr<HoneycombV8Value>& retval,
                       HoneycombString& exception) = 0;
};

///
/// Interface that should be implemented to handle V8 accessor calls. Accessor
/// identifiers are registered by calling HoneycombV8Value::SetValue(). The methods
/// of this class will be called on the thread associated with the V8 accessor.
///
/*--honey(source=client,no_debugct_check)--*/
class HoneycombV8Accessor : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Handle retrieval the accessor value identified by |name|. |object| is the
  /// receiver ('this' object) of the accessor. If retrieval succeeds set
  /// |retval| to the return value. If retrieval fails set |exception| to the
  /// exception that will be thrown. Return true if accessor retrieval was
  /// handled.
  ///
  /*--honey()--*/
  virtual bool Get(const HoneycombString& name,
                   const HoneycombRefPtr<HoneycombV8Value> object,
                   HoneycombRefPtr<HoneycombV8Value>& retval,
                   HoneycombString& exception) = 0;

  ///
  /// Handle assignment of the accessor value identified by |name|. |object| is
  /// the receiver ('this' object) of the accessor. |value| is the new value
  /// being assigned to the accessor. If assignment fails set |exception| to the
  /// exception that will be thrown. Return true if accessor assignment was
  /// handled.
  ///
  /*--honey()--*/
  virtual bool Set(const HoneycombString& name,
                   const HoneycombRefPtr<HoneycombV8Value> object,
                   const HoneycombRefPtr<HoneycombV8Value> value,
                   HoneycombString& exception) = 0;
};

///
/// Interface that should be implemented to handle V8 interceptor calls. The
/// methods of this class will be called on the thread associated with the V8
/// interceptor. Interceptor's named property handlers (with first argument of
/// type HoneycombString) are called when object is indexed by string. Indexed
/// property handlers (with first argument of type int) are called when object
/// is indexed by integer.
///
/*--honey(source=client,no_debugct_check)--*/
class HoneycombV8Interceptor : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Handle retrieval of the interceptor value identified by |name|. |object|
  /// is the receiver ('this' object) of the interceptor. If retrieval succeeds,
  /// set |retval| to the return value. If the requested value does not exist,
  /// don't set either |retval| or |exception|. If retrieval fails, set
  /// |exception| to the exception that will be thrown. If the property has an
  /// associated accessor, it will be called only if you don't set |retval|.
  /// Return true if interceptor retrieval was handled, false otherwise.
  ///
  /*--honey(capi_name=get_byname)--*/
  virtual bool Get(const HoneycombString& name,
                   const HoneycombRefPtr<HoneycombV8Value> object,
                   HoneycombRefPtr<HoneycombV8Value>& retval,
                   HoneycombString& exception) = 0;

  ///
  /// Handle retrieval of the interceptor value identified by |index|. |object|
  /// is the receiver ('this' object) of the interceptor. If retrieval succeeds,
  /// set |retval| to the return value. If the requested value does not exist,
  /// don't set either |retval| or |exception|. If retrieval fails, set
  /// |exception| to the exception that will be thrown.
  /// Return true if interceptor retrieval was handled, false otherwise.
  ///
  /*--honey(capi_name=get_byindex,index_param=index)--*/
  virtual bool Get(int index,
                   const HoneycombRefPtr<HoneycombV8Value> object,
                   HoneycombRefPtr<HoneycombV8Value>& retval,
                   HoneycombString& exception) = 0;

  ///
  /// Handle assignment of the interceptor value identified by |name|. |object|
  /// is the receiver ('this' object) of the interceptor. |value| is the new
  /// value being assigned to the interceptor. If assignment fails, set
  /// |exception| to the exception that will be thrown. This setter will always
  /// be called, even when the property has an associated accessor.
  /// Return true if interceptor assignment was handled, false otherwise.
  ///
  /*--honey(capi_name=set_byname)--*/
  virtual bool Set(const HoneycombString& name,
                   const HoneycombRefPtr<HoneycombV8Value> object,
                   const HoneycombRefPtr<HoneycombV8Value> value,
                   HoneycombString& exception) = 0;

  ///
  /// Handle assignment of the interceptor value identified by |index|. |object|
  /// is the receiver ('this' object) of the interceptor. |value| is the new
  /// value being assigned to the interceptor. If assignment fails, set
  /// |exception| to the exception that will be thrown.
  /// Return true if interceptor assignment was handled, false otherwise.
  ///
  /*--honey(capi_name=set_byindex,index_param=index)--*/
  virtual bool Set(int index,
                   const HoneycombRefPtr<HoneycombV8Value> object,
                   const HoneycombRefPtr<HoneycombV8Value> value,
                   HoneycombString& exception) = 0;
};

///
/// Class representing a V8 exception. The methods of this class may be called
/// on any render process thread.
///
/*--honey(source=library,no_debugct_check)--*/
class HoneycombV8Exception : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Returns the exception message.
  ///
  /*--honey()--*/
  virtual HoneycombString GetMessage() = 0;

  ///
  /// Returns the line of source code that the exception occurred within.
  ///
  /*--honey()--*/
  virtual HoneycombString GetSourceLine() = 0;

  ///
  /// Returns the resource name for the script from where the function causing
  /// the error originates.
  ///
  /*--honey()--*/
  virtual HoneycombString GetScriptResourceName() = 0;

  ///
  /// Returns the 1-based number of the line where the error occurred or 0 if
  /// the line number is unknown.
  ///
  /*--honey()--*/
  virtual int GetLineNumber() = 0;

  ///
  /// Returns the index within the script of the first character where the error
  /// occurred.
  ///
  /*--honey()--*/
  virtual int GetStartPosition() = 0;

  ///
  /// Returns the index within the script of the last character where the error
  /// occurred.
  ///
  /*--honey()--*/
  virtual int GetEndPosition() = 0;

  ///
  /// Returns the index within the line of the first character where the error
  /// occurred.
  ///
  /*--honey()--*/
  virtual int GetStartColumn() = 0;

  ///
  /// Returns the index within the line of the last character where the error
  /// occurred.
  ///
  /*--honey()--*/
  virtual int GetEndColumn() = 0;
};

///
/// Callback interface that is passed to HoneycombV8Value::CreateArrayBuffer.
///
/*--honey(source=client,no_debugct_check)--*/
class HoneycombV8ArrayBufferReleaseCallback : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Called to release |buffer| when the ArrayBuffer JS object is garbage
  /// collected. |buffer| is the value that was passed to CreateArrayBuffer
  /// along with this object.
  ///
  /*--honey()--*/
  virtual void ReleaseBuffer(void* buffer) = 0;
};

///
/// Class representing a V8 value handle. V8 handles can only be accessed from
/// the thread on which they are created. Valid threads for creating a V8 handle
/// include the render process main thread (TID_RENDERER) and WebWorker threads.
/// A task runner for posting tasks on the associated thread can be retrieved
/// via the HoneycombV8Context::GetTaskRunner() method.
///
/*--honey(source=library,no_debugct_check)--*/
class HoneycombV8Value : public virtual HoneycombBaseRefCounted {
 public:
  typedef honey_v8_accesscontrol_t AccessControl;
  typedef honey_v8_propertyattribute_t PropertyAttribute;

  ///
  /// Create a new HoneycombV8Value object of type undefined.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombV8Value> CreateUndefined();

  ///
  /// Create a new HoneycombV8Value object of type null.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombV8Value> CreateNull();

  ///
  /// Create a new HoneycombV8Value object of type bool.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombV8Value> CreateBool(bool value);

  ///
  /// Create a new HoneycombV8Value object of type int.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombV8Value> CreateInt(int32_t value);

  ///
  /// Create a new HoneycombV8Value object of type unsigned int.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombV8Value> CreateUInt(uint32_t value);

  ///
  /// Create a new HoneycombV8Value object of type double.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombV8Value> CreateDouble(double value);

  ///
  /// Create a new HoneycombV8Value object of type Date. This method should only be
  /// called from within the scope of a HoneycombRenderProcessHandler, HoneycombV8Handler or
  /// HoneycombV8Accessor callback, or in combination with calling Enter() and Exit()
  /// on a stored HoneycombV8Context reference.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombV8Value> CreateDate(HoneycombBaseTime date);

  ///
  /// Create a new HoneycombV8Value object of type string.
  ///
  /*--honey(optional_param=value)--*/
  static HoneycombRefPtr<HoneycombV8Value> CreateString(const HoneycombString& value);

  ///
  /// Create a new HoneycombV8Value object of type object with optional accessor
  /// and/or interceptor. This method should only be called from within the
  /// scope of a HoneycombRenderProcessHandler, HoneycombV8Handler or HoneycombV8Accessor
  /// callback, or in combination with calling Enter() and Exit() on a stored
  /// HoneycombV8Context reference.
  ///
  /*--honey(optional_param=accessor, optional_param=interceptor)--*/
  static HoneycombRefPtr<HoneycombV8Value> CreateObject(
      HoneycombRefPtr<HoneycombV8Accessor> accessor,
      HoneycombRefPtr<HoneycombV8Interceptor> interceptor);

  ///
  /// Create a new HoneycombV8Value object of type array with the specified |length|.
  /// If |length| is negative the returned array will have length 0. This method
  /// should only be called from within the scope of a HoneycombRenderProcessHandler,
  /// HoneycombV8Handler or HoneycombV8Accessor callback, or in combination with calling
  /// Enter() and Exit() on a stored HoneycombV8Context reference.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombV8Value> CreateArray(int length);

  ///
  /// Create a new HoneycombV8Value object of type ArrayBuffer which wraps the
  /// provided |buffer| of size |length| bytes. The ArrayBuffer is externalized,
  /// meaning that it does not own |buffer|. The caller is responsible for
  /// freeing |buffer| when requested via a call to
  /// HoneycombV8ArrayBufferReleaseCallback::ReleaseBuffer. This method should only
  /// be called from within the scope of a HoneycombRenderProcessHandler, HoneycombV8Handler
  /// or HoneycombV8Accessor callback, or in combination with calling Enter() and
  /// Exit() on a stored HoneycombV8Context reference.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombV8Value> CreateArrayBuffer(
      void* buffer,
      size_t length,
      HoneycombRefPtr<HoneycombV8ArrayBufferReleaseCallback> release_callback);

  ///
  /// Create a new HoneycombV8Value object of type function. This method should only
  /// be called from within the scope of a HoneycombRenderProcessHandler, HoneycombV8Handler
  /// or HoneycombV8Accessor callback, or in combination with calling Enter() and
  /// Exit() on a stored HoneycombV8Context reference.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombV8Value> CreateFunction(const HoneycombString& name,
                                              HoneycombRefPtr<HoneycombV8Handler> handler);

  ///
  /// Create a new HoneycombV8Value object of type Promise. This method should only be
  /// called from within the scope of a HoneycombRenderProcessHandler, HoneycombV8Handler or
  /// HoneycombV8Accessor callback, or in combination with calling Enter() and Exit()
  /// on a stored HoneycombV8Context reference.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombV8Value> CreatePromise();

  ///
  /// Returns true if the underlying handle is valid and it can be accessed on
  /// the current thread. Do not call any other methods if this method returns
  /// false.
  ///
  /*--honey()--*/
  virtual bool IsValid() = 0;

  ///
  /// True if the value type is undefined.
  ///
  /*--honey()--*/
  virtual bool IsUndefined() = 0;

  ///
  /// True if the value type is null.
  ///
  /*--honey()--*/
  virtual bool IsNull() = 0;

  ///
  /// True if the value type is bool.
  ///
  /*--honey()--*/
  virtual bool IsBool() = 0;

  ///
  /// True if the value type is int.
  ///
  /*--honey()--*/
  virtual bool IsInt() = 0;

  ///
  /// True if the value type is unsigned int.
  ///
  /*--honey()--*/
  virtual bool IsUInt() = 0;

  ///
  /// True if the value type is double.
  ///
  /*--honey()--*/
  virtual bool IsDouble() = 0;

  ///
  /// True if the value type is Date.
  ///
  /*--honey()--*/
  virtual bool IsDate() = 0;

  ///
  /// True if the value type is string.
  ///
  /*--honey()--*/
  virtual bool IsString() = 0;

  ///
  /// True if the value type is object.
  ///
  /*--honey()--*/
  virtual bool IsObject() = 0;

  ///
  /// True if the value type is array.
  ///
  /*--honey()--*/
  virtual bool IsArray() = 0;

  ///
  /// True if the value type is an ArrayBuffer.
  ///
  /*--honey()--*/
  virtual bool IsArrayBuffer() = 0;

  ///
  /// True if the value type is function.
  ///
  /*--honey()--*/
  virtual bool IsFunction() = 0;

  ///
  /// True if the value type is a Promise.
  ///
  /*--honey()--*/
  virtual bool IsPromise() = 0;

  ///
  /// Returns true if this object is pointing to the same handle as |that|
  /// object.
  ///
  /*--honey()--*/
  virtual bool IsSame(HoneycombRefPtr<HoneycombV8Value> that) = 0;

  ///
  /// Return a bool value.
  ///
  /*--honey()--*/
  virtual bool GetBoolValue() = 0;

  ///
  /// Return an int value.
  ///
  /*--honey()--*/
  virtual int32_t GetIntValue() = 0;

  ///
  /// Return an unsigned int value.
  ///
  /*--honey()--*/
  virtual uint32_t GetUIntValue() = 0;

  ///
  /// Return a double value.
  ///
  /*--honey()--*/
  virtual double GetDoubleValue() = 0;

  ///
  /// Return a Date value.
  ///
  /*--honey()--*/
  virtual HoneycombBaseTime GetDateValue() = 0;

  ///
  /// Return a string value.
  ///
  /*--honey()--*/
  virtual HoneycombString GetStringValue() = 0;

  /// OBJECT METHODS - These methods are only available on objects. Arrays and
  /// functions are also objects. String- and integer-based keys can be used
  /// interchangably with the framework converting between them as necessary.

  ///
  /// Returns true if this is a user created object.
  ///
  /*--honey()--*/
  virtual bool IsUserCreated() = 0;

  ///
  /// Returns true if the last method call resulted in an exception. This
  /// attribute exists only in the scope of the current Honeycomb value object.
  ///
  /*--honey()--*/
  virtual bool HasException() = 0;

  ///
  /// Returns the exception resulting from the last method call. This attribute
  /// exists only in the scope of the current Honeycomb value object.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombV8Exception> GetException() = 0;

  ///
  /// Clears the last exception and returns true on success.
  ///
  /*--honey()--*/
  virtual bool ClearException() = 0;

  ///
  /// Returns true if this object will re-throw future exceptions. This
  /// attribute exists only in the scope of the current Honeycomb value object.
  ///
  /*--honey()--*/
  virtual bool WillRethrowExceptions() = 0;

  ///
  /// Set whether this object will re-throw future exceptions. By default
  /// exceptions are not re-thrown. If a exception is re-thrown the current
  /// context should not be accessed again until after the exception has been
  /// caught and not re-thrown. Returns true on success. This attribute exists
  /// only in the scope of the current Honeycomb value object.
  ///
  /*--honey()--*/
  virtual bool SetRethrowExceptions(bool rethrow) = 0;

  ///
  /// Returns true if the object has a value with the specified identifier.
  ///
  /*--honey(capi_name=has_value_bykey,optional_param=key)--*/
  virtual bool HasValue(const HoneycombString& key) = 0;

  ///
  /// Returns true if the object has a value with the specified identifier.
  ///
  /*--honey(capi_name=has_value_byindex,index_param=index)--*/
  virtual bool HasValue(int index) = 0;

  ///
  /// Deletes the value with the specified identifier and returns true on
  /// success. Returns false if this method is called incorrectly or an
  /// exception is thrown. For read-only and don't-delete values this method
  /// will return true even though deletion failed.
  ///
  /*--honey(capi_name=delete_value_bykey,optional_param=key)--*/
  virtual bool DeleteValue(const HoneycombString& key) = 0;

  ///
  /// Deletes the value with the specified identifier and returns true on
  /// success. Returns false if this method is called incorrectly, deletion
  /// fails or an exception is thrown. For read-only and don't-delete values
  /// this method will return true even though deletion failed.
  ///
  /*--honey(capi_name=delete_value_byindex,index_param=index)--*/
  virtual bool DeleteValue(int index) = 0;

  ///
  /// Returns the value with the specified identifier on success. Returns NULL
  /// if this method is called incorrectly or an exception is thrown.
  ///
  /*--honey(capi_name=get_value_bykey,optional_param=key)--*/
  virtual HoneycombRefPtr<HoneycombV8Value> GetValue(const HoneycombString& key) = 0;

  ///
  /// Returns the value with the specified identifier on success. Returns NULL
  /// if this method is called incorrectly or an exception is thrown.
  ///
  /*--honey(capi_name=get_value_byindex,index_param=index)--*/
  virtual HoneycombRefPtr<HoneycombV8Value> GetValue(int index) = 0;

  ///
  /// Associates a value with the specified identifier and returns true on
  /// success. Returns false if this method is called incorrectly or an
  /// exception is thrown. For read-only values this method will return true
  /// even though assignment failed.
  ///
  /*--honey(capi_name=set_value_bykey,optional_param=key)--*/
  virtual bool SetValue(const HoneycombString& key,
                        HoneycombRefPtr<HoneycombV8Value> value,
                        PropertyAttribute attribute) = 0;

  ///
  /// Associates a value with the specified identifier and returns true on
  /// success. Returns false if this method is called incorrectly or an
  /// exception is thrown. For read-only values this method will return true
  /// even though assignment failed.
  ///
  /*--honey(capi_name=set_value_byindex,index_param=index)--*/
  virtual bool SetValue(int index, HoneycombRefPtr<HoneycombV8Value> value) = 0;

  ///
  /// Registers an identifier and returns true on success. Access to the
  /// identifier will be forwarded to the HoneycombV8Accessor instance passed to
  /// HoneycombV8Value::CreateObject(). Returns false if this method is called
  /// incorrectly or an exception is thrown. For read-only values this method
  /// will return true even though assignment failed.
  ///
  /*--honey(capi_name=set_value_byaccessor,optional_param=key)--*/
  virtual bool SetValue(const HoneycombString& key,
                        AccessControl settings,
                        PropertyAttribute attribute) = 0;

  ///
  /// Read the keys for the object's values into the specified vector. Integer-
  /// based keys will also be returned as strings.
  ///
  /*--honey()--*/
  virtual bool GetKeys(std::vector<HoneycombString>& keys) = 0;

  ///
  /// Sets the user data for this object and returns true on success. Returns
  /// false if this method is called incorrectly. This method can only be called
  /// on user created objects.
  ///
  /*--honey(optional_param=user_data)--*/
  virtual bool SetUserData(HoneycombRefPtr<HoneycombBaseRefCounted> user_data) = 0;

  ///
  /// Returns the user data, if any, assigned to this object.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombBaseRefCounted> GetUserData() = 0;

  ///
  /// Returns the amount of externally allocated memory registered for the
  /// object.
  ///
  /*--honey()--*/
  virtual int GetExternallyAllocatedMemory() = 0;

  ///
  /// Adjusts the amount of registered external memory for the object. Used to
  /// give V8 an indication of the amount of externally allocated memory that is
  /// kept alive by JavaScript objects. V8 uses this information to decide when
  /// to perform global garbage collection. Each HoneycombV8Value tracks the amount of
  /// external memory associated with it and automatically decreases the global
  /// total by the appropriate amount on its destruction. |change_in_bytes|
  /// specifies the number of bytes to adjust by. This method returns the number
  /// of bytes associated with the object after the adjustment. This method can
  /// only be called on user created objects.
  ///
  /*--honey()--*/
  virtual int AdjustExternallyAllocatedMemory(int change_in_bytes) = 0;

  // ARRAY METHODS - These methods are only available on arrays.

  ///
  /// Returns the number of elements in the array.
  ///
  /*--honey()--*/
  virtual int GetArrayLength() = 0;

  // ARRAY BUFFER METHODS - These methods are only available on ArrayBuffers.

  ///
  /// Returns the ReleaseCallback object associated with the ArrayBuffer or NULL
  /// if the ArrayBuffer was not created with CreateArrayBuffer.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombV8ArrayBufferReleaseCallback>
  GetArrayBufferReleaseCallback() = 0;

  ///
  /// Prevent the ArrayBuffer from using it's memory block by setting the length
  /// to zero. This operation cannot be undone. If the ArrayBuffer was created
  /// with CreateArrayBuffer then HoneycombV8ArrayBufferReleaseCallback::ReleaseBuffer
  /// will be called to release the underlying buffer.
  ///
  /*--honey()--*/
  virtual bool NeuterArrayBuffer() = 0;

  // FUNCTION METHODS - These methods are only available on functions.

  ///
  /// Returns the function name.
  ///
  /*--honey()--*/
  virtual HoneycombString GetFunctionName() = 0;

  ///
  /// Returns the function handler or NULL if not a Honeycomb-created function.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombV8Handler> GetFunctionHandler() = 0;

  ///
  /// Execute the function using the current V8 context. This method should only
  /// be called from within the scope of a HoneycombV8Handler or HoneycombV8Accessor
  /// callback, or in combination with calling Enter() and Exit() on a stored
  /// HoneycombV8Context reference. |object| is the receiver ('this' object) of the
  /// function. If |object| is empty the current context's global object will be
  /// used. |arguments| is the list of arguments that will be passed to the
  /// function. Returns the function return value on success. Returns NULL if
  /// this method is called incorrectly or an exception is thrown.
  ///
  /*--honey(optional_param=object)--*/
  virtual HoneycombRefPtr<HoneycombV8Value> ExecuteFunction(
      HoneycombRefPtr<HoneycombV8Value> object,
      const HoneycombV8ValueList& arguments) = 0;

  ///
  /// Execute the function using the specified V8 context. |object| is the
  /// receiver ('this' object) of the function. If |object| is empty the
  /// specified context's global object will be used. |arguments| is the list of
  /// arguments that will be passed to the function. Returns the function return
  /// value on success. Returns NULL if this method is called incorrectly or an
  /// exception is thrown.
  ///
  /*--honey(optional_param=object)--*/
  virtual HoneycombRefPtr<HoneycombV8Value> ExecuteFunctionWithContext(
      HoneycombRefPtr<HoneycombV8Context> context,
      HoneycombRefPtr<HoneycombV8Value> object,
      const HoneycombV8ValueList& arguments) = 0;

  // PROMISE METHODS - These methods are only available on Promises.

  ///
  /// Resolve the Promise using the current V8 context. This method should only
  /// be called from within the scope of a HoneycombV8Handler or HoneycombV8Accessor
  /// callback, or in combination with calling Enter() and Exit() on a stored
  /// HoneycombV8Context reference. |arg| is the argument passed to the resolved
  /// promise. Returns true on success. Returns false if this method is called
  /// incorrectly or an exception is thrown.
  ///
  /*--honey(optional_param=arg)--*/
  virtual bool ResolvePromise(HoneycombRefPtr<HoneycombV8Value> arg) = 0;

  ///
  /// Reject the Promise using the current V8 context. This method should only
  /// be called from within the scope of a HoneycombV8Handler or HoneycombV8Accessor
  /// callback, or in combination with calling Enter() and Exit() on a stored
  /// HoneycombV8Context reference. Returns true on success. Returns false if this
  /// method is called incorrectly or an exception is thrown.
  ///
  /*--honey()--*/
  virtual bool RejectPromise(const HoneycombString& errorMsg) = 0;
};

///
/// Class representing a V8 stack trace handle. V8 handles can only be accessed
/// from the thread on which they are created. Valid threads for creating a V8
/// handle include the render process main thread (TID_RENDERER) and WebWorker
/// threads. A task runner for posting tasks on the associated thread can be
/// retrieved via the HoneycombV8Context::GetTaskRunner() method.
///
/*--honey(source=library)--*/
class HoneycombV8StackTrace : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Returns the stack trace for the currently active context. |frame_limit| is
  /// the maximum number of frames that will be captured.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombV8StackTrace> GetCurrent(int frame_limit);

  ///
  /// Returns true if the underlying handle is valid and it can be accessed on
  /// the current thread. Do not call any other methods if this method returns
  /// false.
  ///
  /*--honey()--*/
  virtual bool IsValid() = 0;

  ///
  /// Returns the number of stack frames.
  ///
  /*--honey()--*/
  virtual int GetFrameCount() = 0;

  ///
  /// Returns the stack frame at the specified 0-based index.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombV8StackFrame> GetFrame(int index) = 0;
};

///
/// Class representing a V8 stack frame handle. V8 handles can only be accessed
/// from the thread on which they are created. Valid threads for creating a V8
/// handle include the render process main thread (TID_RENDERER) and WebWorker
/// threads. A task runner for posting tasks on the associated thread can be
/// retrieved via the HoneycombV8Context::GetTaskRunner() method.
///
/*--honey(source=library,no_debugct_check)--*/
class HoneycombV8StackFrame : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Returns true if the underlying handle is valid and it can be accessed on
  /// the current thread. Do not call any other methods if this method returns
  /// false.
  ///
  /*--honey()--*/
  virtual bool IsValid() = 0;

  ///
  /// Returns the name of the resource script that contains the function.
  ///
  /*--honey()--*/
  virtual HoneycombString GetScriptName() = 0;

  ///
  /// Returns the name of the resource script that contains the function or the
  /// sourceURL value if the script name is undefined and its source ends with
  /// a "//@ sourceURL=..." string.
  ///
  /*--honey()--*/
  virtual HoneycombString GetScriptNameOrSourceURL() = 0;

  ///
  /// Returns the name of the function.
  ///
  /*--honey()--*/
  virtual HoneycombString GetFunctionName() = 0;

  ///
  /// Returns the 1-based line number for the function call or 0 if unknown.
  ///
  /*--honey()--*/
  virtual int GetLineNumber() = 0;

  ///
  /// Returns the 1-based column offset on the line for the function call or 0
  /// if unknown.
  ///
  /*--honey()--*/
  virtual int GetColumn() = 0;

  ///
  /// Returns true if the function was compiled using eval().
  ///
  /*--honey()--*/
  virtual bool IsEval() = 0;

  ///
  /// Returns true if the function was called as a constructor via "new".
  ///
  /*--honey()--*/
  virtual bool IsConstructor() = 0;
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_V8_H_
