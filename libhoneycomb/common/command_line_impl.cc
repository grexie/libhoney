// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoneycomb/common/command_line_impl.h"

#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/strings/string_util.h"

HoneycombCommandLineImpl::HoneycombCommandLineImpl(base::CommandLine* value,
                                       bool will_delete,
                                       bool read_only)
    : HoneycombValueBase<HoneycombCommandLine, base::CommandLine>(
          value,
          nullptr,
          will_delete ? kOwnerWillDelete : kOwnerNoDelete,
          read_only,
          nullptr) {}

bool HoneycombCommandLineImpl::IsValid() {
  return !detached();
}

bool HoneycombCommandLineImpl::IsReadOnly() {
  return read_only();
}

HoneycombRefPtr<HoneycombCommandLine> HoneycombCommandLineImpl::Copy() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, nullptr);
  return new HoneycombCommandLineImpl(new base::CommandLine(const_value().argv()),
                                true, false);
}

void HoneycombCommandLineImpl::InitFromArgv(int argc, const char* const* argv) {
#if !BUILDFLAG(IS_WIN)
  HONEYCOMB_VALUE_VERIFY_RETURN_VOID(true);
  mutable_value()->InitFromArgv(argc, argv);
#else
  DCHECK(false) << "method not supported on this platform";
#endif
}

void HoneycombCommandLineImpl::InitFromString(const HoneycombString& command_line) {
#if BUILDFLAG(IS_WIN)
  HONEYCOMB_VALUE_VERIFY_RETURN_VOID(true);
  const std::wstring& str16 = command_line;
  mutable_value()->ParseFromString(str16);
#else
  DCHECK(false) << "method not supported on this platform";
#endif
}

void HoneycombCommandLineImpl::Reset() {
  HONEYCOMB_VALUE_VERIFY_RETURN_VOID(true);
  base::CommandLine::StringVector argv;
  argv.push_back(mutable_value()->GetProgram().value());
  mutable_value()->InitFromArgv(argv);

  const base::CommandLine::SwitchMap& map = mutable_value()->GetSwitches();
  const_cast<base::CommandLine::SwitchMap*>(&map)->clear();
}

void HoneycombCommandLineImpl::GetArgv(std::vector<HoneycombString>& argv) {
  HONEYCOMB_VALUE_VERIFY_RETURN_VOID(false);
  const base::CommandLine::StringVector& cmd_argv = const_value().argv();
  base::CommandLine::StringVector::const_iterator it = cmd_argv.begin();
  for (; it != cmd_argv.end(); ++it) {
    argv.push_back(*it);
  }
}

HoneycombString HoneycombCommandLineImpl::GetCommandLineString() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());
  return const_value().GetCommandLineString();
}

HoneycombString HoneycombCommandLineImpl::GetProgram() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());
  return const_value().GetProgram().value();
}

void HoneycombCommandLineImpl::SetProgram(const HoneycombString& program) {
  HONEYCOMB_VALUE_VERIFY_RETURN_VOID(true);
  mutable_value()->SetProgram(base::FilePath(program));
}

bool HoneycombCommandLineImpl::HasSwitches() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return (const_value().GetSwitches().size() > 0);
}

bool HoneycombCommandLineImpl::HasSwitch(const HoneycombString& name) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return const_value().HasSwitch(base::ToLowerASCII(name.ToString()));
}

HoneycombString HoneycombCommandLineImpl::GetSwitchValue(const HoneycombString& name) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());
  return const_value().GetSwitchValueNative(
      base::ToLowerASCII(name.ToString()));
}

void HoneycombCommandLineImpl::GetSwitches(SwitchMap& switches) {
  HONEYCOMB_VALUE_VERIFY_RETURN_VOID(false);
  const base::CommandLine::SwitchMap& map = const_value().GetSwitches();
  base::CommandLine::SwitchMap::const_iterator it = map.begin();
  for (; it != map.end(); ++it) {
    switches.insert(std::make_pair(it->first, it->second));
  }
}

void HoneycombCommandLineImpl::AppendSwitch(const HoneycombString& name) {
  HONEYCOMB_VALUE_VERIFY_RETURN_VOID(true);
  mutable_value()->AppendSwitch(name.ToString());
}

void HoneycombCommandLineImpl::AppendSwitchWithValue(const HoneycombString& name,
                                               const HoneycombString& value) {
  HONEYCOMB_VALUE_VERIFY_RETURN_VOID(true);
#if BUILDFLAG(IS_WIN)
  mutable_value()->AppendSwitchNative(name.ToString(), value.ToWString());
#else
  mutable_value()->AppendSwitchNative(name.ToString(), value.ToString());
#endif
}

bool HoneycombCommandLineImpl::HasArguments() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return (const_value().GetArgs().size() > 0);
}

void HoneycombCommandLineImpl::GetArguments(ArgumentList& arguments) {
  HONEYCOMB_VALUE_VERIFY_RETURN_VOID(false);
  const base::CommandLine::StringVector& vec = const_value().GetArgs();
  base::CommandLine::StringVector::const_iterator it = vec.begin();
  for (; it != vec.end(); ++it) {
    arguments.push_back(*it);
  }
}

void HoneycombCommandLineImpl::AppendArgument(const HoneycombString& argument) {
  HONEYCOMB_VALUE_VERIFY_RETURN_VOID(true);
#if BUILDFLAG(IS_WIN)
  mutable_value()->AppendArgNative(argument.ToWString());
#else
  mutable_value()->AppendArgNative(argument.ToString());
#endif
}

void HoneycombCommandLineImpl::PrependWrapper(const HoneycombString& wrapper) {
  HONEYCOMB_VALUE_VERIFY_RETURN_VOID(true);
#if BUILDFLAG(IS_WIN)
  mutable_value()->PrependWrapper(wrapper.ToWString());
#else
  mutable_value()->PrependWrapper(wrapper.ToString());
#endif
}

// HoneycombCommandLine implementation.

// static
HoneycombRefPtr<HoneycombCommandLine> HoneycombCommandLine::CreateCommandLine() {
  return new HoneycombCommandLineImpl(
      new base::CommandLine(base::CommandLine::NO_PROGRAM), true, false);
}

// static
HoneycombRefPtr<HoneycombCommandLine> HoneycombCommandLine::GetGlobalCommandLine() {
  // Uses a singleton reference object.
  static HoneycombRefPtr<HoneycombCommandLineImpl> commandLinePtr;
  if (!commandLinePtr.get()) {
    base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
    if (command_line) {
      commandLinePtr = new HoneycombCommandLineImpl(command_line, false, true);
    }
  }
  return commandLinePtr.get();
}
