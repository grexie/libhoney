// Copyright (c) 2023 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the Honeycomb translator tool. If making changes by
// hand only do so within the body of existing method and function
// implementations. See the translator.README.txt file in the tools directory
// for more information.
//
// $hash=1b1c5e3e3cca0da704d30c6577a0c083c3fb389b$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CTOCPP_MENU_MODEL_CTOCPP_H_
#define HONEYCOMB_LIBHONEY_DLL_CTOCPP_MENU_MODEL_CTOCPP_H_
#pragma once

#if !defined(WRAPPING_HONEYCOMB_SHARED)
#error This file can be included wrapper-side only
#endif

#include "include/capi/honey_menu_model_capi.h"
#include "include/honey_menu_model.h"
#include "libhoney_dll/ctocpp/ctocpp_ref_counted.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class HoneycombMenuModelCToCpp : public HoneycombCToCppRefCounted<HoneycombMenuModelCToCpp,
                                                      HoneycombMenuModel,
                                                      honey_menu_model_t> {
 public:
  HoneycombMenuModelCToCpp();
  virtual ~HoneycombMenuModelCToCpp();

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
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CTOCPP_MENU_MODEL_CTOCPP_H_
