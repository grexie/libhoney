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

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_CONTEXT_MENU_HANDLER_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_CONTEXT_MENU_HANDLER_H_
#pragma once

#include "include/honey_base.h"
#include "include/honey_browser.h"
#include "include/honey_frame.h"
#include "include/honey_menu_model.h"

class HoneycombContextMenuParams;

///
/// Callback interface used for continuation of custom context menu display.
///
/*--honey(source=library)--*/
class HoneycombRunContextMenuCallback : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Complete context menu display by selecting the specified |command_id| and
  /// |event_flags|.
  ///
  /*--honey(capi_name=cont)--*/
  virtual void Continue(int command_id, honey_event_flags_t event_flags) = 0;

  ///
  /// Cancel context menu display.
  ///
  /*--honey()--*/
  virtual void Cancel() = 0;
};

///
/// Callback interface used for continuation of custom quick menu display.
///
/*--honey(source=library)--*/
class HoneycombRunQuickMenuCallback : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Complete quick menu display by selecting the specified |command_id| and
  /// |event_flags|.
  ///
  /*--honey(capi_name=cont)--*/
  virtual void Continue(int command_id, honey_event_flags_t event_flags) = 0;

  ///
  /// Cancel quick menu display.
  ///
  /*--honey()--*/
  virtual void Cancel() = 0;
};

///
/// Implement this interface to handle context menu events. The methods of this
/// class will be called on the UI thread.
///
/*--honey(source=client)--*/
class HoneycombContextMenuHandler : public virtual HoneycombBaseRefCounted {
 public:
  typedef honey_event_flags_t EventFlags;
  typedef honey_quick_menu_edit_state_flags_t QuickMenuEditStateFlags;

  ///
  /// Called before a context menu is displayed. |params| provides information
  /// about the context menu state. |model| initially contains the default
  /// context menu. The |model| can be cleared to show no context menu or
  /// modified to show a custom menu. Do not keep references to |params| or
  /// |model| outside of this callback.
  ///
  /*--honey()--*/
  virtual void OnBeforeContextMenu(HoneycombRefPtr<HoneycombBrowser> browser,
                                   HoneycombRefPtr<HoneycombFrame> frame,
                                   HoneycombRefPtr<HoneycombContextMenuParams> params,
                                   HoneycombRefPtr<HoneycombMenuModel> model) {}

  ///
  /// Called to allow custom display of the context menu. |params| provides
  /// information about the context menu state. |model| contains the context
  /// menu model resulting from OnBeforeContextMenu. For custom display return
  /// true and execute |callback| either synchronously or asynchronously with
  /// the selected command ID. For default display return false. Do not keep
  /// references to |params| or |model| outside of this callback.
  ///
  /*--honey()--*/
  virtual bool RunContextMenu(HoneycombRefPtr<HoneycombBrowser> browser,
                              HoneycombRefPtr<HoneycombFrame> frame,
                              HoneycombRefPtr<HoneycombContextMenuParams> params,
                              HoneycombRefPtr<HoneycombMenuModel> model,
                              HoneycombRefPtr<HoneycombRunContextMenuCallback> callback) {
    return false;
  }

  ///
  /// Called to execute a command selected from the context menu. Return true if
  /// the command was handled or false for the default implementation. See
  /// honey_menu_id_t for the command ids that have default implementations. All
  /// user-defined command ids should be between MENU_ID_USER_FIRST and
  /// MENU_ID_USER_LAST. |params| will have the same values as what was passed
  /// to OnBeforeContextMenu(). Do not keep a reference to |params| outside of
  /// this callback.
  ///
  /*--honey()--*/
  virtual bool OnContextMenuCommand(HoneycombRefPtr<HoneycombBrowser> browser,
                                    HoneycombRefPtr<HoneycombFrame> frame,
                                    HoneycombRefPtr<HoneycombContextMenuParams> params,
                                    int command_id,
                                    EventFlags event_flags) {
    return false;
  }

  ///
  /// Called when the context menu is dismissed irregardless of whether the menu
  /// was canceled or a command was selected.
  ///
  /*--honey()--*/
  virtual void OnContextMenuDismissed(HoneycombRefPtr<HoneycombBrowser> browser,
                                      HoneycombRefPtr<HoneycombFrame> frame) {}

  ///
  /// Called to allow custom display of the quick menu for a windowless browser.
  /// |location| is the top left corner of the selected region. |size| is the
  /// size of the selected region. |edit_state_flags| is a combination of flags
  /// that represent the state of the quick menu. Return true if the menu will
  /// be handled and execute |callback| either synchronously or asynchronously
  /// with the selected command ID. Return false to cancel the menu.
  ///
  /*--honey()--*/
  virtual bool RunQuickMenu(HoneycombRefPtr<HoneycombBrowser> browser,
                            HoneycombRefPtr<HoneycombFrame> frame,
                            const HoneycombPoint& location,
                            const HoneycombSize& size,
                            QuickMenuEditStateFlags edit_state_flags,
                            HoneycombRefPtr<HoneycombRunQuickMenuCallback> callback) {
    return false;
  }

  ///
  /// Called to execute a command selected from the quick menu for a windowless
  /// browser. Return true if the command was handled or false for the default
  /// implementation. See honey_menu_id_t for command IDs that have default
  /// implementations.
  ///
  /*--honey()--*/
  virtual bool OnQuickMenuCommand(HoneycombRefPtr<HoneycombBrowser> browser,
                                  HoneycombRefPtr<HoneycombFrame> frame,
                                  int command_id,
                                  EventFlags event_flags) {
    return false;
  }

  ///
  /// Called when the quick menu for a windowless browser is dismissed
  /// irregardless of whether the menu was canceled or a command was selected.
  ///
  /*--honey()--*/
  virtual void OnQuickMenuDismissed(HoneycombRefPtr<HoneycombBrowser> browser,
                                    HoneycombRefPtr<HoneycombFrame> frame) {}
};

///
/// Provides information about the context menu state. The methods of this class
/// can only be accessed on browser process the UI thread.
///
/*--honey(source=library)--*/
class HoneycombContextMenuParams : public virtual HoneycombBaseRefCounted {
 public:
  typedef honey_context_menu_type_flags_t TypeFlags;
  typedef honey_context_menu_media_type_t MediaType;
  typedef honey_context_menu_media_state_flags_t MediaStateFlags;
  typedef honey_context_menu_edit_state_flags_t EditStateFlags;

  ///
  /// Returns the X coordinate of the mouse where the context menu was invoked.
  /// Coords are relative to the associated RenderView's origin.
  ///
  /*--honey()--*/
  virtual int GetXCoord() = 0;

  ///
  /// Returns the Y coordinate of the mouse where the context menu was invoked.
  /// Coords are relative to the associated RenderView's origin.
  ///
  /*--honey()--*/
  virtual int GetYCoord() = 0;

  ///
  /// Returns flags representing the type of node that the context menu was
  /// invoked on.
  ///
  /*--honey(default_retval=CM_TYPEFLAG_NONE)--*/
  virtual TypeFlags GetTypeFlags() = 0;

  ///
  /// Returns the URL of the link, if any, that encloses the node that the
  /// context menu was invoked on.
  ///
  /*--honey()--*/
  virtual HoneycombString GetLinkUrl() = 0;

  ///
  /// Returns the link URL, if any, to be used ONLY for "copy link address". We
  /// don't validate this field in the frontend process.
  ///
  /*--honey()--*/
  virtual HoneycombString GetUnfilteredLinkUrl() = 0;

  ///
  /// Returns the source URL, if any, for the element that the context menu was
  /// invoked on. Example of elements with source URLs are img, audio, and
  /// video.
  ///
  /*--honey()--*/
  virtual HoneycombString GetSourceUrl() = 0;

  ///
  /// Returns true if the context menu was invoked on an image which has
  /// non-empty contents.
  ///
  /*--honey()--*/
  virtual bool HasImageContents() = 0;

  ///
  /// Returns the title text or the alt text if the context menu was invoked on
  /// an image.
  ///
  /*--honey()--*/
  virtual HoneycombString GetTitleText() = 0;

  ///
  /// Returns the URL of the top level page that the context menu was invoked
  /// on.
  ///
  /*--honey()--*/
  virtual HoneycombString GetPageUrl() = 0;

  ///
  /// Returns the URL of the subframe that the context menu was invoked on.
  ///
  /*--honey()--*/
  virtual HoneycombString GetFrameUrl() = 0;

  ///
  /// Returns the character encoding of the subframe that the context menu was
  /// invoked on.
  ///
  /*--honey()--*/
  virtual HoneycombString GetFrameCharset() = 0;

  ///
  /// Returns the type of context node that the context menu was invoked on.
  ///
  /*--honey(default_retval=CM_MEDIATYPE_NONE)--*/
  virtual MediaType GetMediaType() = 0;

  ///
  /// Returns flags representing the actions supported by the media element, if
  /// any, that the context menu was invoked on.
  ///
  /*--honey(default_retval=CM_MEDIAFLAG_NONE)--*/
  virtual MediaStateFlags GetMediaStateFlags() = 0;

  ///
  /// Returns the text of the selection, if any, that the context menu was
  /// invoked on.
  ///
  /*--honey()--*/
  virtual HoneycombString GetSelectionText() = 0;

  ///
  /// Returns the text of the misspelled word, if any, that the context menu was
  /// invoked on.
  ///
  /*--honey()--*/
  virtual HoneycombString GetMisspelledWord() = 0;

  ///
  /// Returns true if suggestions exist, false otherwise. Fills in |suggestions|
  /// from the spell check service for the misspelled word if there is one.
  ///
  /*--honey()--*/
  virtual bool GetDictionarySuggestions(
      std::vector<HoneycombString>& suggestions) = 0;

  ///
  /// Returns true if the context menu was invoked on an editable node.
  ///
  /*--honey()--*/
  virtual bool IsEditable() = 0;

  ///
  /// Returns true if the context menu was invoked on an editable node where
  /// spell-check is enabled.
  ///
  /*--honey()--*/
  virtual bool IsSpellCheckEnabled() = 0;

  ///
  /// Returns flags representing the actions supported by the editable node, if
  /// any, that the context menu was invoked on.
  ///
  /*--honey(default_retval=CM_EDITFLAG_NONE)--*/
  virtual EditStateFlags GetEditStateFlags() = 0;

  ///
  /// Returns true if the context menu contains items specified by the renderer
  /// process.
  ///
  /*--honey()--*/
  virtual bool IsCustomMenu() = 0;
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_CONTEXT_MENU_HANDLER_H_
