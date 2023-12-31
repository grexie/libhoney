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
// $hash=fccbe16b1237f36acf744609774b304eb6de8b98$
//

#include "libhoney_dll/cpptoc/domvisitor_cpptoc.h"
#include "libhoney_dll/ctocpp/domdocument_ctocpp.h"
#include "libhoney_dll/shutdown_checker.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

void HONEYCOMB_CALLBACK domvisitor_visit(struct _honey_domvisitor_t* self,
                                   struct _honey_domdocument_t* document) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: document; type: refptr_diff
  DCHECK(document);
  if (!document) {
    return;
  }

  // Execute
  HoneycombDOMVisitorCppToC::Get(self)->Visit(HoneycombDOMDocumentCToCpp::Wrap(document));
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

HoneycombDOMVisitorCppToC::HoneycombDOMVisitorCppToC() {
  GetStruct()->visit = domvisitor_visit;
}

// DESTRUCTOR - Do not edit by hand.

HoneycombDOMVisitorCppToC::~HoneycombDOMVisitorCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
HoneycombRefPtr<HoneycombDOMVisitor>
HoneycombCppToCRefCounted<HoneycombDOMVisitorCppToC, HoneycombDOMVisitor, honey_domvisitor_t>::
    UnwrapDerived(HoneycombWrapperType type, honey_domvisitor_t* s) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCppToCRefCounted<HoneycombDOMVisitorCppToC,
                                   HoneycombDOMVisitor,
                                   honey_domvisitor_t>::kWrapperType =
    WT_DOMVISITOR;
