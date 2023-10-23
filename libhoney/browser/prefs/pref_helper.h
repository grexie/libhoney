// Copyright (c) 2022 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_PREFS_PREF_HELPER_H_
#define HONEYCOMB_LIBHONEY_BROWSER_PREFS_PREF_HELPER_H_

#include "include/honey_values.h"

class PrefService;

namespace pref_helper {

// Function names and arguments match the HoneycombPreferenceManager interface.

bool HasPreference(PrefService* pref_service, const HoneycombString& name);

HoneycombRefPtr<HoneycombValue> GetPreference(PrefService* pref_service,
                                  const HoneycombString& name);

HoneycombRefPtr<HoneycombDictionaryValue> GetAllPreferences(PrefService* pref_service,
                                                bool include_defaults);

bool CanSetPreference(PrefService* pref_service, const HoneycombString& name);

bool SetPreference(PrefService* pref_service,
                   const HoneycombString& name,
                   HoneycombRefPtr<HoneycombValue> value,
                   HoneycombString& error);

}  // namespace pref_helper

#endif  // HONEYCOMB_LIBHONEY_BROWSER_PREFS_PREF_HELPER_H_
