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

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_CLIENT_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_CLIENT_H_
#pragma once

#include "include/honey_audio_handler.h"
#include "include/honey_base.h"
#include "include/honey_command_handler.h"
#include "include/honey_context_menu_handler.h"
#include "include/honey_dialog_handler.h"
#include "include/honey_display_handler.h"
#include "include/honey_download_handler.h"
#include "include/honey_drag_handler.h"
#include "include/honey_find_handler.h"
#include "include/honey_focus_handler.h"
#include "include/honey_frame_handler.h"
#include "include/honey_jsdialog_handler.h"
#include "include/honey_keyboard_handler.h"
#include "include/honey_life_span_handler.h"
#include "include/honey_load_handler.h"
#include "include/honey_permission_handler.h"
#include "include/honey_print_handler.h"
#include "include/honey_process_message.h"
#include "include/honey_render_handler.h"
#include "include/honey_request_handler.h"

///
/// Implement this interface to provide handler implementations.
///
/*--honey(source=client,no_debugct_check)--*/
class HoneycombClient : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Return the handler for audio rendering events.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombAudioHandler> GetAudioHandler() { return nullptr; }

  ///
  /// Return the handler for commands. If no handler is provided the default
  /// implementation will be used.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombCommandHandler> GetCommandHandler() { return nullptr; }

  ///
  /// Return the handler for context menus. If no handler is provided the
  /// default implementation will be used.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombContextMenuHandler> GetContextMenuHandler() {
    return nullptr;
  }

  ///
  /// Return the handler for dialogs. If no handler is provided the default
  /// implementation will be used.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombDialogHandler> GetDialogHandler() { return nullptr; }

  ///
  /// Return the handler for browser display state events.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombDisplayHandler> GetDisplayHandler() { return nullptr; }

  ///
  /// Return the handler for download events. If no handler is returned
  /// downloads will not be allowed.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombDownloadHandler> GetDownloadHandler() { return nullptr; }

  ///
  /// Return the handler for drag events.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombDragHandler> GetDragHandler() { return nullptr; }

  ///
  /// Return the handler for find result events.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombFindHandler> GetFindHandler() { return nullptr; }

  ///
  /// Return the handler for focus events.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombFocusHandler> GetFocusHandler() { return nullptr; }

  ///
  /// Return the handler for events related to HoneycombFrame lifespan. This method
  /// will be called once during HoneycombBrowser creation and the result will be
  /// cached for performance reasons.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombFrameHandler> GetFrameHandler() { return nullptr; }

  ///
  /// Return the handler for permission requests.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombPermissionHandler> GetPermissionHandler() {
    return nullptr;
  }

  ///
  /// Return the handler for JavaScript dialogs. If no handler is provided the
  /// default implementation will be used.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombJSDialogHandler> GetJSDialogHandler() { return nullptr; }

  ///
  /// Return the handler for keyboard events.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombKeyboardHandler> GetKeyboardHandler() { return nullptr; }

  ///
  /// Return the handler for browser life span events.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombLifeSpanHandler> GetLifeSpanHandler() { return nullptr; }

  ///
  /// Return the handler for browser load status events.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombLoadHandler> GetLoadHandler() { return nullptr; }

  ///
  /// Return the handler for printing on Linux. If a print handler is not
  /// provided then printing will not be supported on the Linux platform.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombPrintHandler> GetPrintHandler() { return nullptr; }

  ///
  /// Return the handler for off-screen rendering events.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombRenderHandler> GetRenderHandler() { return nullptr; }

  ///
  /// Return the handler for browser request events.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombRequestHandler> GetRequestHandler() { return nullptr; }

  ///
  /// Called when a new message is received from a different process. Return
  /// true if the message was handled or false otherwise.  It is safe to keep a
  /// reference to |message| outside of this callback.
  ///
  /*--honey()--*/
  virtual bool OnProcessMessageReceived(HoneycombRefPtr<HoneycombBrowser> browser,
                                        HoneycombRefPtr<HoneycombFrame> frame,
                                        HoneycombProcessId source_process,
                                        HoneycombRefPtr<HoneycombProcessMessage> message) {
    return false;
  }
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_CLIENT_H_
