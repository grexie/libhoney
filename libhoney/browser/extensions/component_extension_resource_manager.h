// Copyright 2014 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_COMPONENT_EXTENSION_RESOURCE_MANAGER_H_
#define HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_COMPONENT_EXTENSION_RESOURCE_MANAGER_H_

#include <map>

#include "base/files/file_path.h"
#include "extensions/browser/component_extension_resource_manager.h"

namespace webui {
struct ResourcePath;
}

namespace extensions {

class HoneycombComponentExtensionResourceManager
    : public ComponentExtensionResourceManager {
 public:
  HoneycombComponentExtensionResourceManager();

  HoneycombComponentExtensionResourceManager(
      const HoneycombComponentExtensionResourceManager&) = delete;
  HoneycombComponentExtensionResourceManager& operator=(
      const HoneycombComponentExtensionResourceManager&) = delete;

  ~HoneycombComponentExtensionResourceManager() override;

  // Overridden from ComponentExtensionResourceManager:
  bool IsComponentExtensionResource(const base::FilePath& extension_path,
                                    const base::FilePath& resource_path,
                                    int* resource_id) const override;
  const ui::TemplateReplacements* GetTemplateReplacementsForExtension(
      const std::string& extension_id) const override;

 private:
  void AddComponentResourceEntries(const webui::ResourcePath* entries,
                                   size_t size);

  // A map from a resource path to the resource ID.  Used by
  // IsComponentExtensionResource.
  std::map<base::FilePath, int> path_to_resource_info_;

  // A map from an extension ID to its i18n template replacements.
  using TemplateReplacementMap =
      std::map<std::string, ui::TemplateReplacements>;
  TemplateReplacementMap template_replacements_;
};

}  // namespace extensions

#endif  // HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_COMPONENT_EXTENSION_RESOURCE_MANAGER_H_
