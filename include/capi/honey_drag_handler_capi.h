// Copyright (c) 2023 Marshall A. Greenblatt. All rights reserved.
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
// This file was generated by the Honeycomb translator tool and should not edited
// by hand. See the translator.README.txt file in the tools directory for
// more information.
//
// $hash=ad16b0f4320d7b363efb152a65e3ce142882b9d9$
//

#ifndef HONEYCOMB_INCLUDE_CAPI_HONEYCOMB_DRAG_HANDLER_CAPI_H_
#define HONEYCOMB_INCLUDE_CAPI_HONEYCOMB_DRAG_HANDLER_CAPI_H_
#pragma once

#include "include/capi/honey_base_capi.h"
#include "include/capi/honey_browser_capi.h"
#include "include/capi/honey_drag_data_capi.h"
#include "include/capi/honey_frame_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

///
/// Implement this structure to handle events related to dragging. The functions
/// of this structure will be called on the UI thread.
///
typedef struct _honey_drag_handler_t {
  ///
  /// Base structure.
  ///
  honey_base_ref_counted_t base;

  ///
  /// Called when an external drag event enters the browser window. |dragData|
  /// contains the drag event data and |mask| represents the type of drag
  /// operation. Return false (0) for default drag handling behavior or true (1)
  /// to cancel the drag event.
  ///
  int(HONEYCOMB_CALLBACK* on_drag_enter)(struct _honey_drag_handler_t* self,
                                   struct _honey_browser_t* browser,
                                   struct _honey_drag_data_t* dragData,
                                   honey_drag_operations_mask_t mask);

  ///
  /// Called whenever draggable regions for the browser window change. These can
  /// be specified using the '-webkit-app-region: drag/no-drag' CSS-property. If
  /// draggable regions are never defined in a document this function will also
  /// never be called. If the last draggable region is removed from a document
  /// this function will be called with an NULL vector.
  ///
  void(HONEYCOMB_CALLBACK* on_draggable_regions_changed)(
      struct _honey_drag_handler_t* self,
      struct _honey_browser_t* browser,
      struct _honey_frame_t* frame,
      size_t regionsCount,
      honey_draggable_region_t const* regions);
} honey_drag_handler_t;

#ifdef __cplusplus
}
#endif

#endif  // HONEYCOMB_INCLUDE_CAPI_HONEYCOMB_DRAG_HANDLER_CAPI_H_
