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

#ifndef HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_WINDOW_DELEGATE_H_
#define HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_WINDOW_DELEGATE_H_
#pragma once

#include "include/views/honey_panel_delegate.h"

class HoneycombWindow;

///
/// Implement this interface to handle window events. The methods of this class
/// will be called on the browser process UI thread unless otherwise indicated.
///
/*--honey(source=client)--*/
class HoneycombWindowDelegate : public HoneycombPanelDelegate {
 public:
  ///
  /// Called when |window| is created.
  ///
  /*--honey()--*/
  virtual void OnWindowCreated(HoneycombRefPtr<HoneycombWindow> window) {}

  ///
  /// Called when |window| is closing.
  ///
  /*--honey()--*/
  virtual void OnWindowClosing(HoneycombRefPtr<HoneycombWindow> window) {}

  ///
  /// Called when |window| is destroyed. Release all references to |window| and
  /// do not attempt to execute any methods on |window| after this callback
  /// returns.
  ///
  /*--honey()--*/
  virtual void OnWindowDestroyed(HoneycombRefPtr<HoneycombWindow> window) {}

  ///
  /// Called when |window| is activated or deactivated.
  ///
  /*--honey()--*/
  virtual void OnWindowActivationChanged(HoneycombRefPtr<HoneycombWindow> window,
                                         bool active) {}

  ///
  /// Called when |window| bounds have changed. |new_bounds| will be in DIP
  /// screen coordinates.
  ///
  /*--honey()--*/
  virtual void OnWindowBoundsChanged(HoneycombRefPtr<HoneycombWindow> window,
                                     const HoneycombRect& new_bounds) {}

  ///
  /// Called when |window| is transitioning to or from fullscreen mode. On MacOS
  /// the transition occurs asynchronously with |is_competed| set to false when
  /// the transition starts and true after the transition completes. On other
  /// platforms the transition occurs synchronously with |is_completed| set to
  /// true after the transition completes. With the Alloy runtime you must also
  /// implement HoneycombDisplayHandler::OnFullscreenModeChange to handle fullscreen
  /// transitions initiated by browser content.
  ///
  /*--honey()--*/
  virtual void OnWindowFullscreenTransition(HoneycombRefPtr<HoneycombWindow> window,
                                            bool is_completed) {}

  ///
  /// Return the parent for |window| or NULL if the |window| does not have a
  /// parent. Windows with parents will not get a taskbar button. Set |is_menu|
  /// to true if |window| will be displayed as a menu, in which case it will not
  /// be clipped to the parent window bounds. Set |can_activate_menu| to false
  /// if |is_menu| is true and |window| should not be activated (given keyboard
  /// focus) when displayed.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombWindow> GetParentWindow(HoneycombRefPtr<HoneycombWindow> window,
                                               bool* is_menu,
                                               bool* can_activate_menu) {
    return nullptr;
  }

  ///
  /// Return true if |window| should be created as a window modal dialog. Only
  /// called when a Window is returned via GetParentWindow() with |is_menu| set
  /// to false. All controls in the parent Window will be disabled while
  /// |window| is visible. This functionality is not supported by all Linux
  /// window managers. Alternately, use HoneycombWindow::ShowAsBrowserModalDialog()
  /// for a browser modal dialog that works on all platforms.
  ///
  /*--honey()--*/
  virtual bool IsWindowModalDialog(HoneycombRefPtr<HoneycombWindow> window) {
    return false;
  }

  ///
  /// Return the initial bounds for |window| in density independent pixel (DIP)
  /// coordinates. If this method returns an empty HoneycombRect then
  /// GetPreferredSize() will be called to retrieve the size, and the window
  /// will be placed on the screen with origin (0,0). This method can be used in
  /// combination with HoneycombView::GetBoundsInScreen() to restore the previous
  /// window bounds.
  ///
  /*--honey()--*/
  virtual HoneycombRect GetInitialBounds(HoneycombRefPtr<HoneycombWindow> window) {
    return HoneycombRect();
  }

  ///
  /// Return the initial show state for |window|.
  ///
  /*--honey(default_retval=HONEYCOMB_SHOW_STATE_NORMAL)--*/
  virtual honey_show_state_t GetInitialShowState(HoneycombRefPtr<HoneycombWindow> window) {
    return HONEYCOMB_SHOW_STATE_NORMAL;
  }

  ///
  /// Return true if |window| should be created without a frame or title bar.
  /// The window will be resizable if CanResize() returns true. Use
  /// HoneycombWindow::SetDraggableRegions() to specify draggable regions.
  ///
  /*--honey()--*/
  virtual bool IsFrameless(HoneycombRefPtr<HoneycombWindow> window) { return false; }

  ///
  /// Return true if |window| should be created with standard window buttons
  /// like close, minimize and zoom. This method is only supported on macOS.
  ///
  /*--honey()--*/
  virtual bool WithStandardWindowButtons(HoneycombRefPtr<HoneycombWindow> window) {
    return !IsFrameless(window);
  }

  ///
  /// Return whether the titlebar height should be overridden,
  /// and sets the height of the titlebar in |titlebar_height|.
  /// On macOS, it can also be used to adjust the vertical position
  /// of the traffic light buttons in frameless windows.
  /// The buttons will be positioned halfway down the titlebar
  /// at a height of |titlebar_height| / 2.
  ///
  /*--honey()--*/
  virtual bool GetTitlebarHeight(HoneycombRefPtr<HoneycombWindow> window,
                                 float* titlebar_height) {
    return false;
  }

  ///
  /// Return true if |window| can be resized.
  ///
  /*--honey()--*/
  virtual bool CanResize(HoneycombRefPtr<HoneycombWindow> window) { return true; }

  ///
  /// Return true if |window| can be maximized.
  ///
  /*--honey()--*/
  virtual bool CanMaximize(HoneycombRefPtr<HoneycombWindow> window) { return true; }

  ///
  /// Return true if |window| can be minimized.
  ///
  /*--honey()--*/
  virtual bool CanMinimize(HoneycombRefPtr<HoneycombWindow> window) { return true; }

  ///
  /// Return true if |window| can be closed. This will be called for user-
  /// initiated window close actions and when HoneycombWindow::Close() is called.
  ///
  /*--honey()--*/
  virtual bool CanClose(HoneycombRefPtr<HoneycombWindow> window) { return true; }

  ///
  /// Called when a keyboard accelerator registered with
  /// HoneycombWindow::SetAccelerator is triggered. Return true if the accelerator was
  /// handled or false otherwise.
  ///
  /*--honey()--*/
  virtual bool OnAccelerator(HoneycombRefPtr<HoneycombWindow> window, int command_id) {
    return false;
  }

  ///
  /// Called after all other controls in the window have had a chance to
  /// handle the event. |event| contains information about the keyboard event.
  /// Return true if the keyboard event was handled or false otherwise.
  ///
  /*--honey()--*/
  virtual bool OnKeyEvent(HoneycombRefPtr<HoneycombWindow> window,
                          const HoneycombKeyEvent& event) {
    return false;
  }
};

#endif  // HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_WINDOW_DELEGATE_H_
