// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_CLIENT_HANDLER_STD_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_CLIENT_HANDLER_STD_H_
#pragma once

#include "tests/honeyclient/browser/client_handler.h"

namespace client {

// Client handler implementation for windowed browsers. There will only ever be
// one browser per handler instance.
class ClientHandlerStd : public ClientHandler {
 public:
  ClientHandlerStd(Delegate* delegate,
                   bool with_controls,
                   const std::string& startup_url);

 private:
  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(ClientHandlerStd);
  DISALLOW_COPY_AND_ASSIGN(ClientHandlerStd);
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_CLIENT_HANDLER_STD_H_
