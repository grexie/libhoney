// Copyright 2015 The Honeycomb Authors.
// Portions copyright 2014 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoney/browser/chrome/extensions/chrome_mime_handler_view_guest_delegate_honey.h"

#include "libhoney/browser/browser_host_base.h"
#include "libhoney/browser/browser_info.h"

#include "extensions/browser/guest_view/mime_handler_view/mime_handler_view_guest.h"

namespace extensions {

ChromeMimeHandlerViewGuestDelegateHoneycomb::ChromeMimeHandlerViewGuestDelegateHoneycomb(
    MimeHandlerViewGuest* guest)
    : guest_(guest), owner_web_contents_(guest_->owner_web_contents()) {}

ChromeMimeHandlerViewGuestDelegateHoneycomb::
    ~ChromeMimeHandlerViewGuestDelegateHoneycomb() = default;

void ChromeMimeHandlerViewGuestDelegateHoneycomb::OnGuestAttached() {
  content::WebContents* web_contents = guest_->web_contents();
  DCHECK(web_contents);

  auto owner_browser =
      HoneycombBrowserHostBase::GetBrowserForContents(owner_web_contents_);
  DCHECK(owner_browser);

  // Associate guest state information with the owner browser.
  owner_browser->browser_info()->MaybeCreateFrame(
      web_contents->GetPrimaryMainFrame(), true /* is_guest_view */);
}

void ChromeMimeHandlerViewGuestDelegateHoneycomb::OnGuestDetached() {
  content::WebContents* web_contents = guest_->web_contents();
  DCHECK(web_contents);

  auto owner_browser =
      HoneycombBrowserHostBase::GetBrowserForContents(owner_web_contents_);
  DCHECK(owner_browser);

  // Disassociate guest state information with the owner browser.
  owner_browser->browser_info()->RemoveFrame(
      web_contents->GetPrimaryMainFrame());
}

}  // namespace extensions
