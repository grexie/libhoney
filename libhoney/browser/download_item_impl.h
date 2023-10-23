// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_DOWNLOAD_ITEM_IMPL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_DOWNLOAD_ITEM_IMPL_H_
#pragma once

#include "include/honey_download_item.h"
#include "libhoney/common/value_base.h"

namespace download {
class DownloadItem;
}

// HoneycombDownloadItem implementation
class HoneycombDownloadItemImpl
    : public HoneycombValueBase<HoneycombDownloadItem, download::DownloadItem> {
 public:
  explicit HoneycombDownloadItemImpl(download::DownloadItem* value);

  HoneycombDownloadItemImpl(const HoneycombDownloadItemImpl&) = delete;
  HoneycombDownloadItemImpl& operator=(const HoneycombDownloadItemImpl&) = delete;

  // HoneycombDownloadItem methods.
  bool IsValid() override;
  bool IsInProgress() override;
  bool IsComplete() override;
  bool IsCanceled() override;
  bool IsInterrupted() override;
  honey_download_interrupt_reason_t GetInterruptReason() override;
  int64_t GetCurrentSpeed() override;
  int GetPercentComplete() override;
  int64_t GetTotalBytes() override;
  int64_t GetReceivedBytes() override;
  HoneycombBaseTime GetStartTime() override;
  HoneycombBaseTime GetEndTime() override;
  HoneycombString GetFullPath() override;
  uint32_t GetId() override;
  HoneycombString GetURL() override;
  HoneycombString GetOriginalUrl() override;
  HoneycombString GetSuggestedFileName() override;
  HoneycombString GetContentDisposition() override;
  HoneycombString GetMimeType() override;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_DOWNLOAD_ITEM_IMPL_H_
