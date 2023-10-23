// Copyright 2021 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_ALLOY_ALLOY_DOWNLOAD_UTIL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_ALLOY_ALLOY_DOWNLOAD_UTIL_H_
#pragma once

class DownloadPrefs;

namespace content {
class BrowserContext;
}  // namespace content

namespace alloy {

// Called from DownloadPrefs::FromBrowserContext.
DownloadPrefs* GetDownloadPrefsFromBrowserContext(
    content::BrowserContext* context);

}  // namespace alloy

#endif  // HONEYCOMB_LIBHONEY_BROWSER_ALLOY_ALLOY_DOWNLOAD_UTIL_H_
