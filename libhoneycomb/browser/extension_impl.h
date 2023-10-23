// Copyright (c) 2017 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_EXTENSION_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_EXTENSION_IMPL_H_
#pragma once

#include <memory>

#include "include/honey_extension.h"
#include "include/honey_extension_handler.h"
#include "include/honey_request_context.h"

namespace extensions {
class Extension;
}

// HoneycombNavigationEntry implementation
class HoneycombExtensionImpl : public HoneycombExtension {
 public:
  HoneycombExtensionImpl(const extensions::Extension* extension,
                   HoneycombRequestContext* loader_context,
                   HoneycombRefPtr<HoneycombExtensionHandler> handler);

  HoneycombExtensionImpl(const HoneycombExtensionImpl&) = delete;
  HoneycombExtensionImpl& operator=(const HoneycombExtensionImpl&) = delete;

  // HoneycombExtension methods.
  HoneycombString GetIdentifier() override;
  HoneycombString GetPath() override;
  HoneycombRefPtr<HoneycombDictionaryValue> GetManifest() override;
  bool IsSame(HoneycombRefPtr<HoneycombExtension> that) override;
  HoneycombRefPtr<HoneycombExtensionHandler> GetHandler() override;
  HoneycombRefPtr<HoneycombRequestContext> GetLoaderContext() override;
  bool IsLoaded() override;
  void Unload() override;

  void OnExtensionLoaded();
  void OnExtensionUnloaded();

  // Use this instead of the GetLoaderContext version during
  // HoneycombRequestContext destruction.
  HoneycombRequestContext* loader_context() const { return loader_context_; }

 private:
  HoneycombString id_;
  HoneycombString path_;
  HoneycombRefPtr<HoneycombDictionaryValue> manifest_;

  HoneycombRequestContext* loader_context_;
  HoneycombRefPtr<HoneycombExtensionHandler> handler_;

  // Only accessed on the UI thread.
  bool unloaded_ = false;

  IMPLEMENT_REFCOUNTING(HoneycombExtensionImpl);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_EXTENSION_IMPL_H_
