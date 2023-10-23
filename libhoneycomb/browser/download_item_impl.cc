// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoneycomb/browser/download_item_impl.h"

#include "libhoneycomb/common/time_util.h"

#include "components/download/public/common/download_item.h"
#include "url/gurl.h"

HoneycombDownloadItemImpl::HoneycombDownloadItemImpl(download::DownloadItem* value)
    : HoneycombValueBase<HoneycombDownloadItem, download::DownloadItem>(
          value,
          nullptr,
          kOwnerNoDelete,
          true,
          new HoneycombValueControllerNonThreadSafe()) {
  // Indicate that this object owns the controller.
  SetOwnsController();
}

bool HoneycombDownloadItemImpl::IsValid() {
  return !detached();
}

bool HoneycombDownloadItemImpl::IsInProgress() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return const_value().GetState() == download::DownloadItem::IN_PROGRESS;
}

bool HoneycombDownloadItemImpl::IsComplete() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return const_value().GetState() == download::DownloadItem::COMPLETE;
}

bool HoneycombDownloadItemImpl::IsCanceled() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return const_value().GetState() == download::DownloadItem::CANCELLED;
}

bool HoneycombDownloadItemImpl::IsInterrupted() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return const_value().GetState() == download::DownloadItem::INTERRUPTED;
}

honey_download_interrupt_reason_t HoneycombDownloadItemImpl::GetInterruptReason() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HONEYCOMB_DOWNLOAD_INTERRUPT_REASON_NONE);
  return static_cast<honey_download_interrupt_reason_t>(
      const_value().GetLastReason());
}

int64_t HoneycombDownloadItemImpl::GetCurrentSpeed() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, 0);
  return const_value().CurrentSpeed();
}

int HoneycombDownloadItemImpl::GetPercentComplete() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, -1);
  return const_value().PercentComplete();
}

int64_t HoneycombDownloadItemImpl::GetTotalBytes() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, 0);
  return const_value().GetTotalBytes();
}

int64_t HoneycombDownloadItemImpl::GetReceivedBytes() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, 0);
  return const_value().GetReceivedBytes();
}

HoneycombBaseTime HoneycombDownloadItemImpl::GetStartTime() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombBaseTime());
  return const_value().GetStartTime();
}

HoneycombBaseTime HoneycombDownloadItemImpl::GetEndTime() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombBaseTime());
  return const_value().GetEndTime();
}

HoneycombString HoneycombDownloadItemImpl::GetFullPath() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());
  return const_value().GetFullPath().value();
}

uint32_t HoneycombDownloadItemImpl::GetId() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, 0);
  return const_value().GetId();
}

HoneycombString HoneycombDownloadItemImpl::GetURL() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());
  return const_value().GetURL().spec();
}

HoneycombString HoneycombDownloadItemImpl::GetOriginalUrl() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());
  return const_value().GetOriginalUrl().spec();
}

HoneycombString HoneycombDownloadItemImpl::GetSuggestedFileName() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());
  return const_value().GetSuggestedFilename();
}

HoneycombString HoneycombDownloadItemImpl::GetContentDisposition() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());
  return const_value().GetContentDisposition();
}

HoneycombString HoneycombDownloadItemImpl::GetMimeType() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());
  return const_value().GetMimeType();
}
