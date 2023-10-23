// Copyright (c) 2022 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoney/browser/global_preference_manager_impl.h"

#include "libhoney/browser/context.h"
#include "libhoney/browser/prefs/pref_helper.h"
#include "libhoney/browser/thread_util.h"

#include "chrome/browser/browser_process.h"

bool HoneycombGlobalPreferenceManagerImpl::HasPreference(const HoneycombString& name) {
  HONEYCOMB_REQUIRE_UIT_RETURN(false);
  return pref_helper::HasPreference(g_browser_process->local_state(), name);
}

HoneycombRefPtr<HoneycombValue> HoneycombGlobalPreferenceManagerImpl::GetPreference(
    const HoneycombString& name) {
  HONEYCOMB_REQUIRE_UIT_RETURN(nullptr);
  return pref_helper::GetPreference(g_browser_process->local_state(), name);
}

HoneycombRefPtr<HoneycombDictionaryValue> HoneycombGlobalPreferenceManagerImpl::GetAllPreferences(
    bool include_defaults) {
  HONEYCOMB_REQUIRE_UIT_RETURN(nullptr);
  return pref_helper::GetAllPreferences(g_browser_process->local_state(),
                                        include_defaults);
}

bool HoneycombGlobalPreferenceManagerImpl::CanSetPreference(const HoneycombString& name) {
  HONEYCOMB_REQUIRE_UIT_RETURN(false);
  return pref_helper::CanSetPreference(g_browser_process->local_state(), name);
}

bool HoneycombGlobalPreferenceManagerImpl::SetPreference(const HoneycombString& name,
                                                   HoneycombRefPtr<HoneycombValue> value,
                                                   HoneycombString& error) {
  HONEYCOMB_REQUIRE_UIT_RETURN(false);
  return pref_helper::SetPreference(g_browser_process->local_state(), name,
                                    value, error);
}

// static
HoneycombRefPtr<HoneycombPreferenceManager>
HoneycombPreferenceManager::GetGlobalPreferenceManager() {
  // Verify that the context is in a valid state.
  if (!CONTEXT_STATE_VALID()) {
    DCHECK(false) << "context not valid";
    return nullptr;
  }

  return new HoneycombGlobalPreferenceManagerImpl();
}
