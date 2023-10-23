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

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_COMMAND_LINE_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_COMMAND_LINE_H_
#pragma once

#include <map>
#include <vector>
#include "include/honey_base.h"

///
/// Class used to create and/or parse command line arguments. Arguments with
/// "--", "-" and, on Windows, "/" prefixes are considered switches. Switches
/// will always precede any arguments without switch prefixes. Switches can
/// optionally have a value specified using the "=" delimiter (e.g.
/// "-switch=value"). An argument of "--" will terminate switch parsing with all
/// subsequent tokens, regardless of prefix, being interpreted as non-switch
/// arguments. Switch names should be lowercase ASCII and will be converted to
/// such if necessary. Switch values will retain the original case and UTF8
/// encoding. This class can be used before HoneycombInitialize() is called.
///
/*--honey(source=library,no_debugct_check)--*/
class HoneycombCommandLine : public virtual HoneycombBaseRefCounted {
 public:
  typedef std::vector<HoneycombString> ArgumentList;
  typedef std::map<HoneycombString, HoneycombString> SwitchMap;

  ///
  /// Create a new HoneycombCommandLine instance.
  ///
  /*--honey(api_hash_check)--*/
  static HoneycombRefPtr<HoneycombCommandLine> CreateCommandLine();

  ///
  /// Returns the singleton global HoneycombCommandLine object. The returned object
  /// will be read-only.
  ///
  /*--honey(api_hash_check)--*/
  static HoneycombRefPtr<HoneycombCommandLine> GetGlobalCommandLine();

  ///
  /// Returns true if this object is valid. Do not call any other methods if
  /// this function returns false.
  ///
  /*--honey()--*/
  virtual bool IsValid() = 0;

  ///
  /// Returns true if the values of this object are read-only. Some APIs may
  /// expose read-only objects.
  ///
  /*--honey()--*/
  virtual bool IsReadOnly() = 0;

  ///
  /// Returns a writable copy of this object.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombCommandLine> Copy() = 0;

  ///
  /// Initialize the command line with the specified |argc| and |argv| values.
  /// The first argument must be the name of the program. This method is only
  /// supported on non-Windows platforms.
  ///
  /*--honey()--*/
  virtual void InitFromArgv(int argc, const char* const* argv) = 0;

  ///
  /// Initialize the command line with the string returned by calling
  /// GetCommandLineW(). This method is only supported on Windows.
  ///
  /*--honey()--*/
  virtual void InitFromString(const HoneycombString& command_line) = 0;

  ///
  /// Reset the command-line switches and arguments but leave the program
  /// component unchanged.
  ///
  /*--honey()--*/
  virtual void Reset() = 0;

  ///
  /// Retrieve the original command line string as a vector of strings.
  /// The argv array:
  /// `{ program, [(--|-|/)switch[=value]]*, [--], [argument]* }`
  ///
  /*--honey()--*/
  virtual void GetArgv(std::vector<HoneycombString>& argv) = 0;

  ///
  /// Constructs and returns the represented command line string. Use this
  /// method cautiously because quoting behavior is unclear.
  ///
  /*--honey()--*/
  virtual HoneycombString GetCommandLineString() = 0;

  ///
  /// Get the program part of the command line string (the first item).
  ///
  /*--honey()--*/
  virtual HoneycombString GetProgram() = 0;

  ///
  /// Set the program part of the command line string (the first item).
  ///
  /*--honey()--*/
  virtual void SetProgram(const HoneycombString& program) = 0;

  ///
  /// Returns true if the command line has switches.
  ///
  /*--honey()--*/
  virtual bool HasSwitches() = 0;

  ///
  /// Returns true if the command line contains the given switch.
  ///
  /*--honey()--*/
  virtual bool HasSwitch(const HoneycombString& name) = 0;

  ///
  /// Returns the value associated with the given switch. If the switch has no
  /// value or isn't present this method returns the empty string.
  ///
  /*--honey()--*/
  virtual HoneycombString GetSwitchValue(const HoneycombString& name) = 0;

  ///
  /// Returns the map of switch names and values. If a switch has no value an
  /// empty string is returned.
  ///
  /*--honey()--*/
  virtual void GetSwitches(SwitchMap& switches) = 0;

  ///
  /// Add a switch to the end of the command line.
  ///
  /*--honey()--*/
  virtual void AppendSwitch(const HoneycombString& name) = 0;

  ///
  /// Add a switch with the specified value to the end of the command line. If
  /// the switch has no value pass an empty value string.
  ///
  /*--honey()--*/
  virtual void AppendSwitchWithValue(const HoneycombString& name,
                                     const HoneycombString& value) = 0;

  ///
  /// True if there are remaining command line arguments.
  ///
  /*--honey()--*/
  virtual bool HasArguments() = 0;

  ///
  /// Get the remaining command line arguments.
  ///
  /*--honey()--*/
  virtual void GetArguments(ArgumentList& arguments) = 0;

  ///
  /// Add an argument to the end of the command line.
  ///
  /*--honey()--*/
  virtual void AppendArgument(const HoneycombString& argument) = 0;

  ///
  /// Insert a command before the current command.
  /// Common for debuggers, like "valgrind" or "gdb --args".
  ///
  /*--honey()--*/
  virtual void PrependWrapper(const HoneycombString& wrapper) = 0;
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_COMMAND_LINE_H_
