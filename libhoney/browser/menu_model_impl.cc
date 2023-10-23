// Copyright (c) 2012 The Honeycomb Authors.
// Portions copyright (c) 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoney/browser/menu_model_impl.h"

#include <vector>

#include "libhoney/browser/thread_util.h"
#include "libhoney/common/task_runner_impl.h"

#include "base/functional/bind.h"
#include "base/logging.h"
#include "third_party/blink/public/mojom/context_menu/context_menu.mojom.h"
#include "ui/base/accelerators/accelerator.h"
#include "ui/base/models/image_model.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/image/image.h"

namespace {

const int kSeparatorId = -1;
const int kInvalidGroupId = -1;
const int kInvalidCommandId = -1;
const int kDefaultIndex = -1;
const int kInvalidIndex = -2;

// A simple MenuModel implementation that delegates to HoneycombMenuModelImpl.
class HoneycombSimpleMenuModel : public ui::MenuModel {
 public:
  // The Delegate can be NULL, though if it is items can't be checked or
  // disabled.
  explicit HoneycombSimpleMenuModel(HoneycombMenuModelImpl* impl) : impl_(impl) {}

  HoneycombSimpleMenuModel(const HoneycombSimpleMenuModel&) = delete;
  HoneycombSimpleMenuModel& operator=(const HoneycombSimpleMenuModel&) = delete;

  // MenuModel methods.
  size_t GetItemCount() const override { return impl_->GetCount(); }

  ItemType GetTypeAt(size_t index) const override {
    switch (impl_->GetTypeAt(index)) {
      case MENUITEMTYPE_COMMAND:
        return TYPE_COMMAND;
      case MENUITEMTYPE_CHECK:
        return TYPE_CHECK;
      case MENUITEMTYPE_RADIO:
        return TYPE_RADIO;
      case MENUITEMTYPE_SEPARATOR:
        return TYPE_SEPARATOR;
      case MENUITEMTYPE_SUBMENU:
        return TYPE_SUBMENU;
      default:
        DCHECK(false);
        return TYPE_COMMAND;
    }
  }

  ui::MenuSeparatorType GetSeparatorTypeAt(size_t index) const override {
    return ui::NORMAL_SEPARATOR;
  }

  int GetCommandIdAt(size_t index) const override {
    return impl_->GetCommandIdAt(index);
  }

  std::u16string GetLabelAt(size_t index) const override {
    return impl_->GetFormattedLabelAt(index);
  }

  bool IsItemDynamicAt(size_t index) const override { return false; }

  const gfx::FontList* GetLabelFontListAt(size_t index) const override {
    return impl_->GetLabelFontListAt(index);
  }

  bool GetAcceleratorAt(size_t index,
                        ui::Accelerator* accelerator) const override {
    int key_code = 0;
    bool shift_pressed = false;
    bool ctrl_pressed = false;
    bool alt_pressed = false;
    if (impl_->GetAcceleratorAt(index, key_code, shift_pressed, ctrl_pressed,
                                alt_pressed)) {
      int modifiers = 0;
      if (shift_pressed) {
        modifiers |= ui::EF_SHIFT_DOWN;
      }
      if (ctrl_pressed) {
        modifiers |= ui::EF_CONTROL_DOWN;
      }
      if (alt_pressed) {
        modifiers |= ui::EF_ALT_DOWN;
      }

      *accelerator =
          ui::Accelerator(static_cast<ui::KeyboardCode>(key_code), modifiers);
      return true;
    }
    return false;
  }

  bool IsItemCheckedAt(size_t index) const override {
    return impl_->IsCheckedAt(index);
  }

  int GetGroupIdAt(size_t index) const override {
    return impl_->GetGroupIdAt(index);
  }

  ui::ImageModel GetIconAt(size_t index) const override {
    return ui::ImageModel();
  }

  ui::ButtonMenuItemModel* GetButtonMenuItemAt(size_t index) const override {
    return nullptr;
  }

  bool IsEnabledAt(size_t index) const override {
    return impl_->IsEnabledAt(index);
  }

  bool IsVisibleAt(size_t index) const override {
    return impl_->IsVisibleAt(index);
  }

  void ActivatedAt(size_t index) override { ActivatedAt(index, 0); }

  void ActivatedAt(size_t index, int event_flags) override {
    impl_->ActivatedAt(index, static_cast<honey_event_flags_t>(event_flags));
  }

  MenuModel* GetSubmenuModelAt(size_t index) const override {
    HoneycombRefPtr<HoneycombMenuModel> submenu = impl_->GetSubMenuAt(index);
    if (submenu.get()) {
      return static_cast<HoneycombMenuModelImpl*>(submenu.get())->model();
    }
    return nullptr;
  }

  void MouseOutsideMenu(const gfx::Point& screen_point) override {
    impl_->MouseOutsideMenu(screen_point);
  }

  void UnhandledOpenSubmenu(bool is_rtl) override {
    impl_->UnhandledOpenSubmenu(is_rtl);
  }

  void UnhandledCloseSubmenu(bool is_rtl) override {
    impl_->UnhandledCloseSubmenu(is_rtl);
  }

  bool GetTextColor(size_t index,
                    bool is_minor,
                    bool is_hovered,
                    SkColor* override_color) const override {
    return impl_->GetTextColor(index, is_minor, is_hovered, override_color);
  }

  bool GetBackgroundColor(size_t index,
                          bool is_hovered,
                          SkColor* override_color) const override {
    return impl_->GetBackgroundColor(index, is_hovered, override_color);
  }

  void MenuWillShow() override { impl_->MenuWillShow(); }

  void MenuWillClose() override { impl_->MenuWillClose(); }

 private:
  HoneycombMenuModelImpl* impl_;
};

honey_menu_color_type_t GetMenuColorType(bool is_text,
                                       bool is_accelerator,
                                       bool is_hovered) {
  if (is_text) {
    if (is_accelerator) {
      return is_hovered ? HONEYCOMB_MENU_COLOR_TEXT_ACCELERATOR_HOVERED
                        : HONEYCOMB_MENU_COLOR_TEXT_ACCELERATOR;
    }
    return is_hovered ? HONEYCOMB_MENU_COLOR_TEXT_HOVERED : HONEYCOMB_MENU_COLOR_TEXT;
  }

  DCHECK(!is_accelerator);
  return is_hovered ? HONEYCOMB_MENU_COLOR_BACKGROUND_HOVERED
                    : HONEYCOMB_MENU_COLOR_BACKGROUND;
}

}  // namespace

// static
HoneycombRefPtr<HoneycombMenuModel> HoneycombMenuModel::CreateMenuModel(
    HoneycombRefPtr<HoneycombMenuModelDelegate> delegate) {
  HONEYCOMB_REQUIRE_UIT_RETURN(nullptr);
  DCHECK(delegate);
  if (!delegate) {
    return nullptr;
  }

  HoneycombRefPtr<HoneycombMenuModelImpl> menu_model =
      new HoneycombMenuModelImpl(nullptr, delegate, false);
  return menu_model;
}

struct HoneycombMenuModelImpl::Item {
  Item(honey_menu_item_type_t type,
       int command_id,
       const HoneycombString& label,
       int group_id)
      : type_(type),
        command_id_(command_id),
        label_(label),
        group_id_(group_id) {}

  // Basic information.
  honey_menu_item_type_t type_;
  int command_id_;
  HoneycombString label_;
  int group_id_;
  HoneycombRefPtr<HoneycombMenuModelImpl> submenu_;

  // State information.
  bool enabled_ = true;
  bool visible_ = true;
  bool checked_ = false;

  // Accelerator information.
  bool has_accelerator_ = false;
  int key_code_ = 0;
  bool shift_pressed_ = false;
  bool ctrl_pressed_ = false;
  bool alt_pressed_ = false;

  honey_color_t colors_[HONEYCOMB_MENU_COLOR_COUNT] = {0};
  gfx::FontList font_list_;
  bool has_font_list_ = false;
};

HoneycombMenuModelImpl::HoneycombMenuModelImpl(
    Delegate* delegate,
    HoneycombRefPtr<HoneycombMenuModelDelegate> menu_model_delegate,
    bool is_submenu)
    : supported_thread_id_(base::PlatformThread::CurrentId()),
      delegate_(delegate),
      menu_model_delegate_(menu_model_delegate),
      is_submenu_(is_submenu) {
  DCHECK(delegate_ || menu_model_delegate_);
  model_.reset(new HoneycombSimpleMenuModel(this));
}

HoneycombMenuModelImpl::~HoneycombMenuModelImpl() {}

bool HoneycombMenuModelImpl::IsSubMenu() {
  if (!VerifyContext()) {
    return false;
  }
  return is_submenu_;
}

bool HoneycombMenuModelImpl::Clear() {
  if (!VerifyContext()) {
    return false;
  }

  items_.clear();
  return true;
}

size_t HoneycombMenuModelImpl::GetCount() {
  if (!VerifyContext()) {
    return 0;
  }

  return items_.size();
}

bool HoneycombMenuModelImpl::AddSeparator() {
  if (!VerifyContext()) {
    return false;
  }

  AppendItem(
      Item(MENUITEMTYPE_SEPARATOR, kSeparatorId, HoneycombString(), kInvalidGroupId));
  return true;
}

bool HoneycombMenuModelImpl::AddItem(int command_id, const HoneycombString& label) {
  if (!VerifyContext()) {
    return false;
  }

  AppendItem(Item(MENUITEMTYPE_COMMAND, command_id, label, kInvalidGroupId));
  return true;
}

bool HoneycombMenuModelImpl::AddCheckItem(int command_id, const HoneycombString& label) {
  if (!VerifyContext()) {
    return false;
  }

  AppendItem(Item(MENUITEMTYPE_CHECK, command_id, label, kInvalidGroupId));
  return true;
}

bool HoneycombMenuModelImpl::AddRadioItem(int command_id,
                                    const HoneycombString& label,
                                    int group_id) {
  if (!VerifyContext()) {
    return false;
  }

  AppendItem(Item(MENUITEMTYPE_RADIO, command_id, label, group_id));
  return true;
}

HoneycombRefPtr<HoneycombMenuModel> HoneycombMenuModelImpl::AddSubMenu(int command_id,
                                                     const HoneycombString& label) {
  if (!VerifyContext()) {
    return nullptr;
  }

  Item item(MENUITEMTYPE_SUBMENU, command_id, label, kInvalidGroupId);
  item.submenu_ = new HoneycombMenuModelImpl(delegate_, menu_model_delegate_, true);
  AppendItem(item);
  return item.submenu_.get();
}

bool HoneycombMenuModelImpl::InsertSeparatorAt(size_t index) {
  if (!VerifyContext()) {
    return false;
  }

  InsertItemAt(
      Item(MENUITEMTYPE_SEPARATOR, kSeparatorId, HoneycombString(), kInvalidGroupId),
      index);
  return true;
}

bool HoneycombMenuModelImpl::InsertItemAt(size_t index,
                                    int command_id,
                                    const HoneycombString& label) {
  if (!VerifyContext()) {
    return false;
  }

  InsertItemAt(Item(MENUITEMTYPE_COMMAND, command_id, label, kInvalidGroupId),
               index);
  return true;
}

bool HoneycombMenuModelImpl::InsertCheckItemAt(size_t index,
                                         int command_id,
                                         const HoneycombString& label) {
  if (!VerifyContext()) {
    return false;
  }

  InsertItemAt(Item(MENUITEMTYPE_CHECK, command_id, label, kInvalidGroupId),
               index);
  return true;
}

bool HoneycombMenuModelImpl::InsertRadioItemAt(size_t index,
                                         int command_id,
                                         const HoneycombString& label,
                                         int group_id) {
  if (!VerifyContext()) {
    return false;
  }

  InsertItemAt(Item(MENUITEMTYPE_RADIO, command_id, label, group_id), index);
  return true;
}

HoneycombRefPtr<HoneycombMenuModel> HoneycombMenuModelImpl::InsertSubMenuAt(
    size_t index,
    int command_id,
    const HoneycombString& label) {
  if (!VerifyContext()) {
    return nullptr;
  }

  Item item(MENUITEMTYPE_SUBMENU, command_id, label, kInvalidGroupId);
  item.submenu_ = new HoneycombMenuModelImpl(delegate_, menu_model_delegate_, true);
  InsertItemAt(item, index);
  return item.submenu_.get();
}

bool HoneycombMenuModelImpl::Remove(int command_id) {
  return RemoveAt(GetIndexOf(command_id));
}

bool HoneycombMenuModelImpl::RemoveAt(size_t index) {
  if (!VerifyContext()) {
    return false;
  }

  if (index < items_.size()) {
    items_.erase(items_.begin() + index);
    return true;
  }
  return false;
}

int HoneycombMenuModelImpl::GetIndexOf(int command_id) {
  if (!VerifyContext()) {
    return kInvalidIndex;
  }

  for (ItemVector::iterator i = items_.begin(); i != items_.end(); ++i) {
    if ((*i).command_id_ == command_id) {
      return static_cast<int>(std::distance(items_.begin(), i));
    }
  }
  return kInvalidIndex;
}

int HoneycombMenuModelImpl::GetCommandIdAt(size_t index) {
  if (!VerifyContext()) {
    return kInvalidCommandId;
  }

  if (index < items_.size()) {
    return items_[index].command_id_;
  }
  return kInvalidCommandId;
}

bool HoneycombMenuModelImpl::SetCommandIdAt(size_t index, int command_id) {
  if (!VerifyContext()) {
    return false;
  }

  if (index < items_.size()) {
    items_[index].command_id_ = command_id;
    return true;
  }
  return false;
}

HoneycombString HoneycombMenuModelImpl::GetLabel(int command_id) {
  return GetLabelAt(GetIndexOf(command_id));
}

HoneycombString HoneycombMenuModelImpl::GetLabelAt(size_t index) {
  if (!VerifyContext()) {
    return HoneycombString();
  }

  if (index < items_.size()) {
    return items_[index].label_;
  }
  return HoneycombString();
}

bool HoneycombMenuModelImpl::SetLabel(int command_id, const HoneycombString& label) {
  return SetLabelAt(GetIndexOf(command_id), label);
}

bool HoneycombMenuModelImpl::SetLabelAt(size_t index, const HoneycombString& label) {
  if (!VerifyContext()) {
    return false;
  }

  if (index < items_.size()) {
    items_[index].label_ = label;
    return true;
  }
  return false;
}

HoneycombMenuModelImpl::MenuItemType HoneycombMenuModelImpl::GetType(int command_id) {
  return GetTypeAt(GetIndexOf(command_id));
}

HoneycombMenuModelImpl::MenuItemType HoneycombMenuModelImpl::GetTypeAt(size_t index) {
  if (!VerifyContext()) {
    return MENUITEMTYPE_NONE;
  }

  if (index < items_.size()) {
    return items_[index].type_;
  }
  return MENUITEMTYPE_NONE;
}

int HoneycombMenuModelImpl::GetGroupId(int command_id) {
  return GetGroupIdAt(GetIndexOf(command_id));
}

int HoneycombMenuModelImpl::GetGroupIdAt(size_t index) {
  if (!VerifyContext()) {
    return kInvalidGroupId;
  }

  if (index < items_.size()) {
    return items_[index].group_id_;
  }
  return kInvalidGroupId;
}

bool HoneycombMenuModelImpl::SetGroupId(int command_id, int group_id) {
  return SetGroupIdAt(GetIndexOf(command_id), group_id);
}

bool HoneycombMenuModelImpl::SetGroupIdAt(size_t index, int group_id) {
  if (!VerifyContext()) {
    return false;
  }

  if (index < items_.size()) {
    items_[index].group_id_ = group_id;
    return true;
  }
  return false;
}

HoneycombRefPtr<HoneycombMenuModel> HoneycombMenuModelImpl::GetSubMenu(int command_id) {
  return GetSubMenuAt(GetIndexOf(command_id));
}

HoneycombRefPtr<HoneycombMenuModel> HoneycombMenuModelImpl::GetSubMenuAt(size_t index) {
  if (!VerifyContext()) {
    return nullptr;
  }

  if (index < items_.size()) {
    return items_[index].submenu_.get();
  }
  return nullptr;
}

bool HoneycombMenuModelImpl::IsVisible(int command_id) {
  return IsVisibleAt(GetIndexOf(command_id));
}

bool HoneycombMenuModelImpl::IsVisibleAt(size_t index) {
  if (!VerifyContext()) {
    return false;
  }

  if (index < items_.size()) {
    return items_[index].visible_;
  }
  return false;
}

bool HoneycombMenuModelImpl::SetVisible(int command_id, bool visible) {
  return SetVisibleAt(GetIndexOf(command_id), visible);
}

bool HoneycombMenuModelImpl::SetVisibleAt(size_t index, bool visible) {
  if (!VerifyContext()) {
    return false;
  }

  if (index < items_.size()) {
    items_[index].visible_ = visible;
    return true;
  }
  return false;
}

bool HoneycombMenuModelImpl::IsEnabled(int command_id) {
  return IsEnabledAt(GetIndexOf(command_id));
}

bool HoneycombMenuModelImpl::IsEnabledAt(size_t index) {
  if (!VerifyContext()) {
    return false;
  }

  if (index < items_.size()) {
    return items_[index].enabled_;
  }
  return false;
}

bool HoneycombMenuModelImpl::SetEnabled(int command_id, bool enabled) {
  return SetEnabledAt(GetIndexOf(command_id), enabled);
}

bool HoneycombMenuModelImpl::SetEnabledAt(size_t index, bool enabled) {
  if (!VerifyContext()) {
    return false;
  }

  if (index < items_.size()) {
    items_[index].enabled_ = enabled;
    return true;
  }
  return false;
}

bool HoneycombMenuModelImpl::IsChecked(int command_id) {
  return IsCheckedAt(GetIndexOf(command_id));
}

bool HoneycombMenuModelImpl::IsCheckedAt(size_t index) {
  if (!VerifyContext()) {
    return false;
  }

  if (index < items_.size()) {
    return items_[index].checked_;
  }
  return false;
}

bool HoneycombMenuModelImpl::SetChecked(int command_id, bool checked) {
  return SetCheckedAt(GetIndexOf(command_id), checked);
}

bool HoneycombMenuModelImpl::SetCheckedAt(size_t index, bool checked) {
  if (!VerifyContext()) {
    return false;
  }

  if (index < items_.size()) {
    items_[index].checked_ = checked;
    return true;
  }
  return false;
}

bool HoneycombMenuModelImpl::HasAccelerator(int command_id) {
  return HasAcceleratorAt(GetIndexOf(command_id));
}

bool HoneycombMenuModelImpl::HasAcceleratorAt(size_t index) {
  if (!VerifyContext()) {
    return false;
  }

  if (index < items_.size()) {
    return items_[index].has_accelerator_;
  }
  return false;
}

bool HoneycombMenuModelImpl::SetAccelerator(int command_id,
                                      int key_code,
                                      bool shift_pressed,
                                      bool ctrl_pressed,
                                      bool alt_pressed) {
  return SetAcceleratorAt(GetIndexOf(command_id), key_code, shift_pressed,
                          ctrl_pressed, alt_pressed);
}

bool HoneycombMenuModelImpl::SetAcceleratorAt(size_t index,
                                        int key_code,
                                        bool shift_pressed,
                                        bool ctrl_pressed,
                                        bool alt_pressed) {
  if (!VerifyContext()) {
    return false;
  }

  if (index < items_.size()) {
    Item& item = items_[index];
    item.has_accelerator_ = true;
    item.key_code_ = key_code;
    item.shift_pressed_ = shift_pressed;
    item.ctrl_pressed_ = ctrl_pressed;
    item.alt_pressed_ = alt_pressed;
    return true;
  }
  return false;
}

bool HoneycombMenuModelImpl::RemoveAccelerator(int command_id) {
  return RemoveAcceleratorAt(GetIndexOf(command_id));
}

bool HoneycombMenuModelImpl::RemoveAcceleratorAt(size_t index) {
  if (!VerifyContext()) {
    return false;
  }

  if (index < items_.size()) {
    Item& item = items_[index];
    if (item.has_accelerator_) {
      item.has_accelerator_ = false;
      item.key_code_ = 0;
      item.shift_pressed_ = false;
      item.ctrl_pressed_ = false;
      item.alt_pressed_ = false;
    }
    return true;
  }
  return false;
}

bool HoneycombMenuModelImpl::GetAccelerator(int command_id,
                                      int& key_code,
                                      bool& shift_pressed,
                                      bool& ctrl_pressed,
                                      bool& alt_pressed) {
  return GetAcceleratorAt(GetIndexOf(command_id), key_code, shift_pressed,
                          ctrl_pressed, alt_pressed);
}

bool HoneycombMenuModelImpl::GetAcceleratorAt(size_t index,
                                        int& key_code,
                                        bool& shift_pressed,
                                        bool& ctrl_pressed,
                                        bool& alt_pressed) {
  if (!VerifyContext()) {
    return false;
  }

  if (index < items_.size()) {
    const Item& item = items_[index];
    if (item.has_accelerator_) {
      key_code = item.key_code_;
      shift_pressed = item.shift_pressed_;
      ctrl_pressed = item.ctrl_pressed_;
      alt_pressed = item.alt_pressed_;
      return true;
    }
  }
  return false;
}

bool HoneycombMenuModelImpl::SetColor(int command_id,
                                honey_menu_color_type_t color_type,
                                honey_color_t color) {
  return SetColorAt(GetIndexOf(command_id), color_type, color);
}

bool HoneycombMenuModelImpl::SetColorAt(int index,
                                  honey_menu_color_type_t color_type,
                                  honey_color_t color) {
  if (!VerifyContext()) {
    return false;
  }

  if (color_type < 0 || color_type >= HONEYCOMB_MENU_COLOR_COUNT) {
    return false;
  }

  if (index == kDefaultIndex) {
    default_colors_[color_type] = color;
    return true;
  }

  if (index >= 0 && index < static_cast<int>(items_.size())) {
    Item& item = items_[index];
    item.colors_[color_type] = color;
    return true;
  }

  return false;
}

bool HoneycombMenuModelImpl::GetColor(int command_id,
                                honey_menu_color_type_t color_type,
                                honey_color_t& color) {
  return GetColorAt(GetIndexOf(command_id), color_type, color);
}

bool HoneycombMenuModelImpl::GetColorAt(int index,
                                  honey_menu_color_type_t color_type,
                                  honey_color_t& color) {
  if (!VerifyContext()) {
    return false;
  }

  if (color_type < 0 || color_type >= HONEYCOMB_MENU_COLOR_COUNT) {
    return false;
  }

  if (index == kDefaultIndex) {
    color = default_colors_[color_type];
    return true;
  }

  if (index >= 0 && index < static_cast<int>(items_.size())) {
    Item& item = items_[index];
    color = item.colors_[color_type];
    return true;
  }

  return false;
}

bool HoneycombMenuModelImpl::SetFontList(int command_id, const HoneycombString& font_list) {
  return SetFontListAt(GetIndexOf(command_id), font_list);
}

bool HoneycombMenuModelImpl::SetFontListAt(int index, const HoneycombString& font_list) {
  if (!VerifyContext()) {
    return false;
  }

  if (index == kDefaultIndex) {
    if (font_list.empty()) {
      has_default_font_list_ = false;
    } else {
      default_font_list_ = gfx::FontList(font_list);
      has_default_font_list_ = true;
    }
    return true;
  }

  if (index >= 0 && index < static_cast<int>(items_.size())) {
    Item& item = items_[index];
    if (font_list.empty()) {
      item.has_font_list_ = false;
    } else {
      item.font_list_ = gfx::FontList(font_list);
      item.has_font_list_ = true;
    }
    return true;
  }
  return false;
}

void HoneycombMenuModelImpl::ActivatedAt(size_t index,
                                   honey_event_flags_t event_flags) {
  if (!VerifyContext()) {
    return;
  }

  const int command_id = GetCommandIdAt(index);
  if (delegate_) {
    delegate_->ExecuteCommand(this, command_id, event_flags);
  }
  if (menu_model_delegate_) {
    menu_model_delegate_->ExecuteCommand(this, command_id, event_flags);
  }
}

void HoneycombMenuModelImpl::MouseOutsideMenu(const gfx::Point& screen_point) {
  if (!VerifyContext()) {
    return;
  }

  // Allow the callstack to unwind before notifying the delegate since it may
  // result in the menu being destroyed.
  HoneycombTaskRunnerImpl::GetCurrentTaskRunner()->PostTask(
      FROM_HERE, base::BindOnce(&HoneycombMenuModelImpl::OnMouseOutsideMenu, this,
                                screen_point));
}

void HoneycombMenuModelImpl::UnhandledOpenSubmenu(bool is_rtl) {
  if (!VerifyContext()) {
    return;
  }

  // Allow the callstack to unwind before notifying the delegate since it may
  // result in the menu being destroyed.
  HoneycombTaskRunnerImpl::GetCurrentTaskRunner()->PostTask(
      FROM_HERE,
      base::BindOnce(&HoneycombMenuModelImpl::OnUnhandledOpenSubmenu, this, is_rtl));
}

void HoneycombMenuModelImpl::UnhandledCloseSubmenu(bool is_rtl) {
  if (!VerifyContext()) {
    return;
  }

  // Allow the callstack to unwind before notifying the delegate since it may
  // result in the menu being destroyed.
  HoneycombTaskRunnerImpl::GetCurrentTaskRunner()->PostTask(
      FROM_HERE,
      base::BindOnce(&HoneycombMenuModelImpl::OnUnhandledCloseSubmenu, this, is_rtl));
}

bool HoneycombMenuModelImpl::GetTextColor(size_t index,
                                    bool is_accelerator,
                                    bool is_hovered,
                                    SkColor* override_color) const {
  if (index < items_.size()) {
    const Item& item = items_[index];
    if (!item.enabled_) {
      // Use accelerator color for disabled item text.
      is_accelerator = true;
    }

    const honey_menu_color_type_t color_type =
        GetMenuColorType(true, is_accelerator, is_hovered);
    if (item.colors_[color_type] != 0) {
      *override_color = item.colors_[color_type];
      return true;
    }
  }

  const honey_menu_color_type_t color_type =
      GetMenuColorType(true, is_accelerator, is_hovered);
  if (default_colors_[color_type] != 0) {
    *override_color = default_colors_[color_type];
    return true;
  }

  return false;
}

bool HoneycombMenuModelImpl::GetBackgroundColor(size_t index,
                                          bool is_hovered,
                                          SkColor* override_color) const {
  const honey_menu_color_type_t color_type =
      GetMenuColorType(false, false, is_hovered);

  if (index < items_.size()) {
    const Item& item = items_[index];
    if (item.colors_[color_type] != 0) {
      *override_color = item.colors_[color_type];
      return true;
    }
  }

  if (default_colors_[color_type] != 0) {
    *override_color = default_colors_[color_type];
    return true;
  }

  return false;
}

void HoneycombMenuModelImpl::MenuWillShow() {
  if (!VerifyContext()) {
    return;
  }

  if (delegate_) {
    delegate_->MenuWillShow(this);
  }
  if (menu_model_delegate_) {
    menu_model_delegate_->MenuWillShow(this);
  }
}

void HoneycombMenuModelImpl::MenuWillClose() {
  if (!VerifyContext()) {
    return;
  }

  if (!auto_notify_menu_closed_) {
    return;
  }

  // Due to how menus work on the different platforms, ActivatedAt will be
  // called after this.  It's more convenient for the delegate to be called
  // afterwards, though, so post a task.
  HoneycombTaskRunnerImpl::GetCurrentTaskRunner()->PostTask(
      FROM_HERE, base::BindOnce(&HoneycombMenuModelImpl::OnMenuClosed, this));
}

std::u16string HoneycombMenuModelImpl::GetFormattedLabelAt(size_t index) {
  std::u16string label = GetLabelAt(index).ToString16();
  if (delegate_) {
    delegate_->FormatLabel(this, label);
  }
  if (menu_model_delegate_) {
    HoneycombString new_label = label;
    if (menu_model_delegate_->FormatLabel(this, new_label)) {
      label = new_label;
    }
  }
  return label;
}

const gfx::FontList* HoneycombMenuModelImpl::GetLabelFontListAt(size_t index) const {
  if (index < items_.size()) {
    const Item& item = items_[index];
    if (item.has_font_list_) {
      return &item.font_list_;
    }
  }

  if (has_default_font_list_) {
    return &default_font_list_;
  }
  return nullptr;
}

bool HoneycombMenuModelImpl::VerifyRefCount() {
  if (!VerifyContext()) {
    return false;
  }

  if (!HasOneRef()) {
    return false;
  }

  for (ItemVector::iterator i = items_.begin(); i != items_.end(); ++i) {
    if ((*i).submenu_.get()) {
      if (!(*i).submenu_->VerifyRefCount()) {
        return false;
      }
    }
  }

  return true;
}

void HoneycombMenuModelImpl::AddMenuItem(
    const blink::mojom::CustomContextMenuItem& menu_item) {
  const int command_id = static_cast<int>(menu_item.action);

  switch (menu_item.type) {
    case blink::mojom::CustomContextMenuItemType::kOption:
      AddItem(command_id, menu_item.label);
      break;
    case blink::mojom::CustomContextMenuItemType::kCheckableOption:
      AddCheckItem(command_id, menu_item.label);
      break;
    case blink::mojom::CustomContextMenuItemType::kGroup:
      AddRadioItem(command_id, menu_item.label, 0);
      break;
    case blink::mojom::CustomContextMenuItemType::kSeparator:
      AddSeparator();
      break;
    case blink::mojom::CustomContextMenuItemType::kSubMenu: {
      HoneycombRefPtr<HoneycombMenuModelImpl> sub_menu = static_cast<HoneycombMenuModelImpl*>(
          AddSubMenu(command_id, menu_item.label).get());
      for (size_t i = 0; i < menu_item.submenu.size(); ++i) {
        sub_menu->AddMenuItem(*menu_item.submenu[i]);
      }
      break;
    }
  }

  if (!menu_item.enabled &&
      menu_item.type != blink::mojom::CustomContextMenuItemType::kSeparator) {
    SetEnabled(command_id, false);
  }

  if (menu_item.checked &&
      (menu_item.type ==
           blink::mojom::CustomContextMenuItemType::kCheckableOption ||
       menu_item.type == blink::mojom::CustomContextMenuItemType::kGroup)) {
    SetChecked(command_id, true);
  }
}

void HoneycombMenuModelImpl::NotifyMenuClosed() {
  DCHECK(!auto_notify_menu_closed_);
  OnMenuClosed();
}

void HoneycombMenuModelImpl::AppendItem(const Item& item) {
  ValidateItem(item);
  items_.push_back(item);
}

void HoneycombMenuModelImpl::InsertItemAt(const Item& item, size_t index) {
  // Sanitize the index.
  if (index > items_.size()) {
    index = items_.size();
  }

  ValidateItem(item);
  items_.insert(items_.begin() + index, item);
}

void HoneycombMenuModelImpl::ValidateItem(const Item& item) {
#if DCHECK_IS_ON()
  if (item.type_ == MENUITEMTYPE_SEPARATOR) {
    DCHECK_EQ(item.command_id_, kSeparatorId);
  } else {
    DCHECK_GE(item.command_id_, 0);
  }
#endif
}

void HoneycombMenuModelImpl::OnMouseOutsideMenu(const gfx::Point& screen_point) {
  if (delegate_) {
    delegate_->MouseOutsideMenu(this, screen_point);
  }
  if (menu_model_delegate_) {
    menu_model_delegate_->MouseOutsideMenu(
        this, HoneycombPoint(screen_point.x(), screen_point.y()));
  }
}

void HoneycombMenuModelImpl::OnUnhandledOpenSubmenu(bool is_rtl) {
  if (delegate_) {
    delegate_->UnhandledOpenSubmenu(this, is_rtl);
  }
  if (menu_model_delegate_) {
    menu_model_delegate_->UnhandledOpenSubmenu(this, is_rtl);
  }
}

void HoneycombMenuModelImpl::OnUnhandledCloseSubmenu(bool is_rtl) {
  if (delegate_) {
    delegate_->UnhandledCloseSubmenu(this, is_rtl);
  }
  if (menu_model_delegate_) {
    menu_model_delegate_->UnhandledCloseSubmenu(this, is_rtl);
  }
}

void HoneycombMenuModelImpl::OnMenuClosed() {
  if (delegate_) {
    delegate_->MenuClosed(this);
  }
  if (menu_model_delegate_) {
    menu_model_delegate_->MenuClosed(this);
  }
}

bool HoneycombMenuModelImpl::VerifyContext() {
  if (base::PlatformThread::CurrentId() != supported_thread_id_) {
    // This object should only be accessed from the thread that created it.
    DCHECK(false);
    return false;
  }

  return true;
}
