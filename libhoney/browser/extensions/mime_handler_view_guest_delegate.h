// Copyright 2015 The Honeycomb Authors.
// Portions copyright 2014 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_MIME_HANDLER_VIEW_GUEST_DELEGATE_H_
#define HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_MIME_HANDLER_VIEW_GUEST_DELEGATE_H_

#include "extensions/browser/guest_view/mime_handler_view/mime_handler_view_guest.h"
#include "extensions/browser/guest_view/mime_handler_view/mime_handler_view_guest_delegate.h"

namespace content {
struct ContextMenuParams;
}

namespace extensions {

class HoneycombMimeHandlerViewGuestDelegate : public MimeHandlerViewGuestDelegate {
 public:
  explicit HoneycombMimeHandlerViewGuestDelegate(MimeHandlerViewGuest* guest);

  HoneycombMimeHandlerViewGuestDelegate(const HoneycombMimeHandlerViewGuestDelegate&) =
      delete;
  HoneycombMimeHandlerViewGuestDelegate& operator=(
      const HoneycombMimeHandlerViewGuestDelegate&) = delete;

  ~HoneycombMimeHandlerViewGuestDelegate() override;

  // MimeHandlerViewGuestDelegate methods.
  void OverrideWebContentsCreateParams(
      content::WebContents::CreateParams* params) override;
  void OnGuestAttached() override;
  void OnGuestDetached() override;
  bool HandleContextMenu(content::RenderFrameHost& render_frame_host,
                         const content::ContextMenuParams& params) override;

 private:
  MimeHandlerViewGuest* guest_;  // Owns us.
  content::WebContents* owner_web_contents_;
};

}  // namespace extensions

#endif  // HONEYCOMB_LIBHONEY_BROWSER_EXTENSIONS_MIME_HANDLER_VIEW_GUEST_DELEGATE_H_
