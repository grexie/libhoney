// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_FRAME_HOST_IMPL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_FRAME_HOST_IMPL_H_
#pragma once

#include <memory>
#include <queue>
#include <string>

#include "include/honey_frame.h"

#include "base/synchronization/lock.h"
#include "honey/libhoney/common/mojom/honey.mojom.h"
#include "mojo/public/cpp/bindings/receiver_set.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "ui/base/page_transition_types.h"

namespace content {
class RenderFrameHost;
struct Referrer;
}  // namespace content

class GURL;

class HoneycombBrowserInfo;
class HoneycombBrowserHostBase;

// Implementation of HoneycombFrame. HoneycombFrameHostImpl objects should always be created
// or retrieved via HoneycombBrowerInfo.
class HoneycombFrameHostImpl : public HoneycombFrame, public honey::mojom::BrowserFrame {
 public:
  // Create a temporary sub-frame.
  HoneycombFrameHostImpl(scoped_refptr<HoneycombBrowserInfo> browser_info,
                   int64_t parent_frame_id);

  // Create a frame backed by a RFH and owned by HoneycombBrowserInfo.
  HoneycombFrameHostImpl(scoped_refptr<HoneycombBrowserInfo> browser_info,
                   content::RenderFrameHost* render_frame_host);

  HoneycombFrameHostImpl(const HoneycombFrameHostImpl&) = delete;
  HoneycombFrameHostImpl& operator=(const HoneycombFrameHostImpl&) = delete;

  ~HoneycombFrameHostImpl() override;

  // HoneycombFrame methods
  bool IsValid() override;
  void Undo() override;
  void Redo() override;
  void Cut() override;
  void Copy() override;
  void Paste() override;
  void Delete() override;
  void SelectAll() override;
  void ViewSource() override;
  void GetSource(HoneycombRefPtr<HoneycombStringVisitor> visitor) override;
  void GetText(HoneycombRefPtr<HoneycombStringVisitor> visitor) override;
  void LoadRequest(HoneycombRefPtr<HoneycombRequest> request) override;
  void LoadURL(const HoneycombString& url) override;
  void ExecuteJavaScript(const HoneycombString& jsCode,
                         const HoneycombString& scriptUrl,
                         int startLine) override;
  bool IsMain() override;
  bool IsFocused() override;
  HoneycombString GetName() override;
  int64_t GetIdentifier() override;
  HoneycombRefPtr<HoneycombFrame> GetParent() override;
  HoneycombString GetURL() override;
  HoneycombRefPtr<HoneycombBrowser> GetBrowser() override;
  HoneycombRefPtr<HoneycombV8Context> GetV8Context() override;
  void VisitDOM(HoneycombRefPtr<HoneycombDOMVisitor> visitor) override;
  HoneycombRefPtr<HoneycombURLRequest> CreateURLRequest(
      HoneycombRefPtr<HoneycombRequest> request,
      HoneycombRefPtr<HoneycombURLRequestClient> client) override;
  void SendProcessMessage(HoneycombProcessId target_process,
                          HoneycombRefPtr<HoneycombProcessMessage> message) override;

  bool is_temporary() const { return frame_id_ == kInvalidFrameId; }

  void SetFocused(bool focused);
  void RefreshAttributes();

  // Notification that a move or resize of the renderer's containing window has
  // started. Used on Windows and Linux with the Alloy runtime.
  void NotifyMoveOrResizeStarted();

  // Load the specified request.
  void LoadRequest(honey::mojom::RequestParamsPtr params);

  // Load the specified URL.
  void LoadURLWithExtras(const std::string& url,
                         const content::Referrer& referrer,
                         ui::PageTransition transition,
                         const std::string& extra_headers);

  // Send a command to the renderer for execution.
  void SendCommand(const std::string& command);
  void SendCommandWithResponse(
      const std::string& command,
      honey::mojom::RenderFrame::SendCommandWithResponseCallback
          response_callback);

  // Send JavaScript to the renderer for execution.
  void SendJavaScript(const std::u16string& jsCode,
                      const std::string& scriptUrl,
                      int startLine);

  // Called from HoneycombBrowserHostBase::DidStopLoading.
  void MaybeSendDidStopLoading();

  void ExecuteJavaScriptWithUserGestureForTests(const HoneycombString& javascript);

  // Returns the RFH associated with this frame. Must be called on the UI
  // thread.
  content::RenderFrameHost* GetRenderFrameHost() const;

  enum class DetachReason {
    RENDER_FRAME_DELETED,
    NEW_MAIN_FRAME,
    BROWSER_DESTROYED,
  };

  // Owned frame objects will be detached explicitly when the associated
  // RenderFrame is deleted. Temporary frame objects will be detached
  // implicitly via HoneycombBrowserInfo::browser() returning nullptr. Returns true
  // if this was the first call to Detach() for the frame.
  bool Detach(DetachReason reason);

  // A frame has swapped to active status from prerendering or the back-forward
  // cache. We may need to re-attach if the RFH has changed. See
  // https://crbug.com/1179502#c8 for additional background.
  void MaybeReAttach(scoped_refptr<HoneycombBrowserInfo> browser_info,
                     content::RenderFrameHost* render_frame_host);

  // honey::mojom::BrowserFrame methods forwarded from HoneycombBrowserFrame.
  void SendMessage(const std::string& name,
                   base::Value::List arguments) override;
  void SendSharedMemoryRegion(const std::string& name,
                              base::WritableSharedMemoryRegion region) override;
  void FrameAttached(mojo::PendingRemote<honey::mojom::RenderFrame> render_frame,
                     bool reattached) override;
  void UpdateDraggableRegions(
      absl::optional<std::vector<honey::mojom::DraggableRegionEntryPtr>> regions)
      override;

  static const int64_t kMainFrameId;
  static const int64_t kFocusedFrameId;
  static const int64_t kUnspecifiedFrameId;
  static const int64_t kInvalidFrameId;

  // PageTransition type for explicit navigations. This must pass the check in
  // ContentBrowserClient::IsExplicitNavigation for debug URLs (HandleDebugURL)
  // to work as expected.
  static const ui::PageTransition kPageTransitionExplicit;

 private:
  int64_t GetFrameId() const;
  scoped_refptr<HoneycombBrowserInfo> GetBrowserInfo() const;
  HoneycombRefPtr<HoneycombBrowserHostBase> GetBrowserHostBase() const;

  // Send an action to the remote RenderFrame. This will queue the action if the
  // remote frame is not yet attached.
  using RenderFrameType = mojo::Remote<honey::mojom::RenderFrame>;
  using RenderFrameAction = base::OnceCallback<void(const RenderFrameType&)>;
  void SendToRenderFrame(const std::string& function_name,
                         RenderFrameAction action);

  void OnRenderFrameDisconnect();

  std::string GetDebugString() const;

  const bool is_main_frame_;

  // The following members may be read/modified from any thread. All access must
  // be protected by |state_lock_|.
  mutable base::Lock state_lock_;
  int64_t frame_id_;
  scoped_refptr<HoneycombBrowserInfo> browser_info_;
  bool is_focused_;
  HoneycombString url_;
  HoneycombString name_;
  int64_t parent_frame_id_;

  // The following members are only accessed on the UI thread.
  content::RenderFrameHost* render_frame_host_ = nullptr;

  std::queue<std::pair<std::string, RenderFrameAction>>
      queued_renderer_actions_;

  mojo::Remote<honey::mojom::RenderFrame> render_frame_;

  IMPLEMENT_REFCOUNTING(HoneycombFrameHostImpl);
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_FRAME_HOST_IMPL_H_
