// Copyright (c) 2022 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_PREFS_PREF_REGISTRAR_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_PREFS_PREF_REGISTRAR_H_

#include "include/internal/honey_types.h"

class PrefRegistrySimple;

namespace pref_registrar {

void RegisterCustomPrefs(honey_preferences_type_t type,
                         PrefRegistrySimple* registry);

}  // namespace pref_registrar

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_PREFS_PREF_REGISTRAR_H_
