// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/shared/renderer/client_app_renderer.h"

#include "include/base/honey_logging.h"

namespace client {

ClientAppRenderer::ClientAppRenderer() {
  CreateDelegates(delegates_);
}

void ClientAppRenderer::OnWebKitInitialized() {
  DelegateSet::iterator it = delegates_.begin();
  for (; it != delegates_.end(); ++it) {
    (*it)->OnWebKitInitialized(this);
  }
}

void ClientAppRenderer::OnBrowserCreated(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombDictionaryValue> extra_info) {
  DelegateSet::iterator it = delegates_.begin();
  for (; it != delegates_.end(); ++it) {
    (*it)->OnBrowserCreated(this, browser, extra_info);
  }
}

void ClientAppRenderer::OnBrowserDestroyed(HoneycombRefPtr<HoneycombBrowser> browser) {
  DelegateSet::iterator it = delegates_.begin();
  for (; it != delegates_.end(); ++it) {
    (*it)->OnBrowserDestroyed(this, browser);
  }
}

HoneycombRefPtr<HoneycombLoadHandler> ClientAppRenderer::GetLoadHandler() {
  HoneycombRefPtr<HoneycombLoadHandler> load_handler;
  DelegateSet::iterator it = delegates_.begin();
  for (; it != delegates_.end() && !load_handler.get(); ++it) {
    load_handler = (*it)->GetLoadHandler(this);
  }

  return load_handler;
}

void ClientAppRenderer::OnContextCreated(HoneycombRefPtr<HoneycombBrowser> browser,
                                         HoneycombRefPtr<HoneycombFrame> frame,
                                         HoneycombRefPtr<HoneycombV8Context> context) {
  DelegateSet::iterator it = delegates_.begin();
  for (; it != delegates_.end(); ++it) {
    (*it)->OnContextCreated(this, browser, frame, context);
  }
}

void ClientAppRenderer::OnContextReleased(HoneycombRefPtr<HoneycombBrowser> browser,
                                          HoneycombRefPtr<HoneycombFrame> frame,
                                          HoneycombRefPtr<HoneycombV8Context> context) {
  DelegateSet::iterator it = delegates_.begin();
  for (; it != delegates_.end(); ++it) {
    (*it)->OnContextReleased(this, browser, frame, context);
  }
}

void ClientAppRenderer::OnUncaughtException(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombFrame> frame,
    HoneycombRefPtr<HoneycombV8Context> context,
    HoneycombRefPtr<HoneycombV8Exception> exception,
    HoneycombRefPtr<HoneycombV8StackTrace> stackTrace) {
  DelegateSet::iterator it = delegates_.begin();
  for (; it != delegates_.end(); ++it) {
    (*it)->OnUncaughtException(this, browser, frame, context, exception,
                               stackTrace);
  }
}

void ClientAppRenderer::OnFocusedNodeChanged(HoneycombRefPtr<HoneycombBrowser> browser,
                                             HoneycombRefPtr<HoneycombFrame> frame,
                                             HoneycombRefPtr<HoneycombDOMNode> node) {
  DelegateSet::iterator it = delegates_.begin();
  for (; it != delegates_.end(); ++it) {
    (*it)->OnFocusedNodeChanged(this, browser, frame, node);
  }
}

bool ClientAppRenderer::OnProcessMessageReceived(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombFrame> frame,
    HoneycombProcessId source_process,
    HoneycombRefPtr<HoneycombProcessMessage> message) {
  DCHECK_EQ(source_process, PID_BROWSER);

  bool handled = false;

  DelegateSet::iterator it = delegates_.begin();
  for (; it != delegates_.end() && !handled; ++it) {
    handled = (*it)->OnProcessMessageReceived(this, browser, frame,
                                              source_process, message);
  }

  return handled;
}

}  // namespace client
