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
//
// ---------------------------------------------------------------------------
//
// The contents of this file are only available to applications that link
// against the libhoney_dll_wrapper target.
//

#ifndef HONEYCOMB_INCLUDE_WRAPPER_HONEYCOMB_HELPERS_H_
#define HONEYCOMB_INCLUDE_WRAPPER_HONEYCOMB_HELPERS_H_
#pragma once

#include <cstring>
#include <string>
#include <vector>

#include "include/base/honey_bind.h"
#include "include/base/honey_logging.h"
#include "include/honey_task.h"

#define HONEYCOMB_REQUIRE_UI_THREAD() DCHECK(HoneycombCurrentlyOn(TID_UI));
#define HONEYCOMB_REQUIRE_IO_THREAD() DCHECK(HoneycombCurrentlyOn(TID_IO));
#define HONEYCOMB_REQUIRE_FILE_BACKGROUND_THREAD() \
  DCHECK(HoneycombCurrentlyOn(TID_FILE_BACKGROUND));
#define HONEYCOMB_REQUIRE_FILE_USER_VISIBLE_THREAD() \
  DCHECK(HoneycombCurrentlyOn(TID_FILE_USER_VISIBLE));
#define HONEYCOMB_REQUIRE_FILE_USER_BLOCKING_THREAD() \
  DCHECK(HoneycombCurrentlyOn(TID_FILE_USER_BLOCKING));
#define HONEYCOMB_REQUIRE_RENDERER_THREAD() DCHECK(HoneycombCurrentlyOn(TID_RENDERER));

///
/// Use this struct in conjuction with refcounted types to ensure that an
/// object is deleted on the specified thread. For example:
///
/// <pre>
///   class Foo : public base::RefCountedThreadSafe<Foo, HoneycombDeleteOnUIThread> {
///    public:
///     Foo();
///     void DoSomething();
///
///    private:
///     // Allow deletion via scoped_refptr only.
///     friend struct HoneycombDeleteOnThread<TID_UI>;
///     friend class base::RefCountedThreadSafe<Foo, HoneycombDeleteOnUIThread>;
///
///     virtual ~Foo() {}
///   };
///
///   base::scoped_refptr<Foo> foo = new Foo();
///   foo->DoSomething();
///   foo = NULL;  /// Deletion of |foo| will occur on the UI thread.
/// </pre>
///
template <HoneycombThreadId thread>
struct HoneycombDeleteOnThread {
  template <typename T>
  static void Destruct(const T* x) {
    if (HoneycombCurrentlyOn(thread)) {
      delete x;
    } else {
      HoneycombPostTask(thread,
                  base::BindOnce(&HoneycombDeleteOnThread<thread>::Destruct<T>,
                                 base::Unretained(x)));
    }
  }
};

struct HoneycombDeleteOnUIThread : public HoneycombDeleteOnThread<TID_UI> {};
struct HoneycombDeleteOnIOThread : public HoneycombDeleteOnThread<TID_IO> {};
struct HoneycombDeleteOnFileBackgroundThread
    : public HoneycombDeleteOnThread<TID_FILE_BACKGROUND> {};
struct HoneycombDeleteOnFileUserVisibleThread
    : public HoneycombDeleteOnThread<TID_FILE_USER_VISIBLE> {};
struct HoneycombDeleteOnFileUserBlockingThread
    : public HoneycombDeleteOnThread<TID_FILE_USER_BLOCKING> {};
struct HoneycombDeleteOnRendererThread : public HoneycombDeleteOnThread<TID_RENDERER> {};

// Same as IMPLEMENT_REFCOUNTING() but using the specified Destructor.
#define IMPLEMENT_REFCOUNTING_EX(ClassName, Destructor) \
 public:                                                \
  void AddRef() const override {                        \
    ref_count_.AddRef();                                \
  }                                                     \
  bool Release() const override {                       \
    if (ref_count_.Release()) {                         \
      Destructor::Destruct(this);                       \
      return true;                                      \
    }                                                   \
    return false;                                       \
  }                                                     \
  bool HasOneRef() const override {                     \
    return ref_count_.HasOneRef();                      \
  }                                                     \
  bool HasAtLeastOneRef() const override {              \
    return ref_count_.HasAtLeastOneRef();               \
  }                                                     \
                                                        \
 private:                                               \
  HoneycombRefCount ref_count_

#define IMPLEMENT_REFCOUNTING_DELETE_ON_UIT(ClassName) \
  IMPLEMENT_REFCOUNTING_EX(ClassName, HoneycombDeleteOnUIThread)

#define IMPLEMENT_REFCOUNTING_DELETE_ON_IOT(ClassName) \
  IMPLEMENT_REFCOUNTING_EX(ClassName, HoneycombDeleteOnIOThread)

///
/// Helper class to manage a scoped copy of |argv|.
///
class HoneycombScopedArgArray {
 public:
  HoneycombScopedArgArray(int argc, char* argv[]) {
    // argv should have (argc + 1) elements, the last one always being NULL.
    array_ = new char*[argc + 1];
    values_.resize(argc);
    for (int i = 0; i < argc; ++i) {
      values_[i] = argv[i];
      array_[i] = const_cast<char*>(values_[i].c_str());
    }
    array_[argc] = NULL;
  }

  HoneycombScopedArgArray(const HoneycombScopedArgArray&) = delete;
  HoneycombScopedArgArray& operator=(const HoneycombScopedArgArray&) = delete;

  ~HoneycombScopedArgArray() { delete[] array_; }

  char** array() const { return array_; }

 private:
  char** array_;

  // Keep values in a vector separate from |array_| because various users may
  // modify |array_| and we still want to clean up memory properly.
  std::vector<std::string> values_;
};

#endif  // HONEYCOMB_INCLUDE_WRAPPER_HONEYCOMB_HELPERS_H_
