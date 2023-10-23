// Copyright (c) 2012 Marshall A. Greenblatt. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the name Chromium Embedded
// Framework nor the names of its contributors may be used to endorse
// or promote products derived from this software without specific prior
// written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ---------------------------------------------------------------------------
//
// The contents of this file must follow a specific format in order to
// support the Honeycomb translator tool. See the translator.README.txt file in the
// tools directory for more information.
//

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_MENU_MODEL_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_MENU_MODEL_H_
#pragma once

#include "include/honey_base.h"
#include "include/honey_menu_model_delegate.h"

///
/// Supports creation and modification of menus. See honey_menu_id_t for the
/// command ids that have default implementations. All user-defined command ids
/// should be between MENU_ID_USER_FIRST and MENU_ID_USER_LAST. The methods of
/// this class can only be accessed on the browser process the UI thread.
///
/*--honey(source=library)--*/
class HoneycombMenuModel : public virtual HoneycombBaseRefCounted {
 public:
  typedef honey_menu_item_type_t MenuItemType;

  ///
  /// Create a new MenuModel with the specified |delegate|.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombMenuModel> CreateMenuModel(
      HoneycombRefPtr<HoneycombMenuModelDelegate> delegate);

  ///
  /// Returns true if this menu is a submenu.
  ///
  /*--honey()--*/
  virtual bool IsSubMenu() = 0;

  ///
  /// Clears the menu. Returns true on success.
  ///
  /*--honey()--*/
  virtual bool Clear() = 0;

  ///
  /// Returns the number of items in this menu.
  ///
  /*--honey()--*/
  virtual size_t GetCount() = 0;

  ///
  /// Add a separator to the menu. Returns true on success.
  ///
  /*--honey()--*/
  virtual bool AddSeparator() = 0;

  ///
  /// Add an item to the menu. Returns true on success.
  ///
  /*--honey()--*/
  virtual bool AddItem(int command_id, const HoneycombString& label) = 0;

  ///
  /// Add a check item to the menu. Returns true on success.
  ///
  /*--honey()--*/
  virtual bool AddCheckItem(int command_id, const HoneycombString& label) = 0;
  ///
  /// Add a radio item to the menu. Only a single item with the specified
  /// |group_id| can be checked at a time. Returns true on success.
  ///
  /*--honey()--*/
  virtual bool AddRadioItem(int command_id,
                            const HoneycombString& label,
                            int group_id) = 0;

  ///
  /// Add a sub-menu to the menu. The new sub-menu is returned.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombMenuModel> AddSubMenu(int command_id,
                                             const HoneycombString& label) = 0;

  ///
  /// Insert a separator in the menu at the specified |index|. Returns true on
  /// success.
  ///
  /*--honey()--*/
  virtual bool InsertSeparatorAt(size_t index) = 0;

  ///
  /// Insert an item in the menu at the specified |index|. Returns true on
  /// success.
  ///
  /*--honey()--*/
  virtual bool InsertItemAt(size_t index,
                            int command_id,
                            const HoneycombString& label) = 0;

  ///
  /// Insert a check item in the menu at the specified |index|. Returns true on
  /// success.
  ///
  /*--honey()--*/
  virtual bool InsertCheckItemAt(size_t index,
                                 int command_id,
                                 const HoneycombString& label) = 0;

  ///
  /// Insert a radio item in the menu at the specified |index|. Only a single
  /// item with the specified |group_id| can be checked at a time. Returns true
  /// on success.
  ///
  /*--honey()--*/
  virtual bool InsertRadioItemAt(size_t index,
                                 int command_id,
                                 const HoneycombString& label,
                                 int group_id) = 0;

  ///
  /// Insert a sub-menu in the menu at the specified |index|. The new sub-menu
  /// is returned.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombMenuModel> InsertSubMenuAt(size_t index,
                                                  int command_id,
                                                  const HoneycombString& label) = 0;

  ///
  /// Removes the item with the specified |command_id|. Returns true on success.
  ///
  /*--honey()--*/
  virtual bool Remove(int command_id) = 0;

  ///
  /// Removes the item at the specified |index|. Returns true on success.
  ///
  /*--honey()--*/
  virtual bool RemoveAt(size_t index) = 0;

  ///
  /// Returns the index associated with the specified |command_id| or -1 if not
  /// found due to the command id not existing in the menu.
  ///
  /*--honey()--*/
  virtual int GetIndexOf(int command_id) = 0;

  ///
  /// Returns the command id at the specified |index| or -1 if not found due to
  /// invalid range or the index being a separator.
  ///
  /*--honey()--*/
  virtual int GetCommandIdAt(size_t index) = 0;

  ///
  /// Sets the command id at the specified |index|. Returns true on success.
  ///
  /*--honey()--*/
  virtual bool SetCommandIdAt(size_t index, int command_id) = 0;

  ///
  /// Returns the label for the specified |command_id| or empty if not found.
  ///
  /*--honey()--*/
  virtual HoneycombString GetLabel(int command_id) = 0;

  ///
  /// Returns the label at the specified |index| or empty if not found due to
  /// invalid range or the index being a separator.
  ///
  /*--honey()--*/
  virtual HoneycombString GetLabelAt(size_t index) = 0;

  ///
  /// Sets the label for the specified |command_id|. Returns true on success.
  ///
  /*--honey()--*/
  virtual bool SetLabel(int command_id, const HoneycombString& label) = 0;

  ///
  /// Set the label at the specified |index|. Returns true on success.
  ///
  /*--honey()--*/
  virtual bool SetLabelAt(size_t index, const HoneycombString& label) = 0;

  ///
  /// Returns the item type for the specified |command_id|.
  ///
  /*--honey(default_retval=MENUITEMTYPE_NONE)--*/
  virtual MenuItemType GetType(int command_id) = 0;

  ///
  /// Returns the item type at the specified |index|.
  ///
  /*--honey(default_retval=MENUITEMTYPE_NONE)--*/
  virtual MenuItemType GetTypeAt(size_t index) = 0;

  ///
  /// Returns the group id for the specified |command_id| or -1 if invalid.
  ///
  /*--honey()--*/
  virtual int GetGroupId(int command_id) = 0;

  ///
  /// Returns the group id at the specified |index| or -1 if invalid.
  ///
  /*--honey()--*/
  virtual int GetGroupIdAt(size_t index) = 0;

  ///
  /// Sets the group id for the specified |command_id|. Returns true on success.
  ///
  /*--honey()--*/
  virtual bool SetGroupId(int command_id, int group_id) = 0;

  ///
  /// Sets the group id at the specified |index|. Returns true on success.
  ///
  /*--honey()--*/
  virtual bool SetGroupIdAt(size_t index, int group_id) = 0;

  ///
  /// Returns the submenu for the specified |command_id| or empty if invalid.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombMenuModel> GetSubMenu(int command_id) = 0;

  ///
  /// Returns the submenu at the specified |index| or empty if invalid.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombMenuModel> GetSubMenuAt(size_t index) = 0;

  ///
  /// Returns true if the specified |command_id| is visible.
  ///
  /*--honey()--*/
  virtual bool IsVisible(int command_id) = 0;

  ///
  /// Returns true if the specified |index| is visible.
  ///
  /*--honey()--*/
  virtual bool IsVisibleAt(size_t index) = 0;

  ///
  /// Change the visibility of the specified |command_id|. Returns true on
  /// success.
  ///
  /*--honey()--*/
  virtual bool SetVisible(int command_id, bool visible) = 0;

  ///
  /// Change the visibility at the specified |index|. Returns true on success.
  ///
  /*--honey()--*/
  virtual bool SetVisibleAt(size_t index, bool visible) = 0;

  ///
  /// Returns true if the specified |command_id| is enabled.
  ///
  /*--honey()--*/
  virtual bool IsEnabled(int command_id) = 0;

  ///
  /// Returns true if the specified |index| is enabled.
  ///
  /*--honey()--*/
  virtual bool IsEnabledAt(size_t index) = 0;

  ///
  /// Change the enabled status of the specified |command_id|. Returns true on
  /// success.
  ///
  /*--honey()--*/
  virtual bool SetEnabled(int command_id, bool enabled) = 0;

  ///
  /// Change the enabled status at the specified |index|. Returns true on
  /// success.
  ///
  /*--honey()--*/
  virtual bool SetEnabledAt(size_t index, bool enabled) = 0;

  ///
  /// Returns true if the specified |command_id| is checked. Only applies to
  /// check and radio items.
  ///
  /*--honey()--*/
  virtual bool IsChecked(int command_id) = 0;

  ///
  /// Returns true if the specified |index| is checked. Only applies to check
  /// and radio items.
  ///
  /*--honey()--*/
  virtual bool IsCheckedAt(size_t index) = 0;

  ///
  /// Check the specified |command_id|. Only applies to check and radio items.
  /// Returns true on success.
  ///
  /*--honey()--*/
  virtual bool SetChecked(int command_id, bool checked) = 0;

  ///
  /// Check the specified |index|. Only applies to check and radio items.
  /// Returns true on success.
  ///
  /*--honey()--*/
  virtual bool SetCheckedAt(size_t index, bool checked) = 0;

  ///
  /// Returns true if the specified |command_id| has a keyboard accelerator
  /// assigned.
  ///
  /*--honey()--*/
  virtual bool HasAccelerator(int command_id) = 0;

  ///
  /// Returns true if the specified |index| has a keyboard accelerator assigned.
  ///
  /*--honey()--*/
  virtual bool HasAcceleratorAt(size_t index) = 0;

  ///
  /// Set the keyboard accelerator for the specified |command_id|. |key_code|
  /// can be any virtual key or character value. Returns true on success.
  ///
  /*--honey()--*/
  virtual bool SetAccelerator(int command_id,
                              int key_code,
                              bool shift_pressed,
                              bool ctrl_pressed,
                              bool alt_pressed) = 0;

  ///
  /// Set the keyboard accelerator at the specified |index|. |key_code| can be
  /// any virtual key or character value. Returns true on success.
  ///
  /*--honey()--*/
  virtual bool SetAcceleratorAt(size_t index,
                                int key_code,
                                bool shift_pressed,
                                bool ctrl_pressed,
                                bool alt_pressed) = 0;

  ///
  /// Remove the keyboard accelerator for the specified |command_id|. Returns
  /// true on success.
  ///
  /*--honey()--*/
  virtual bool RemoveAccelerator(int command_id) = 0;

  ///
  /// Remove the keyboard accelerator at the specified |index|. Returns true on
  /// success.
  ///
  /*--honey()--*/
  virtual bool RemoveAcceleratorAt(size_t index) = 0;

  ///
  /// Retrieves the keyboard accelerator for the specified |command_id|. Returns
  /// true on success.
  ///
  /*--honey()--*/
  virtual bool GetAccelerator(int command_id,
                              int& key_code,
                              bool& shift_pressed,
                              bool& ctrl_pressed,
                              bool& alt_pressed) = 0;

  ///
  /// Retrieves the keyboard accelerator for the specified |index|. Returns true
  /// on success.
  ///
  /*--honey()--*/
  virtual bool GetAcceleratorAt(size_t index,
                                int& key_code,
                                bool& shift_pressed,
                                bool& ctrl_pressed,
                                bool& alt_pressed) = 0;

  ///
  /// Set the explicit color for |command_id| and |color_type| to |color|.
  /// Specify a |color| value of 0 to remove the explicit color. If no explicit
  /// color or default color is set for |color_type| then the system color will
  /// be used. Returns true on success.
  ///
  /*--honey()--*/
  virtual bool SetColor(int command_id,
                        honey_menu_color_type_t color_type,
                        honey_color_t color) = 0;

  ///
  /// Set the explicit color for |command_id| and |index| to |color|. Specify a
  /// |color| value of 0 to remove the explicit color. Specify an |index| value
  /// of -1 to set the default color for items that do not have an explicit
  /// color set. If no explicit color or default color is set for |color_type|
  /// then the system color will be used. Returns true on success.
  ///
  /*--honey()--*/
  virtual bool SetColorAt(int index,
                          honey_menu_color_type_t color_type,
                          honey_color_t color) = 0;

  ///
  /// Returns in |color| the color that was explicitly set for |command_id| and
  /// |color_type|. If a color was not set then 0 will be returned in |color|.
  /// Returns true on success.
  ///
  /*--honey()--*/
  virtual bool GetColor(int command_id,
                        honey_menu_color_type_t color_type,
                        honey_color_t& color) = 0;

  ///
  /// Returns in |color| the color that was explicitly set for |command_id| and
  /// |color_type|. Specify an |index| value of -1 to return the default color
  /// in |color|. If a color was not set then 0 will be returned in |color|.
  /// Returns true on success.
  ///
  /*--honey()--*/
  virtual bool GetColorAt(int index,
                          honey_menu_color_type_t color_type,
                          honey_color_t& color) = 0;

  ///
  /// Sets the font list for the specified |command_id|. If |font_list| is empty
  /// the system font will be used. Returns true on success. The format is
  /// "<FONT_FAMILY_LIST>,[STYLES] <SIZE>", where:
  /// - FONT_FAMILY_LIST is a comma-separated list of font family names,
  /// - STYLES is an optional space-separated list of style names
  ///   (case-sensitive "Bold" and "Italic" are supported), and
  /// - SIZE is an integer font size in pixels with the suffix "px".
  ///
  /// Here are examples of valid font description strings:
  /// - "Arial, Helvetica, Bold Italic 14px"
  /// - "Arial, 14px"
  ///
  /*--honey(optional_param=font_list)--*/
  virtual bool SetFontList(int command_id, const HoneycombString& font_list) = 0;

  ///
  /// Sets the font list for the specified |index|. Specify an |index| value of
  /// -1 to set the default font. If |font_list| is empty the system font will
  /// be used. Returns true on success. The format is
  /// "<FONT_FAMILY_LIST>,[STYLES] <SIZE>", where:
  /// - FONT_FAMILY_LIST is a comma-separated list of font family names,
  /// - STYLES is an optional space-separated list of style names
  ///   (case-sensitive "Bold" and "Italic" are supported), and
  /// - SIZE is an integer font size in pixels with the suffix "px".
  ///
  /// Here are examples of valid font description strings:
  /// - "Arial, Helvetica, Bold Italic 14px"
  /// - "Arial, 14px"
  ///
  /*--honey(optional_param=font_list)--*/
  virtual bool SetFontListAt(int index, const HoneycombString& font_list) = 0;
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_MENU_MODEL_H_
