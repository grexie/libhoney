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
// $hash=403513566867d0fd85166242648f269fac987ca5$
//

#include "libhoney_dll/cpptoc/response_filter_cpptoc.h"
#include "libhoney_dll/shutdown_checker.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

int HONEYCOMB_CALLBACK
response_filter_init_filter(struct _honey_response_filter_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombResponseFilterCppToC::Get(self)->InitFilter();

  // Return type: bool
  return _retval;
}

honey_response_filter_status_t HONEYCOMB_CALLBACK
response_filter_filter(struct _honey_response_filter_t* self,
                       void* data_in,
                       size_t data_in_size,
                       size_t* data_in_read,
                       void* data_out,
                       size_t data_out_size,
                       size_t* data_out_written) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return RESPONSE_FILTER_ERROR;
  }
  // Verify param: data_in_read; type: simple_byref
  DCHECK(data_in_read);
  if (!data_in_read) {
    return RESPONSE_FILTER_ERROR;
  }
  // Verify param: data_out; type: simple_byaddr
  DCHECK(data_out);
  if (!data_out) {
    return RESPONSE_FILTER_ERROR;
  }
  // Verify param: data_out_written; type: simple_byref
  DCHECK(data_out_written);
  if (!data_out_written) {
    return RESPONSE_FILTER_ERROR;
  }
  // Unverified params: data_in

  // Translate param: data_in_read; type: simple_byref
  size_t data_in_readVal = data_in_read ? *data_in_read : 0;
  // Translate param: data_out_written; type: simple_byref
  size_t data_out_writtenVal = data_out_written ? *data_out_written : 0;

  // Execute
  honey_response_filter_status_t _retval =
      HoneycombResponseFilterCppToC::Get(self)->Filter(
          data_in, data_in_size, data_in_readVal, data_out, data_out_size,
          data_out_writtenVal);

  // Restore param: data_in_read; type: simple_byref
  if (data_in_read) {
    *data_in_read = data_in_readVal;
  }
  // Restore param: data_out_written; type: simple_byref
  if (data_out_written) {
    *data_out_written = data_out_writtenVal;
  }

  // Return type: simple
  return _retval;
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

HoneycombResponseFilterCppToC::HoneycombResponseFilterCppToC() {
  GetStruct()->init_filter = response_filter_init_filter;
  GetStruct()->filter = response_filter_filter;
}

// DESTRUCTOR - Do not edit by hand.

HoneycombResponseFilterCppToC::~HoneycombResponseFilterCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
HoneycombRefPtr<HoneycombResponseFilter> HoneycombCppToCRefCounted<
    HoneycombResponseFilterCppToC,
    HoneycombResponseFilter,
    honey_response_filter_t>::UnwrapDerived(HoneycombWrapperType type,
                                          honey_response_filter_t* s) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCppToCRefCounted<HoneycombResponseFilterCppToC,
                                   HoneycombResponseFilter,
                                   honey_response_filter_t>::kWrapperType =
    WT_RESPONSE_FILTER;