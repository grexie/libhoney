// Copyright (c) 2010 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_PREFS_RENDERER_PREFS_H_
#define HONEYCOMB_LIBHONEY_BROWSER_PREFS_RENDERER_PREFS_H_
#pragma once

#include "include/internal/honey_types_wrappers.h"

#include "third_party/skia/include/core/SkColor.h"

class CommandLinePrefStore;

namespace blink {
namespace web_pref {
struct WebPreferences;
}
}  // namespace blink

namespace content {
class RenderViewHost;
class WebContents;
}  // namespace content

namespace user_prefs {
class PrefRegistrySyncable;
}

namespace renderer_prefs {

// Register additional renderer-related preferences.
void RegisterProfilePrefs(user_prefs::PrefRegistrySyncable* registry,
                          const std::string& locale);

// Set default values based on Honeycomb command-line flags for preferences that are
// available via the PrefService. Chromium command-line flags should not exist
// for these preferences.
void SetCommandLinePrefDefaults(CommandLinePrefStore* prefs);

// Set default values based on Honeycomb command-line flags for preferences that are
// not available via the PrefService. Chromium command-line flags should not
// exist for these preferences.
void SetDefaultPrefs(blink::web_pref::WebPreferences& web);

// Set preferences based on HoneycombBrowserSettings.
void SetHoneycombPrefs(const HoneycombBrowserSettings& honey,
                 blink::web_pref::WebPreferences& web);

// Populate WebPreferences based on a combination of command-line values,
// PrefService and HoneycombBrowserSettings.
void PopulateWebPreferences(content::RenderViewHost* rvh,
                            blink::web_pref::WebPreferences& web,
                            SkColor& base_background_color);
bool PopulateWebPreferencesAfterNavigation(
    content::WebContents* web_contents,
    blink::web_pref::WebPreferences& web);

}  // namespace renderer_prefs

#endif  // HONEYCOMB_LIBHONEY_BROWSER_PREFS_RENDERER_PREFS_H_
