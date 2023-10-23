// Copyright 2019 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoney/browser/chrome/chrome_browser_main_extra_parts_honey.h"

#include "libhoney/browser/chrome/chrome_context_menu_handler.h"
#include "libhoney/browser/context.h"
#include "libhoney/browser/file_dialog_runner.h"
#include "libhoney/browser/net/chrome_scheme_handler.h"
#include "libhoney/browser/permission_prompt.h"

#include "base/task/thread_pool.h"
#include "chrome/browser/profiles/profile.h"

ChromeBrowserMainExtraPartsHoneycomb::ChromeBrowserMainExtraPartsHoneycomb() = default;

ChromeBrowserMainExtraPartsHoneycomb::~ChromeBrowserMainExtraPartsHoneycomb() = default;

void ChromeBrowserMainExtraPartsHoneycomb::PostProfileInit(Profile* profile,
                                                     bool is_initial_profile) {
  if (!is_initial_profile) {
    return;
  }

  HoneycombRequestContextSettings settings;
  HoneycombContext::Get()->PopulateGlobalRequestContextSettings(&settings);

  // Use the existing path for the initial profile.
  HoneycombString(&settings.cache_path) = profile->GetPath().value();

  // Create the global RequestContext.
  global_request_context_ =
      HoneycombRequestContextImpl::CreateGlobalRequestContext(settings);
}

void ChromeBrowserMainExtraPartsHoneycomb::PreMainMessageLoopRun() {
  background_task_runner_ = base::ThreadPool::CreateSingleThreadTaskRunner(
      {base::TaskPriority::BEST_EFFORT,
       base::TaskShutdownBehavior::BLOCK_SHUTDOWN, base::MayBlock()});
  user_visible_task_runner_ = base::ThreadPool::CreateSingleThreadTaskRunner(
      {base::TaskPriority::USER_VISIBLE,
       base::TaskShutdownBehavior::BLOCK_SHUTDOWN, base::MayBlock()});
  user_blocking_task_runner_ = base::ThreadPool::CreateSingleThreadTaskRunner(
      {base::TaskPriority::USER_BLOCKING,
       base::TaskShutdownBehavior::BLOCK_SHUTDOWN, base::MayBlock()});

  scheme::RegisterWebUIControllerFactory();
  context_menu::RegisterMenuCreatedCallback();
  file_dialog_runner::RegisterFactory();
  permission_prompt::RegisterCreateCallback();
}
