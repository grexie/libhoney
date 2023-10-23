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
// $hash=486d31ad20241c84c224f0b8c90fe0c0f915ad18$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CTOCPP_DOWNLOAD_HANDLER_CTOCPP_H_
#define HONEYCOMB_LIBHONEY_DLL_CTOCPP_DOWNLOAD_HANDLER_CTOCPP_H_
#pragma once

#if !defined(BUILDING_HONEYCOMB_SHARED)
#error This file can be included DLL-side only
#endif

#include "include/capi/honey_download_handler_capi.h"
#include "include/honey_download_handler.h"
#include "libhoney_dll/ctocpp/ctocpp_ref_counted.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed DLL-side only.
class HoneycombDownloadHandlerCToCpp
    : public HoneycombCToCppRefCounted<HoneycombDownloadHandlerCToCpp,
                                 HoneycombDownloadHandler,
                                 honey_download_handler_t> {
 public:
  HoneycombDownloadHandlerCToCpp();
  virtual ~HoneycombDownloadHandlerCToCpp();

  // HoneycombDownloadHandler methods.
  bool CanDownload(HoneycombRefPtr<HoneycombBrowser> browser,
                   const HoneycombString& url,
                   const HoneycombString& request_method) override;
  void OnBeforeDownload(HoneycombRefPtr<HoneycombBrowser> browser,
                        HoneycombRefPtr<HoneycombDownloadItem> download_item,
                        const HoneycombString& suggested_name,
                        HoneycombRefPtr<HoneycombBeforeDownloadCallback> callback) override;
  void OnDownloadUpdated(HoneycombRefPtr<HoneycombBrowser> browser,
                         HoneycombRefPtr<HoneycombDownloadItem> download_item,
                         HoneycombRefPtr<HoneycombDownloadItemCallback> callback) override;
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CTOCPP_DOWNLOAD_HANDLER_CTOCPP_H_