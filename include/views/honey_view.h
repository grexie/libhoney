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

#ifndef HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_VIEW_H_
#define HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_VIEW_H_
#pragma once

#include "include/views/honey_view_delegate.h"

class HoneycombBrowserView;
class HoneycombButton;
class HoneycombPanel;
class HoneycombScrollView;
class HoneycombTextfield;
class HoneycombWindow;

///
/// A View is a rectangle within the views View hierarchy. It is the base class
/// for all Views. All size and position values are in density independent
/// pixels (DIP) unless otherwise indicated. Methods must be called on the
/// browser process UI thread unless otherwise indicated.
///
/*--honey(source=library)--*/
class HoneycombView : public HoneycombBaseRefCounted {
 public:
  ///
  /// Returns this View as a BrowserView or NULL if this is not a BrowserView.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombBrowserView> AsBrowserView() = 0;

  ///
  /// Returns this View as a Button or NULL if this is not a Button.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombButton> AsButton() = 0;

  ///
  /// Returns this View as a Panel or NULL if this is not a Panel.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombPanel> AsPanel() = 0;

  ///
  /// Returns this View as a ScrollView or NULL if this is not a ScrollView.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombScrollView> AsScrollView() = 0;

  ///
  /// Returns this View as a Textfield or NULL if this is not a Textfield.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombTextfield> AsTextfield() = 0;

  ///
  /// Returns the type of this View as a string. Used primarily for testing
  /// purposes.
  ///
  /*--honey()--*/
  virtual HoneycombString GetTypeString() = 0;

  ///
  /// Returns a string representation of this View which includes the type and
  /// various type-specific identifying attributes. If |include_children| is
  /// true any child Views will also be included. Used primarily for testing
  /// purposes.
  ///
  /*--honey()--*/
  virtual HoneycombString ToString(bool include_children) = 0;

  ///
  /// Returns true if this View is valid.
  ///
  /*--honey()--*/
  virtual bool IsValid() = 0;

  ///
  /// Returns true if this View is currently attached to another View. A View
  /// can only be attached to one View at a time.
  ///
  /*--honey()--*/
  virtual bool IsAttached() = 0;

  ///
  /// Returns true if this View is the same as |that| View.
  ///
  /*--honey()--*/
  virtual bool IsSame(HoneycombRefPtr<HoneycombView> that) = 0;

  ///
  /// Returns the delegate associated with this View, if any.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombViewDelegate> GetDelegate() = 0;

  ///
  /// Returns the top-level Window hosting this View, if any.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombWindow> GetWindow() = 0;

  ///
  /// Returns the ID for this View.
  ///
  /*--honey()--*/
  virtual int GetID() = 0;

  ///
  /// Sets the ID for this View. ID should be unique within the subtree that you
  /// intend to search for it. 0 is the default ID for views.
  ///
  /*--honey()--*/
  virtual void SetID(int id) = 0;

  ///
  /// Returns the group id of this View, or -1 if not set.
  ///
  /*--honey()--*/
  virtual int GetGroupID() = 0;

  ///
  /// A group id is used to tag Views which are part of the same logical group.
  /// Focus can be moved between views with the same group using the arrow keys.
  /// The group id is immutable once it's set.
  ///
  /*--honey()--*/
  virtual void SetGroupID(int group_id) = 0;

  ///
  /// Returns the View that contains this View, if any.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombView> GetParentView() = 0;

  ///
  /// Recursively descends the view tree starting at this View, and returns the
  /// first child that it encounters with the given ID. Returns NULL if no
  /// matching child view is found.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombView> GetViewForID(int id) = 0;

  ///
  /// Sets the bounds (size and position) of this View. |bounds| is in parent
  /// coordinates, or DIP screen coordinates if there is no parent.
  ///
  /*--honey()--*/
  virtual void SetBounds(const HoneycombRect& bounds) = 0;

  ///
  /// Returns the bounds (size and position) of this View in parent coordinates,
  /// or DIP screen coordinates if there is no parent.
  ///
  /*--honey()--*/
  virtual HoneycombRect GetBounds() = 0;

  ///
  /// Returns the bounds (size and position) of this View in DIP screen
  /// coordinates.
  ///
  /*--honey()--*/
  virtual HoneycombRect GetBoundsInScreen() = 0;

  ///
  /// Sets the size of this View without changing the position. |size| in
  /// parent coordinates, or DIP screen coordinates if there is no parent.
  ///
  /*--honey()--*/
  virtual void SetSize(const HoneycombSize& size) = 0;

  ///
  /// Returns the size of this View in parent coordinates, or DIP screen
  /// coordinates if there is no parent.
  ///
  /*--honey()--*/
  virtual HoneycombSize GetSize() = 0;

  ///
  /// Sets the position of this View without changing the size. |position| is in
  /// parent coordinates, or DIP screen coordinates if there is no parent.
  ///
  /*--honey()--*/
  virtual void SetPosition(const HoneycombPoint& position) = 0;

  ///
  /// Returns the position of this View. Position is in parent coordinates, or
  /// DIP screen coordinates if there is no parent.
  ///
  /*--honey()--*/
  virtual HoneycombPoint GetPosition() = 0;

  ///
  /// Sets the insets for this View. |insets| is in parent coordinates, or DIP
  /// screen coordinates if there is no parent.
  ///
  /*--honey()--*/
  virtual void SetInsets(const HoneycombInsets& insets) = 0;

  ///
  /// Returns the insets for this View in parent coordinates, or DIP screen
  /// coordinates if there is no parent.
  ///
  /*--honey()--*/
  virtual HoneycombInsets GetInsets() = 0;

  ///
  /// Returns the size this View would like to be if enough space is available.
  /// Size is in parent coordinates, or DIP screen coordinates if there is no
  /// parent.
  ///
  /*--honey()--*/
  virtual HoneycombSize GetPreferredSize() = 0;

  ///
  /// Size this View to its preferred size. Size is in parent coordinates, or
  /// DIP screen coordinates if there is no parent.
  ///
  /*--honey()--*/
  virtual void SizeToPreferredSize() = 0;

  ///
  /// Returns the minimum size for this View. Size is in parent coordinates, or
  /// DIP screen coordinates if there is no parent.
  ///
  /*--honey()--*/
  virtual HoneycombSize GetMinimumSize() = 0;

  ///
  /// Returns the maximum size for this View. Size is in parent coordinates, or
  /// DIP screen coordinates if there is no parent.
  ///
  /*--honey()--*/
  virtual HoneycombSize GetMaximumSize() = 0;

  ///
  /// Returns the height necessary to display this View with the provided width.
  ///
  /*--honey()--*/
  virtual int GetHeightForWidth(int width) = 0;

  ///
  /// Indicate that this View and all parent Views require a re-layout. This
  /// ensures the next call to Layout() will propagate to this View even if the
  /// bounds of parent Views do not change.
  ///
  /*--honey()--*/
  virtual void InvalidateLayout() = 0;

  ///
  /// Sets whether this View is visible. Windows are hidden by default and other
  /// views are visible by default. This View and any parent views must be set
  /// as visible for this View to be drawn in a Window. If this View is set as
  /// hidden then it and any child views will not be drawn and, if any of those
  /// views currently have focus, then focus will also be cleared. Painting is
  /// scheduled as needed. If this View is a Window then calling this method is
  /// equivalent to calling the Window Show() and Hide() methods.
  ///
  /*--honey()--*/
  virtual void SetVisible(bool visible) = 0;

  ///
  /// Returns whether this View is visible. A view may be visible but still not
  /// drawn in a Window if any parent views are hidden. If this View is a Window
  /// then a return value of true indicates that this Window is currently
  /// visible to the user on-screen. If this View is not a Window then call
  /// IsDrawn() to determine whether this View and all parent views are visible
  /// and will be drawn.
  ///
  /*--honey()--*/
  virtual bool IsVisible() = 0;

  ///
  /// Returns whether this View is visible and drawn in a Window. A view is
  /// drawn if it and all parent views are visible. If this View is a Window
  /// then calling this method is equivalent to calling IsVisible(). Otherwise,
  /// to determine if the containing Window is visible to the user on-screen
  /// call IsVisible() on the Window.
  ///
  /*--honey()--*/
  virtual bool IsDrawn() = 0;

  ///
  /// Set whether this View is enabled. A disabled View does not receive
  /// keyboard or mouse inputs. If |enabled| differs from the current value the
  /// View will be repainted. Also, clears focus if the focused View is
  /// disabled.
  ///
  /*--honey()--*/
  virtual void SetEnabled(bool enabled) = 0;

  ///
  /// Returns whether this View is enabled.
  ///
  /*--honey()--*/
  virtual bool IsEnabled() = 0;

  ///
  /// Sets whether this View is capable of taking focus. It will clear focus if
  /// the focused View is set to be non-focusable. This is false by default so
  /// that a View used as a container does not get the focus.
  ///
  /*--honey()--*/
  virtual void SetFocusable(bool focusable) = 0;

  ///
  /// Returns true if this View is focusable, enabled and drawn.
  ///
  /*--honey()--*/
  virtual bool IsFocusable() = 0;

  ///
  /// Return whether this View is focusable when the user requires full keyboard
  /// access, even though it may not be normally focusable.
  ///
  /*--honey()--*/
  virtual bool IsAccessibilityFocusable() = 0;

  ///
  /// Request keyboard focus. If this View is focusable it will become the
  /// focused View.
  ///
  /*--honey()--*/
  virtual void RequestFocus() = 0;

  ///
  /// Sets the background color for this View.
  ///
  /*--honey()--*/
  virtual void SetBackgroundColor(honey_color_t color) = 0;

  ///
  /// Returns the background color for this View.
  ///
  /*--honey()--*/
  virtual honey_color_t GetBackgroundColor() = 0;

  ///
  /// Convert |point| from this View's coordinate system to DIP screen
  /// coordinates. This View must belong to a Window when calling this method.
  /// Returns true if the conversion is successful or false otherwise. Use
  /// HoneycombDisplay::ConvertPointToPixels() after calling this method if further
  /// conversion to display-specific pixel coordinates is desired.
  ///
  /*--honey()--*/
  virtual bool ConvertPointToScreen(HoneycombPoint& point) = 0;

  ///
  /// Convert |point| to this View's coordinate system from DIP screen
  /// coordinates. This View must belong to a Window when calling this method.
  /// Returns true if the conversion is successful or false otherwise. Use
  /// HoneycombDisplay::ConvertPointFromPixels() before calling this method if
  /// conversion from display-specific pixel coordinates is necessary.
  ///
  /*--honey()--*/
  virtual bool ConvertPointFromScreen(HoneycombPoint& point) = 0;

  ///
  /// Convert |point| from this View's coordinate system to that of the Window.
  /// This View must belong to a Window when calling this method. Returns true
  /// if the conversion is successful or false otherwise.
  ///
  /*--honey()--*/
  virtual bool ConvertPointToWindow(HoneycombPoint& point) = 0;

  ///
  /// Convert |point| to this View's coordinate system from that of the Window.
  /// This View must belong to a Window when calling this method. Returns true
  /// if the conversion is successful or false otherwise.
  ///
  /*--honey()--*/
  virtual bool ConvertPointFromWindow(HoneycombPoint& point) = 0;

  ///
  /// Convert |point| from this View's coordinate system to that of |view|.
  /// |view| needs to be in the same Window but not necessarily the same view
  /// hierarchy. Returns true if the conversion is successful or false
  /// otherwise.
  ///
  /*--honey()--*/
  virtual bool ConvertPointToView(HoneycombRefPtr<HoneycombView> view, HoneycombPoint& point) = 0;

  ///
  /// Convert |point| to this View's coordinate system from that |view|. |view|
  /// needs to be in the same Window but not necessarily the same view
  /// hierarchy. Returns true if the conversion is successful or false
  /// otherwise.
  ///
  /*--honey()--*/
  virtual bool ConvertPointFromView(HoneycombRefPtr<HoneycombView> view,
                                    HoneycombPoint& point) = 0;
};

#endif  // HONEYCOMB_INCLUDE_VIEWS_HONEYCOMB_VIEW_H_
