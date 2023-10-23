// Copyright (c) 2009 The Honeycomb Authors. All rights
// reserved.
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

#ifndef HONEYCOMB_INCLUDE_INTERNAL_HONEYCOMB_EXPORT_H_
#define HONEYCOMB_INCLUDE_INTERNAL_HONEYCOMB_EXPORT_H_
#pragma once

#include "include/base/honey_build.h"

#if defined(COMPILER_MSVC)

#ifdef BUILDING_HONEYCOMB_SHARED
#define HONEYCOMB_EXPORT __declspec(dllexport)
#elif USING_HONEYCOMB_SHARED
#define HONEYCOMB_EXPORT __declspec(dllimport)
#else
#define HONEYCOMB_EXPORT
#endif

#elif defined(COMPILER_GCC)

#define HONEYCOMB_EXPORT __attribute__((visibility("default")))

#endif  // COMPILER_GCC

#if defined(OS_WIN)
#define HONEYCOMB_CALLBACK __stdcall
#else
#define HONEYCOMB_CALLBACK
#endif

#endif  // HONEYCOMB_INCLUDE_INTERNAL_HONEYCOMB_EXPORT_H_
