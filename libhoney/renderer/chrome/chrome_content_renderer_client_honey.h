// Copyright 2020 The Honeycomb Authors.
// Portions copyright 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_RENDERER_CHROME_CHROME_CONTENT_RENDERER_CLIENT_HONEYCOMB_
#define HONEYCOMB_LIBHONEY_RENDERER_CHROME_CHROME_CONTENT_RENDERER_CLIENT_HONEYCOMB_

#include <memory>

#include "base/memory/scoped_refptr.h"
#include "base/task/single_thread_task_runner.h"
#include "chrome/renderer/chrome_content_renderer_client.h"

class HoneycombRenderManager;

// Honeycomb override of ChromeContentRendererClient.
class ChromeContentRendererClientHoneycomb : public ChromeContentRendererClient {
 public:
  ChromeContentRendererClientHoneycomb();

  ChromeContentRendererClientHoneycomb(const ChromeContentRendererClientHoneycomb&) =
      delete;
  ChromeContentRendererClientHoneycomb& operator=(
      const ChromeContentRendererClientHoneycomb&) = delete;

  ~ChromeContentRendererClientHoneycomb() override;

  // Render thread task runner.
  base::SingleThreadTaskRunner* render_task_runner() const {
    return render_task_runner_.get();
  }

  // Returns the task runner for the current thread. Returns NULL if the current
  // thread is not the main render process thread.
  scoped_refptr<base::SingleThreadTaskRunner> GetCurrentTaskRunner();

  // ChromeContentRendererClient overrides.
  void RenderThreadStarted() override;
  void RenderThreadConnected() override;
  void RenderFrameCreated(content::RenderFrame* render_frame) override;
  void WebViewCreated(blink::WebView* web_view,
                      bool was_created_by_renderer,
                      const url::Origin* outermost_origin) override;
  void DevToolsAgentAttached() override;
  void DevToolsAgentDetached() override;
  void ExposeInterfacesToBrowser(mojo::BinderMap* binders) override;

 private:
  std::unique_ptr<HoneycombRenderManager> render_manager_;

  scoped_refptr<base::SingleThreadTaskRunner> render_task_runner_;
};

#endif  // HONEYCOMB_LIBHONEY_RENDERER_CHROME_CHROME_CONTENT_RENDERER_CLIENT_HONEYCOMB_
