// Copyright (c) 2022 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeyclient/browser/client_prefs.h"

#include <memory>

#include "include/base/honey_logging.h"
#include "include/honey_command_line.h"
#include "include/honey_preference.h"
#include "include/honey_values.h"
#include "include/views/honey_display.h"
#include "include/wrapper/honey_helpers.h"
#include "tests/shared/common/client_switches.h"
#include "tests/shared/common/string_util.h"

namespace client {
namespace prefs {

namespace {

constexpr char kPrefWindowRestore[] = "honeyclient.window_restore";

constexpr char kWindowRestoreStateKey[] = "state";
constexpr char kWindowRestoreBoundsKey[] = "bounds";
constexpr char kWindowRestoreBoundsKey_X[] = "x";
constexpr char kWindowRestoreBoundsKey_Y[] = "y";
constexpr char kWindowRestoreBoundsKey_W[] = "w";
constexpr char kWindowRestoreBoundsKey_H[] = "h";

static struct {
  const char* str;
  honey_show_state_t state;
} const kWindowRestoreStateValueMap[] = {
    {"normal", HONEYCOMB_SHOW_STATE_NORMAL},
    {"minimized", HONEYCOMB_SHOW_STATE_MINIMIZED},
    {"maximized", HONEYCOMB_SHOW_STATE_MAXIMIZED},
    {"fullscreen", HONEYCOMB_SHOW_STATE_FULLSCREEN},
};

std::optional<honey_show_state_t> ShowStateFromString(const std::string& str) {
  const auto strLower = AsciiStrToLower(str);
  for (size_t i = 0; i < std::size(kWindowRestoreStateValueMap); ++i) {
    if (strLower == kWindowRestoreStateValueMap[i].str) {
      return kWindowRestoreStateValueMap[i].state;
    }
  }
  return std::nullopt;
}

const char* ShowStateToString(honey_show_state_t show_state) {
  for (size_t i = 0; i < std::size(kWindowRestoreStateValueMap); ++i) {
    if (show_state == kWindowRestoreStateValueMap[i].state) {
      return kWindowRestoreStateValueMap[i].str;
    }
  }
  NOTREACHED();
  return nullptr;
}

// Create the HoneycombValue representation that will be stored in preferences.
HoneycombRefPtr<HoneycombValue> CreateWindowRestoreValue(
    honey_show_state_t show_state,
    std::optional<HoneycombRect> dip_bounds) {
  auto dict = HoneycombDictionaryValue::Create();

  // Show state is required.
  dict->SetString(kWindowRestoreStateKey, ShowStateToString(show_state));

  // Bounds is optional.
  if (dip_bounds) {
    auto bounds_dict = HoneycombDictionaryValue::Create();
    bounds_dict->SetInt(kWindowRestoreBoundsKey_X, dip_bounds->x);
    bounds_dict->SetInt(kWindowRestoreBoundsKey_Y, dip_bounds->y);
    bounds_dict->SetInt(kWindowRestoreBoundsKey_W, dip_bounds->width);
    bounds_dict->SetInt(kWindowRestoreBoundsKey_H, dip_bounds->height);
    dict->SetDictionary(kWindowRestoreBoundsKey, bounds_dict);
  }

  auto value = HoneycombValue::Create();
  value->SetDictionary(dict);
  return value;
}

HoneycombRefPtr<HoneycombValue> CreateDefaultWindowRestoreValue() {
  return CreateWindowRestoreValue(HONEYCOMB_SHOW_STATE_NORMAL, std::nullopt);
}

// Parse the HoneycombValue representation that was stored in preferences.
bool ParseWindowRestoreValue(HoneycombRefPtr<HoneycombValue> value,
                             honey_show_state_t& show_state,
                             std::optional<HoneycombRect>& dip_bounds) {
  if (!value || value->GetType() != VTYPE_DICTIONARY) {
    return false;
  }

  auto dict = value->GetDictionary();

  bool has_state = false;

  // Show state is required.
  if (dict->GetType(kWindowRestoreStateKey) == VTYPE_STRING) {
    auto result = ShowStateFromString(dict->GetString(kWindowRestoreStateKey));
    if (result) {
      show_state = *result;
      has_state = true;
    }
  }

  // Bounds is optional.
  if (has_state && dict->GetType(kWindowRestoreBoundsKey) == VTYPE_DICTIONARY) {
    auto bounds_dict = dict->GetDictionary(kWindowRestoreBoundsKey);
    if (bounds_dict->GetType(kWindowRestoreBoundsKey_X) == VTYPE_INT &&
        bounds_dict->GetType(kWindowRestoreBoundsKey_Y) == VTYPE_INT &&
        bounds_dict->GetType(kWindowRestoreBoundsKey_W) == VTYPE_INT &&
        bounds_dict->GetType(kWindowRestoreBoundsKey_H) == VTYPE_INT) {
      dip_bounds = HoneycombRect(bounds_dict->GetInt(kWindowRestoreBoundsKey_X),
                           bounds_dict->GetInt(kWindowRestoreBoundsKey_Y),
                           bounds_dict->GetInt(kWindowRestoreBoundsKey_W),
                           bounds_dict->GetInt(kWindowRestoreBoundsKey_H));
    }
  }

  return has_state;
}

// Keep the bounds inside the closest display work area.
HoneycombRect ClampBoundsToDisplay(const HoneycombRect& dip_bounds) {
  auto display = HoneycombDisplay::GetDisplayMatchingBounds(
      dip_bounds, /*input_pixel_coords=*/false);
  const auto work_area = display->GetWorkArea();

  HoneycombRect bounds = dip_bounds;

  if (bounds.width > work_area.width) {
    bounds.width = work_area.width;
  }
  if (bounds.height > work_area.height) {
    bounds.height = work_area.height;
  }

  if (bounds.x < work_area.x) {
    bounds.x = work_area.x;
  } else if (bounds.x + bounds.width >= work_area.x + work_area.width) {
    bounds.x = work_area.x + work_area.width - bounds.width;
  }

  if (bounds.y < work_area.y) {
    bounds.y = work_area.y;
  } else if (bounds.y + bounds.height >= work_area.y + work_area.height) {
    bounds.y = work_area.y + work_area.height - bounds.height;
  }

  return bounds;
}

}  // namespace

void RegisterGlobalPreferences(HoneycombRawPtr<HoneycombPreferenceRegistrar> registrar) {
  registrar->AddPreference(kPrefWindowRestore,
                           CreateDefaultWindowRestoreValue());
}

bool LoadWindowRestorePreferences(honey_show_state_t& show_state,
                                  std::optional<HoneycombRect>& dip_bounds) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  // Check if show state was specified on the command-line.
  auto command_line = HoneycombCommandLine::GetGlobalCommandLine();
  if (command_line->HasSwitch(switches::kInitialShowState)) {
    auto result = ShowStateFromString(
        command_line->GetSwitchValue(switches::kInitialShowState));
    if (result) {
      show_state = *result;
      return true;
    }
  }

  // Check if show state was saved in global preferences.
  auto manager = HoneycombPreferenceManager::GetGlobalPreferenceManager();
  if (ParseWindowRestoreValue(manager->GetPreference(kPrefWindowRestore),
                              show_state, dip_bounds)) {
    if (dip_bounds) {
      // Keep the bounds inside the closest display.
      dip_bounds = ClampBoundsToDisplay(*dip_bounds);
    }
    return true;
  }

  return false;
}

bool SaveWindowRestorePreferences(honey_show_state_t show_state,
                                  std::optional<HoneycombRect> dip_bounds) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  auto manager = HoneycombPreferenceManager::GetGlobalPreferenceManager();

  HoneycombString error;
  return manager->SetPreference(
      kPrefWindowRestore, CreateWindowRestoreValue(show_state, dip_bounds),
      error);
}

}  // namespace prefs
}  // namespace client
