// Copyright (c) 2017 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_DLL_CTOCPP_CTOCPP_SCOPED_H_
#define HONEYCOMB_LIBHONEY_DLL_CTOCPP_CTOCPP_SCOPED_H_
#pragma once

#include "include/base/honey_logging.h"
#include "include/capi/honey_base_capi.h"
#include "include/honey_base.h"
#include "libhoney_dll/wrapper_types.h"

// Wrap a C structure with a C++ class. This is used when the implementation
// exists on the other side of the DLL boundary but will have methods called on
// this side of the DLL boundary.
template <class ClassName, class BaseName, class StructName>
class HoneycombCToCppScoped : public BaseName {
 public:
  HoneycombCToCppScoped(const HoneycombCToCppScoped&) = delete;
  HoneycombCToCppScoped& operator=(const HoneycombCToCppScoped&) = delete;

  // Create a new wrapper instance for a structure reference received from the
  // other side. The caller owns the CToCpp wrapper instance but not necessarily
  // the underling object on the CppToC side (depends if s->del is non-NULL).
  // The returned wrapper object can be used as either a scoped argument or to
  // pass ownership. For example:
  //
  // void my_method(my_type1_t* struct1, my_type2_t* struct2) {
  //  // Passes ownership to MyMethod1().
  //  MyMethod1(MyType1CToCpp::Wrap(struct1));
  //
  //  // Passes reference to MyMethod2().
  //  HoneycombOwnPtr<MyType1> obj2 = MyType2CToCpp::Wrap(struct2);
  //  MyMethod2(obj2.get());
  //  // |obj2| is deleted when my_method() goes out of scope.
  // }
  //
  // void MyMethod1(HoneycombOwnPtr<MyType1> obj1) {
  //   // |obj1| is deleted when MyMethod1() goes out of scope.
  // }
  //
  // void MyMethod2(HoneycombRawPtr<MyType2> obj2) {
  // }
  static HoneycombOwnPtr<BaseName> Wrap(StructName* s);

  // Retrieve the underlying structure reference from a wrapper instance for
  // return back to the other side. Ownership will be passed back to the other
  // side and the wrapper will be deleted. For example:
  //
  // void MyMethod(HoneycombOwnPtr<MyType> obj) {
  //   // Ownership of the underlying MyType object is passed to my_method().
  //   my_method(MyTypeCToCpp::UnwrapOwn(std::move(obj)));
  //   // |obj| is now NULL.
  // }
  static StructName* UnwrapOwn(HoneycombOwnPtr<BaseName> c);

  // Retrieve the underlying structure reference from a wrapper instance for
  // return back to the other side. Ownership does not change. For example:
  //
  // void MyMethod(HoneycombRawPtr<MyType> obj) {
  //   // A reference is passed to my_method(). Ownership does not change.
  //   my_method2(MyTypeCToCpp::UnwrapRaw(obj));
  // }
  static StructName* UnwrapRaw(HoneycombRawPtr<BaseName> c);

  // Override delete operator to properly delete the WrapperStruct.
  // ~HoneycombCToCppScoped will be called first followed by this method.
  static void operator delete(void* ptr);

 protected:
  HoneycombCToCppScoped() = default;
  virtual ~HoneycombCToCppScoped() = default;

  // If returning the structure across the DLL boundary use Unwrap() instead.
  StructName* GetStruct() const {
    WrapperStruct* wrapperStruct = GetWrapperStruct(this);
    // Verify that the wrapper offset was calculated correctly.
    DCHECK_EQ(kWrapperType, wrapperStruct->type_);
    return wrapperStruct->struct_;
  }

 private:
  // Used to associate this wrapper object and the structure reference received
  // from the other side.
  struct WrapperStruct;

  static WrapperStruct* GetWrapperStruct(const BaseName* obj);

  // Unwrap as the derived type.
  static StructName* UnwrapDerivedOwn(HoneycombWrapperType type,
                                      HoneycombOwnPtr<BaseName> c);
  static StructName* UnwrapDerivedRaw(HoneycombWrapperType type,
                                      HoneycombRawPtr<BaseName> c);

  static HoneycombWrapperType kWrapperType;
};

template <class ClassName, class BaseName, class StructName>
struct HoneycombCToCppScoped<ClassName, BaseName, StructName>::WrapperStruct {
  HoneycombWrapperType type_;
  StructName* struct_;
  ClassName wrapper_;
};

template <class ClassName, class BaseName, class StructName>
HoneycombOwnPtr<BaseName> HoneycombCToCppScoped<ClassName, BaseName, StructName>::Wrap(
    StructName* s) {
  if (!s) {
    return HoneycombOwnPtr<BaseName>();
  }

  // Wrap their structure with the HoneycombCToCpp object.
  WrapperStruct* wrapperStruct = new WrapperStruct;
  wrapperStruct->type_ = kWrapperType;
  wrapperStruct->struct_ = s;

  return HoneycombOwnPtr<BaseName>(&wrapperStruct->wrapper_);
}

template <class ClassName, class BaseName, class StructName>
StructName* HoneycombCToCppScoped<ClassName, BaseName, StructName>::UnwrapOwn(
    HoneycombOwnPtr<BaseName> c) {
  if (!c.get()) {
    return nullptr;
  }

  WrapperStruct* wrapperStruct = GetWrapperStruct(c.get());

  // If the type does not match this object then we need to unwrap as the
  // derived type.
  if (wrapperStruct->type_ != kWrapperType) {
    return UnwrapDerivedOwn(wrapperStruct->type_, std::move(c));
  }

  StructName* orig_struct = wrapperStruct->struct_;

#if DCHECK_IS_ON()
  // We should own the object currently.
  honey_base_scoped_t* base = reinterpret_cast<honey_base_scoped_t*>(orig_struct);
  DCHECK(base && base->del);
#endif

  // Don't delete the original object when the wrapper is deleted.
  wrapperStruct->struct_ = nullptr;

  // Return the original structure.
  return orig_struct;
  // The wrapper |c| is deleted when this method goes out of scope.
}

template <class ClassName, class BaseName, class StructName>
StructName* HoneycombCToCppScoped<ClassName, BaseName, StructName>::UnwrapRaw(
    HoneycombRawPtr<BaseName> c) {
  if (!c) {
    return nullptr;
  }

  WrapperStruct* wrapperStruct = GetWrapperStruct(c);

  // If the type does not match this object then we need to unwrap as the
  // derived type.
  if (wrapperStruct->type_ != kWrapperType) {
    return UnwrapDerivedRaw(wrapperStruct->type_, c);
  }

  // Return the original structure.
  return wrapperStruct->struct_;
}

template <class ClassName, class BaseName, class StructName>
NO_SANITIZE("cfi-icall")
void HoneycombCToCppScoped<ClassName, BaseName, StructName>::operator delete(
    void* ptr) {
  WrapperStruct* wrapperStruct = GetWrapperStruct(static_cast<BaseName*>(ptr));
  // Verify that the wrapper offset was calculated correctly.
  DCHECK_EQ(kWrapperType, wrapperStruct->type_);

  // May be NULL if UnwrapOwn() was called.
  honey_base_scoped_t* base =
      reinterpret_cast<honey_base_scoped_t*>(wrapperStruct->struct_);

  // If we own the object (base->del != NULL) then notify the other side that
  // the object has been deleted.
  if (base && base->del) {
    base->del(base);
  }

  // Delete the wrapper structure without executing ~HoneycombCToCppScoped() an
  // additional time.
  ::operator delete(wrapperStruct);
}

template <class ClassName, class BaseName, class StructName>
typename HoneycombCToCppScoped<ClassName, BaseName, StructName>::WrapperStruct*
HoneycombCToCppScoped<ClassName, BaseName, StructName>::GetWrapperStruct(
    const BaseName* obj) {
  // Offset using the WrapperStruct size instead of individual member sizes to
  // avoid problems due to platform/compiler differences in structure padding.
  return reinterpret_cast<WrapperStruct*>(
      reinterpret_cast<char*>(const_cast<BaseName*>(obj)) -
      (sizeof(WrapperStruct) - sizeof(ClassName)));
}

#endif  // HONEYCOMB_LIBHONEY_DLL_CTOCPP_CTOCPP_SCOPED_H_
