// Copyright (c) 2012 The Honeycomb Authors.
// Portions copyright (c) 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_NATIVE_JAVASCRIPT_DIALOG_RUNNER_MAC_H_
#define HONEYCOMB_LIBHONEY_BROWSER_NATIVE_JAVASCRIPT_DIALOG_RUNNER_MAC_H_
#pragma once

#include "libhoney/browser/javascript_dialog_runner.h"

#include "base/functional/callback.h"
#include "base/memory/weak_ptr.h"

#if __OBJC__
@class HoneycombJavaScriptDialogHelper;
#else
class HoneycombJavaScriptDialogHelper;
#endif  // __OBJC__

class HoneycombJavaScriptDialogRunnerMac : public HoneycombJavaScriptDialogRunner {
 public:
  HoneycombJavaScriptDialogRunnerMac();
  ~HoneycombJavaScriptDialogRunnerMac() override;

  // HoneycombJavaScriptDialogRunner methods:
  void Run(HoneycombBrowserHostBase* browser,
           content::JavaScriptDialogType message_type,
           const GURL& origin_url,
           const std::u16string& message_text,
           const std::u16string& default_prompt_text,
           DialogClosedCallback callback) override;
  void Handle(bool accept, const std::u16string* prompt_override) override;
  void Cancel() override;

  // Callback from HoneycombJavaScriptDialogHelper when the dialog is closed.
  void DialogClosed(bool success, const std::u16string& user_input);

 private:
  DialogClosedCallback callback_;

  HoneycombJavaScriptDialogHelper* __strong helper_;

  // Must be the last member.
  base::WeakPtrFactory<HoneycombJavaScriptDialogRunnerMac> weak_ptr_factory_;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_NATIVE_JAVASCRIPT_DIALOG_RUNNER_MAC_H_
