// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoneycomb/browser/net/scheme_handler.h"

#include <string>

#include "libhoneycomb/browser/net/chrome_scheme_handler.h"
#include "libhoneycomb/browser/net/devtools_scheme_handler.h"
#include "libhoneycomb/common/net/scheme_registration.h"
#include "libhoneycomb/features/runtime.h"

#include "content/public/common/url_constants.h"

namespace scheme {

void RegisterInternalHandlers(HoneycombIOThreadState* iothread_state) {
  if (!honey::IsAlloyRuntimeEnabled()) {
    return;
  }

  scheme::RegisterChromeDevToolsHandler(iothread_state);
}

}  // namespace scheme
