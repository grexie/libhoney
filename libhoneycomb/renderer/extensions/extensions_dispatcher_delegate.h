// Copyright 2015 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_RENDERER_EXTENSIONS_EXTENSIONS_DISPATCHER_DELEGATE_H_
#define HONEYCOMB_LIBHONEYCOMB_RENDERER_EXTENSIONS_EXTENSIONS_DISPATCHER_DELEGATE_H_

#include "extensions/renderer/dispatcher_delegate.h"

namespace extensions {

class HoneycombExtensionsDispatcherDelegate : public DispatcherDelegate {
 public:
  HoneycombExtensionsDispatcherDelegate();

  HoneycombExtensionsDispatcherDelegate(const HoneycombExtensionsDispatcherDelegate&) =
      delete;
  HoneycombExtensionsDispatcherDelegate& operator=(
      const HoneycombExtensionsDispatcherDelegate&) = delete;

  ~HoneycombExtensionsDispatcherDelegate() override;

  void PopulateSourceMap(
      extensions::ResourceBundleSourceMap* source_map) override;
};

}  // namespace extensions

#endif  // HONEYCOMB_LIBHONEYCOMB_RENDERER_EXTENSIONS_EXTENSIONS_DISPATCHER_DELEGATE_H_
