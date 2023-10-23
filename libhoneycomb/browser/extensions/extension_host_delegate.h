// Copyright 2017 the Honeycomb Authors. Portions copyright
// 2014 The Chromium Authors and 2016 the CEF Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#ifndef libhoneycombCOMB_BROWSER_EXTENSIONS_EXTENSION_HOST_DELEGATE_H_
#define libhoneycombCOMB_BROWSER_EXTENSIONS_EXTENSION_HOST_DELEGATE_H_

#include "extensions/browser/extension_host_delegate.h"

class AlloyBrowserHostImpl;

namespace extensions {

class HoneycombExtensionHostDelegate : public ExtensionHostDelegate {
 public:
  explicit HoneycombExtensionHostDelegate(AlloyBrowserHostImpl* browser);

  HoneycombExtensionHostDelegate(const HoneycombExtensionHostDelegate&) = delete;
  HoneycombExtensionHostDelegate& operator=(const HoneycombExtensionHostDelegate&) = delete;

  ~HoneycombExtensionHostDelegate() override;

  // ExtensionHostDelegate implementation.
  void OnExtensionHostCreated(content::WebContents* web_contents) override;
  void OnMainFrameCreatedForBackgroundPage(ExtensionHost* host) override;
  content::JavaScriptDialogManager* GetJavaScriptDialogManager() override;
  void CreateTab(std::unique_ptr<content::WebContents> web_contents,
                 const std::string& extension_id,
                 WindowOpenDisposition disposition,
                 const blink::mojom::WindowFeatures& window_features,
                 bool user_gesture) override;
  void ProcessMediaAccessRequest(content::WebContents* web_contents,
                                 const content::MediaStreamRequest& request,
                                 content::MediaResponseCallback callback,
                                 const Extension* extension) override;
  bool CheckMediaAccessPermission(content::RenderFrameHost* render_frame_host,
                                  const GURL& security_origin,
                                  blink::mojom::MediaStreamType type,
                                  const Extension* extension) override;
  content::PictureInPictureResult EnterPictureInPicture(
      content::WebContents* web_contents) override;
  void ExitPictureInPicture() override;
};

}  // namespace extensions

#endif  // libhoneycombCOMB_BROWSER_EXTENSIONS_EXTENSION_HOST_DELEGATE_H_
