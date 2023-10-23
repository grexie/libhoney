// Copyright (c) 2012 The Honeycomb Authors.
// Portions copyright (c) 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_JAVASCRIPT_DIALOG_RUNNER_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_JAVASCRIPT_DIALOG_RUNNER_H_
#pragma once

#include "content/public/browser/javascript_dialog_manager.h"
#include "content/public/common/javascript_dialog_type.h"

class HoneycombBrowserHostBase;

class HoneycombJavaScriptDialogRunner {
 public:
  HoneycombJavaScriptDialogRunner(const HoneycombJavaScriptDialogRunner&) = delete;
  HoneycombJavaScriptDialogRunner& operator=(const HoneycombJavaScriptDialogRunner&) =
      delete;

  using DialogClosedCallback =
      content::JavaScriptDialogManager::DialogClosedCallback;

  // Run the dialog. Execute |callback| on completion.
  virtual void Run(HoneycombBrowserHostBase* browser,
                   content::JavaScriptDialogType message_type,
                   const GURL& origin_url,
                   const std::u16string& message_text,
                   const std::u16string& default_prompt_text,
                   DialogClosedCallback callback) = 0;

  // Dismiss the dialog with the specified results.
  virtual void Handle(bool accept, const std::u16string* prompt_override) = 0;

  // Cancel a dialog mid-flight.
  virtual void Cancel() = 0;

 protected:
  // Allow deletion via std::unique_ptr only.
  friend std::default_delete<HoneycombJavaScriptDialogRunner>;

  HoneycombJavaScriptDialogRunner() = default;
  virtual ~HoneycombJavaScriptDialogRunner() = default;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_JAVASCRIPT_DIALOG_RUNNER_H_
