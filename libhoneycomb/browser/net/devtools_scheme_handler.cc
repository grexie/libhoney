// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "libhoneycomb/browser/net/devtools_scheme_handler.h"

#include <string>

#include "libhoneycomb/browser/iothread_state.h"
#include "libhoneycomb/browser/net/internal_scheme_handler.h"

#include "base/memory/ptr_util.h"
#include "base/strings/string_util.h"
#include "content/public/browser/devtools_frontend_host.h"
#include "content/public/common/url_constants.h"

namespace scheme {

const char kChromeDevToolsHost[] = "devtools";

namespace {

class Delegate : public InternalHandlerDelegate {
 public:
  Delegate() {}

  bool OnRequest(HoneycombRefPtr<HoneycombBrowser> browser,
                 HoneycombRefPtr<HoneycombRequest> request,
                 Action* action) override {
    GURL url = GURL(request->GetURL().ToString());
    std::string path = url.path();
    if (path.length() > 0) {
      path = path.substr(1);
    }

    action->bytes =
        content::DevToolsFrontendHost::GetFrontendResourceBytes(path);
    return !!action->bytes;
  }
};

}  // namespace

void RegisterChromeDevToolsHandler(HoneycombIOThreadState* iothread_state) {
  iothread_state->RegisterSchemeHandlerFactory(
      content::kChromeDevToolsScheme, kChromeDevToolsHost,
      CreateInternalHandlerFactory(base::WrapUnique(new Delegate())));
}

}  // namespace scheme
