// Copyright (c) 2022 Marshall A. Greenblatt. All rights reserved.
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

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_PERMISSION_HANDLER_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_PERMISSION_HANDLER_H_
#pragma once

#include "include/honey_base.h"
#include "include/honey_browser.h"

///
/// Callback interface used for asynchronous continuation of media access
/// permission requests.
///
/*--honey(source=library)--*/
class HoneycombMediaAccessCallback : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Call to allow or deny media access. If this callback was initiated in
  /// response to a getUserMedia (indicated by
  /// HONEYCOMB_MEDIA_PERMISSION_DEVICE_AUDIO_CAPTURE and/or
  /// HONEYCOMB_MEDIA_PERMISSION_DEVICE_VIDEO_CAPTURE being set) then
  /// |allowed_permissions| must match |required_permissions| passed to
  /// OnRequestMediaAccessPermission.
  ///
  /*--honey(capi_name=cont)--*/
  virtual void Continue(uint32_t allowed_permissions) = 0;

  ///
  /// Cancel the media access request.
  ///
  /*--honey()--*/
  virtual void Cancel() = 0;
};

///
/// Callback interface used for asynchronous continuation of permission prompts.
///
/*--honey(source=library)--*/
class HoneycombPermissionPromptCallback : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Complete the permissions request with the specified |result|.
  ///
  /*--honey(capi_name=cont)--*/
  virtual void Continue(honey_permission_request_result_t result) = 0;
};

///
/// Implement this interface to handle events related to permission requests.
/// The methods of this class will be called on the browser process UI thread.
///
/*--honey(source=client)--*/
class HoneycombPermissionHandler : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Called when a page requests permission to access media.
  /// |requesting_origin| is the URL origin requesting permission.
  /// |requested_permissions| is a combination of values from
  /// honey_media_access_permission_types_t that represent the requested
  /// permissions. Return true and call HoneycombMediaAccessCallback methods either in
  /// this method or at a later time to continue or cancel the request. Return
  /// false to proceed with default handling. With the Chrome runtime, default
  /// handling will display the permission request UI. With the Alloy runtime,
  /// default handling will deny the request. This method will not be called if
  /// the "--enable-media-stream" command-line switch is used to grant all
  /// permissions.
  ///
  /*--honey()--*/
  virtual bool OnRequestMediaAccessPermission(
      HoneycombRefPtr<HoneycombBrowser> browser,
      HoneycombRefPtr<HoneycombFrame> frame,
      const HoneycombString& requesting_origin,
      uint32_t requested_permissions,
      HoneycombRefPtr<HoneycombMediaAccessCallback> callback) {
    return false;
  }

  ///
  /// Called when a page should show a permission prompt. |prompt_id| uniquely
  /// identifies the prompt. |requesting_origin| is the URL origin requesting
  /// permission. |requested_permissions| is a combination of values from
  /// honey_permission_request_types_t that represent the requested permissions.
  /// Return true and call HoneycombPermissionPromptCallback::Continue either in this
  /// method or at a later time to continue or cancel the request. Return false
  /// to proceed with default handling. With the Chrome runtime, default
  /// handling will display the permission prompt UI. With the Alloy runtime,
  /// default handling is HONEYCOMB_PERMISSION_RESULT_IGNORE.
  ///
  /*--honey()--*/
  virtual bool OnShowPermissionPrompt(
      HoneycombRefPtr<HoneycombBrowser> browser,
      uint64_t prompt_id,
      const HoneycombString& requesting_origin,
      uint32_t requested_permissions,
      HoneycombRefPtr<HoneycombPermissionPromptCallback> callback) {
    return false;
  }

  ///
  /// Called when a permission prompt handled via OnShowPermissionPrompt is
  /// dismissed. |prompt_id| will match the value that was passed to
  /// OnShowPermissionPrompt. |result| will be the value passed to
  /// HoneycombPermissionPromptCallback::Continue or HONEYCOMB_PERMISSION_RESULT_IGNORE if
  /// the dialog was dismissed for other reasons such as navigation, browser
  /// closure, etc. This method will not be called if OnShowPermissionPrompt
  /// returned false for |prompt_id|.
  ///
  /*--honey()--*/
  virtual void OnDismissPermissionPrompt(
      HoneycombRefPtr<HoneycombBrowser> browser,
      uint64_t prompt_id,
      honey_permission_request_result_t result) {}
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_PERMISSION_HANDLER_H_
