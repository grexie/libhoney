// Copyright (c) 2014 Marshall A. Greenblatt. All rights reserved.
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

#ifndef HONEYCOMB_INCLUDE_INTERNAL_HONEYCOMB_THREAD_INTERNAL_H_
#define HONEYCOMB_INCLUDE_INTERNAL_HONEYCOMB_THREAD_INTERNAL_H_
#pragma once

#if defined(OS_WIN)
#include <windows.h>
#elif defined(OS_POSIX)
#include <pthread.h>
#include <unistd.h>
#endif

#include "include/internal/honey_export.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(OS_WIN)
typedef DWORD honey_platform_thread_id_t;
#define kInvalidPlatformThreadId 0U
#elif defined(OS_POSIX)
typedef pid_t honey_platform_thread_id_t;
#define kInvalidPlatformThreadId 0
#endif

///
/// Returns the current platform thread ID.
///
HONEYCOMB_EXPORT honey_platform_thread_id_t honey_get_current_platform_thread_id(void);

#if defined(OS_WIN)
typedef DWORD honey_platform_thread_handle_t;
#define kInvalidPlatformThreadHandle 0U
#elif defined(OS_POSIX)
typedef pthread_t honey_platform_thread_handle_t;
#define kInvalidPlatformThreadHandle 0
#endif

///
/// Returns the current platform thread handle.
///
HONEYCOMB_EXPORT honey_platform_thread_handle_t
honey_get_current_platform_thread_handle(void);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // HONEYCOMB_INCLUDE_INTERNAL_HONEYCOMB_THREAD_INTERNAL_H_
