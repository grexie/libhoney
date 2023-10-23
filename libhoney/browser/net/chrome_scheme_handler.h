// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_NET_CHROME_SCHEME_HANDLER_H_
#define HONEYCOMB_LIBHONEY_BROWSER_NET_CHROME_SCHEME_HANDLER_H_
#pragma once

#include <string>

#include "include/honey_browser.h"
#include "include/honey_frame.h"
#include "include/honey_process_message.h"

#include "url/gurl.h"

namespace base {
class ListValue;
}

namespace content {
class BrowserURLHandler;
}

namespace url {
class Origin;
}

namespace scheme {

extern const char kChromeURL[];

// Register the WebUI controller factory.
void RegisterWebUIControllerFactory();

// Register the WebUI handler.
void BrowserURLHandlerCreated(content::BrowserURLHandler* handler);

// Returns true if WebUI is allowed to make network requests.
bool IsWebUIAllowedToMakeNetworkRequests(const url::Origin& origin);

}  // namespace scheme

#endif  // HONEYCOMB_LIBHONEY_BROWSER_CHROME_SCHEME_HANDLER_H_
