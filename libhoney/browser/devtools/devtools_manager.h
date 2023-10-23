// Copyright (c) 2020 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_DEVTOOLS_DEVTOOLS_MANAGER_H_
#define HONEYCOMB_LIBHONEY_BROWSER_DEVTOOLS_DEVTOOLS_MANAGER_H_
#pragma once

#include "include/honey_browser.h"

#include "base/memory/weak_ptr.h"

class HoneycombBrowserHostBase;
class HoneycombDevToolsController;
class HoneycombDevToolsFrontend;

namespace content {
class WebContents;
}

// Manages DevTools instances. Methods must be called on the UI thread unless
// otherwise indicated.
class HoneycombDevToolsManager {
 public:
  // |inspected_browser| will outlive this object.
  explicit HoneycombDevToolsManager(HoneycombBrowserHostBase* inspected_browser);

  HoneycombDevToolsManager(const HoneycombDevToolsManager&) = delete;
  HoneycombDevToolsManager& operator=(const HoneycombDevToolsManager&) = delete;

  ~HoneycombDevToolsManager();

  // See HoneycombBrowserHost methods of the same name for documentation.
  void ShowDevTools(const HoneycombWindowInfo& windowInfo,
                    HoneycombRefPtr<HoneycombClient> client,
                    const HoneycombBrowserSettings& settings,
                    const HoneycombPoint& inspect_element_at);
  void CloseDevTools();
  bool HasDevTools();
  bool SendDevToolsMessage(const void* message, size_t message_size);
  int ExecuteDevToolsMethod(int message_id,
                            const HoneycombString& method,
                            HoneycombRefPtr<HoneycombDictionaryValue> param);

  // These methods are used to implement
  // HoneycombBrowserHost::AddDevToolsMessageObserver. CreateRegistration is safe to
  // call on any thread. InitializeRegistrationOnUIThread should be called
  // immediately afterwards on the UI thread.
  static HoneycombRefPtr<HoneycombRegistration> CreateRegistration(
      HoneycombRefPtr<HoneycombDevToolsMessageObserver> observer);
  void InitializeRegistrationOnUIThread(
      HoneycombRefPtr<HoneycombRegistration> registration);

 private:
  void OnFrontEndDestroyed();

  bool EnsureController();

  HoneycombBrowserHostBase* const inspected_browser_;

  // HoneycombDevToolsFrontend will delete itself when the frontend WebContents is
  // destroyed.
  HoneycombDevToolsFrontend* devtools_frontend_ = nullptr;

  // Used for sending DevTools protocol messages without an active frontend.
  std::unique_ptr<HoneycombDevToolsController> devtools_controller_;

  base::WeakPtrFactory<HoneycombDevToolsManager> weak_ptr_factory_;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_DEVTOOLS_DEVTOOLS_MANAGER_H_
