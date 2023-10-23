// Copyright (c) 2022 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_GLOBAL_PREFERENCE_MANAGER_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_GLOBAL_PREFERENCE_MANAGER_IMPL_H_
#pragma once

#include "include/honey_preference.h"

// Implementation of the HoneycombPreferenceManager interface for global preferences.
class HoneycombGlobalPreferenceManagerImpl : public HoneycombPreferenceManager {
 public:
  HoneycombGlobalPreferenceManagerImpl() = default;

  HoneycombGlobalPreferenceManagerImpl(const HoneycombGlobalPreferenceManagerImpl&) =
      delete;
  HoneycombGlobalPreferenceManagerImpl& operator=(
      const HoneycombGlobalPreferenceManagerImpl&) = delete;

  // HoneycombPreferenceManager methods.
  bool HasPreference(const HoneycombString& name) override;
  HoneycombRefPtr<HoneycombValue> GetPreference(const HoneycombString& name) override;
  HoneycombRefPtr<HoneycombDictionaryValue> GetAllPreferences(
      bool include_defaults) override;
  bool CanSetPreference(const HoneycombString& name) override;
  bool SetPreference(const HoneycombString& name,
                     HoneycombRefPtr<HoneycombValue> value,
                     HoneycombString& error) override;

 private:
  IMPLEMENT_REFCOUNTING(HoneycombGlobalPreferenceManagerImpl);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_GLOBAL_PREFERENCE_MANAGER_IMPL_H_
