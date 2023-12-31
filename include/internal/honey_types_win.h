// Copyright (c) 2009 Marshall A. Greenblatt. All rights reserved.
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

#ifndef HONEYCOMB_INCLUDE_INTERNAL_HONEYCOMB_TYPES_WIN_H_
#define HONEYCOMB_INCLUDE_INTERNAL_HONEYCOMB_TYPES_WIN_H_
#pragma once

#include "include/base/honey_build.h"

#if defined(OS_WIN)
#include <windows.h>

#include "include/internal/honey_string.h"
#include "include/internal/honey_types_geometry.h"

// Handle types.
#define honey_cursor_handle_t HCURSOR
#define honey_event_handle_t MSG*
#define honey_window_handle_t HWND

#define kNullCursorHandle NULL
#define kNullEventHandle NULL
#define kNullWindowHandle NULL

#ifdef __cplusplus
extern "C" {
#endif

///
/// Structure representing HoneycombExecuteProcess arguments.
///
typedef struct _honey_main_args_t {
  HINSTANCE instance;
} honey_main_args_t;

///
/// Structure representing window information.
///
typedef struct _honey_window_info_t {
  // Standard parameters required by CreateWindowEx()
  DWORD ex_style;
  honey_string_t window_name;
  DWORD style;
  honey_rect_t bounds;
  honey_window_handle_t parent_window;
  HMENU menu;

  ///
  /// Set to true (1) to create the browser using windowless (off-screen)
  /// rendering. No window will be created for the browser and all rendering
  /// will occur via the HoneycombRenderHandler interface. The |parent_window| value
  /// will be used to identify monitor info and to act as the parent window for
  /// dialogs, context menus, etc. If |parent_window| is not provided then the
  /// main screen monitor will be used and some functionality that requires a
  /// parent window may not function correctly. In order to create windowless
  /// browsers the HoneycombSettings.windowless_rendering_enabled value must be set to
  /// true. Transparent painting is enabled by default but can be disabled by
  /// setting HoneycombBrowserSettings.background_color to an opaque value.
  ///
  int windowless_rendering_enabled;

  ///
  /// Set to true (1) to enable shared textures for windowless rendering. Only
  /// valid if windowless_rendering_enabled above is also set to true. Currently
  /// only supported on Windows (D3D11).
  ///
  int shared_texture_enabled;

  ///
  /// Set to true (1) to enable the ability to issue BeginFrame requests from
  /// the client application by calling HoneycombBrowserHost::SendExternalBeginFrame.
  ///
  int external_begin_frame_enabled;

  ///
  /// Handle for the new browser window. Only used with windowed rendering.
  ///
  honey_window_handle_t window;
} honey_window_info_t;

#ifdef __cplusplus
}
#endif

#endif  // OS_WIN

#endif  // HONEYCOMB_INCLUDE_INTERNAL_HONEYCOMB_TYPES_WIN_H_
