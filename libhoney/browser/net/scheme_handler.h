// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_NET_SCHEME_HANDLER_H_
#define HONEYCOMB_LIBHONEY_BROWSER_NET_SCHEME_HANDLER_H_
#pragma once

#include "include/honey_frame.h"

#include "content/public/browser/browser_context.h"
#include "url/gurl.h"

class HoneycombIOThreadState;

namespace scheme {

// Register the internal scheme handlers that can be overridden.
void RegisterInternalHandlers(HoneycombIOThreadState* iothread_state);

}  // namespace scheme

#endif  // HONEYCOMB_LIBHONEY_BROWSER_NET_SCHEME_HANDLER_H_
