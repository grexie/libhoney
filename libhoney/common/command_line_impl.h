// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_COMMON_COMMAND_LINE_IMPL_H_
#define HONEYCOMB_LIBHONEY_COMMON_COMMAND_LINE_IMPL_H_
#pragma once

#include "include/honey_command_line.h"
#include "libhoney/common/value_base.h"

#include "base/command_line.h"

// HoneycombCommandLine implementation
class HoneycombCommandLineImpl
    : public HoneycombValueBase<HoneycombCommandLine, base::CommandLine> {
 public:
  HoneycombCommandLineImpl(base::CommandLine* value,
                     bool will_delete,
                     bool read_only);

  HoneycombCommandLineImpl(const HoneycombCommandLineImpl&) = delete;
  HoneycombCommandLineImpl& operator=(const HoneycombCommandLineImpl&) = delete;

  // HoneycombCommandLine methods.
  bool IsValid() override;
  bool IsReadOnly() override;
  HoneycombRefPtr<HoneycombCommandLine> Copy() override;
  void InitFromArgv(int argc, const char* const* argv) override;
  void InitFromString(const HoneycombString& command_line) override;
  void Reset() override;
  void GetArgv(std::vector<HoneycombString>& argv) override;
  HoneycombString GetCommandLineString() override;
  HoneycombString GetProgram() override;
  void SetProgram(const HoneycombString& program) override;
  bool HasSwitches() override;
  bool HasSwitch(const HoneycombString& name) override;
  HoneycombString GetSwitchValue(const HoneycombString& name) override;
  void GetSwitches(SwitchMap& switches) override;
  void AppendSwitch(const HoneycombString& name) override;
  void AppendSwitchWithValue(const HoneycombString& name,
                             const HoneycombString& value) override;
  bool HasArguments() override;
  void GetArguments(ArgumentList& arguments) override;
  void AppendArgument(const HoneycombString& argument) override;
  void PrependWrapper(const HoneycombString& wrapper) override;

  // Must hold the controller lock while using this value.
  const base::CommandLine& command_line() { return const_value(); }
};

#endif  // HONEYCOMB_LIBHONEY_COMMON_COMMAND_LINE_IMPL_H_
