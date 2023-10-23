// Copyright (c) 2022 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_PRINTING_PRINT_UTIL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_PRINTING_PRINT_UTIL_H_
#pragma once

#include "include/honey_browser.h"

namespace content {
class WebContents;
}

namespace print_util {

// Called from HoneycombBrowserHostBase::Print.
void Print(content::WebContents* web_contents, bool print_preview_disabled);

// Called from HoneycombBrowserHostBase::PrintToPDF.
void PrintToPDF(content::WebContents* web_contents,
                const HoneycombString& path,
                const HoneycombPdfPrintSettings& settings,
                HoneycombRefPtr<HoneycombPdfPrintCallback> callback);

}  // namespace print_util

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_PRINTING_PRINT_UTIL_H_
