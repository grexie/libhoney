// Copyright 2015 The Honeycomb Authors.
// Portions copyright 2014 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoney/browser/extensions/mime_handler_view_guest_delegate.h"

#include "libhoney/browser/alloy/alloy_browser_host_impl.h"
#include "libhoney/browser/alloy/alloy_content_browser_client.h"
#include "libhoney/browser/browser_context.h"
#include "libhoney/browser/browser_info.h"
#include "libhoney/browser/osr/web_contents_view_osr.h"

#include "extensions/browser/guest_view/mime_handler_view/mime_handler_view_guest.h"

namespace extensions {

HoneycombMimeHandlerViewGuestDelegate::HoneycombMimeHandlerViewGuestDelegate(
    MimeHandlerViewGuest* guest)
    : guest_(guest), owner_web_contents_(guest_->owner_web_contents()) {}

HoneycombMimeHandlerViewGuestDelegate::~HoneycombMimeHandlerViewGuestDelegate() {}

void HoneycombMimeHandlerViewGuestDelegate::OverrideWebContentsCreateParams(
    content::WebContents::CreateParams* params) {
  DCHECK(params->guest_delegate);

  HoneycombRefPtr<AlloyBrowserHostImpl> owner_browser =
      AlloyBrowserHostImpl::GetBrowserForContents(owner_web_contents_);
  DCHECK(owner_browser);

  if (owner_browser->IsWindowless()) {
    HoneycombWebContentsViewOSR* view_osr = new HoneycombWebContentsViewOSR(
        owner_browser->GetBackgroundColor(), false, false);
    params->view = view_osr;
    params->delegate_view = view_osr;
  }
}

void HoneycombMimeHandlerViewGuestDelegate::OnGuestAttached() {
  content::WebContents* web_contents = guest_->web_contents();
  DCHECK(web_contents);

  HoneycombRefPtr<AlloyBrowserHostImpl> owner_browser =
      AlloyBrowserHostImpl::GetBrowserForContents(owner_web_contents_);
  DCHECK(owner_browser);

  // Associate guest state information with the owner browser.
  owner_browser->browser_info()->MaybeCreateFrame(
      web_contents->GetPrimaryMainFrame(), true /* is_guest_view */);
}

void HoneycombMimeHandlerViewGuestDelegate::OnGuestDetached() {
  content::WebContents* web_contents = guest_->web_contents();
  DCHECK(web_contents);

  HoneycombRefPtr<AlloyBrowserHostImpl> owner_browser =
      AlloyBrowserHostImpl::GetBrowserForContents(owner_web_contents_);
  DCHECK(owner_browser);

  // Disassociate guest state information with the owner browser.
  owner_browser->browser_info()->RemoveFrame(
      web_contents->GetPrimaryMainFrame());
}

bool HoneycombMimeHandlerViewGuestDelegate::HandleContextMenu(
    content::RenderFrameHost& render_frame_host,
    const content::ContextMenuParams& params) {
  HoneycombRefPtr<AlloyBrowserHostImpl> owner_browser =
      AlloyBrowserHostImpl::GetBrowserForContents(owner_web_contents_);
  DCHECK(owner_browser);

  return owner_browser->ShowContextMenu(params);
}

}  // namespace extensions
