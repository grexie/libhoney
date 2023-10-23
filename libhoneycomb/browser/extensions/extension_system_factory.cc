// Copyright 2014 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoneycomb/browser/extensions/extension_system_factory.h"

#include "libhoneycomb/browser/extensions/extension_system.h"

#include "chrome/browser/profiles/incognito_helpers.h"
#include "components/keyed_service/content/browser_context_dependency_manager.h"
#include "extensions/browser/extension_prefs_factory.h"
#include "extensions/browser/extension_registry_factory.h"

using content::BrowserContext;

namespace extensions {

ExtensionSystem* HoneycombExtensionSystemFactory::GetForBrowserContext(
    BrowserContext* context) {
  return static_cast<HoneycombExtensionSystem*>(
      GetInstance()->GetServiceForBrowserContext(context, true));
}

// static
HoneycombExtensionSystemFactory* HoneycombExtensionSystemFactory::GetInstance() {
  return base::Singleton<HoneycombExtensionSystemFactory>::get();
}

HoneycombExtensionSystemFactory::HoneycombExtensionSystemFactory()
    : ExtensionSystemProvider("HoneycombExtensionSystem",
                              BrowserContextDependencyManager::GetInstance()) {
  // Other factories that this factory depends on. See
  // libhoneycomb/common/extensions/api/README.txt for additional details.
  DependsOn(ExtensionPrefsFactory::GetInstance());
  DependsOn(ExtensionRegistryFactory::GetInstance());
}

HoneycombExtensionSystemFactory::~HoneycombExtensionSystemFactory() {}

std::unique_ptr<KeyedService>
HoneycombExtensionSystemFactory::BuildServiceInstanceForBrowserContext(
    BrowserContext* context) const {
  return std::make_unique<HoneycombExtensionSystem>(context);
}

BrowserContext* HoneycombExtensionSystemFactory::GetBrowserContextToUse(
    BrowserContext* context) const {
  // Use a separate instance for incognito.
  return chrome::GetBrowserContextOwnInstanceInIncognito(context);
}

bool HoneycombExtensionSystemFactory::ServiceIsCreatedWithBrowserContext() const {
  return true;
}

}  // namespace extensions
