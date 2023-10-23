// Copyright (c) 2022 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_CLIENT_PREFS_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_CLIENT_PREFS_H_
#pragma once

#include <optional>

#include "include/honey_base.h"
#include "include/honey_preference.h"

namespace client {
namespace prefs {

// Register global preferences with default values.
void RegisterGlobalPreferences(HoneycombRawPtr<HoneycombPreferenceRegistrar> registrar);

// Load/save window restore info.
bool LoadWindowRestorePreferences(honey_show_state_t& show_state,
                                  std::optional<HoneycombRect>& dip_bounds);
bool SaveWindowRestorePreferences(honey_show_state_t show_state,
                                  std::optional<HoneycombRect> dip_bounds);

}  // namespace prefs
}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_CLIENT_PREFS_H_
