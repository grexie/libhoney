// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBSIMPLE_SIMPLE_APP_H_
#define HONEYCOMB_TESTS_HONEYCOMBSIMPLE_SIMPLE_APP_H_

#include "include/honey_app.h"

// Implement application-level callbacks for the browser process.
class SimpleApp : public HoneycombApp, public HoneycombBrowserProcessHandler {
 public:
  SimpleApp();

  // HoneycombApp methods:
  HoneycombRefPtr<HoneycombBrowserProcessHandler> GetBrowserProcessHandler() override {
    return this;
  }

  // HoneycombBrowserProcessHandler methods:
  void OnContextInitialized() override;
  HoneycombRefPtr<HoneycombClient> GetDefaultClient() override;

 private:
  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(SimpleApp);
};

#endif  // HONEYCOMB_TESTS_HONEYCOMBSIMPLE_SIMPLE_APP_H_
