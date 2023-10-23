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

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_RENDER_HANDLER_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_RENDER_HANDLER_H_
#pragma once

#include <vector>

#include "include/honey_accessibility_handler.h"
#include "include/honey_base.h"
#include "include/honey_browser.h"
#include "include/honey_drag_data.h"

///
/// Implement this interface to handle events when window rendering is disabled.
/// The methods of this class will be called on the UI thread.
///
/*--honey(source=client)--*/
class HoneycombRenderHandler : public virtual HoneycombBaseRefCounted {
 public:
  typedef honey_drag_operations_mask_t DragOperation;
  typedef honey_drag_operations_mask_t DragOperationsMask;
  typedef honey_paint_element_type_t PaintElementType;
  typedef std::vector<HoneycombRect> RectList;
  typedef honey_text_input_mode_t TextInputMode;

  ///
  /// Return the handler for accessibility notifications. If no handler is
  /// provided the default implementation will be used.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombAccessibilityHandler> GetAccessibilityHandler() {
    return nullptr;
  }

  ///
  /// Called to retrieve the root window rectangle in screen DIP coordinates.
  /// Return true if the rectangle was provided. If this method returns false
  /// the rectangle from GetViewRect will be used.
  ///
  /*--honey()--*/
  virtual bool GetRootScreenRect(HoneycombRefPtr<HoneycombBrowser> browser, HoneycombRect& rect) {
    return false;
  }

  ///
  /// Called to retrieve the view rectangle in screen DIP coordinates. This
  /// method must always provide a non-empty rectangle.
  ///
  /*--honey()--*/
  virtual void GetViewRect(HoneycombRefPtr<HoneycombBrowser> browser, HoneycombRect& rect) = 0;

  ///
  /// Called to retrieve the translation from view DIP coordinates to screen
  /// coordinates. Windows/Linux should provide screen device (pixel)
  /// coordinates and MacOS should provide screen DIP coordinates. Return true
  /// if the requested coordinates were provided.
  ///
  /*--honey()--*/
  virtual bool GetScreenPoint(HoneycombRefPtr<HoneycombBrowser> browser,
                              int viewX,
                              int viewY,
                              int& screenX,
                              int& screenY) {
    return false;
  }

  ///
  /// Called to allow the client to fill in the HoneycombScreenInfo object with
  /// appropriate values. Return true if the |screen_info| structure has been
  /// modified.
  ///
  /// If the screen info rectangle is left empty the rectangle from GetViewRect
  /// will be used. If the rectangle is still empty or invalid popups may not be
  /// drawn correctly.
  ///
  /*--honey()--*/
  virtual bool GetScreenInfo(HoneycombRefPtr<HoneycombBrowser> browser,
                             HoneycombScreenInfo& screen_info) {
    return false;
  }

  ///
  /// Called when the browser wants to show or hide the popup widget. The popup
  /// should be shown if |show| is true and hidden if |show| is false.
  ///
  /*--honey()--*/
  virtual void OnPopupShow(HoneycombRefPtr<HoneycombBrowser> browser, bool show) {}

  ///
  /// Called when the browser wants to move or resize the popup widget. |rect|
  /// contains the new location and size in view coordinates.
  ///
  /*--honey()--*/
  virtual void OnPopupSize(HoneycombRefPtr<HoneycombBrowser> browser, const HoneycombRect& rect) {
  }

  ///
  /// Called when an element should be painted. Pixel values passed to this
  /// method are scaled relative to view coordinates based on the value of
  /// HoneycombScreenInfo.device_scale_factor returned from GetScreenInfo. |type|
  /// indicates whether the element is the view or the popup widget. |buffer|
  /// contains the pixel data for the whole image. |dirtyRects| contains the set
  /// of rectangles in pixel coordinates that need to be repainted. |buffer|
  /// will be |width|*|height|*4 bytes in size and represents a BGRA image with
  /// an upper-left origin. This method is only called when
  /// HoneycombWindowInfo::shared_texture_enabled is set to false.
  ///
  /*--honey()--*/
  virtual void OnPaint(HoneycombRefPtr<HoneycombBrowser> browser,
                       PaintElementType type,
                       const RectList& dirtyRects,
                       const void* buffer,
                       int width,
                       int height) = 0;

  ///
  /// Called when an element has been rendered to the shared texture handle.
  /// |type| indicates whether the element is the view or the popup widget.
  /// |dirtyRects| contains the set of rectangles in pixel coordinates that need
  /// to be repainted. |shared_handle| is the handle for a D3D11 Texture2D that
  /// can be accessed via ID3D11Device using the OpenSharedResource method. This
  /// method is only called when HoneycombWindowInfo::shared_texture_enabled is set to
  /// true, and is currently only supported on Windows.
  ///
  /*--honey()--*/
  virtual void OnAcceleratedPaint(HoneycombRefPtr<HoneycombBrowser> browser,
                                  PaintElementType type,
                                  const RectList& dirtyRects,
                                  void* shared_handle) {}

  ///
  /// Called to retrieve the size of the touch handle for the specified
  /// |orientation|.
  ///
  /*--honey()--*/
  virtual void GetTouchHandleSize(HoneycombRefPtr<HoneycombBrowser> browser,
                                  honey_horizontal_alignment_t orientation,
                                  HoneycombSize& size) {}

  ///
  /// Called when touch handle state is updated. The client is responsible for
  /// rendering the touch handles.
  ///
  /*--honey()--*/
  virtual void OnTouchHandleStateChanged(HoneycombRefPtr<HoneycombBrowser> browser,
                                         const HoneycombTouchHandleState& state) {}

  ///
  /// Called when the user starts dragging content in the web view. Contextual
  /// information about the dragged content is supplied by |drag_data|.
  /// (|x|, |y|) is the drag start location in screen coordinates.
  /// OS APIs that run a system message loop may be used within the
  /// StartDragging call.
  ///
  /// Return false to abort the drag operation. Don't call any of
  /// HoneycombBrowserHost::DragSource*Ended* methods after returning false.
  ///
  /// Return true to handle the drag operation. Call
  /// HoneycombBrowserHost::DragSourceEndedAt and DragSourceSystemDragEnded either
  /// synchronously or asynchronously to inform the web view that the drag
  /// operation has ended.
  ///
  /*--honey()--*/
  virtual bool StartDragging(HoneycombRefPtr<HoneycombBrowser> browser,
                             HoneycombRefPtr<HoneycombDragData> drag_data,
                             DragOperationsMask allowed_ops,
                             int x,
                             int y) {
    return false;
  }

  ///
  /// Called when the web view wants to update the mouse cursor during a
  /// drag & drop operation. |operation| describes the allowed operation
  /// (none, move, copy, link).
  ///
  /*--honey()--*/
  virtual void UpdateDragCursor(HoneycombRefPtr<HoneycombBrowser> browser,
                                DragOperation operation) {}

  ///
  /// Called when the scroll offset has changed.
  ///
  /*--honey()--*/
  virtual void OnScrollOffsetChanged(HoneycombRefPtr<HoneycombBrowser> browser,
                                     double x,
                                     double y) {}

  ///
  /// Called when the IME composition range has changed. |selected_range| is the
  /// range of characters that have been selected. |character_bounds| is the
  /// bounds of each character in view coordinates.
  ///
  /*--honey()--*/
  virtual void OnImeCompositionRangeChanged(HoneycombRefPtr<HoneycombBrowser> browser,
                                            const HoneycombRange& selected_range,
                                            const RectList& character_bounds) {}

  ///
  /// Called when text selection has changed for the specified |browser|.
  /// |selected_text| is the currently selected text and |selected_range| is
  /// the character range.
  ///
  /*--honey(optional_param=selected_text,optional_param=selected_range)--*/
  virtual void OnTextSelectionChanged(HoneycombRefPtr<HoneycombBrowser> browser,
                                      const HoneycombString& selected_text,
                                      const HoneycombRange& selected_range) {}

  ///
  /// Called when an on-screen keyboard should be shown or hidden for the
  /// specified |browser|. |input_mode| specifies what kind of keyboard
  /// should be opened. If |input_mode| is HONEYCOMB_TEXT_INPUT_MODE_NONE, any
  /// existing keyboard for this browser should be hidden.
  ///
  /*--honey()--*/
  virtual void OnVirtualKeyboardRequested(HoneycombRefPtr<HoneycombBrowser> browser,
                                          TextInputMode input_mode) {}
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_RENDER_HANDLER_H_
