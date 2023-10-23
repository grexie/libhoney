// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_SHARED_RENDERER_CLIENT_APP_RENDERER_H_
#define HONEYCOMB_TESTS_SHARED_RENDERER_CLIENT_APP_RENDERER_H_
#pragma once

#include <set>

#include "tests/shared/common/client_app.h"

namespace client {

// Client app implementation for the renderer process.
class ClientAppRenderer : public ClientApp, public HoneycombRenderProcessHandler {
 public:
  // Interface for renderer delegates. All Delegates must be returned via
  // CreateDelegates. Do not perform work in the Delegate
  // constructor. See HoneycombRenderProcessHandler for documentation.
  class Delegate : public virtual HoneycombBaseRefCounted {
   public:
    virtual void OnWebKitInitialized(HoneycombRefPtr<ClientAppRenderer> app) {}

    virtual void OnBrowserCreated(HoneycombRefPtr<ClientAppRenderer> app,
                                  HoneycombRefPtr<HoneycombBrowser> browser,
                                  HoneycombRefPtr<HoneycombDictionaryValue> extra_info) {}

    virtual void OnBrowserDestroyed(HoneycombRefPtr<ClientAppRenderer> app,
                                    HoneycombRefPtr<HoneycombBrowser> browser) {}

    virtual HoneycombRefPtr<HoneycombLoadHandler> GetLoadHandler(
        HoneycombRefPtr<ClientAppRenderer> app) {
      return nullptr;
    }

    virtual void OnContextCreated(HoneycombRefPtr<ClientAppRenderer> app,
                                  HoneycombRefPtr<HoneycombBrowser> browser,
                                  HoneycombRefPtr<HoneycombFrame> frame,
                                  HoneycombRefPtr<HoneycombV8Context> context) {}

    virtual void OnContextReleased(HoneycombRefPtr<ClientAppRenderer> app,
                                   HoneycombRefPtr<HoneycombBrowser> browser,
                                   HoneycombRefPtr<HoneycombFrame> frame,
                                   HoneycombRefPtr<HoneycombV8Context> context) {}

    virtual void OnUncaughtException(HoneycombRefPtr<ClientAppRenderer> app,
                                     HoneycombRefPtr<HoneycombBrowser> browser,
                                     HoneycombRefPtr<HoneycombFrame> frame,
                                     HoneycombRefPtr<HoneycombV8Context> context,
                                     HoneycombRefPtr<HoneycombV8Exception> exception,
                                     HoneycombRefPtr<HoneycombV8StackTrace> stackTrace) {}

    virtual void OnFocusedNodeChanged(HoneycombRefPtr<ClientAppRenderer> app,
                                      HoneycombRefPtr<HoneycombBrowser> browser,
                                      HoneycombRefPtr<HoneycombFrame> frame,
                                      HoneycombRefPtr<HoneycombDOMNode> node) {}

    // Called when a process message is received. Return true if the message was
    // handled and should not be passed on to other handlers. Delegates
    // should check for unique message names to avoid interfering with each
    // other.
    virtual bool OnProcessMessageReceived(
        HoneycombRefPtr<ClientAppRenderer> app,
        HoneycombRefPtr<HoneycombBrowser> browser,
        HoneycombRefPtr<HoneycombFrame> frame,
        HoneycombProcessId source_process,
        HoneycombRefPtr<HoneycombProcessMessage> message) {
      return false;
    }
  };

  typedef std::set<HoneycombRefPtr<Delegate>> DelegateSet;

  ClientAppRenderer();

 private:
  // Creates all of the Delegate objects. Implemented by honeyclient in
  // client_app_delegates_renderer.cc
  static void CreateDelegates(DelegateSet& delegates);

  // HoneycombApp methods.
  HoneycombRefPtr<HoneycombRenderProcessHandler> GetRenderProcessHandler() override {
    return this;
  }

  // HoneycombRenderProcessHandler methods.
  void OnWebKitInitialized() override;
  void OnBrowserCreated(HoneycombRefPtr<HoneycombBrowser> browser,
                        HoneycombRefPtr<HoneycombDictionaryValue> extra_info) override;
  void OnBrowserDestroyed(HoneycombRefPtr<HoneycombBrowser> browser) override;
  HoneycombRefPtr<HoneycombLoadHandler> GetLoadHandler() override;
  void OnContextCreated(HoneycombRefPtr<HoneycombBrowser> browser,
                        HoneycombRefPtr<HoneycombFrame> frame,
                        HoneycombRefPtr<HoneycombV8Context> context) override;
  void OnContextReleased(HoneycombRefPtr<HoneycombBrowser> browser,
                         HoneycombRefPtr<HoneycombFrame> frame,
                         HoneycombRefPtr<HoneycombV8Context> context) override;
  void OnUncaughtException(HoneycombRefPtr<HoneycombBrowser> browser,
                           HoneycombRefPtr<HoneycombFrame> frame,
                           HoneycombRefPtr<HoneycombV8Context> context,
                           HoneycombRefPtr<HoneycombV8Exception> exception,
                           HoneycombRefPtr<HoneycombV8StackTrace> stackTrace) override;
  void OnFocusedNodeChanged(HoneycombRefPtr<HoneycombBrowser> browser,
                            HoneycombRefPtr<HoneycombFrame> frame,
                            HoneycombRefPtr<HoneycombDOMNode> node) override;
  bool OnProcessMessageReceived(HoneycombRefPtr<HoneycombBrowser> browser,
                                HoneycombRefPtr<HoneycombFrame> frame,
                                HoneycombProcessId source_process,
                                HoneycombRefPtr<HoneycombProcessMessage> message) override;

 private:
  // Set of supported Delegates.
  DelegateSet delegates_;

  IMPLEMENT_REFCOUNTING(ClientAppRenderer);
  DISALLOW_COPY_AND_ASSIGN(ClientAppRenderer);
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_SHARED_RENDERER_CLIENT_APP_RENDERER_H_
