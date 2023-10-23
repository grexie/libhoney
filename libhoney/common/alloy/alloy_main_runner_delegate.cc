// Copyright 2020 The Honeycomb Authors.
// Portions copyright 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoney/common/alloy/alloy_main_runner_delegate.h"

#include "libhoney/browser/alloy/chrome_browser_process_alloy.h"
#include "libhoney/common/alloy/alloy_main_delegate.h"
#include "libhoney/renderer/alloy/alloy_content_renderer_client.h"

#include "content/public/browser/render_process_host.h"
#include "ui/base/resource/resource_bundle.h"

AlloyMainRunnerDelegate::AlloyMainRunnerDelegate(HoneycombMainRunnerHandler* runner,
                                                 HoneycombSettings* settings,
                                                 HoneycombRefPtr<HoneycombApp> application)
    : runner_(runner), settings_(settings), application_(application) {}
AlloyMainRunnerDelegate::~AlloyMainRunnerDelegate() = default;

content::ContentMainDelegate*
AlloyMainRunnerDelegate::GetContentMainDelegate() {
  if (!main_delegate_) {
    main_delegate_ =
        std::make_unique<AlloyMainDelegate>(runner_, settings_, application_);
  }
  return main_delegate_.get();
}

void AlloyMainRunnerDelegate::BeforeMainThreadInitialize(
    const HoneycombMainArgs& args) {
  g_browser_process = new ChromeBrowserProcessAlloy();
}

void AlloyMainRunnerDelegate::BeforeMainThreadRun(
    bool multi_threaded_message_loop) {
  static_cast<ChromeBrowserProcessAlloy*>(g_browser_process)->Initialize();
}

void AlloyMainRunnerDelegate::AfterUIThreadInitialize() {
  static_cast<ChromeBrowserProcessAlloy*>(g_browser_process)
      ->OnContextInitialized();
}

void AlloyMainRunnerDelegate::AfterUIThreadShutdown() {
  static_cast<ChromeBrowserProcessAlloy*>(g_browser_process)
      ->CleanupOnUIThread();

  ui::ResourceBundle::GetSharedInstance().CleanupOnUIThread();
}

void AlloyMainRunnerDelegate::BeforeMainThreadShutdown() {
  if (content::RenderProcessHost::run_renderer_in_process()) {
    // Blocks until RenderProcess cleanup is complete.
    AlloyContentRendererClient::Get()->RunSingleProcessCleanup();
  }
}

void AlloyMainRunnerDelegate::AfterMainThreadShutdown() {
  if (g_browser_process) {
    delete g_browser_process;
    g_browser_process = nullptr;
  }
}
