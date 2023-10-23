// Copyright (c) 2023 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the Honeycomb translator tool. If making changes by
// hand only do so within the body of existing method and function
// implementations. See the translator.README.txt file in the tools directory
// for more information.
//
// $hash=9b1ad32d9167bdaaeee14834a82b13927ac71c82$
//

#include "libhoney_dll/cpptoc/v8handler_cpptoc.h"
#include "libhoney_dll/ctocpp/v8value_ctocpp.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

int HONEYCOMB_CALLBACK v8handler_execute(struct _honey_v8handler_t* self,
                                   const honey_string_t* name,
                                   struct _honey_v8value_t* object,
                                   size_t argumentsCount,
                                   struct _honey_v8value_t* const* arguments,
                                   struct _honey_v8value_t** retval,
                                   honey_string_t* exception) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }
  // Verify param: name; type: string_byref_const
  DCHECK(name);
  if (!name) {
    return 0;
  }
  // Verify param: object; type: refptr_diff
  DCHECK(object);
  if (!object) {
    return 0;
  }
  // Verify param: arguments; type: refptr_vec_diff_byref_const
  DCHECK(argumentsCount == 0 || arguments);
  if (argumentsCount > 0 && !arguments) {
    return 0;
  }
  // Verify param: retval; type: refptr_diff_byref
  DCHECK(retval);
  if (!retval) {
    return 0;
  }
  // Verify param: exception; type: string_byref
  DCHECK(exception);
  if (!exception) {
    return 0;
  }

  // Translate param: arguments; type: refptr_vec_diff_byref_const
  std::vector<HoneycombRefPtr<HoneycombV8Value>> argumentsList;
  if (argumentsCount > 0) {
    for (size_t i = 0; i < argumentsCount; ++i) {
      HoneycombRefPtr<HoneycombV8Value> argumentsVal = HoneycombV8ValueCToCpp::Wrap(arguments[i]);
      argumentsList.push_back(argumentsVal);
    }
  }
  // Translate param: retval; type: refptr_diff_byref
  HoneycombRefPtr<HoneycombV8Value> retvalPtr;
  if (retval && *retval) {
    retvalPtr = HoneycombV8ValueCToCpp::Wrap(*retval);
  }
  HoneycombV8Value* retvalOrig = retvalPtr.get();
  // Translate param: exception; type: string_byref
  HoneycombString exceptionStr(exception);

  // Execute
  bool _retval = HoneycombV8HandlerCppToC::Get(self)->Execute(
      HoneycombString(name), HoneycombV8ValueCToCpp::Wrap(object), argumentsList, retvalPtr,
      exceptionStr);

  // Restore param: retval; type: refptr_diff_byref
  if (retval) {
    if (retvalPtr.get()) {
      if (retvalPtr.get() != retvalOrig) {
        *retval = HoneycombV8ValueCToCpp::Unwrap(retvalPtr);
      }
    } else {
      *retval = nullptr;
    }
  }

  // Return type: bool
  return _retval;
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

HoneycombV8HandlerCppToC::HoneycombV8HandlerCppToC() {
  GetStruct()->execute = v8handler_execute;
}

// DESTRUCTOR - Do not edit by hand.

HoneycombV8HandlerCppToC::~HoneycombV8HandlerCppToC() {}

template <>
HoneycombRefPtr<HoneycombV8Handler>
HoneycombCppToCRefCounted<HoneycombV8HandlerCppToC, HoneycombV8Handler, honey_v8handler_t>::
    UnwrapDerived(HoneycombWrapperType type, honey_v8handler_t* s) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCppToCRefCounted<HoneycombV8HandlerCppToC,
                                   HoneycombV8Handler,
                                   honey_v8handler_t>::kWrapperType =
    WT_V8HANDLER;