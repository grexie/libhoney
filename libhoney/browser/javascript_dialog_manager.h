// Copyright (c) 2012 The Honeycomb Authors.
// Portions copyright (c) 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_JAVASCRIPT_DIALOG_MANAGER_H_
#define HONEYCOMB_LIBHONEY_BROWSER_JAVASCRIPT_DIALOG_MANAGER_H_
#pragma once

#include <memory>
#include <string>

#include "include/honey_jsdialog_handler.h"
#include "libhoney/browser/javascript_dialog_runner.h"

#include "base/memory/weak_ptr.h"
#include "content/public/browser/javascript_dialog_manager.h"

class HoneycombBrowserHostBase;

class HoneycombJavaScriptDialogManager : public content::JavaScriptDialogManager {
 public:
  // |runner| may be NULL if the platform doesn't implement dialogs.
  explicit HoneycombJavaScriptDialogManager(HoneycombBrowserHostBase* browser);

  HoneycombJavaScriptDialogManager(const HoneycombJavaScriptDialogManager&) = delete;
  HoneycombJavaScriptDialogManager& operator=(const HoneycombJavaScriptDialogManager&) =
      delete;

  ~HoneycombJavaScriptDialogManager() override;

  // Delete the runner to free any platform constructs.
  void Destroy();

  // JavaScriptDialogManager methods.
  void RunJavaScriptDialog(content::WebContents* web_contents,
                           content::RenderFrameHost* render_frame_host,
                           content::JavaScriptDialogType message_type,
                           const std::u16string& message_text,
                           const std::u16string& default_prompt_text,
                           DialogClosedCallback callback,
                           bool* did_suppress_message) override;
  void RunBeforeUnloadDialog(content::WebContents* web_contents,
                             content::RenderFrameHost* render_frame_host,
                             bool is_reload,
                             DialogClosedCallback callback) override;
  bool HandleJavaScriptDialog(content::WebContents* web_contents,
                              bool accept,
                              const std::u16string* prompt_override) override;
  void CancelDialogs(content::WebContents* web_contents,
                     bool reset_state) override;

 private:
  // Method executed by the callback passed to HoneycombJavaScriptDialogRunner::Run.
  void DialogClosed(DialogClosedCallback callback,
                    bool success,
                    const std::u16string& user_input);

  bool InitializeRunner();

  bool CanUseChromeDialogs() const;

  // HoneycombBrowserHostBase pointer is guaranteed to outlive this object.
  HoneycombBrowserHostBase* const browser_;

  HoneycombRefPtr<HoneycombJSDialogHandler> handler_;

  std::unique_ptr<HoneycombJavaScriptDialogRunner> runner_;
  bool runner_initialized_ = false;

  // Must be the last member.
  base::WeakPtrFactory<HoneycombJavaScriptDialogManager> weak_ptr_factory_;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_JAVASCRIPT_DIALOG_MANAGER_H_
