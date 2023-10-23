// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_MENU_MANAGER_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_MENU_MANAGER_H_
#pragma once

#include "libhoneycomb/browser/menu_model_impl.h"

#include "libhoneycomb/browser/menu_runner.h"

#include "base/memory/weak_ptr.h"
#include "content/public/browser/context_menu_params.h"
#include "content/public/browser/web_contents_observer.h"

namespace content {
class RenderFrameHost;
class WebContents;
}  // namespace content

class AlloyBrowserHostImpl;
class HoneycombRunContextMenuCallback;

class HoneycombMenuManager : public HoneycombMenuModelImpl::Delegate,
                       public content::WebContentsObserver {
 public:
  HoneycombMenuManager(AlloyBrowserHostImpl* browser,
                 std::unique_ptr<HoneycombMenuRunner> runner);

  HoneycombMenuManager(const HoneycombMenuManager&) = delete;
  HoneycombMenuManager& operator=(const HoneycombMenuManager&) = delete;

  ~HoneycombMenuManager() override;

  // Delete the runner to free any platform constructs.
  void Destroy();

  // Returns true if the context menu is currently showing.
  bool IsShowingContextMenu();

  // Create the context menu.
  bool CreateContextMenu(const content::ContextMenuParams& params);
  void CancelContextMenu();

 private:
  // HoneycombMenuModelImpl::Delegate methods.
  void ExecuteCommand(HoneycombRefPtr<HoneycombMenuModelImpl> source,
                      int command_id,
                      honey_event_flags_t event_flags) override;
  void MenuWillShow(HoneycombRefPtr<HoneycombMenuModelImpl> source) override;
  void MenuClosed(HoneycombRefPtr<HoneycombMenuModelImpl> source) override;
  bool FormatLabel(HoneycombRefPtr<HoneycombMenuModelImpl> source,
                   std::u16string& label) override;

  void ExecuteCommandCallback(int command_id, honey_event_flags_t event_flags);

  // Create the default menu model.
  void CreateDefaultModel();
  // Execute the default command handling.
  void ExecuteDefaultCommand(int command_id);

  // Returns true if the specified id is a custom context menu command.
  bool IsCustomContextMenuCommand(int command_id);

  // AlloyBrowserHostImpl pointer is guaranteed to outlive this object.
  AlloyBrowserHostImpl* browser_;

  std::unique_ptr<HoneycombMenuRunner> runner_;

  HoneycombRefPtr<HoneycombMenuModelImpl> model_;
  content::ContextMenuParams params_;

  // Not owned by this class.
  HoneycombRunContextMenuCallback* custom_menu_callback_;

  // Must be the last member.
  base::WeakPtrFactory<HoneycombMenuManager> weak_ptr_factory_;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_MENU_MANAGER_H_
