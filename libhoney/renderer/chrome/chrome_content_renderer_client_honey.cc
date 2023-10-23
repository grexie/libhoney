// Copyright 2020 The Honeycomb Authors.
// Portions copyright 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoney/renderer/chrome/chrome_content_renderer_client_honey.h"

#include "libhoney/renderer/render_frame_observer.h"
#include "libhoney/renderer/render_manager.h"
#include "libhoney/renderer/thread_util.h"

#include "content/public/renderer/render_thread.h"

ChromeContentRendererClientHoneycomb::ChromeContentRendererClientHoneycomb()
    : render_manager_(new HoneycombRenderManager) {}

ChromeContentRendererClientHoneycomb::~ChromeContentRendererClientHoneycomb() = default;

scoped_refptr<base::SingleThreadTaskRunner>
ChromeContentRendererClientHoneycomb::GetCurrentTaskRunner() {
  // Check if currently on the render thread.
  if (HONEYCOMB_CURRENTLY_ON_RT()) {
    return render_task_runner_;
  }
  return nullptr;
}

void ChromeContentRendererClientHoneycomb::RenderThreadStarted() {
  ChromeContentRendererClient::RenderThreadStarted();

  render_task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
}

void ChromeContentRendererClientHoneycomb::RenderThreadConnected() {
  ChromeContentRendererClient::RenderThreadConnected();

  render_manager_->RenderThreadConnected();
}

void ChromeContentRendererClientHoneycomb::RenderFrameCreated(
    content::RenderFrame* render_frame) {
  ChromeContentRendererClient::RenderFrameCreated(render_frame);

  // Will delete itself when no longer needed.
  HoneycombRenderFrameObserver* render_frame_observer =
      new HoneycombRenderFrameObserver(render_frame);

  bool browser_created;
  absl::optional<bool> is_windowless;
  render_manager_->RenderFrameCreated(render_frame, render_frame_observer,
                                      browser_created, is_windowless);
  if (is_windowless.has_value() && *is_windowless) {
    LOG(ERROR) << "The chrome runtime does not support windowless browsers";
  }
}

void ChromeContentRendererClientHoneycomb::WebViewCreated(
    blink::WebView* web_view,
    bool was_created_by_renderer,
    const url::Origin* outermost_origin) {
  ChromeContentRendererClient::WebViewCreated(web_view, was_created_by_renderer,
                                              outermost_origin);

  bool browser_created;
  absl::optional<bool> is_windowless;
  render_manager_->WebViewCreated(web_view, browser_created, is_windowless);
  if (is_windowless.has_value() && *is_windowless) {
    LOG(ERROR) << "The chrome runtime does not support windowless browsers";
  }
}

void ChromeContentRendererClientHoneycomb::DevToolsAgentAttached() {
  // WebWorkers may be creating agents on a different thread.
  if (!render_task_runner_->BelongsToCurrentThread()) {
    render_task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(&ChromeContentRendererClientHoneycomb::DevToolsAgentAttached,
                       base::Unretained(this)));
    return;
  }

  render_manager_->DevToolsAgentAttached();
}

void ChromeContentRendererClientHoneycomb::DevToolsAgentDetached() {
  // WebWorkers may be creating agents on a different thread.
  if (!render_task_runner_->BelongsToCurrentThread()) {
    render_task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(&ChromeContentRendererClientHoneycomb::DevToolsAgentDetached,
                       base::Unretained(this)));
    return;
  }

  render_manager_->DevToolsAgentDetached();
}

void ChromeContentRendererClientHoneycomb::ExposeInterfacesToBrowser(
    mojo::BinderMap* binders) {
  ChromeContentRendererClient::ExposeInterfacesToBrowser(binders);

  render_manager_->ExposeInterfacesToBrowser(binders);
}