// Copyright (c) 2017 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_DLL_CPPTOC_CPPTOC_SCOPED_H_
#define HONEYCOMB_LIBHONEY_DLL_CPPTOC_CPPTOC_SCOPED_H_
#pragma once

#include "include/base/honey_logging.h"
#include "include/capi/honey_base_capi.h"
#include "include/honey_base.h"
#include "libhoney_dll/wrapper_types.h"

// Wrap a C++ class with a C structure. This is used when the class
// implementation exists on this side of the DLL boundary but will have methods
// called from the other side of the DLL boundary.
template <class ClassName, class BaseName, class StructName>
class HoneycombCppToCScoped : public HoneycombBaseScoped {
 public:
  HoneycombCppToCScoped(const HoneycombCppToCScoped&) = delete;
  HoneycombCppToCScoped& operator=(const HoneycombCppToCScoped&) = delete;

  // Create a new wrapper instance and associated structure reference for
  // passing an object instance the other side. The wrapper object will be
  // deleted when |del| is called on the associated structure. The wrapped
  // object will be deleted when the wrapper object is deleted. For example:
  //
  // void MyMethod(HoneycombOwnPtr<MyType> obj) {
  //   my_method(MyTypeCppToC::WrapOwn(obj));
  // }
  //
  // void my_method(my_type_t* struct) {
  //   // Delete the MyTypeCppToC wrapper and the owned MyType object.
  //   struct->del(struct);
  // }
  static StructName* WrapOwn(HoneycombOwnPtr<BaseName> c) {
    if (!c) {
      return nullptr;
    }

    // Wrap our object with the HoneycombCppToC class.
    ClassName* wrapper = new ClassName();
    wrapper->Initialize(c.release(), true);

    // Return the structure pointer that can now be passed to the other side.
    return wrapper->GetStruct();
  }

  // Create a new wrapper instance and associated structure reference for
  // passing an object instance to the other side. The wrapper object is owned
  // by the caller. The wrapped object is unowned and must outlive the wrapper.
  // For example:
  //
  // void MyMethod(MyType* obj) {
  //   HoneycombOwnPtr<MyTypeCppToC> MyTypeWrapper = MyTypeCppToC::WrapRaw(obj);
  //   my_method(MyTypeWrapper->GetStruct());
  //   // MyTypeWrapper is deleted when MyMethod() goes out of scope.
  // }
  //
  // void my_method(my_type_t* struct) {
  //   // Access |struct| here but you can't delete it.
  // }
  static HoneycombOwnPtr<ClassName> WrapRaw(HoneycombRawPtr<BaseName> c) {
    if (!c) {
      return HoneycombOwnPtr<ClassName>();
    }

    // Wrap our object with the HoneycombCppToC class.
    ClassName* wrapper = new ClassName();
    wrapper->Initialize(c, false);

    // Return the owned wrapper object.
    return HoneycombOwnPtr<ClassName>(wrapper);
  }

  // Retrieve the underlying object instance for a structure reference passed
  // back from the other side. The caller takes ownership of the object. For
  // example:
  //
  // void my_method(my_type_t* struct) {
  //   HoneycombOwnPtr<MyType> MyTypePtr = MyTypeCppToC::UnwrapOwn(struct);
  //   // |struct| has been deleted and should no longer be accessed.
  // }
  static HoneycombOwnPtr<BaseName> UnwrapOwn(StructName* s) {
    if (!s) {
      return HoneycombOwnPtr<BaseName>();
    }

    // Cast our structure to the wrapper structure type.
    WrapperStruct* wrapperStruct = GetWrapperStruct(s);

    // If the type does not match this object then we need to unwrap as the
    // derived type.
    if (wrapperStruct->type_ != kWrapperType) {
      return UnwrapDerivedOwn(wrapperStruct->type_, s);
    }

    // We should own the underlying object currently.
    DCHECK(wrapperStruct->wrapper_->owned_);
    DCHECK(wrapperStruct->object_);

    // We're giving up ownership of the underlying object. Clear the pointer so
    // it doesn't get deleted.
    BaseName* object = wrapperStruct->object_;
    wrapperStruct->object_ = nullptr;

    delete wrapperStruct->wrapper_;

    // Return the underlying object instance.
    return HoneycombOwnPtr<BaseName>(object);
  }

  // Retrieve the underlying object instance for a structure reference passed
  // back from the other side. Ownership does not change. For example:
  //
  // void my_method(my_type_t* struct) {
  //   HoneycombRawPtr<MyType> MyTypePtr = MyTypeCppToC::UnwrapRaw(struct);
  //   // |struct| is still valid.
  // }
  static HoneycombRawPtr<BaseName> UnwrapRaw(StructName* s) {
    if (!s) {
      return nullptr;
    }

    // Cast our structure to the wrapper structure type.
    WrapperStruct* wrapperStruct = GetWrapperStruct(s);

    // If the type does not match this object then we need to unwrap as the
    // derived type.
    if (wrapperStruct->type_ != kWrapperType) {
      return UnwrapDerivedRaw(wrapperStruct->type_, s);
    }

    // Return the underlying object instance.
    return wrapperStruct->object_;
  }

  // Retrieve the same side wrapper associated with the structure. Ownership
  // does not change.
  static ClassName* GetWrapper(StructName* s) {
    DCHECK(s);
    WrapperStruct* wrapperStruct = GetWrapperStruct(s);
    // Verify that the wrapper offset was calculated correctly.
    DCHECK_EQ(kWrapperType, wrapperStruct->type_);
    return static_cast<ClassName*>(wrapperStruct->wrapper_);
  }

  // Retrieve the underlying object instance from our own structure reference
  // when the reference is passed as the required first parameter of a C API
  // function call. Ownership of the object does not change.
  static BaseName* Get(StructName* s) {
    DCHECK(s);
    WrapperStruct* wrapperStruct = GetWrapperStruct(s);
    // Verify that the wrapper offset was calculated correctly.
    DCHECK_EQ(kWrapperType, wrapperStruct->type_);
    return wrapperStruct->object_;
  }

  // If returning the structure across the DLL boundary you should call
  // AddRef() on this HoneycombCppToC object.  On the other side of the DLL boundary,
  // call UnderlyingRelease() on the wrapping HoneycombCToCpp object.
  StructName* GetStruct() { return &wrapper_struct_.struct_; }

 protected:
  HoneycombCppToCScoped() {
    wrapper_struct_.type_ = kWrapperType;
    wrapper_struct_.wrapper_ = this;
    memset(GetStruct(), 0, sizeof(StructName));
  }

  virtual ~HoneycombCppToCScoped() {
    // Only delete the underlying object if we own it.
    if (owned_ && wrapper_struct_.object_) {
      delete wrapper_struct_.object_;
    }
  }

 private:
  // Used to associate this wrapper object, the underlying object instance and
  // the structure that will be passed to the other side.
  struct WrapperStruct {
    HoneycombWrapperType type_;
    BaseName* object_;
    HoneycombCppToCScoped<ClassName, BaseName, StructName>* wrapper_;
    StructName struct_;
  };

  void Initialize(BaseName* obj, bool owned) {
    wrapper_struct_.object_ = obj;
    owned_ = owned;

    honey_base_scoped_t* base = reinterpret_cast<honey_base_scoped_t*>(GetStruct());
    base->size = sizeof(StructName);
    if (owned) {
      base->del = struct_del;
    }
  }

  static WrapperStruct* GetWrapperStruct(StructName* s) {
    // Offset using the WrapperStruct size instead of individual member sizes
    // to avoid problems due to platform/compiler differences in structure
    // padding.
    return reinterpret_cast<WrapperStruct*>(
        reinterpret_cast<char*>(s) -
        (sizeof(WrapperStruct) - sizeof(StructName)));
  }

  // Unwrap as the derived type.
  static HoneycombOwnPtr<BaseName> UnwrapDerivedOwn(HoneycombWrapperType type,
                                              StructName* s);
  static HoneycombRawPtr<BaseName> UnwrapDerivedRaw(HoneycombWrapperType type,
                                              StructName* s);

  static void HONEYCOMB_CALLBACK struct_del(honey_base_scoped_t* base) {
    DCHECK(base);
    if (!base) {
      return;
    }

    WrapperStruct* wrapperStruct =
        GetWrapperStruct(reinterpret_cast<StructName*>(base));
    // Verify that the wrapper offset was calculated correctly.
    DCHECK_EQ(kWrapperType, wrapperStruct->type_);

    // Should only be deleting wrappers that own the underlying object.
    DCHECK(wrapperStruct->wrapper_->owned_);
    delete wrapperStruct->wrapper_;
  }

  WrapperStruct wrapper_struct_;
  bool owned_;

  static HoneycombWrapperType kWrapperType;
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CPPTOC_CPPTOC_SCOPED_H_
