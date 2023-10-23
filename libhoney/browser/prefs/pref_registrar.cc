// Copyright (c) 2022 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "libhoney/browser/prefs/pref_registrar.h"

#include "include/honey_app.h"
#include "include/honey_browser_process_handler.h"
#include "include/honey_preference.h"
#include "libhoney/common/app_manager.h"
#include "libhoney/common/values_impl.h"

#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/pref_store.h"

namespace pref_registrar {

namespace {

class HoneycombPreferenceRegistrarImpl : public HoneycombPreferenceRegistrar {
 public:
  explicit HoneycombPreferenceRegistrarImpl(PrefRegistrySimple* registry)
      : registry_(registry) {}

  HoneycombPreferenceRegistrarImpl(const HoneycombPreferenceRegistrarImpl&) = delete;
  HoneycombPreferenceRegistrarImpl& operator=(const HoneycombPreferenceRegistrarImpl&) =
      delete;

  // HoneycombPreferenceRegistrar methods.
  bool AddPreference(const HoneycombString& name,
                     HoneycombRefPtr<HoneycombValue> default_value) override {
    const std::string nameStr = name;
    if (registry_->defaults()->GetValue(nameStr, nullptr)) {
      LOG(ERROR) << "Trying to register a previously registered preference: "
                 << nameStr;
      return false;
    }

    switch (default_value->GetType()) {
      case VTYPE_INVALID:
      case VTYPE_NULL:
      case VTYPE_BINARY:
        break;
      case VTYPE_BOOL:
        registry_->RegisterBooleanPref(nameStr, default_value->GetBool());
        return true;
      case VTYPE_INT:
        registry_->RegisterIntegerPref(nameStr, default_value->GetInt());
        return true;
      case VTYPE_DOUBLE:
        registry_->RegisterDoublePref(nameStr, default_value->GetDouble());
        return true;
      case VTYPE_STRING:
        registry_->RegisterStringPref(nameStr, default_value->GetString());
        return true;
      case VTYPE_DICTIONARY:
      case VTYPE_LIST:
        RegisterComplexPref(nameStr, default_value);
        return true;
    };

    LOG(ERROR) << "Invalid value type for preference: " << nameStr;
    return false;
  }

 private:
  void RegisterComplexPref(const std::string& name,
                           HoneycombRefPtr<HoneycombValue> default_value) {
    HoneycombValueImpl* impl = static_cast<HoneycombValueImpl*>(default_value.get());
    auto impl_value = impl->CopyValue();

    if (impl_value.type() == base::Value::Type::DICT) {
      registry_->RegisterDictionaryPref(name, std::move(impl_value.GetDict()));
    } else if (impl_value.type() == base::Value::Type::LIST) {
      registry_->RegisterListPref(name, std::move(impl_value.GetList()));
    } else {
      DCHECK(false);
    }
  }

  PrefRegistrySimple* const registry_;
};

}  // namespace

void RegisterCustomPrefs(honey_preferences_type_t type,
                         PrefRegistrySimple* registry) {
  if (auto app = HoneycombAppManager::Get()->GetApplication()) {
    if (auto handler = app->GetBrowserProcessHandler()) {
      HoneycombPreferenceRegistrarImpl registrar(registry);
      handler->OnRegisterCustomPreferences(type, &registrar);
    }
  }
}

}  // namespace pref_registrar
