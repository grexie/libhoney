// Copyright (c) 2023 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the Honeycomb translator tool. If making changes by
// hand only do so within the body of existing method and function
// implementations. See the translator.README.txt file in the tools directory
// for more information.
//
// $hash=b6fc182f3444ce3926bff6d2b30d14aeca4cb9ba$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CTOCPP_TEST_TRANSLATOR_TEST_SCOPED_LIBRARY_CHILD_CHILD_CTOCPP_H_
#define HONEYCOMB_LIBHONEY_DLL_CTOCPP_TEST_TRANSLATOR_TEST_SCOPED_LIBRARY_CHILD_CHILD_CTOCPP_H_
#pragma once

#if !defined(WRAPPING_HONEYCOMB_SHARED)
#error This file can be included wrapper-side only
#endif

#include "include/capi/test/honey_translator_test_capi.h"
#include "include/test/honey_translator_test.h"
#include "libhoney_dll/ctocpp/ctocpp_scoped.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class HoneycombTranslatorTestScopedLibraryChildChildCToCpp
    : public HoneycombCToCppScoped<HoneycombTranslatorTestScopedLibraryChildChildCToCpp,
                             HoneycombTranslatorTestScopedLibraryChildChild,
                             honey_translator_test_scoped_library_child_child_t> {
 public:
  HoneycombTranslatorTestScopedLibraryChildChildCToCpp();
  virtual ~HoneycombTranslatorTestScopedLibraryChildChildCToCpp();

  // HoneycombTranslatorTestScopedLibraryChildChild methods.
  int GetOtherOtherValue() override;
  void SetOtherOtherValue(int value) override;

  // HoneycombTranslatorTestScopedLibraryChild methods.
  int GetOtherValue() override;
  void SetOtherValue(int value) override;

  // HoneycombTranslatorTestScopedLibrary methods.
  int GetValue() override;
  void SetValue(int value) override;
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CTOCPP_TEST_TRANSLATOR_TEST_SCOPED_LIBRARY_CHILD_CHILD_CTOCPP_H_
