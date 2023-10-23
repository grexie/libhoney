// Copyright 2017 the Honeycomb Authors. Portions copyright
// 2014 The Chromium Authors and 2016 the CEF Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#include "libhoneycomb/browser/extensions/extension_host_delegate.h"

#include "libhoneycomb/browser/extensions/extensions_browser_client.h"

#include "base/logging.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_delegate.h"

namespace extensions {

HoneycombExtensionHostDelegate::HoneycombExtensionHostDelegate(
    AlloyBrowserHostImpl* browser) {}

HoneycombExtensionHostDelegate::~HoneycombExtensionHostDelegate() {}

void HoneycombExtensionHostDelegate::OnExtensionHostCreated(
    content::WebContents* web_contents) {}

void HoneycombExtensionHostDelegate::OnMainFrameCreatedForBackgroundPage(
    ExtensionHost* host) {}

content::JavaScriptDialogManager*
HoneycombExtensionHostDelegate::GetJavaScriptDialogManager() {
  // Never routed here from AlloyBrowserHostImpl.
  DCHECK(false);
  return nullptr;
}

void HoneycombExtensionHostDelegate::CreateTab(
    std::unique_ptr<content::WebContents> web_contents,
    const std::string& extension_id,
    WindowOpenDisposition disposition,
    const blink::mojom::WindowFeatures& window_features,
    bool user_gesture) {
  // TODO(honey): Add support for extensions opening popup windows.
  NOTIMPLEMENTED();
}

void HoneycombExtensionHostDelegate::ProcessMediaAccessRequest(
    content::WebContents* web_contents,
    const content::MediaStreamRequest& request,
    content::MediaResponseCallback callback,
    const Extension* extension) {
  // Never routed here from AlloyBrowserHostImpl.
  DCHECK(false);
}

bool HoneycombExtensionHostDelegate::CheckMediaAccessPermission(
    content::RenderFrameHost* render_frame_host,
    const GURL& security_origin,
    blink::mojom::MediaStreamType type,
    const Extension* extension) {
  // Never routed here from AlloyBrowserHostImpl.
  DCHECK(false);
  return false;
}

content::PictureInPictureResult HoneycombExtensionHostDelegate::EnterPictureInPicture(
    content::WebContents* web_contents) {
  DCHECK(false);
  return content::PictureInPictureResult::kNotSupported;
}

void HoneycombExtensionHostDelegate::ExitPictureInPicture() {
  DCHECK(false);
}

}  // namespace extensions
