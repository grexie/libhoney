// Copyright (c) 2016 The Honeycomb Authors.
// Portions copyright (c) 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoneycomb/browser/alloy/chrome_profile_manager_alloy.h"

#include "libhoneycomb/browser/browser_context.h"
#include "libhoneycomb/browser/request_context_impl.h"
#include "libhoneycomb/common/app_manager.h"

namespace {

// Return the active browser context. This is primarily called from Chrome code
// that handles WebUI views and wishes to associate the view's data with a
// particular context (profile). Chrome stores multiple profiles in sub-
// directories of |user_data_dir| and then uses ProfileManager to track which
// profile (sub-directory name) was last active.
//
// TODO(honey): To most closely match Chrome behavior this should return the
// context for the currently active browser (e.g. the browser with input focus).
// Return the main context for now since we don't currently have a good way to
// determine that.
HoneycombBrowserContext* GetActiveBrowserContext() {
  auto request_context = static_cast<HoneycombRequestContextImpl*>(
      HoneycombAppManager::Get()->GetGlobalRequestContext().get());
  return request_context->GetBrowserContext();
}

}  // namespace

ChromeProfileManagerAlloy::ChromeProfileManagerAlloy()
    : ProfileManager(base::FilePath()) {}

ChromeProfileManagerAlloy::~ChromeProfileManagerAlloy() {}

Profile* ChromeProfileManagerAlloy::GetProfile(
    const base::FilePath& profile_dir) {
  HoneycombBrowserContext* browser_context =
      HoneycombBrowserContext::FromCachePath(profile_dir);
  if (!browser_context) {
    // ProfileManager makes assumptions about profile directory paths that do
    // not match Honeycomb usage. For example, the default Chrome profile name is
    // "Default" so it will append that sub-directory name to an empty
    // |user_data_dir| value and then call this method. Use the active context
    // in cases such as this where we don't understand what ProfileManager is
    // asking for.
    browser_context = GetActiveBrowserContext();
  }
  return browser_context->AsProfile();
}

bool ChromeProfileManagerAlloy::IsValidProfile(const void* profile) {
  if (!profile) {
    return false;
  }
  return !!HoneycombBrowserContext::FromBrowserContext(
      static_cast<const content::BrowserContext*>(profile));
}
