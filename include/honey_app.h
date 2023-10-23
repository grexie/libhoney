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

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_APP_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_APP_H_
#pragma once

#include "include/honey_base.h"
#include "include/honey_browser_process_handler.h"
#include "include/honey_command_line.h"
#include "include/honey_render_process_handler.h"
#include "include/honey_resource_bundle_handler.h"
#include "include/honey_scheme.h"

class HoneycombApp;

///
/// This function should be called from the application entry point function to
/// execute a secondary process. It can be used to run secondary processes from
/// the browser client executable (default behavior) or from a separate
/// executable specified by the honey_settings_t.browser_subprocess_path value. If
/// called for the browser process (identified by no "type" command-line value)
/// it will return immediately with a value of -1. If called for a recognized
/// secondary process it will block until the process should exit and then
/// return the process exit code. The |application| parameter may be empty. The
/// |windows_sandbox_info| parameter is only used on Windows and may be NULL
/// (see honey_sandbox_win.h for details).
///
/*--honey(api_hash_check,optional_param=application,
        optional_param=windows_sandbox_info)--*/
int HoneycombExecuteProcess(const HoneycombMainArgs& args,
                      HoneycombRefPtr<HoneycombApp> application,
                      void* windows_sandbox_info);

///
/// This function should be called on the main application thread to initialize
/// the Honeycomb browser process. The |application| parameter may be empty. A return
/// value of true indicates that it succeeded and false indicates that it
/// failed. The |windows_sandbox_info| parameter is only used on Windows and may
/// be NULL (see honey_sandbox_win.h for details).
///
/*--honey(api_hash_check,optional_param=application,
        optional_param=windows_sandbox_info)--*/
bool HoneycombInitialize(const HoneycombMainArgs& args,
                   const HoneycombSettings& settings,
                   HoneycombRefPtr<HoneycombApp> application,
                   void* windows_sandbox_info);

///
/// This function should be called on the main application thread to shut down
/// the Honeycomb browser process before the application exits.
///
/*--honey()--*/
void HoneycombShutdown();

///
/// Perform a single iteration of Honeycomb message loop processing. This function is
/// provided for cases where the Honeycomb message loop must be integrated into an
/// existing application message loop. Use of this function is not recommended
/// for most users; use either the HoneycombRunMessageLoop() function or
/// honey_settings_t.multi_threaded_message_loop if possible. When using this
/// function care must be taken to balance performance against excessive CPU
/// usage. It is recommended to enable the honey_settings_t.external_message_pump
/// option when using this function so that
/// HoneycombBrowserProcessHandler::OnScheduleMessagePumpWork() callbacks can
/// facilitate the scheduling process. This function should only be called on
/// the main application thread and only if HoneycombInitialize() is called with a
/// honey_settings_t.multi_threaded_message_loop value of false. This function
/// will not block.
///
/*--honey()--*/
void HoneycombDoMessageLoopWork();

///
/// Run the Honeycomb message loop. Use this function instead of an application-
/// provided message loop to get the best balance between performance and CPU
/// usage. This function should only be called on the main application thread
/// and only if HoneycombInitialize() is called with a
/// honey_settings_t.multi_threaded_message_loop value of false. This function
/// will block until a quit message is received by the system.
///
/*--honey()--*/
void HoneycombRunMessageLoop();

///
/// Quit the Honeycomb message loop that was started by calling HoneycombRunMessageLoop().
/// This function should only be called on the main application thread and only
/// if HoneycombRunMessageLoop() was used.
///
/*--honey()--*/
void HoneycombQuitMessageLoop();

///
/// Implement this interface to provide handler implementations. Methods will be
/// called by the process and/or thread indicated.
///
/*--honey(source=client,no_debugct_check)--*/
class HoneycombApp : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Provides an opportunity to view and/or modify command-line arguments
  /// before processing by Honeycomb and Chromium. The |process_type| value will be
  /// empty for the browser process. Do not keep a reference to the
  /// HoneycombCommandLine object passed to this method. The
  /// honey_settings_t.command_line_args_disabled value can be used to start with
  /// an empty command-line object. Any values specified in HoneycombSettings that
  /// equate to command-line arguments will be set before this method is called.
  /// Be cautious when using this method to modify command-line arguments for
  /// non-browser processes as this may result in undefined behavior including
  /// crashes.
  ///
  /*--honey(optional_param=process_type)--*/
  virtual void OnBeforeCommandLineProcessing(
      const HoneycombString& process_type,
      HoneycombRefPtr<HoneycombCommandLine> command_line) {}

  ///
  /// Provides an opportunity to register custom schemes. Do not keep a
  /// reference to the |registrar| object. This method is called on the main
  /// thread for each process and the registered schemes should be the same
  /// across all processes.
  ///
  /*--honey()--*/
  virtual void OnRegisterCustomSchemes(
      HoneycombRawPtr<HoneycombSchemeRegistrar> registrar) {}

  ///
  /// Return the handler for resource bundle events. If
  /// honey_settings_t.pack_loading_disabled is true a handler must be returned.
  /// If no handler is returned resources will be loaded from pack files. This
  /// method is called by the browser and render processes on multiple threads.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombResourceBundleHandler> GetResourceBundleHandler() {
    return nullptr;
  }

  ///
  /// Return the handler for functionality specific to the browser process. This
  /// method is called on multiple threads in the browser process.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombBrowserProcessHandler> GetBrowserProcessHandler() {
    return nullptr;
  }

  ///
  /// Return the handler for functionality specific to the render process. This
  /// method is called on the render process main thread.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombRenderProcessHandler> GetRenderProcessHandler() {
    return nullptr;
  }
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_APP_H_
