// Copyright (c) 2017 Marshall A. Greenblatt. All rights reserved.
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

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_EXTENSION_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_EXTENSION_H_
#pragma once

#include "include/honey_base.h"
#include "include/honey_values.h"

class HoneycombExtensionHandler;
class HoneycombRequestContext;

///
/// Object representing an extension. Methods may be called on any thread unless
/// otherwise indicated.
///
/*--honey(source=library)--*/
class HoneycombExtension : public HoneycombBaseRefCounted {
 public:
  ///
  /// Returns the unique extension identifier. This is calculated based on the
  /// extension public key, if available, or on the extension path. See
  /// https://developer.chrome.com/extensions/manifest/key for details.
  ///
  /*--honey()--*/
  virtual HoneycombString GetIdentifier() = 0;

  ///
  /// Returns the absolute path to the extension directory on disk. This value
  /// will be prefixed with PK_DIR_RESOURCES if a relative path was passed to
  /// HoneycombRequestContext::LoadExtension.
  ///
  /*--honey()--*/
  virtual HoneycombString GetPath() = 0;

  ///
  /// Returns the extension manifest contents as a HoneycombDictionaryValue object.
  /// See https://developer.chrome.com/extensions/manifest for details.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombDictionaryValue> GetManifest() = 0;

  ///
  /// Returns true if this object is the same extension as |that| object.
  /// Extensions are considered the same if identifier, path and loader context
  /// match.
  ///
  /*--honey()--*/
  virtual bool IsSame(HoneycombRefPtr<HoneycombExtension> that) = 0;

  ///
  /// Returns the handler for this extension. Will return NULL for internal
  /// extensions or if no handler was passed to
  /// HoneycombRequestContext::LoadExtension.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombExtensionHandler> GetHandler() = 0;

  ///
  /// Returns the request context that loaded this extension. Will return NULL
  /// for internal extensions or if the extension has been unloaded. See the
  /// HoneycombRequestContext::LoadExtension documentation for more information about
  /// loader contexts. Must be called on the browser process UI thread.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombRequestContext> GetLoaderContext() = 0;

  ///
  /// Returns true if this extension is currently loaded. Must be called on the
  /// browser process UI thread.
  ///
  /*--honey()--*/
  virtual bool IsLoaded() = 0;

  ///
  /// Unload this extension if it is not an internal extension and is currently
  /// loaded. Will result in a call to HoneycombExtensionHandler::OnExtensionUnloaded
  /// on success.
  ///
  /*--honey()--*/
  virtual void Unload() = 0;
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_EXTENSION_H_
