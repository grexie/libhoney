// Copyright 2015 The Honeycomb Authors.
// Portions copyright 2014 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_CHROME_EXTENSIONS_CHROME_MIME_HANDLER_VIEW_GUEST_DELEGATE_HONEYCOMB_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_CHROME_EXTENSIONS_CHROME_MIME_HANDLER_VIEW_GUEST_DELEGATE_HONEYCOMB_H_

#include "chrome/browser/guest_view/mime_handler_view/chrome_mime_handler_view_guest_delegate.h"
#include "extensions/browser/guest_view/mime_handler_view/mime_handler_view_guest.h"

namespace extensions {

class ChromeMimeHandlerViewGuestDelegateHoneycomb
    : public ChromeMimeHandlerViewGuestDelegate {
 public:
  explicit ChromeMimeHandlerViewGuestDelegateHoneycomb(MimeHandlerViewGuest* guest);

  ChromeMimeHandlerViewGuestDelegateHoneycomb(
      const ChromeMimeHandlerViewGuestDelegateHoneycomb&) = delete;
  ChromeMimeHandlerViewGuestDelegateHoneycomb& operator=(
      const ChromeMimeHandlerViewGuestDelegateHoneycomb&) = delete;

  ~ChromeMimeHandlerViewGuestDelegateHoneycomb() override;

  // MimeHandlerViewGuestDelegate methods.
  void OnGuestAttached() override;
  void OnGuestDetached() override;

 private:
  MimeHandlerViewGuest* guest_;  // Owns us.
  content::WebContents* owner_web_contents_;
};

}  // namespace extensions

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_CHROME_EXTENSIONS_CHROME_MIME_HANDLER_VIEW_GUEST_DELEGATE_HONEYCOMB_H_
