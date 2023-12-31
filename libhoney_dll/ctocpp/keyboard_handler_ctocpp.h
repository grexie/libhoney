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
// $hash=a25080ecb1a098b748d8384bc5af591ea773deff$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CTOCPP_KEYBOARD_HANDLER_CTOCPP_H_
#define HONEYCOMB_LIBHONEY_DLL_CTOCPP_KEYBOARD_HANDLER_CTOCPP_H_
#pragma once

#if !defined(BUILDING_HONEYCOMB_SHARED)
#error This file can be included DLL-side only
#endif

#include "include/capi/honey_keyboard_handler_capi.h"
#include "include/honey_keyboard_handler.h"
#include "libhoney_dll/ctocpp/ctocpp_ref_counted.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed DLL-side only.
class HoneycombKeyboardHandlerCToCpp
    : public HoneycombCToCppRefCounted<HoneycombKeyboardHandlerCToCpp,
                                 HoneycombKeyboardHandler,
                                 honey_keyboard_handler_t> {
 public:
  HoneycombKeyboardHandlerCToCpp();
  virtual ~HoneycombKeyboardHandlerCToCpp();

  // HoneycombKeyboardHandler methods.
  bool OnPreKeyEvent(HoneycombRefPtr<HoneycombBrowser> browser,
                     const HoneycombKeyEvent& event,
                     HoneycombEventHandle os_event,
                     bool* is_keyboard_shortcut) override;
  bool OnKeyEvent(HoneycombRefPtr<HoneycombBrowser> browser,
                  const HoneycombKeyEvent& event,
                  HoneycombEventHandle os_event) override;
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CTOCPP_KEYBOARD_HANDLER_CTOCPP_H_
