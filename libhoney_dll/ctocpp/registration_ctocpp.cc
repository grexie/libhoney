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
// $hash=004bfcbf30ee0b2ecc97828c943d1302c033b7d9$
//

#include "libhoney_dll/ctocpp/registration_ctocpp.h"
#include "libhoney_dll/shutdown_checker.h"

// CONSTRUCTOR - Do not edit by hand.

HoneycombRegistrationCToCpp::HoneycombRegistrationCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

HoneycombRegistrationCToCpp::~HoneycombRegistrationCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
honey_registration_t*
HoneycombCToCppRefCounted<HoneycombRegistrationCToCpp,
                    HoneycombRegistration,
                    honey_registration_t>::UnwrapDerived(HoneycombWrapperType type,
                                                       HoneycombRegistration* c) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCToCppRefCounted<HoneycombRegistrationCToCpp,
                                   HoneycombRegistration,
                                   honey_registration_t>::kWrapperType =
    WT_REGISTRATION;
