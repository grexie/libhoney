// Copyright (c) 2016 Marshall A. Greenblatt. All rights reserved.
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

#ifndef HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_WINDOW_H_
#define HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_WINDOW_H_
#pragma once

#include "include/honey_image.h"
#include "include/honey_menu_model.h"
#include "include/views/honey_display.h"
#include "include/views/honey_overlay_controller.h"
#include "include/views/honey_panel.h"
#include "include/views/honey_window_delegate.h"

class HoneycombBrowserView;

///
/// A Window is a top-level Window/widget in the Views hierarchy. By default it
/// will have a non-client area with title bar, icon and buttons that supports
/// moving and resizing. All size and position values are in density independent
/// pixels (DIP) unless otherwise indicated. Methods must be called on the
/// browser process UI thread unless otherwise indicated.
///
/*--honey(source=library)--*/
class HoneycombWindow : public HoneycombPanel {
 public:
  ///
  /// Create a new Window.
  ///
  /*--honey(optional_param=delegate)--*/
  static HoneycombRefPtr<HoneycombWindow> CreateTopLevelWindow(
      HoneycombRefPtr<HoneycombWindowDelegate> delegate);

  ///
  /// Show the Window.
  ///
  /*--honey()--*/
  virtual void Show() = 0;

  ///
  /// Show the Window as a browser modal dialog relative to |browser_view|. A
  /// parent Window must be returned via HoneycombWindowDelegate::GetParentWindow()
  /// and |browser_view| must belong to that parent Window. While this Window is
  /// visible, |browser_view| will be disabled while other controls in the
  /// parent Window remain enabled. Navigating or destroying the |browser_view|
  /// will close this Window automatically. Alternately, use Show() and return
  /// true from HoneycombWindowDelegate::IsWindowModalDialog() for a window modal
  /// dialog where all controls in the parent Window are disabled.
  ///
  /*--honey()--*/
  virtual void ShowAsBrowserModalDialog(
      HoneycombRefPtr<HoneycombBrowserView> browser_view) = 0;

  ///
  /// Hide the Window.
  ///
  /*--honey()--*/
  virtual void Hide() = 0;

  ///
  /// Sizes the Window to |size| and centers it in the current display.
  ///
  /*--honey()--*/
  virtual void CenterWindow(const HoneycombSize& size) = 0;

  ///
  /// Close the Window.
  ///
  /*--honey()--*/
  virtual void Close() = 0;

  ///
  /// Returns true if the Window has been closed.
  ///
  /*--honey()--*/
  virtual bool IsClosed() = 0;

  ///
  /// Activate the Window, assuming it already exists and is visible.
  ///
  /*--honey()--*/
  virtual void Activate() = 0;

  ///
  /// Deactivate the Window, making the next Window in the Z order the active
  /// Window.
  ///
  /*--honey()--*/
  virtual void Deactivate() = 0;

  ///
  /// Returns whether the Window is the currently active Window.
  ///
  /*--honey()--*/
  virtual bool IsActive() = 0;

  ///
  /// Bring this Window to the top of other Windows in the Windowing system.
  ///
  /*--honey()--*/
  virtual void BringToTop() = 0;

  ///
  /// Set the Window to be on top of other Windows in the Windowing system.
  ///
  /*--honey()--*/
  virtual void SetAlwaysOnTop(bool on_top) = 0;

  ///
  /// Returns whether the Window has been set to be on top of other Windows in
  /// the Windowing system.
  ///
  /*--honey()--*/
  virtual bool IsAlwaysOnTop() = 0;

  ///
  /// Maximize the Window.
  ///
  /*--honey()--*/
  virtual void Maximize() = 0;

  ///
  /// Minimize the Window.
  ///
  /*--honey()--*/
  virtual void Minimize() = 0;

  ///
  /// Restore the Window.
  ///
  /*--honey()--*/
  virtual void Restore() = 0;

  ///
  /// Set fullscreen Window state. The
  /// HoneycombWindowDelegate::OnWindowFullscreenTransition method will be called
  /// during the fullscreen transition for notification purposes.
  ///
  /*--honey()--*/
  virtual void SetFullscreen(bool fullscreen) = 0;

  ///
  /// Returns true if the Window is maximized.
  ///
  /*--honey()--*/
  virtual bool IsMaximized() = 0;

  ///
  /// Returns true if the Window is minimized.
  ///
  /*--honey()--*/
  virtual bool IsMinimized() = 0;

  ///
  /// Returns true if the Window is fullscreen.
  ///
  /*--honey()--*/
  virtual bool IsFullscreen() = 0;

  ///
  /// Set the Window title.
  ///
  /*--honey(optional_param=title)--*/
  virtual void SetTitle(const HoneycombString& title) = 0;

  ///
  /// Get the Window title.
  ///
  /*--honey()--*/
  virtual HoneycombString GetTitle() = 0;

  ///
  /// Set the Window icon. This should be a 16x16 icon suitable for use in the
  /// Windows's title bar.
  ///
  /*--honey()--*/
  virtual void SetWindowIcon(HoneycombRefPtr<HoneycombImage> image) = 0;

  ///
  /// Get the Window icon.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombImage> GetWindowIcon() = 0;

  ///
  /// Set the Window App icon. This should be a larger icon for use in the host
  /// environment app switching UI. On Windows, this is the ICON_BIG used in
  /// Alt-Tab list and Windows taskbar. The Window icon will be used by default
  /// if no Window App icon is specified.
  ///
  /*--honey()--*/
  virtual void SetWindowAppIcon(HoneycombRefPtr<HoneycombImage> image) = 0;

  ///
  /// Get the Window App icon.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombImage> GetWindowAppIcon() = 0;

  ///
  /// Add a View that will be overlayed on the Window contents with absolute
  /// positioning and high z-order. Positioning is controlled by |docking_mode|
  /// as described below. The returned HoneycombOverlayController object is used to
  /// control the overlay. Overlays are hidden by default.
  ///
  /// With HONEYCOMB_DOCKING_MODE_CUSTOM:
  ///   1. The overlay is initially hidden, sized to |view|'s preferred size,
  ///      and positioned in the top-left corner.
  ///   2. Optionally change the overlay position and/or size by calling
  ///      HoneycombOverlayController methods.
  ///   3. Call HoneycombOverlayController::SetVisible(true) to show the overlay.
  ///   4. The overlay will be automatically re-sized if |view|'s layout
  ///      changes. Optionally change the overlay position and/or size when
  ///      OnLayoutChanged is called on the Window's delegate to indicate a
  ///      change in Window bounds.
  ///
  /// With other docking modes:
  ///   1. The overlay is initially hidden, sized to |view|'s preferred size,
  ///      and positioned based on |docking_mode|.
  ///   2. Call HoneycombOverlayController::SetVisible(true) to show the overlay.
  ///   3. The overlay will be automatically re-sized if |view|'s layout changes
  ///      and re-positioned as appropriate when the Window resizes.
  ///
  /// Overlays created by this method will receive a higher z-order then any
  /// child Views added previously. It is therefore recommended to call this
  /// method last after all other child Views have been added so that the
  /// overlay displays as the top-most child of the Window.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombOverlayController> AddOverlayView(
      HoneycombRefPtr<HoneycombView> view,
      honey_docking_mode_t docking_mode) = 0;

  ///
  /// Show a menu with contents |menu_model|. |screen_point| specifies the menu
  /// position in screen coordinates. |anchor_position| specifies how the menu
  /// will be anchored relative to |screen_point|.
  ///
  /*--honey()--*/
  virtual void ShowMenu(HoneycombRefPtr<HoneycombMenuModel> menu_model,
                        const HoneycombPoint& screen_point,
                        honey_menu_anchor_position_t anchor_position) = 0;

  ///
  /// Cancel the menu that is currently showing, if any.
  ///
  /*--honey()--*/
  virtual void CancelMenu() = 0;

  ///
  /// Returns the Display that most closely intersects the bounds of this
  /// Window. May return NULL if this Window is not currently displayed.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombDisplay> GetDisplay() = 0;

  ///
  /// Returns the bounds (size and position) of this Window's client area.
  /// Position is in screen coordinates.
  ///
  /*--honey()--*/
  virtual HoneycombRect GetClientAreaBoundsInScreen() = 0;

  ///
  /// Set the regions where mouse events will be intercepted by this Window to
  /// support drag operations. Call this method with an empty vector to clear
  /// the draggable regions. The draggable region bounds should be in window
  /// coordinates.
  ///
  /*--honey(optional_param=regions)--*/
  virtual void SetDraggableRegions(
      const std::vector<HoneycombDraggableRegion>& regions) = 0;

  ///
  /// Retrieve the platform window handle for this Window.
  ///
  /*--honey()--*/
  virtual HoneycombWindowHandle GetWindowHandle() = 0;

  ///
  /// Simulate a key press. |key_code| is the VKEY_* value from Chromium's
  /// ui/events/keycodes/keyboard_codes.h header (VK_* values on Windows).
  /// |event_flags| is some combination of EVENTFLAG_SHIFT_DOWN,
  /// EVENTFLAG_CONTROL_DOWN and/or EVENTFLAG_ALT_DOWN. This method is exposed
  /// primarily for testing purposes.
  ///
  /*--honey()--*/
  virtual void SendKeyPress(int key_code, uint32_t event_flags) = 0;

  ///
  /// Simulate a mouse move. The mouse cursor will be moved to the specified
  /// (screen_x, screen_y) position. This method is exposed primarily for
  /// testing purposes.
  ///
  /*--honey()--*/
  virtual void SendMouseMove(int screen_x, int screen_y) = 0;

  ///
  /// Simulate mouse down and/or mouse up events. |button| is the mouse button
  /// type. If |mouse_down| is true a mouse down event will be sent. If
  /// |mouse_up| is true a mouse up event will be sent. If both are true a mouse
  /// down event will be sent followed by a mouse up event (equivalent to
  /// clicking the mouse button). The events will be sent using the current
  /// cursor position so make sure to call SendMouseMove() first to position the
  /// mouse. This method is exposed primarily for testing purposes.
  ///
  /*--honey()--*/
  virtual void SendMouseEvents(honey_mouse_button_type_t button,
                               bool mouse_down,
                               bool mouse_up) = 0;

  ///
  /// Set the keyboard accelerator for the specified |command_id|. |key_code|
  /// can be any virtual key or character value.
  /// HoneycombWindowDelegate::OnAccelerator will be called if the keyboard
  /// combination is triggered while this window has focus.
  ///
  /*--honey()--*/
  virtual void SetAccelerator(int command_id,
                              int key_code,
                              bool shift_pressed,
                              bool ctrl_pressed,
                              bool alt_pressed) = 0;

  ///
  /// Remove the keyboard accelerator for the specified |command_id|.
  ///
  /*--honey()--*/
  virtual void RemoveAccelerator(int command_id) = 0;

  ///
  /// Remove all keyboard accelerators.
  ///
  /*--honey()--*/
  virtual void RemoveAllAccelerators() = 0;
};

#endif  // HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_WINDOW_H_
