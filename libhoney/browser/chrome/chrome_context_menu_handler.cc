// Copyright (c) 2021 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "libhoney/browser/chrome/chrome_context_menu_handler.h"

#include "libhoney/browser/browser_host_base.h"
#include "libhoney/browser/context_menu_params_impl.h"
#include "libhoney/browser/simple_menu_model_impl.h"

#include "chrome/browser/renderer_context_menu/render_view_context_menu.h"

namespace context_menu {

namespace {

// Lifespan is controlled by RenderViewContextMenu.
class HoneycombContextMenuObserver : public RenderViewContextMenuObserver,
                               public HoneycombSimpleMenuModelImpl::StateDelegate {
 public:
  HoneycombContextMenuObserver(RenderViewContextMenu* context_menu,
                         HoneycombRefPtr<HoneycombBrowserHostBase> browser,
                         HoneycombRefPtr<HoneycombContextMenuHandler> handler)
      : context_menu_(context_menu), browser_(browser), handler_(handler) {}

  HoneycombContextMenuObserver(const HoneycombContextMenuObserver&) = delete;
  HoneycombContextMenuObserver& operator=(const HoneycombContextMenuObserver&) = delete;

  // RenderViewContextMenuObserver methods:

  void InitMenu(const content::ContextMenuParams& params) override {
    params_ = new HoneycombContextMenuParamsImpl(
        const_cast<content::ContextMenuParams*>(&context_menu_->params()));
    model_ = new HoneycombSimpleMenuModelImpl(
        const_cast<ui::SimpleMenuModel*>(&context_menu_->menu_model()),
        context_menu_, this, /*is_owned=*/false, /*is_popup=*/false);

    handler_->OnBeforeContextMenu(browser_, GetFrame(), params_, model_);
  }

  bool IsCommandIdSupported(int command_id) override {
    // Always claim support for the reserved user ID range.
    if (command_id >= MENU_ID_USER_FIRST && command_id <= MENU_ID_USER_LAST) {
      return true;
    }

    // Also claim support in specific cases where an ItemInfo exists.
    return GetItemInfo(command_id) != nullptr;
  }

  // Only called if IsCommandIdSupported() returns true.
  bool IsCommandIdEnabled(int command_id) override {
    // Always return true to use the SimpleMenuModel state.
    return true;
  }

  // Only called if IsCommandIdSupported() returns true.
  bool IsCommandIdChecked(int command_id) override {
    auto* info = GetItemInfo(command_id);
    return info ? info->checked : false;
  }

  // Only called if IsCommandIdSupported() returns true.
  bool GetAccelerator(int command_id, ui::Accelerator* accel) override {
    auto* info = GetItemInfo(command_id);
    if (info && info->accel) {
      *accel = *info->accel;
      return true;
    }
    return false;
  }

  void CommandWillBeExecuted(int command_id) override {
    if (handler_->OnContextMenuCommand(browser_, GetFrame(), params_,
                                       command_id, EVENTFLAG_NONE)) {
      // Create an ItemInfo so that we get the ExecuteCommand() callback
      // instead of the default handler.
      GetOrCreateItemInfo(command_id);
    }
  }

  // Only called if IsCommandIdSupported() returns true.
  void ExecuteCommand(int command_id) override {
    auto* info = GetItemInfo(command_id);
    if (info) {
      // In case it was added in CommandWillBeExecuted().
      MaybeDeleteItemInfo(command_id, info);
    }
  }

  void OnMenuClosed() override {
    handler_->OnContextMenuDismissed(browser_, GetFrame());
    model_->Detach();

    // Clear stored state because this object won't be deleted until a new
    // context menu is created or the associated browser is destroyed.
    browser_ = nullptr;
    handler_ = nullptr;
    params_ = nullptr;
    model_ = nullptr;
    iteminfomap_.clear();
  }

  // HoneycombSimpleMenuModelImpl::StateDelegate methods:

  void SetChecked(int command_id, bool checked) override {
    // No-op if already at the default state.
    if (!checked && !GetItemInfo(command_id)) {
      return;
    }

    auto* info = GetOrCreateItemInfo(command_id);
    info->checked = checked;
    if (!checked) {
      MaybeDeleteItemInfo(command_id, info);
    }
  }

  void SetAccelerator(int command_id,
                      absl::optional<ui::Accelerator> accel) override {
    // No-op if already at the default state.
    if (!accel && !GetItemInfo(command_id)) {
      return;
    }

    auto* info = GetOrCreateItemInfo(command_id);
    info->accel = accel;
    if (!accel) {
      MaybeDeleteItemInfo(command_id, info);
    }
  }

 private:
  struct ItemInfo {
    ItemInfo() {}

    bool checked = false;
    absl::optional<ui::Accelerator> accel;
  };

  ItemInfo* GetItemInfo(int command_id) {
    auto it = iteminfomap_.find(command_id);
    if (it != iteminfomap_.end()) {
      return &it->second;
    }
    return nullptr;
  }

  ItemInfo* GetOrCreateItemInfo(int command_id) {
    if (auto info = GetItemInfo(command_id)) {
      return info;
    }

    auto result = iteminfomap_.insert(std::make_pair(command_id, ItemInfo()));
    return &result.first->second;
  }

  void MaybeDeleteItemInfo(int command_id, ItemInfo* info) {
    // Remove if all info has reverted to the default state.
    if (!info->checked && !info->accel) {
      auto it = iteminfomap_.find(command_id);
      iteminfomap_.erase(it);
    }
  }

  HoneycombRefPtr<HoneycombFrame> GetFrame() const {
    HoneycombRefPtr<HoneycombFrame> frame;

    // May return nullptr if the frame is destroyed while the menu is pending.
    auto* rfh = context_menu_->GetRenderFrameHost();
    if (rfh) {
      frame = browser_->GetFrameForHost(rfh);
    }
    if (!frame) {
      frame = browser_->GetMainFrame();
    }
    return frame;
  }

  RenderViewContextMenu* const context_menu_;
  HoneycombRefPtr<HoneycombBrowserHostBase> browser_;
  HoneycombRefPtr<HoneycombContextMenuHandler> handler_;
  HoneycombRefPtr<HoneycombContextMenuParams> params_;
  HoneycombRefPtr<HoneycombSimpleMenuModelImpl> model_;

  // Map of command_id to ItemInfo.
  using ItemInfoMap = std::map<int, ItemInfo>;
  ItemInfoMap iteminfomap_;
};

std::unique_ptr<RenderViewContextMenuObserver> MenuCreatedCallback(
    RenderViewContextMenu* context_menu) {
  auto browser = HoneycombBrowserHostBase::GetBrowserForContents(
      context_menu->source_web_contents());
  if (browser) {
    if (auto client = browser->GetClient()) {
      if (auto handler = client->GetContextMenuHandler()) {
        return std::make_unique<HoneycombContextMenuObserver>(context_menu, browser,
                                                        handler);
      }
    }
  }

  return nullptr;
}

}  // namespace

void RegisterMenuCreatedCallback() {
  RenderViewContextMenu::RegisterMenuCreatedCallback(
      base::BindRepeating(&MenuCreatedCallback));
}

}  // namespace context_menu
