// Copyright (c) 2012 The Honeycomb Authors.
// Portions copyright (c) 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_MENU_MODEL_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_MENU_MODEL_IMPL_H_
#pragma once

#include <vector>

#include "include/honey_menu_model.h"
#include "include/honey_menu_model_delegate.h"

#include "base/threading/platform_thread.h"
#include "third_party/blink/public/mojom/context_menu/context_menu.mojom-forward.h"
#include "ui/base/models/menu_model.h"
#include "ui/gfx/font_list.h"

class HoneycombMenuModelImpl : public HoneycombMenuModel {
 public:
  class Delegate {
   public:
    // Perform the action associated with the specified |command_id| and
    // optional |event_flags|.
    virtual void ExecuteCommand(HoneycombRefPtr<HoneycombMenuModelImpl> source,
                                int command_id,
                                honey_event_flags_t event_flags) = 0;

    // Called when the user moves the mouse outside the menu and over the owning
    // window.
    virtual void MouseOutsideMenu(HoneycombRefPtr<HoneycombMenuModelImpl> source,
                                  const gfx::Point& screen_point) {}

    // Called on unhandled open/close submenu keyboard commands. |is_rtl| will
    // be true if the menu is displaying a right-to-left language.
    virtual void UnhandledOpenSubmenu(HoneycombRefPtr<HoneycombMenuModelImpl> source,
                                      bool is_rtl) {}
    virtual void UnhandledCloseSubmenu(HoneycombRefPtr<HoneycombMenuModelImpl> source,
                                       bool is_rtl) {}

    // Called when the menu is about to show.
    virtual void MenuWillShow(HoneycombRefPtr<HoneycombMenuModelImpl> source) = 0;

    // Called when the menu has closed.
    virtual void MenuClosed(HoneycombRefPtr<HoneycombMenuModelImpl> source) = 0;

    // Allows the delegate to modify a menu item label before it's displayed.
    virtual bool FormatLabel(HoneycombRefPtr<HoneycombMenuModelImpl> source,
                             std::u16string& label) = 0;

   protected:
    virtual ~Delegate() {}
  };

  // Either |delegate| or |menu_model_delegate| must be non-nullptr.
  // If |delegate| is non-nullptr it must outlive this class.
  HoneycombMenuModelImpl(Delegate* delegate,
                   HoneycombRefPtr<HoneycombMenuModelDelegate> menu_model_delegate,
                   bool is_submenu);

  HoneycombMenuModelImpl(const HoneycombMenuModelImpl&) = delete;
  HoneycombMenuModelImpl& operator=(const HoneycombMenuModelImpl&) = delete;

  ~HoneycombMenuModelImpl() override;

  // HoneycombMenuModel methods.
  bool IsSubMenu() override;
  bool Clear() override;
  size_t GetCount() override;
  bool AddSeparator() override;
  bool AddItem(int command_id, const HoneycombString& label) override;
  bool AddCheckItem(int command_id, const HoneycombString& label) override;
  bool AddRadioItem(int command_id,
                    const HoneycombString& label,
                    int group_id) override;
  HoneycombRefPtr<HoneycombMenuModel> AddSubMenu(int command_id,
                                     const HoneycombString& label) override;
  bool InsertSeparatorAt(size_t index) override;
  bool InsertItemAt(size_t index,
                    int command_id,
                    const HoneycombString& label) override;
  bool InsertCheckItemAt(size_t index,
                         int command_id,
                         const HoneycombString& label) override;
  bool InsertRadioItemAt(size_t index,
                         int command_id,
                         const HoneycombString& label,
                         int group_id) override;
  HoneycombRefPtr<HoneycombMenuModel> InsertSubMenuAt(size_t index,
                                          int command_id,
                                          const HoneycombString& label) override;
  bool Remove(int command_id) override;
  bool RemoveAt(size_t index) override;
  int GetIndexOf(int command_id) override;
  int GetCommandIdAt(size_t index) override;
  bool SetCommandIdAt(size_t index, int command_id) override;
  HoneycombString GetLabel(int command_id) override;
  HoneycombString GetLabelAt(size_t index) override;
  bool SetLabel(int command_id, const HoneycombString& label) override;
  bool SetLabelAt(size_t index, const HoneycombString& label) override;
  MenuItemType GetType(int command_id) override;
  MenuItemType GetTypeAt(size_t index) override;
  int GetGroupId(int command_id) override;
  int GetGroupIdAt(size_t index) override;
  bool SetGroupId(int command_id, int group_id) override;
  bool SetGroupIdAt(size_t index, int group_id) override;
  HoneycombRefPtr<HoneycombMenuModel> GetSubMenu(int command_id) override;
  HoneycombRefPtr<HoneycombMenuModel> GetSubMenuAt(size_t index) override;
  bool IsVisible(int command_id) override;
  bool IsVisibleAt(size_t index) override;
  bool SetVisible(int command_id, bool visible) override;
  bool SetVisibleAt(size_t index, bool visible) override;
  bool IsEnabled(int command_id) override;
  bool IsEnabledAt(size_t index) override;
  bool SetEnabled(int command_id, bool enabled) override;
  bool SetEnabledAt(size_t index, bool enabled) override;
  bool IsChecked(int command_id) override;
  bool IsCheckedAt(size_t index) override;
  bool SetChecked(int command_id, bool checked) override;
  bool SetCheckedAt(size_t index, bool checked) override;
  bool HasAccelerator(int command_id) override;
  bool HasAcceleratorAt(size_t index) override;
  bool SetAccelerator(int command_id,
                      int key_code,
                      bool shift_pressed,
                      bool ctrl_pressed,
                      bool alt_pressed) override;
  bool SetAcceleratorAt(size_t index,
                        int key_code,
                        bool shift_pressed,
                        bool ctrl_pressed,
                        bool alt_pressed) override;
  bool RemoveAccelerator(int command_id) override;
  bool RemoveAcceleratorAt(size_t index) override;
  bool GetAccelerator(int command_id,
                      int& key_code,
                      bool& shift_pressed,
                      bool& ctrl_pressed,
                      bool& alt_pressed) override;
  bool GetAcceleratorAt(size_t index,
                        int& key_code,
                        bool& shift_pressed,
                        bool& ctrl_pressed,
                        bool& alt_pressed) override;
  bool SetColor(int command_id,
                honey_menu_color_type_t color_type,
                honey_color_t color) override;
  bool SetColorAt(int index,
                  honey_menu_color_type_t color_type,
                  honey_color_t color) override;
  bool GetColor(int command_id,
                honey_menu_color_type_t color_type,
                honey_color_t& color) override;
  bool GetColorAt(int index,
                  honey_menu_color_type_t color_type,
                  honey_color_t& color) override;
  bool SetFontList(int command_id, const HoneycombString& font_list) override;
  bool SetFontListAt(int index, const HoneycombString& font_list) override;

  // Callbacks from the ui::MenuModel implementation.
  void ActivatedAt(size_t index, honey_event_flags_t event_flags);
  void MouseOutsideMenu(const gfx::Point& screen_point);
  void UnhandledOpenSubmenu(bool is_rtl);
  void UnhandledCloseSubmenu(bool is_rtl);
  bool GetTextColor(size_t index,
                    bool is_accelerator,
                    bool is_hovered,
                    SkColor* override_color) const;
  bool GetBackgroundColor(size_t index,
                          bool is_hovered,
                          SkColor* override_color) const;
  void MenuWillShow();
  void MenuWillClose();
  std::u16string GetFormattedLabelAt(size_t index);
  const gfx::FontList* GetLabelFontListAt(size_t index) const;

  // Verify that only a single reference exists to all HoneycombMenuModelImpl objects.
  bool VerifyRefCount();

  // Helper for adding custom menu items originating from the renderer process.
  void AddMenuItem(const blink::mojom::CustomContextMenuItem& menu_item);

  ui::MenuModel* model() const { return model_.get(); }

  // Used when created via HoneycombMenuManager.
  Delegate* delegate() const { return delegate_; }
  void set_delegate(Delegate* delegate) { delegate_ = delegate; }

  // Used for menus run via HoneycombWindowImpl::ShowMenu to provide more accurate
  // menu close notification.
  void set_auto_notify_menu_closed(bool val) { auto_notify_menu_closed_ = val; }
  void NotifyMenuClosed();

 private:
  struct Item;

  using ItemVector = std::vector<Item>;

  // Functions for inserting items into |items_|.
  void AppendItem(const Item& item);
  void InsertItemAt(const Item& item, size_t index);
  void ValidateItem(const Item& item);

  // Notify the delegate asynchronously.
  void OnMouseOutsideMenu(const gfx::Point& screen_point);
  void OnUnhandledOpenSubmenu(bool is_rtl);
  void OnUnhandledCloseSubmenu(bool is_rtl);
  void OnMenuClosed();

  // Verify that the object is being accessed from the correct thread.
  bool VerifyContext();

  base::PlatformThreadId supported_thread_id_;

  // Used when created via HoneycombMenuManager.
  Delegate* delegate_;

  // Used when created via HoneycombMenuModel::CreateMenuModel().
  HoneycombRefPtr<HoneycombMenuModelDelegate> menu_model_delegate_;

  const bool is_submenu_;

  ItemVector items_;
  std::unique_ptr<ui::MenuModel> model_;

  // Style information.
  honey_color_t default_colors_[HONEYCOMB_MENU_COLOR_COUNT] = {0};
  gfx::FontList default_font_list_;
  bool has_default_font_list_ = false;

  bool auto_notify_menu_closed_ = true;

  IMPLEMENT_REFCOUNTING(HoneycombMenuModelImpl);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_MENU_MODEL_IMPL_H_
