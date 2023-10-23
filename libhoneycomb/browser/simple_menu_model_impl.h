// Copyright (c) 2021 The Honeycomb Authors.
// Portions copyright (c) 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_SIMPLE_MENU_MODEL_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_SIMPLE_MENU_MODEL_IMPL_H_
#pragma once

#include <vector>

#include "include/honey_menu_model.h"

#include "base/threading/platform_thread.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
#include "ui/base/models/simple_menu_model.h"

// Implementation of HoneycombMenuModel that wraps an existing ui::SimpleMenuModel.
class HoneycombSimpleMenuModelImpl : public HoneycombMenuModel {
 public:
  // Interface for setting state using HoneycombMenuModel methods that will later be
  // retrieved via the ui::SimpleMenuModel::Delegate implementation.
  class StateDelegate {
   public:
    virtual void SetChecked(int command_id, bool checked) = 0;
    virtual void SetAccelerator(int command_id,
                                absl::optional<ui::Accelerator> accel) = 0;

   protected:
    virtual ~StateDelegate() {}
  };

  // |delegate| should be the same that was used to create |model|.
  // If |is_owned| is true then |model| will be deleted on Detach().
  HoneycombSimpleMenuModelImpl(ui::SimpleMenuModel* model,
                         ui::SimpleMenuModel::Delegate* delegate,
                         StateDelegate* state_delegate,
                         bool is_owned,
                         bool is_submenu);

  HoneycombSimpleMenuModelImpl(const HoneycombSimpleMenuModelImpl&) = delete;
  HoneycombSimpleMenuModelImpl& operator=(const HoneycombSimpleMenuModelImpl&) = delete;

  ~HoneycombSimpleMenuModelImpl() override;

  // Must be called before the object is deleted.
  void Detach();

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

  ui::SimpleMenuModel* model() const { return model_; }

 private:
  // Verify that the object is attached and being accessed from the correct
  // thread.
  bool VerifyContext();

  // Returns true if |index| is valid.
  bool ValidIndex(size_t index);

  HoneycombRefPtr<HoneycombSimpleMenuModelImpl> CreateNewSubMenu(
      ui::SimpleMenuModel* model);

  base::PlatformThreadId supported_thread_id_;

  ui::SimpleMenuModel* model_;
  ui::SimpleMenuModel::Delegate* const delegate_;
  StateDelegate* const state_delegate_;
  const bool is_owned_;
  const bool is_submenu_;

  // Keep the submenus alive until they're removed, or we're destroyed.
  using SubMenuMap =
      std::map<ui::SimpleMenuModel*, HoneycombRefPtr<HoneycombSimpleMenuModelImpl>>;
  SubMenuMap submenumap_;

  IMPLEMENT_REFCOUNTING(HoneycombSimpleMenuModelImpl);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_SIMPLE_MENU_MODEL_IMPL_H_
