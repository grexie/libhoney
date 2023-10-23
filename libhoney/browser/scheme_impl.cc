// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "include/honey_scheme.h"
#include "libhoney/browser/context.h"

#include "base/logging.h"

bool HoneycombRegisterSchemeHandlerFactory(
    const HoneycombString& scheme_name,
    const HoneycombString& domain_name,
    HoneycombRefPtr<HoneycombSchemeHandlerFactory> factory) {
  // Verify that the context is in a valid state.
  if (!CONTEXT_STATE_VALID()) {
    DCHECK(false) << "context not valid";
    return false;
  }

  return HoneycombRequestContext::GetGlobalContext()->RegisterSchemeHandlerFactory(
      scheme_name, domain_name, factory);
}

bool HoneycombClearSchemeHandlerFactories() {
  // Verify that the context is in a valid state.
  if (!CONTEXT_STATE_VALID()) {
    DCHECK(false) << "context not valid";
    return false;
  }

  return HoneycombRequestContext::GetGlobalContext()->ClearSchemeHandlerFactories();
}
