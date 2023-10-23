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
// $hash=c3caec6488343dacaf00231a9304bcd4b3c95d9e$
//

#include "libhoney_dll/cpptoc/write_handler_cpptoc.h"
#include "libhoney_dll/shutdown_checker.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

size_t HONEYCOMB_CALLBACK write_handler_write(struct _honey_write_handler_t* self,
                                        const void* ptr,
                                        size_t size,
                                        size_t n) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }
  // Verify param: ptr; type: simple_byaddr
  DCHECK(ptr);
  if (!ptr) {
    return 0;
  }

  // Execute
  size_t _retval = HoneycombWriteHandlerCppToC::Get(self)->Write(ptr, size, n);

  // Return type: simple
  return _retval;
}

int HONEYCOMB_CALLBACK write_handler_seek(struct _honey_write_handler_t* self,
                                    int64_t offset,
                                    int whence) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  int _retval = HoneycombWriteHandlerCppToC::Get(self)->Seek(offset, whence);

  // Return type: simple
  return _retval;
}

int64_t HONEYCOMB_CALLBACK write_handler_tell(struct _honey_write_handler_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  int64_t _retval = HoneycombWriteHandlerCppToC::Get(self)->Tell();

  // Return type: simple
  return _retval;
}

int HONEYCOMB_CALLBACK write_handler_flush(struct _honey_write_handler_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  int _retval = HoneycombWriteHandlerCppToC::Get(self)->Flush();

  // Return type: simple
  return _retval;
}

int HONEYCOMB_CALLBACK write_handler_may_block(struct _honey_write_handler_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombWriteHandlerCppToC::Get(self)->MayBlock();

  // Return type: bool
  return _retval;
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

HoneycombWriteHandlerCppToC::HoneycombWriteHandlerCppToC() {
  GetStruct()->write = write_handler_write;
  GetStruct()->seek = write_handler_seek;
  GetStruct()->tell = write_handler_tell;
  GetStruct()->flush = write_handler_flush;
  GetStruct()->may_block = write_handler_may_block;
}

// DESTRUCTOR - Do not edit by hand.

HoneycombWriteHandlerCppToC::~HoneycombWriteHandlerCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
HoneycombRefPtr<HoneycombWriteHandler> HoneycombCppToCRefCounted<
    HoneycombWriteHandlerCppToC,
    HoneycombWriteHandler,
    honey_write_handler_t>::UnwrapDerived(HoneycombWrapperType type,
                                        honey_write_handler_t* s) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCppToCRefCounted<HoneycombWriteHandlerCppToC,
                                   HoneycombWriteHandler,
                                   honey_write_handler_t>::kWrapperType =
    WT_WRITE_HANDLER;