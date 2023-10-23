// Copyright (c) 2014 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_OSR_DRAGDROP_EVENTS_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_OSR_DRAGDROP_EVENTS_H_
#pragma once

#include "include/honey_render_handler.h"
#include "tests/honeyclient/browser/client_handler.h"

namespace client {

class OsrDragEvents {
 public:
  virtual HoneycombBrowserHost::DragOperationsMask OnDragEnter(
      HoneycombRefPtr<HoneycombDragData> drag_data,
      HoneycombMouseEvent ev,
      HoneycombBrowserHost::DragOperationsMask effect) = 0;

  virtual HoneycombBrowserHost::DragOperationsMask OnDragOver(
      HoneycombMouseEvent ev,
      HoneycombBrowserHost::DragOperationsMask effect) = 0;

  virtual void OnDragLeave() = 0;

  virtual HoneycombBrowserHost::DragOperationsMask OnDrop(
      HoneycombMouseEvent ev,
      HoneycombBrowserHost::DragOperationsMask effect) = 0;

 protected:
  virtual ~OsrDragEvents() {}
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_OSR_DRAGDROP_EVENTS_H_
