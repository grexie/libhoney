// Copyright (c) 2009 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_DLL_CTOCPP_CTOCPP_REF_COUNTED_H_
#define HONEYCOMB_LIBHONEY_DLL_CTOCPP_CTOCPP_REF_COUNTED_H_
#pragma once

#include "include/base/honey_logging.h"
#include "include/capi/honey_base_capi.h"
#include "include/honey_base.h"
#include "libhoney_dll/wrapper_types.h"

// Wrap a C structure with a C++ class. This is used when the implementation
// exists on the other side of the DLL boundary but will have methods called on
// this side of the DLL boundary.
template <class ClassName, class BaseName, class StructName>
class HoneycombCToCppRefCounted : public BaseName {
 public:
  HoneycombCToCppRefCounted(const HoneycombCToCppRefCounted&) = delete;
  HoneycombCToCppRefCounted& operator=(const HoneycombCToCppRefCounted&) = delete;

  // Create a new wrapper instance for a structure reference received from the
  // other side.
  static HoneycombRefPtr<BaseName> Wrap(StructName* s);

  // Retrieve the underlying structure reference from a wrapper instance for
  // return back to the other side.
  static StructName* Unwrap(HoneycombRefPtr<BaseName> c);

  // HoneycombBaseRefCounted methods increment/decrement reference counts on both this
  // object and the underlying wrapped structure.
  void AddRef() const {
    UnderlyingAddRef();
    ref_count_.AddRef();
  }
  bool Release() const;
  bool HasOneRef() const { return UnderlyingHasOneRef(); }
  bool HasAtLeastOneRef() const { return UnderlyingHasAtLeastOneRef(); }

 protected:
  HoneycombCToCppRefCounted() = default;
  virtual ~HoneycombCToCppRefCounted() = default;

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
  static StructName* UnwrapDerived(HoneycombWrapperType type, BaseName* c);

  // Increment/decrement reference counts on only the underlying class.
  NO_SANITIZE("cfi-icall")
  void UnderlyingAddRef() const {
    honey_base_ref_counted_t* base =
        reinterpret_cast<honey_base_ref_counted_t*>(GetStruct());
    if (base->add_ref) {
      base->add_ref(base);
    }
  }

  NO_SANITIZE("cfi-icall")
  bool UnderlyingRelease() const {
    honey_base_ref_counted_t* base =
        reinterpret_cast<honey_base_ref_counted_t*>(GetStruct());
    if (!base->release) {
      return false;
    }
    return base->release(base) ? true : false;
  }

  NO_SANITIZE("cfi-icall")
  bool UnderlyingHasOneRef() const {
    honey_base_ref_counted_t* base =
        reinterpret_cast<honey_base_ref_counted_t*>(GetStruct());
    if (!base->has_one_ref) {
      return false;
    }
    return base->has_one_ref(base) ? true : false;
  }

  NO_SANITIZE("cfi-icall")
  bool UnderlyingHasAtLeastOneRef() const {
    honey_base_ref_counted_t* base =
        reinterpret_cast<honey_base_ref_counted_t*>(GetStruct());
    if (!base->has_at_least_one_ref) {
      return false;
    }
    return base->has_at_least_one_ref(base) ? true : false;
  }

  HoneycombRefCount ref_count_;

  static HoneycombWrapperType kWrapperType;
};

template <class ClassName, class BaseName, class StructName>
struct HoneycombCToCppRefCounted<ClassName, BaseName, StructName>::WrapperStruct {
  HoneycombWrapperType type_;
  StructName* struct_;
  ClassName wrapper_;
};

template <class ClassName, class BaseName, class StructName>
HoneycombRefPtr<BaseName> HoneycombCToCppRefCounted<ClassName, BaseName, StructName>::Wrap(
    StructName* s) {
  if (!s) {
    return nullptr;
  }

  // Wrap their structure with the HoneycombCToCppRefCounted object.
  WrapperStruct* wrapperStruct = new WrapperStruct;
  wrapperStruct->type_ = kWrapperType;
  wrapperStruct->struct_ = s;

  // Put the wrapper object in a smart pointer.
  HoneycombRefPtr<BaseName> wrapperPtr(&wrapperStruct->wrapper_);
  // Release the reference that was added to the HoneycombCppToC wrapper object on
  // the other side before their structure was passed to us.
  wrapperStruct->wrapper_.UnderlyingRelease();
  // Return the smart pointer.
  return wrapperPtr;
}

template <class ClassName, class BaseName, class StructName>
StructName* HoneycombCToCppRefCounted<ClassName, BaseName, StructName>::Unwrap(
    HoneycombRefPtr<BaseName> c) {
  if (!c.get()) {
    return nullptr;
  }

  WrapperStruct* wrapperStruct = GetWrapperStruct(c.get());

  // If the type does not match this object then we need to unwrap as the
  // derived type.
  if (wrapperStruct->type_ != kWrapperType) {
    return UnwrapDerived(wrapperStruct->type_, c.get());
  }

  // Add a reference to the HoneycombCppToC wrapper object on the other side that
  // will be released once the structure is received.
  wrapperStruct->wrapper_.UnderlyingAddRef();
  // Return their original structure.
  return wrapperStruct->struct_;
}

template <class ClassName, class BaseName, class StructName>
bool HoneycombCToCppRefCounted<ClassName, BaseName, StructName>::Release() const {
  UnderlyingRelease();
  if (ref_count_.Release()) {
    WrapperStruct* wrapperStruct = GetWrapperStruct(this);
    // Verify that the wrapper offset was calculated correctly.
    DCHECK_EQ(kWrapperType, wrapperStruct->type_);
    delete wrapperStruct;
    return true;
  }
  return false;
}

template <class ClassName, class BaseName, class StructName>
typename HoneycombCToCppRefCounted<ClassName, BaseName, StructName>::WrapperStruct*
HoneycombCToCppRefCounted<ClassName, BaseName, StructName>::GetWrapperStruct(
    const BaseName* obj) {
  // Offset using the WrapperStruct size instead of individual member sizes to
  // avoid problems due to platform/compiler differences in structure padding.
  return reinterpret_cast<WrapperStruct*>(
      reinterpret_cast<char*>(const_cast<BaseName*>(obj)) -
      (sizeof(WrapperStruct) - sizeof(ClassName)));
}

#endif  // HONEYCOMB_LIBHONEY_DLL_CTOCPP_CTOCPP_REF_COUNTED_H_
