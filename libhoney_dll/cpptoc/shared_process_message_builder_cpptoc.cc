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
// $hash=820d021d1a2ca4ef4b3fa858f10649676ca46bc8$
//

#include "libhoney_dll/cpptoc/shared_process_message_builder_cpptoc.h"
#include "libhoney_dll/cpptoc/process_message_cpptoc.h"
#include "libhoney_dll/shutdown_checker.h"

// GLOBAL FUNCTIONS - Body may be edited by hand.

HONEYCOMB_EXPORT honey_shared_process_message_builder_t*
honey_shared_process_message_builder_create(const honey_string_t* name,
                                          size_t byte_size) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: name; type: string_byref_const
  DCHECK(name);
  if (!name) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombSharedProcessMessageBuilder> _retval =
      HoneycombSharedProcessMessageBuilder::Create(HoneycombString(name), byte_size);

  // Return type: refptr_same
  return HoneycombSharedProcessMessageBuilderCppToC::Wrap(_retval);
}

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

int HONEYCOMB_CALLBACK shared_process_message_builder_is_valid(
    struct _honey_shared_process_message_builder_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombSharedProcessMessageBuilderCppToC::Get(self)->IsValid();

  // Return type: bool
  return _retval;
}

size_t HONEYCOMB_CALLBACK shared_process_message_builder_size(
    struct _honey_shared_process_message_builder_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  size_t _retval = HoneycombSharedProcessMessageBuilderCppToC::Get(self)->Size();

  // Return type: simple
  return _retval;
}

void* HONEYCOMB_CALLBACK shared_process_message_builder_memory(
    struct _honey_shared_process_message_builder_t* self) {
  shutdown_checker::AssertNotShutdown();

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  void* _retval = HoneycombSharedProcessMessageBuilderCppToC::Get(self)->Memory();

  // Return type: simple
  return _retval;
}

honey_process_message_t* HONEYCOMB_CALLBACK shared_process_message_builder_build(
    struct _honey_shared_process_message_builder_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombProcessMessage> _retval =
      HoneycombSharedProcessMessageBuilderCppToC::Get(self)->Build();

  // Return type: refptr_same
  return HoneycombProcessMessageCppToC::Wrap(_retval);
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

HoneycombSharedProcessMessageBuilderCppToC::HoneycombSharedProcessMessageBuilderCppToC() {
  GetStruct()->is_valid = shared_process_message_builder_is_valid;
  GetStruct()->size = shared_process_message_builder_size;
  GetStruct()->memory = shared_process_message_builder_memory;
  GetStruct()->build = shared_process_message_builder_build;
}

// DESTRUCTOR - Do not edit by hand.

HoneycombSharedProcessMessageBuilderCppToC::~HoneycombSharedProcessMessageBuilderCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
HoneycombRefPtr<HoneycombSharedProcessMessageBuilder>
HoneycombCppToCRefCounted<HoneycombSharedProcessMessageBuilderCppToC,
                    HoneycombSharedProcessMessageBuilder,
                    honey_shared_process_message_builder_t>::
    UnwrapDerived(HoneycombWrapperType type,
                  honey_shared_process_message_builder_t* s) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType
    HoneycombCppToCRefCounted<HoneycombSharedProcessMessageBuilderCppToC,
                        HoneycombSharedProcessMessageBuilder,
                        honey_shared_process_message_builder_t>::kWrapperType =
        WT_SHARED_PROCESS_MESSAGE_BUILDER;