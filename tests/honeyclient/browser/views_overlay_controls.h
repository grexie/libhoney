// Copyright (c) 2021 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_VIEWS_OVERLAY_CONTROLS_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_VIEWS_OVERLAY_CONTROLS_H_
#pragma once

#include "include/views/honey_button_delegate.h"
#include "include/views/honey_label_button.h"
#include "include/views/honey_menu_button.h"
#include "include/views/honey_overlay_controller.h"
#include "include/views/honey_panel.h"

namespace client {

// Implements window overlay controls that receive absolute positioning on top
// of the browser view. All methods must be called on the browser process UI
// thread.
class ViewsOverlayControls : public HoneycombButtonDelegate {
 public:
  enum class Command {
    kMinimize = 1,
    kMaximize,
    kClose,
  };

  ViewsOverlayControls(bool with_window_buttons, bool use_bottom_controls);

  void Initialize(HoneycombRefPtr<HoneycombWindow> window,
                  HoneycombRefPtr<HoneycombMenuButton> menu_button,
                  HoneycombRefPtr<HoneycombView> location_bar,
                  bool is_chrome_toolbar);
  void Destroy();

  // Update window control button state and location bar bounds.
  void UpdateControls();

  // Exclude all regions obscured by overlays.
  void UpdateDraggableRegions(std::vector<HoneycombDraggableRegion>& window_regions);

 private:
  // HoneycombButtonDelegate methods:
  void OnButtonPressed(HoneycombRefPtr<HoneycombButton> button) override;

  HoneycombRefPtr<HoneycombLabelButton> CreateButton(Command command);

  void MaybeUpdateMaximizeButton();

  HoneycombRefPtr<HoneycombWindow> window_;
  bool window_maximized_;

  // Window control buttons.
  HoneycombRefPtr<HoneycombPanel> panel_;
  HoneycombRefPtr<HoneycombOverlayController> panel_controller_;
  const bool with_window_buttons_;
  const bool use_bottom_controls_;

  // Location bar.
  HoneycombRefPtr<HoneycombView> location_bar_;
  bool is_chrome_toolbar_;
  HoneycombRefPtr<HoneycombOverlayController> location_controller_;

  // Menu button.
  HoneycombRefPtr<HoneycombOverlayController> menu_controller_;

  IMPLEMENT_REFCOUNTING(ViewsOverlayControls);
  DISALLOW_COPY_AND_ASSIGN(ViewsOverlayControls);
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_VIEWS_OVERLAY_CONTROLS_H_
