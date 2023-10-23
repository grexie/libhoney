// Copyright 2014 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef LIBHONEY_RENDERER_RENDER_FRAME_OBSERVER_H_
#define LIBHONEY_RENDERER_RENDER_FRAME_OBSERVER_H_

#include "content/public/renderer/render_frame_observer.h"

#include "services/service_manager/public/cpp/binder_registry.h"
#include "third_party/blink/public/common/associated_interfaces/associated_interface_registry.h"

namespace content {
class RenderFrame;
class RenderView;
}  // namespace content

class HoneycombFrameImpl;

class HoneycombRenderFrameObserver : public content::RenderFrameObserver {
 public:
  explicit HoneycombRenderFrameObserver(content::RenderFrame* render_frame);

  HoneycombRenderFrameObserver(const HoneycombRenderFrameObserver&) = delete;
  HoneycombRenderFrameObserver& operator=(const HoneycombRenderFrameObserver&) = delete;

  ~HoneycombRenderFrameObserver() override;

  // RenderFrameObserver methods:
  void WasShown() override;
  void DidCommitProvisionalLoad(ui::PageTransition transition) override;
  void DidFailProvisionalLoad() override;
  void DidFinishLoad() override;
  void WillDetach() override;
  void FocusedElementChanged(const blink::WebElement& element) override;
  void DraggableRegionsChanged() override;
  void DidCreateScriptContext(v8::Handle<v8::Context> context,
                              int world_id) override;
  void WillReleaseScriptContext(v8::Handle<v8::Context> context,
                                int world_id) override;
  void OnDestruct() override;
  void OnInterfaceRequestForFrame(
      const std::string& interface_name,
      mojo::ScopedMessagePipeHandle* interface_pipe) override;
  bool OnAssociatedInterfaceRequestForFrame(
      const std::string& interface_name,
      mojo::ScopedInterfaceEndpointHandle* handle) override;

  service_manager::BinderRegistry* registry() { return &registry_; }
  blink::AssociatedInterfaceRegistry* associated_interfaces() {
    return &associated_interfaces_;
  }

  void AttachFrame(HoneycombFrameImpl* frame);

 private:
  void OnLoadStart();
  void OnLoadError();

  HoneycombFrameImpl* frame_ = nullptr;

  service_manager::BinderRegistry registry_;

  // For interfaces which must be associated with some IPC::ChannelProxy,
  // meaning that messages on the interface retain FIFO with respect to legacy
  // Chrome IPC messages sent or dispatched on the channel.
  blink::AssociatedInterfaceRegistry associated_interfaces_;
};

#endif  // LIBHONEY_RENDERER_RENDER_FRAME_OBSERVER_H_
