// Copyright (c) 2016 The Honeycomb Authors.
// Portions copyright (c) 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file provides a stub implementation of Chrome's ProfileManager object
// for use as an interop layer between Honeycomb and files that live in chrome/.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_ALLOY_CHROME_PROFILE_MANAGER_ALLOY_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_ALLOY_CHROME_PROFILE_MANAGER_ALLOY_H_

#include "chrome/browser/profiles/profile_manager.h"

class ChromeProfileManagerAlloy : public ProfileManager {
 public:
  ChromeProfileManagerAlloy();

  ChromeProfileManagerAlloy(const ChromeProfileManagerAlloy&) = delete;
  ChromeProfileManagerAlloy& operator=(const ChromeProfileManagerAlloy&) =
      delete;

  ~ChromeProfileManagerAlloy() override;

  Profile* GetProfile(const base::FilePath& profile_dir) override;
  bool IsValidProfile(const void* profile) override;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_ALLOY_CHROME_PROFILE_MANAGER_ALLOY_H_
