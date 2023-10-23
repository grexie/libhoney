// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_RENDERER_FRAME_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_RENDERER_FRAME_IMPL_H_
#pragma once

#include <queue>
#include <string>

#include "include/honey_frame.h"
#include "include/honey_v8.h"
#include "libhoneycomb/renderer/blink_glue.h"

#include "base/memory/weak_ptr.h"
#include "base/timer/timer.h"
#include "honey/libhoneycomb/common/mojom/honey.mojom.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "mojo/public/cpp/bindings/remote.h"

namespace base {
class ListValue;
}

namespace blink {
class ResourceLoadInfoNotifierWrapper;
class WebLocalFrame;
}  // namespace blink

class GURL;

class HoneycombBrowserImpl;

// Implementation of HoneycombFrame. HoneycombFrameImpl objects are owned by the
// HoneycombBrowerImpl and will be detached when the browser is notified that the
// associated renderer WebFrame will close.
class HoneycombFrameImpl
    : public HoneycombFrame,
      public honey::mojom::RenderFrame,
      public blink_glue::HoneycombExecutionContextLifecycleStateObserver {
 public:
  HoneycombFrameImpl(HoneycombBrowserImpl* browser,
               blink::WebLocalFrame* frame,
               int64_t frame_id);

  HoneycombFrameImpl(const HoneycombFrameImpl&) = delete;
  HoneycombFrameImpl& operator=(const HoneycombFrameImpl&) = delete;

  ~HoneycombFrameImpl() override;

  // HoneycombFrame implementation.
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

  // Forwarded from HoneycombRenderFrameObserver.
  void OnAttached();
  void OnWasShown();
  void OnDidCommitProvisionalLoad();
  void OnDidFinishLoad();
  void OnDraggableRegionsChanged();
  void OnContextCreated(v8::Local<v8::Context> context);
  void OnContextReleased();
  void OnDetached();

  blink::WebLocalFrame* web_frame() const { return frame_; }

 private:
  // Execute an action on the associated WebLocalFrame. This will queue the
  // action if the JavaScript context is not yet created.
  using LocalFrameAction =
      base::OnceCallback<void(blink::WebLocalFrame* frame)>;
  void ExecuteOnLocalFrame(const std::string& function_name,
                           LocalFrameAction action);

  enum class ConnectReason {
    RENDER_FRAME_CREATED,
    WAS_SHOWN,
    RETRY,
  };

  // Initiate the connection to the BrowserFrame channel.
  void ConnectBrowserFrame(ConnectReason reason);

  // Returns the remote BrowserFrame object.
  using BrowserFrameType = mojo::Remote<honey::mojom::BrowserFrame>;
  const BrowserFrameType& GetBrowserFrame(bool expect_acked = true);

  // Called if the BrowserFrame connection attempt times out.
  void OnBrowserFrameTimeout();

  enum class DisconnectReason {
    DETACHED,
    BROWSER_FRAME_DETACHED,
    CONNECT_TIMEOUT,
    RENDER_FRAME_DISCONNECT,
    BROWSER_FRAME_DISCONNECT,
  };

  // Called if/when a disconnect occurs. This may occur due to frame navigation,
  // destruction, or insertion into the bfcache (when the browser-side frame
  // representation is destroyed and closes the connection).
  void OnDisconnect(DisconnectReason reason);

  // Send an action to the remote BrowserFrame. This will queue the action if
  // the remote frame is not yet attached.
  using BrowserFrameAction = base::OnceCallback<void(const BrowserFrameType&)>;
  void SendToBrowserFrame(const std::string& function_name,
                          BrowserFrameAction action);

  void MaybeInitializeScriptContext();

  // honey::mojom::RenderFrame methods:
  void FrameAttachedAck() override;
  void FrameDetached() override;
  void SendMessage(const std::string& name,
                   base::Value::List arguments) override;
  void SendSharedMemoryRegion(const std::string& name,
                              base::WritableSharedMemoryRegion region) override;
  void SendCommand(const std::string& command) override;
  void SendCommandWithResponse(
      const std::string& command,
      honey::mojom::RenderFrame::SendCommandWithResponseCallback callback)
      override;
  void SendJavaScript(const std::u16string& jsCode,
                      const std::string& scriptUrl,
                      int32_t startLine) override;
  void LoadRequest(honey::mojom::RequestParamsPtr params) override;
  void DidStopLoading() override;
  void MoveOrResizeStarted() override;

  // blink_glue::HoneycombExecutionContextLifecycleStateObserver methods:
  void ContextLifecycleStateChanged(
      blink::mojom::blink::FrameLifecycleState state) override;

  std::string GetDebugString() const;

  HoneycombBrowserImpl* browser_;
  blink::WebLocalFrame* frame_;
  const int64_t frame_id_;

  bool did_commit_provisional_load_ = false;
  bool did_initialize_script_context_ = false;

  bool context_created_ = false;
  std::queue<std::pair<std::string, LocalFrameAction>> queued_context_actions_;

  // Number of times that browser reconnect has been attempted.
  size_t browser_connect_retry_ct_ = 0;

  // Current browser connection state.
  enum class ConnectionState {
    DISCONNECTED,
    CONNECTION_PENDING,
    CONNECTION_ACKED,
    RECONNECT_PENDING,
  } browser_connection_state_ = ConnectionState::DISCONNECTED;

  base::OneShotTimer browser_connect_timer_;

  std::queue<std::pair<std::string, BrowserFrameAction>>
      queued_browser_actions_;

  mojo::Receiver<honey::mojom::RenderFrame> receiver_{this};

  mojo::Remote<honey::mojom::BrowserFrame> browser_frame_;

  std::unique_ptr<blink_glue::HoneycombObserverRegistration>
      execution_context_lifecycle_state_observer_;

  base::WeakPtrFactory<HoneycombFrameImpl> weak_ptr_factory_{this};

  IMPLEMENT_REFCOUNTING(HoneycombFrameImpl);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_RENDERER_FRAME_IMPL_H_
