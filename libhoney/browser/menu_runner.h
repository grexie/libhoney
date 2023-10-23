// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_MENU_RUNNER_H_
#define HONEYCOMB_LIBHONEY_BROWSER_MENU_RUNNER_H_

#include <memory>
#include <string>

namespace content {
struct ContextMenuParams;
}

class AlloyBrowserHostImpl;
class HoneycombMenuModelImpl;

// Provides platform-specific menu implementations for HoneycombMenuCreator.
class HoneycombMenuRunner {
 public:
  HoneycombMenuRunner(const HoneycombMenuRunner&) = delete;
  HoneycombMenuRunner& operator=(const HoneycombMenuRunner&) = delete;

  virtual bool RunContextMenu(AlloyBrowserHostImpl* browser,
                              HoneycombMenuModelImpl* model,
                              const content::ContextMenuParams& params) = 0;
  virtual void CancelContextMenu() {}
  virtual bool FormatLabel(std::u16string& label) { return false; }

 protected:
  // Allow deletion via std::unique_ptr only.
  friend std::default_delete<HoneycombMenuRunner>;

  HoneycombMenuRunner() = default;
  virtual ~HoneycombMenuRunner() = default;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_MENU_RUNNER_H_
