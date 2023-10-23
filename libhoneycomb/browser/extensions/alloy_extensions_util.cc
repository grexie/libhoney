// Copyright 2021 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoneycomb/browser/extensions/alloy_extensions_util.h"

#include "libhoneycomb/browser/alloy/alloy_browser_host_impl.h"

namespace extensions {
namespace alloy {

int GetTabIdForWebContents(content::WebContents* web_contents) {
  auto browser = AlloyBrowserHostImpl::GetBrowserForContents(web_contents);
  if (!browser) {
    return -1;
  }
  return browser->GetIdentifier();
}

}  // namespace alloy
}  // namespace extensions
