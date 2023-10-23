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

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_BROWSER_PROCESS_HANDLER_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_BROWSER_PROCESS_HANDLER_H_
#pragma once

#include "include/honey_base.h"
#include "include/honey_client.h"
#include "include/honey_command_line.h"
#include "include/honey_preference.h"
#include "include/honey_values.h"

///
/// Class used to implement browser process callbacks. The methods of this class
/// will be called on the browser process main thread unless otherwise
/// indicated.
///
/*--honey(source=client,no_debugct_check)--*/
class HoneycombBrowserProcessHandler : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Provides an opportunity to register custom preferences prior to
  /// global and request context initialization.
  ///
  /// If |type| is HONEYCOMB_PREFERENCES_TYPE_GLOBAL the registered preferences can be
  /// accessed via HoneycombPreferenceManager::GetGlobalPreferences after
  /// OnContextInitialized is called. Global preferences are registered a single
  /// time at application startup. See related honey_settings_t.cache_path and
  /// honey_settings_t.persist_user_preferences configuration.
  ///
  /// If |type| is HONEYCOMB_PREFERENCES_TYPE_REQUEST_CONTEXT the preferences can be
  /// accessed via the HoneycombRequestContext after
  /// HoneycombRequestContextHandler::OnRequestContextInitialized is called. Request
  /// context preferences are registered each time a new HoneycombRequestContext is
  /// created. It is intended but not required that all request contexts have
  /// the same registered preferences. See related
  /// honey_request_context_settings_t.cache_path and
  /// honey_request_context_settings_t.persist_user_preferences configuration.
  ///
  /// Do not keep a reference to the |registrar| object. This method is called
  /// on the browser process UI thread.
  ///
  /*--honey()--*/
  virtual void OnRegisterCustomPreferences(
      honey_preferences_type_t type,
      HoneycombRawPtr<HoneycombPreferenceRegistrar> registrar) {}

  ///
  /// Called on the browser process UI thread immediately after the Honeycomb context
  /// has been initialized.
  ///
  /*--honey()--*/
  virtual void OnContextInitialized() {}

  ///
  /// Called before a child process is launched. Will be called on the browser
  /// process UI thread when launching a render process and on the browser
  /// process IO thread when launching a GPU process. Provides an opportunity to
  /// modify the child process command line. Do not keep a reference to
  /// |command_line| outside of this method.
  ///
  /*--honey()--*/
  virtual void OnBeforeChildProcessLaunch(
      HoneycombRefPtr<HoneycombCommandLine> command_line) {}

  ///
  /// Called from any thread when work has been scheduled for the browser
  /// process main (UI) thread. This callback is used in combination with
  /// honey_settings_t.external_message_pump and HoneycombDoMessageLoopWork() in cases
  /// where the Honeycomb message loop must be integrated into an existing application
  /// message loop (see additional comments and warnings on
  /// HoneycombDoMessageLoopWork). This callback should schedule a
  /// HoneycombDoMessageLoopWork() call to happen on the main (UI) thread. |delay_ms|
  /// is the requested delay in milliseconds. If |delay_ms| is <= 0 then the
  /// call should happen reasonably soon. If |delay_ms| is > 0 then the call
  /// should be scheduled to happen after the specified delay and any currently
  /// pending scheduled call should be cancelled.
  ///
  /*--honey()--*/
  virtual void OnScheduleMessagePumpWork(int64_t delay_ms) {}

  ///
  /// Return the default client for use with a newly created browser window. If
  /// null is returned the browser will be unmanaged (no callbacks will be
  /// executed for that browser) and application shutdown will be blocked until
  /// the browser window is closed manually. This method is currently only used
  /// with the chrome runtime.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombClient> GetDefaultClient() { return nullptr; }
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_BROWSER_PROCESS_HANDLER_H_
