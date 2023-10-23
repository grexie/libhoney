// Copyright 2017 the Honeycomb Authors. Portions copyright
// 2013 The Chromium Authors and 2016 the CEF Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#include "libhoneycomb/browser/extensions/extension_view_host.h"

#include "libhoneycomb/browser/browser_platform_delegate.h"
#include "libhoneycomb/browser/extensions/extension_host_delegate.h"

#include "content/public/browser/web_contents.h"
#include "extensions/browser/process_util.h"
#include "third_party/blink/public/common/input/web_gesture_event.h"

using content::NativeWebKeyboardEvent;
using content::OpenURLParams;
using content::WebContents;
using content::WebContentsObserver;

namespace extensions {

HoneycombExtensionViewHost::HoneycombExtensionViewHost(AlloyBrowserHostImpl* browser,
                                           const Extension* extension,
                                           content::WebContents* host_contents,
                                           const GURL& url,
                                           mojom::ViewType host_type)
    : ExtensionHost(new HoneycombExtensionHostDelegate(browser),
                    extension,
                    host_contents->GetBrowserContext(),
                    host_contents,
                    url,
                    host_type) {
  // Only used for dialogs and popups.
  DCHECK(host_type == mojom::ViewType::kExtensionDialog ||
         host_type == mojom::ViewType::kExtensionPopup);
}

HoneycombExtensionViewHost::~HoneycombExtensionViewHost() {}

void HoneycombExtensionViewHost::OnDidStopFirstLoad() {
  // Nothing to do here, but don't call the base class method.
}

void HoneycombExtensionViewHost::LoadInitialURL() {
  if (process_util::GetPersistentBackgroundPageState(*extension(),
                                                     browser_context()) ==
      process_util::PersistentBackgroundPageState::kNotReady) {
    // Make sure the background page loads before any others.
    host_registry_observation_.Observe(
        ExtensionHostRegistry::Get(browser_context()));
    return;
  }

  ExtensionHost::LoadInitialURL();
}

bool HoneycombExtensionViewHost::IsBackgroundPage() const {
  return false;
}

bool HoneycombExtensionViewHost::ShouldAllowRendererInitiatedCrossProcessNavigation(
    bool is_main_frame_navigation) {
  // Block navigations that cause the main frame to navigate to non-extension
  // content (i.e. to web content).
  return !is_main_frame_navigation;
}

bool HoneycombExtensionViewHost::PreHandleGestureEvent(
    content::WebContents* source,
    const blink::WebGestureEvent& event) {
  // Disable pinch zooming.
  return blink::WebInputEvent::IsPinchGestureEventType(event.GetType());
}

WebContents* HoneycombExtensionViewHost::GetVisibleWebContents() const {
  if (extension_host_type() == mojom::ViewType::kExtensionPopup) {
    return host_contents();
  }
  return nullptr;
}

void HoneycombExtensionViewHost::OnExtensionHostDocumentElementAvailable(
    content::BrowserContext* host_browser_context,
    ExtensionHost* extension_host) {
  DCHECK(extension_host->extension());
  if (host_browser_context != browser_context() ||
      extension_host->extension() != extension() ||
      extension_host->extension_host_type() !=
          mojom::ViewType::kExtensionBackgroundPage) {
    return;
  }

  DCHECK_EQ(process_util::PersistentBackgroundPageState::kReady,
            process_util::GetPersistentBackgroundPageState(*extension(),
                                                           browser_context()));
  // We only needed to wait for the background page to load, so stop observing.
  host_registry_observation_.Reset();
  LoadInitialURL();
}

}  // namespace extensions
