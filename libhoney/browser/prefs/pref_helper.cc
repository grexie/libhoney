// Copyright (c) 2022 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "libhoney/browser/prefs/pref_helper.h"

#include "libhoney/browser/thread_util.h"
#include "libhoney/common/values_impl.h"

#include "base/notreached.h"
#include "base/strings/stringprintf.h"
#include "components/prefs/pref_service.h"

namespace pref_helper {

namespace {

const char* GetTypeString(base::Value::Type type) {
  switch (type) {
    case base::Value::Type::NONE:
      return "NULL";
    case base::Value::Type::BOOLEAN:
      return "BOOLEAN";
    case base::Value::Type::INTEGER:
      return "INTEGER";
    case base::Value::Type::DOUBLE:
      return "DOUBLE";
    case base::Value::Type::STRING:
      return "STRING";
    case base::Value::Type::BINARY:
      return "BINARY";
    case base::Value::Type::DICT:
      return "DICTIONARY";
    case base::Value::Type::LIST:
      return "LIST";
  }

  DCHECK(false);
  return "UNKNOWN";
}

}  // namespace

bool HasPreference(PrefService* pref_service, const HoneycombString& name) {
  return (pref_service->FindPreference(name) != nullptr);
}

HoneycombRefPtr<HoneycombValue> GetPreference(PrefService* pref_service,
                                  const HoneycombString& name) {
  const PrefService::Preference* pref = pref_service->FindPreference(name);
  if (!pref) {
    return nullptr;
  }
  return new HoneycombValueImpl(pref->GetValue()->Clone());
}

HoneycombRefPtr<HoneycombDictionaryValue> GetAllPreferences(PrefService* pref_service,
                                                bool include_defaults) {
  // Returns a DeepCopy of the value.
  auto values = pref_service->GetPreferenceValues(
      include_defaults ? PrefService::INCLUDE_DEFAULTS
                       : PrefService::EXCLUDE_DEFAULTS);

  // HoneycombDictionaryValueImpl takes ownership of |values| contents.
  return new HoneycombDictionaryValueImpl(std::move(values), /*read_only=*/false);
}

bool CanSetPreference(PrefService* pref_service, const HoneycombString& name) {
  const PrefService::Preference* pref = pref_service->FindPreference(name);
  return (pref && pref->IsUserModifiable());
}

bool SetPreference(PrefService* pref_service,
                   const HoneycombString& name,
                   HoneycombRefPtr<HoneycombValue> value,
                   HoneycombString& error) {
  // The below validation logic should match PrefService::SetUserPrefValue.

  const PrefService::Preference* pref = pref_service->FindPreference(name);
  if (!pref) {
    error = "Trying to modify an unregistered preference";
    return false;
  }

  if (!pref->IsUserModifiable()) {
    error = "Trying to modify a preference that is not user modifiable";
    return false;
  }

  if (!value.get()) {
    // Reset the preference to its default value.
    pref_service->ClearPref(name);
    return true;
  }

  if (!value->IsValid()) {
    error = "A valid value is required";
    return false;
  }

  HoneycombValueImpl* impl = static_cast<HoneycombValueImpl*>(value.get());

  HoneycombValueImpl::ScopedLockedValue scoped_locked_value(impl);
  base::Value* impl_value = impl->GetValueUnsafe();

  if (pref->GetType() != impl_value->type()) {
    error = base::StringPrintf(
        "Trying to set a preference of type %s to value of type %s",
        GetTypeString(pref->GetType()), GetTypeString(impl_value->type()));
    return false;
  }

  // PrefService will make a DeepCopy of |impl_value|.
  pref_service->Set(name, *impl_value);
  return true;
}

}  // namespace pref_helper
