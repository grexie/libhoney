// Copyright 2021 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_ALLOY_EXTENSIONS_UTIL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_ALLOY_EXTENSIONS_UTIL_H_

namespace content {
class WebContents;
}

namespace extensions {
namespace alloy {

// Returns the tabId for |web_contents|, or -1 if not found.
int GetTabIdForWebContents(content::WebContents* web_contents);

}  // namespace alloy
}  // namespace extensions

#endif  // HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_ALLOY_EXTENSIONS_UTIL_H_
