// Copyright 2014 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_EXTENSION_SYSTEM_FACTORY_H_
#define HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_EXTENSION_SYSTEM_FACTORY_H_

#include "base/memory/singleton.h"
#include "extensions/browser/extension_system_provider.h"

namespace extensions {

// Factory that provides HoneycombExtensionSystem.
class HoneycombExtensionSystemFactory : public ExtensionSystemProvider {
 public:
  HoneycombExtensionSystemFactory(const HoneycombExtensionSystemFactory&) = delete;
  HoneycombExtensionSystemFactory& operator=(const HoneycombExtensionSystemFactory&) =
      delete;

  // ExtensionSystemProvider implementation:
  ExtensionSystem* GetForBrowserContext(
      content::BrowserContext* context) override;

  static HoneycombExtensionSystemFactory* GetInstance();

 private:
  friend struct base::DefaultSingletonTraits<HoneycombExtensionSystemFactory>;

  HoneycombExtensionSystemFactory();
  ~HoneycombExtensionSystemFactory() override;

  // BrowserContextKeyedServiceFactory implementation:
  std::unique_ptr<KeyedService> BuildServiceInstanceForBrowserContext(
      content::BrowserContext* context) const override;
  content::BrowserContext* GetBrowserContextToUse(
      content::BrowserContext* context) const override;
  bool ServiceIsCreatedWithBrowserContext() const override;
};

}  // namespace extensions

#endif  // HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_EXTENSION_SYSTEM_FACTORY_H_
