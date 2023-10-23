// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_NET_DEVTOOLS_SCHEME_HANDLER_H_
#define HONEYCOMB_LIBHONEY_BROWSER_NET_DEVTOOLS_SCHEME_HANDLER_H_
#pragma once

class HoneycombIOThreadState;

namespace scheme {

extern const char kChromeDevToolsHost[];

// Register the chrome-devtools scheme handler.
void RegisterChromeDevToolsHandler(HoneycombIOThreadState* iothread_state);

}  // namespace scheme

#endif  // HONEYCOMB_LIBHONEY_BROWSER_NET_DEVTOOLS_SCHEME_HANDLER_H_
