// Copyright (c) 2014 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoney/browser/navigation_entry_impl.h"

#include "libhoney/browser/ssl_status_impl.h"
#include "libhoney/common/time_util.h"

#include "content/public/browser/navigation_entry.h"
#include "url/gurl.h"

HoneycombNavigationEntryImpl::HoneycombNavigationEntryImpl(content::NavigationEntry* value)
    : HoneycombValueBase<HoneycombNavigationEntry, content::NavigationEntry>(
          value,
          nullptr,
          kOwnerNoDelete,
          false,
          new HoneycombValueControllerNonThreadSafe()) {
  // Indicate that this object owns the controller.
  SetOwnsController();
}

bool HoneycombNavigationEntryImpl::IsValid() {
  return !detached();
}

HoneycombString HoneycombNavigationEntryImpl::GetURL() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());
  return mutable_value()->GetURL().spec();
}

HoneycombString HoneycombNavigationEntryImpl::GetDisplayURL() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());
  return mutable_value()->GetVirtualURL().spec();
}

HoneycombString HoneycombNavigationEntryImpl::GetOriginalURL() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());
  return mutable_value()->GetUserTypedURL().spec();
}

HoneycombString HoneycombNavigationEntryImpl::GetTitle() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());
  return mutable_value()->GetTitle();
}

HoneycombNavigationEntry::TransitionType HoneycombNavigationEntryImpl::GetTransitionType() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, TT_EXPLICIT);
  return static_cast<TransitionType>(mutable_value()->GetTransitionType());
}

bool HoneycombNavigationEntryImpl::HasPostData() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return mutable_value()->GetHasPostData();
}

HoneycombBaseTime HoneycombNavigationEntryImpl::GetCompletionTime() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombBaseTime());
  return mutable_value()->GetTimestamp();
}

int HoneycombNavigationEntryImpl::GetHttpStatusCode() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, 0);
  return mutable_value()->GetHttpStatusCode();
}

HoneycombRefPtr<HoneycombSSLStatus> HoneycombNavigationEntryImpl::GetSSLStatus() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, nullptr);
  return new HoneycombSSLStatusImpl(mutable_value()->GetSSL());
}
