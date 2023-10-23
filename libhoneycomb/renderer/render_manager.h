// Copyright 2015 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_RENDERER_RENDER_MANAGER_H_
#define HONEYCOMB_LIBHONEYCOMB_RENDERER_RENDER_MANAGER_H_
#pragma once

#include <map>
#include <memory>

#include "include/internal/honey_ptr.h"

#include "honey/libhoneycomb/common/mojom/honey.mojom.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/receiver_set.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace blink {
class WebFrame;
class WebView;
}  // namespace blink

namespace content {
class RenderFrame;
}  // namespace content

namespace mojo {
class BinderMap;
}  // namespace mojo

class HoneycombBrowserImpl;
class HoneycombGuestView;
class HoneycombRenderFrameObserver;

// Singleton object for managing BrowserImpl instances. Only accessed on the
// main renderer thread.
class HoneycombRenderManager : public honey::mojom::RenderManager {
 public:
  HoneycombRenderManager();

  HoneycombRenderManager(const HoneycombRenderManager&) = delete;
  HoneycombRenderManager& operator=(const HoneycombRenderManager&) = delete;

  ~HoneycombRenderManager();

  // Returns this singleton instance of this class.
  static HoneycombRenderManager* Get();

  // Called from ContentRendererClient methods of the same name.
  void RenderThreadConnected();
  void RenderFrameCreated(content::RenderFrame* render_frame,
                          HoneycombRenderFrameObserver* render_frame_observer,
                          bool& browser_created,
                          absl::optional<bool>& is_windowless);
  void WebViewCreated(blink::WebView* web_view,
                      bool& browser_created,
                      absl::optional<bool>& is_windowless);
  void DevToolsAgentAttached();
  void DevToolsAgentDetached();
  void ExposeInterfacesToBrowser(mojo::BinderMap* binders);

  // Returns the browser associated with the specified RenderView.
  HoneycombRefPtr<HoneycombBrowserImpl> GetBrowserForView(blink::WebView* view);

  // Returns the browser associated with the specified main WebFrame.
  HoneycombRefPtr<HoneycombBrowserImpl> GetBrowserForMainFrame(blink::WebFrame* frame);

  // Connects to HoneycombBrowserManager in the browser process.
  mojo::Remote<honey::mojom::BrowserManager>& GetBrowserManager();

  // Returns true if this renderer process is hosting an extension.
  static bool IsExtensionProcess();

  // Returns true if this renderer process is hosting a PDF.
  static bool IsPdfProcess();

 private:
  friend class HoneycombBrowserImpl;
  friend class HoneycombGuestView;

  // Binds receivers for the RenderManager interface.
  void BindReceiver(mojo::PendingReceiver<honey::mojom::RenderManager> receiver);

  // honey::mojom::RenderManager methods:
  void ModifyCrossOriginWhitelistEntry(
      bool add,
      honey::mojom::CrossOriginWhiteListEntryPtr entry) override;
  void ClearCrossOriginWhitelist() override;

  void WebKitInitialized();

  // Maybe create a new browser object, return the existing one, or return
  // nullptr for guest views.
  HoneycombRefPtr<HoneycombBrowserImpl> MaybeCreateBrowser(
      blink::WebView* web_view,
      content::RenderFrame* render_frame,
      bool* browser_created,
      absl::optional<bool>* is_windowless);

  // Called from HoneycombBrowserImpl::OnDestruct().
  void OnBrowserDestroyed(HoneycombBrowserImpl* browser);

  // Returns the guest view associated with the specified RenderView if any.
  HoneycombGuestView* GetGuestViewForView(blink::WebView* view);

  // Called from HoneycombGuestView::OnDestruct().
  void OnGuestViewDestroyed(HoneycombGuestView* guest_view);

  // Map of RenderView pointers to HoneycombBrowserImpl references.
  using BrowserMap = std::map<blink::WebView*, HoneycombRefPtr<HoneycombBrowserImpl>>;
  BrowserMap browsers_;

  // Map of RenderView poiners to HoneycombGuestView implementations.
  using GuestViewMap = std::map<blink::WebView*, std::unique_ptr<HoneycombGuestView>>;
  GuestViewMap guest_views_;

  // Cross-origin white list entries that need to be registered with WebKit.
  std::vector<honey::mojom::CrossOriginWhiteListEntryPtr>
      cross_origin_whitelist_entries_;

  int devtools_agent_count_ = 0;
  int uncaught_exception_stack_size_ = 0;

  mojo::ReceiverSet<honey::mojom::RenderManager> receivers_;

  mojo::Remote<honey::mojom::BrowserManager> browser_manager_;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_RENDERER_RENDER_MANAGER_H_
