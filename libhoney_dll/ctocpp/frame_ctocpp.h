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
// $hash=6215ffe1d66a8b1fd9c9acbebfc792e5acc68490$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CTOCPP_FRAME_CTOCPP_H_
#define HONEYCOMB_LIBHONEY_DLL_CTOCPP_FRAME_CTOCPP_H_
#pragma once

#if !defined(WRAPPING_HONEYCOMB_SHARED)
#error This file can be included wrapper-side only
#endif

#include "include/capi/honey_browser_capi.h"
#include "include/capi/honey_frame_capi.h"
#include "include/capi/honey_urlrequest_capi.h"
#include "include/capi/honey_v8_capi.h"
#include "include/honey_browser.h"
#include "include/honey_frame.h"
#include "include/honey_urlrequest.h"
#include "include/honey_v8.h"
#include "libhoney_dll/ctocpp/ctocpp_ref_counted.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class HoneycombFrameCToCpp
    : public HoneycombCToCppRefCounted<HoneycombFrameCToCpp, HoneycombFrame, honey_frame_t> {
 public:
  HoneycombFrameCToCpp();
  virtual ~HoneycombFrameCToCpp();

  // HoneycombFrame methods.
  bool IsValid() override;
  void Undo() override;
  void Redo() override;
  void Cut() override;
  void Copy() override;
  void Paste() override;
  void Delete() override;
  void SelectAll() override;
  void ViewSource() override;
  void GetSource(HoneycombRefPtr<HoneycombStringVisitor> visitor) override;
  void GetText(HoneycombRefPtr<HoneycombStringVisitor> visitor) override;
  void LoadRequest(HoneycombRefPtr<HoneycombRequest> request) override;
  void LoadURL(const HoneycombString& url) override;
  void ExecuteJavaScript(const HoneycombString& code,
                         const HoneycombString& script_url,
                         int start_line) override;
  bool IsMain() override;
  bool IsFocused() override;
  HoneycombString GetName() override;
  int64_t GetIdentifier() override;
  HoneycombRefPtr<HoneycombFrame> GetParent() override;
  HoneycombString GetURL() override;
  HoneycombRefPtr<HoneycombBrowser> GetBrowser() override;
  HoneycombRefPtr<HoneycombV8Context> GetV8Context() override;
  void VisitDOM(HoneycombRefPtr<HoneycombDOMVisitor> visitor) override;
  HoneycombRefPtr<HoneycombURLRequest> CreateURLRequest(
      HoneycombRefPtr<HoneycombRequest> request,
      HoneycombRefPtr<HoneycombURLRequestClient> client) override;
  void SendProcessMessage(HoneycombProcessId target_process,
                          HoneycombRefPtr<HoneycombProcessMessage> message) override;
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CTOCPP_FRAME_CTOCPP_H_
