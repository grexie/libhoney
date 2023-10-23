// Copyright (c) 2014 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_NAVIGATION_ENTRY_IMPL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_NAVIGATION_ENTRY_IMPL_H_
#pragma once

#include "include/honey_navigation_entry.h"
#include "libhoney/common/value_base.h"

namespace content {
class NavigationEntry;
}

// HoneycombNavigationEntry implementation
class HoneycombNavigationEntryImpl
    : public HoneycombValueBase<HoneycombNavigationEntry, content::NavigationEntry> {
 public:
  explicit HoneycombNavigationEntryImpl(content::NavigationEntry* value);

  HoneycombNavigationEntryImpl(const HoneycombNavigationEntryImpl&) = delete;
  HoneycombNavigationEntryImpl& operator=(const HoneycombNavigationEntryImpl&) = delete;

  // HoneycombNavigationEntry methods.
  bool IsValid() override;
  HoneycombString GetURL() override;
  HoneycombString GetDisplayURL() override;
  HoneycombString GetOriginalURL() override;
  HoneycombString GetTitle() override;
  TransitionType GetTransitionType() override;
  bool HasPostData() override;
  HoneycombBaseTime GetCompletionTime() override;
  int GetHttpStatusCode() override;
  HoneycombRefPtr<HoneycombSSLStatus> GetSSLStatus() override;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_NAVIGATION_ENTRY_IMPL_H_
