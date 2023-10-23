// Copyright (c) 2023 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the Honeycomb translator tool. If making changes by
// hand only do so within the body of existing method and function
// implementations. See the translator.README.txt file in the tools directory
// for more information.
//
// $hash=09c6bbd82ec82943b6f310c5ffb347bfbef2654c$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CTOCPP_NAVIGATION_ENTRY_CTOCPP_H_
#define HONEYCOMB_LIBHONEY_DLL_CTOCPP_NAVIGATION_ENTRY_CTOCPP_H_
#pragma once

#if !defined(WRAPPING_HONEYCOMB_SHARED)
#error This file can be included wrapper-side only
#endif

#include "include/capi/honey_navigation_entry_capi.h"
#include "include/honey_navigation_entry.h"
#include "libhoney_dll/ctocpp/ctocpp_ref_counted.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class HoneycombNavigationEntryCToCpp
    : public HoneycombCToCppRefCounted<HoneycombNavigationEntryCToCpp,
                                 HoneycombNavigationEntry,
                                 honey_navigation_entry_t> {
 public:
  HoneycombNavigationEntryCToCpp();
  virtual ~HoneycombNavigationEntryCToCpp();

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

#endif  // HONEYCOMB_LIBHONEY_DLL_CTOCPP_NAVIGATION_ENTRY_CTOCPP_H_