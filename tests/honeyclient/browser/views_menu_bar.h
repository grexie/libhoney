// Copyright (c) 2017 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_VIEWS_MENU_BAR_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_VIEWS_MENU_BAR_H_
#pragma once

#include <map>
#include <string>
#include <vector>

#include "include/honey_menu_model.h"
#include "include/honey_menu_model_delegate.h"
#include "include/views/honey_menu_button.h"
#include "include/views/honey_menu_button_delegate.h"
#include "include/views/honey_panel.h"

namespace client {

// Implements a menu bar which is composed of HoneycombMenuButtons positioned in a
// row with automatic switching between them via mouse/keyboard. All methods
// must be called on the browser process UI thread.
class ViewsMenuBar : public HoneycombMenuButtonDelegate, public HoneycombMenuModelDelegate {
 public:
  // Delegate methods will be called on the browser process UI thread.
  class Delegate {
   public:
    // Called when a menu command is selected.
    virtual void MenuBarExecuteCommand(HoneycombRefPtr<HoneycombMenuModel> menu_model,
                                       int command_id,
                                       honey_event_flags_t event_flags) = 0;

   protected:
    virtual ~Delegate() {}
  };

  // |delegate| must outlive this object.
  // |menu_id_start| is the ID for the first HoneycombMenuButton in the bar. An ID
  // range starting with |menu_id_start| and extending for a reasonable distance
  // should be reserved in the client for MenuBar usage.
  ViewsMenuBar(Delegate* delegate, int menu_id_start, bool use_bottom_controls);

  // Returns true if |menu_id| exists in the menu bar.
  bool HasMenuId(int menu_id) const;

  // Returns the HoneycombPanel that represents the menu bar.
  HoneycombRefPtr<HoneycombPanel> GetMenuPanel();

  // Create a new menu with the specified |label|. If |menu_id| is non-nullptr
  // it will be populated with the new menu ID.
  HoneycombRefPtr<HoneycombMenuModel> CreateMenuModel(const HoneycombString& label, int* menu_id);

  // Returns the menu with the specified |menu_id|, or nullptr if no such menu
  // exists.
  HoneycombRefPtr<HoneycombMenuModel> GetMenuModel(int menu_id) const;

  // Assign or remove focus from the menu bar.
  // Focus is assigned to the menu bar by ViewsWindow::OnKeyEvent when the ALT
  // key is pressed. Focus is removed from the menu bar by ViewsWindow::OnFocus
  // when a control not in the menu bar gains focus.
  void SetMenuFocusable(bool focusable);

  // Key events forwarded from ViewsWindow::OnKeyEvent when the menu bar has
  // focus.
  bool OnKeyEvent(const HoneycombKeyEvent& event);

  // Reset menu bar state.
  void Reset();

 protected:
  // HoneycombButtonDelegate methods:
  void OnButtonPressed(HoneycombRefPtr<HoneycombButton> button) override {}

  // HoneycombMenuButtonDelegate methods:
  void OnMenuButtonPressed(
      HoneycombRefPtr<HoneycombMenuButton> menu_button,
      const HoneycombPoint& screen_point,
      HoneycombRefPtr<HoneycombMenuButtonPressedLock> button_pressed_lock) override;

  // HoneycombMenuModelDelegate methods:
  void ExecuteCommand(HoneycombRefPtr<HoneycombMenuModel> menu_model,
                      int command_id,
                      honey_event_flags_t event_flags) override;
  void MouseOutsideMenu(HoneycombRefPtr<HoneycombMenuModel> menu_model,
                        const HoneycombPoint& screen_point) override;
  void UnhandledOpenSubmenu(HoneycombRefPtr<HoneycombMenuModel> menu_model,
                            bool is_rtl) override;
  void UnhandledCloseSubmenu(HoneycombRefPtr<HoneycombMenuModel> menu_model,
                             bool is_rtl) override;
  void MenuClosed(HoneycombRefPtr<HoneycombMenuModel> menu_model) override;

 private:
  // Creates the menu panel if it doesn't already exist.
  void EnsureMenuPanel();

  // Returns the ID for the currently active menu, or -1 if no menu is currently
  // active.
  int GetActiveMenuId();

  // Triggers the menu at the specified |offset| from the currently active menu.
  void TriggerNextMenu(int offset);

  // Triggers the specified MenuButton |button|.
  void TriggerMenuButton(HoneycombRefPtr<HoneycombView> button);

  Delegate* delegate_;  // Not owned by this object.
  const int id_start_;
  const bool use_bottom_controls_;
  int id_next_;
  HoneycombRefPtr<HoneycombPanel> panel_;
  std::vector<HoneycombRefPtr<HoneycombMenuModel>> models_;
  bool last_nav_with_keyboard_;

  // Map of mnemonic to MenuButton ID.
  typedef std::map<char16_t, int> MnemonicMap;
  MnemonicMap mnemonics_;

  IMPLEMENT_REFCOUNTING(ViewsMenuBar);
  DISALLOW_COPY_AND_ASSIGN(ViewsMenuBar);
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_VIEWS_MENU_BAR_H_
