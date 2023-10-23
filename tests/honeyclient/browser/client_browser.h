// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_CLIENT_BROWSER_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_CLIENT_BROWSER_H_
#pragma once

#include "include/honey_base.h"
#include "tests/shared/browser/client_app_browser.h"

namespace client {
namespace browser {

// Create the browser delegate. Called from client_app_delegates_browser.cc.
void CreateDelegates(ClientAppBrowser::DelegateSet& delegates);

}  // namespace browser
}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_CLIENT_BROWSER_H_
