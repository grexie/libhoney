// Copyright 2019 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_CHROME_CHROME_BROWSER_MAIN_EXTRA_PARTS_HONEYCOMB_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_CHROME_CHROME_BROWSER_MAIN_EXTRA_PARTS_HONEYCOMB_H_

#include <memory>

#include "libhoneycomb/browser/request_context_impl.h"

#include "base/task/single_thread_task_runner.h"
#include "chrome/browser/chrome_browser_main_extra_parts.h"

// Wrapper that owns and initialize the browser memory-related extra parts.
class ChromeBrowserMainExtraPartsHoneycomb : public ChromeBrowserMainExtraParts {
 public:
  ChromeBrowserMainExtraPartsHoneycomb();

  ChromeBrowserMainExtraPartsHoneycomb(const ChromeBrowserMainExtraPartsHoneycomb&) =
      delete;
  ChromeBrowserMainExtraPartsHoneycomb& operator=(
      const ChromeBrowserMainExtraPartsHoneycomb&) = delete;

  ~ChromeBrowserMainExtraPartsHoneycomb() override;

  HoneycombRefPtr<HoneycombRequestContextImpl> request_context() const {
    return global_request_context_;
  }

  scoped_refptr<base::SingleThreadTaskRunner> background_task_runner() const {
    return background_task_runner_;
  }
  scoped_refptr<base::SingleThreadTaskRunner> user_visible_task_runner() const {
    return user_visible_task_runner_;
  }
  scoped_refptr<base::SingleThreadTaskRunner> user_blocking_task_runner()
      const {
    return user_blocking_task_runner_;
  }

 private:
  // ChromeBrowserMainExtraParts overrides.
  void PostProfileInit(Profile* profile, bool is_initial_profile) override;
  void PreMainMessageLoopRun() override;

  HoneycombRefPtr<HoneycombRequestContextImpl> global_request_context_;

  // Blocking task runners exposed via HoneycombTaskRunner. For consistency with
  // previous named thread behavior always execute all pending tasks before
  // shutdown (e.g. to make sure critical data is saved to disk).
  scoped_refptr<base::SingleThreadTaskRunner> background_task_runner_;
  scoped_refptr<base::SingleThreadTaskRunner> user_visible_task_runner_;
  scoped_refptr<base::SingleThreadTaskRunner> user_blocking_task_runner_;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_CHROME_CHROME_BROWSER_MAIN_EXTRA_PARTS_HONEYCOMB_H_
