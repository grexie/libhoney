// Copyright 2014 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "build/build_config.h"

// Enable deprecation warnings on Windows. See http://crbug.com/585142.
#if BUILDFLAG(IS_WIN)
#if defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic error "-Wdeprecated-declarations"
#else
#pragma warning(push)
#pragma warning(default : 4996)
#endif
#endif

#include "libhoneycomb/renderer/render_frame_observer.h"

#include "libhoneycomb/common/app_manager.h"
#include "libhoneycomb/renderer/blink_glue.h"
#include "libhoneycomb/renderer/browser_impl.h"
#include "libhoneycomb/renderer/dom_document_impl.h"
#include "libhoneycomb/renderer/v8_impl.h"

#include "content/public/renderer/render_frame.h"
#include "third_party/blink/public/web/blink.h"
#include "third_party/blink/public/web/web_document.h"
#include "third_party/blink/public/web/web_element.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/public/web/web_node.h"

HoneycombRenderFrameObserver::HoneycombRenderFrameObserver(
    content::RenderFrame* render_frame)
    : content::RenderFrameObserver(render_frame) {}

HoneycombRenderFrameObserver::~HoneycombRenderFrameObserver() = default;

void HoneycombRenderFrameObserver::DidCommitProvisionalLoad(
    ui::PageTransition transition) {
  if (!frame_) {
    return;
  }

  frame_->OnDidCommitProvisionalLoad();

  if (frame_->GetParent() == nullptr) {
    blink::WebLocalFrame* frame = render_frame()->GetWebFrame();
    HoneycombRefPtr<HoneycombBrowserImpl> browserPtr =
        HoneycombBrowserImpl::GetBrowserForMainFrame(frame->Top());
    browserPtr->OnLoadingStateChange(true);
  }
  OnLoadStart();
}

void HoneycombRenderFrameObserver::WasShown() {
  if (frame_) {
    frame_->OnWasShown();
  }
}

void HoneycombRenderFrameObserver::DidFailProvisionalLoad() {
  if (frame_) {
    OnLoadError();
  }
}

void HoneycombRenderFrameObserver::DidFinishLoad() {
  if (frame_) {
    frame_->OnDidFinishLoad();
  }
}

void HoneycombRenderFrameObserver::WillDetach() {
  if (frame_) {
    frame_->OnDetached();
    frame_ = nullptr;
  }
}

void HoneycombRenderFrameObserver::FocusedElementChanged(
    const blink::WebElement& element) {
  if (!frame_) {
    return;
  }

  blink::WebLocalFrame* frame = render_frame()->GetWebFrame();
  HoneycombRefPtr<HoneycombBrowserImpl> browserPtr =
      HoneycombBrowserImpl::GetBrowserForMainFrame(frame->Top());
  if (!browserPtr) {
    return;
  }

  HoneycombRefPtr<HoneycombRenderProcessHandler> handler;
  HoneycombRefPtr<HoneycombApp> application = HoneycombAppManager::Get()->GetApplication();
  if (application) {
    handler = application->GetRenderProcessHandler();
  }
  if (!handler) {
    return;
  }

  HoneycombRefPtr<HoneycombFrameImpl> framePtr = browserPtr->GetWebFrameImpl(frame);

  if (element.IsNull()) {
    handler->OnFocusedNodeChanged(browserPtr.get(), framePtr.get(), nullptr);
    return;
  }

  if (element.GetDocument().IsNull()) {
    return;
  }

  HoneycombRefPtr<HoneycombDOMDocumentImpl> documentImpl =
      new HoneycombDOMDocumentImpl(browserPtr.get(), frame);
  handler->OnFocusedNodeChanged(browserPtr.get(), framePtr.get(),
                                documentImpl->GetOrCreateNode(element));
  documentImpl->Detach();
}

void HoneycombRenderFrameObserver::DraggableRegionsChanged() {
  if (frame_) {
    frame_->OnDraggableRegionsChanged();
  }
}

void HoneycombRenderFrameObserver::DidCreateScriptContext(
    v8::Handle<v8::Context> context,
    int world_id) {
  if (!frame_) {
    return;
  }

  blink::WebLocalFrame* frame = render_frame()->GetWebFrame();
  HoneycombRefPtr<HoneycombBrowserImpl> browserPtr =
      HoneycombBrowserImpl::GetBrowserForMainFrame(frame->Top());
  if (!browserPtr) {
    return;
  }

  HoneycombRefPtr<HoneycombRenderProcessHandler> handler;
  HoneycombRefPtr<HoneycombApp> application = HoneycombAppManager::Get()->GetApplication();
  if (application) {
    handler = application->GetRenderProcessHandler();
  }

  HoneycombRefPtr<HoneycombFrameImpl> framePtr = browserPtr->GetWebFrameImpl(frame);

  if (handler) {
    v8::Isolate* isolate = blink::MainThreadIsolate();
    v8::HandleScope handle_scope(isolate);
    v8::Context::Scope scope(context);
    v8::MicrotasksScope microtasks_scope(isolate,
                                         v8::MicrotasksScope::kRunMicrotasks);

    HoneycombRefPtr<HoneycombV8Context> contextPtr(new HoneycombV8ContextImpl(isolate, context));

    handler->OnContextCreated(browserPtr.get(), framePtr.get(), contextPtr);
  }

  // Do this last, in case the client callback modified the window object.
  framePtr->OnContextCreated(context);
}

void HoneycombRenderFrameObserver::WillReleaseScriptContext(
    v8::Handle<v8::Context> context,
    int world_id) {
  blink::WebLocalFrame* frame = render_frame()->GetWebFrame();
  HoneycombRefPtr<HoneycombBrowserImpl> browserPtr =
      HoneycombBrowserImpl::GetBrowserForMainFrame(frame->Top());
  if (!browserPtr) {
    return;
  }

  HoneycombRefPtr<HoneycombRenderProcessHandler> handler;
  HoneycombRefPtr<HoneycombApp> application = HoneycombAppManager::Get()->GetApplication();
  if (application) {
    handler = application->GetRenderProcessHandler();
  }

  HoneycombRefPtr<HoneycombFrameImpl> framePtr = browserPtr->GetWebFrameImpl(frame);

  if (handler) {
    v8::Isolate* isolate = blink::MainThreadIsolate();
    v8::HandleScope handle_scope(isolate);

    // The released context should not be used for script execution.
    // Depending on how the context is released this may or may not already
    // be set.
    blink_glue::HoneycombScriptForbiddenScope forbidScript;

    HoneycombRefPtr<HoneycombV8Context> contextPtr(new HoneycombV8ContextImpl(isolate, context));

    handler->OnContextReleased(browserPtr.get(), framePtr.get(), contextPtr);
  }

  framePtr->OnContextReleased();

  HoneycombV8ReleaseContext(context);
}

void HoneycombRenderFrameObserver::OnDestruct() {
  delete this;
}

void HoneycombRenderFrameObserver::OnInterfaceRequestForFrame(
    const std::string& interface_name,
    mojo::ScopedMessagePipeHandle* interface_pipe) {
  registry_.TryBindInterface(interface_name, interface_pipe);
}

bool HoneycombRenderFrameObserver::OnAssociatedInterfaceRequestForFrame(
    const std::string& interface_name,
    mojo::ScopedInterfaceEndpointHandle* handle) {
  return associated_interfaces_.TryBindInterface(interface_name, handle);
}

void HoneycombRenderFrameObserver::AttachFrame(HoneycombFrameImpl* frame) {
  DCHECK(frame);
  DCHECK(!frame_);
  frame_ = frame;
  frame_->OnAttached();
}

void HoneycombRenderFrameObserver::OnLoadStart() {
  HoneycombRefPtr<HoneycombApp> app = HoneycombAppManager::Get()->GetApplication();
  if (app.get()) {
    HoneycombRefPtr<HoneycombRenderProcessHandler> handler = app->GetRenderProcessHandler();
    if (handler.get()) {
      HoneycombRefPtr<HoneycombLoadHandler> load_handler = handler->GetLoadHandler();
      if (load_handler.get()) {
        blink::WebLocalFrame* frame = render_frame()->GetWebFrame();
        HoneycombRefPtr<HoneycombBrowserImpl> browserPtr =
            HoneycombBrowserImpl::GetBrowserForMainFrame(frame->Top());
        load_handler->OnLoadStart(browserPtr.get(), frame_, TT_EXPLICIT);
      }
    }
  }
}

void HoneycombRenderFrameObserver::OnLoadError() {
  HoneycombRefPtr<HoneycombApp> app = HoneycombAppManager::Get()->GetApplication();
  if (app.get()) {
    HoneycombRefPtr<HoneycombRenderProcessHandler> handler = app->GetRenderProcessHandler();
    if (handler.get()) {
      HoneycombRefPtr<HoneycombLoadHandler> load_handler = handler->GetLoadHandler();
      // Error codes were removed from DidFailProvisionalLoad() so we now always
      // pass the same value.
      if (load_handler.get()) {
        const honey_errorcode_t errorCode =
            static_cast<honey_errorcode_t>(net::ERR_ABORTED);
        const std::string& errorText = net::ErrorToString(errorCode);
        blink::WebLocalFrame* frame = render_frame()->GetWebFrame();
        HoneycombRefPtr<HoneycombBrowserImpl> browserPtr =
            HoneycombBrowserImpl::GetBrowserForMainFrame(frame->Top());
        load_handler->OnLoadError(browserPtr.get(), frame_, errorCode,
                                  errorText, frame_->GetURL());
      }
    }
  }
}

// Enable deprecation warnings on Windows. See http://crbug.com/585142.
#if BUILDFLAG(IS_WIN)
#if defined(__clang__)
#pragma GCC diagnostic pop
#else
#pragma warning(pop)
#endif
#endif
