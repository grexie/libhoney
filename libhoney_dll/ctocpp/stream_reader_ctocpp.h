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
// $hash=f9d6d1b80e4e2d30bddc9eb6e473c1f530186654$
//

#ifndef HONEYCOMB_LIBHONEY_DLL_CTOCPP_STREAM_READER_CTOCPP_H_
#define HONEYCOMB_LIBHONEY_DLL_CTOCPP_STREAM_READER_CTOCPP_H_
#pragma once

#if !defined(WRAPPING_HONEYCOMB_SHARED)
#error This file can be included wrapper-side only
#endif

#include "include/capi/honey_stream_capi.h"
#include "include/honey_stream.h"
#include "libhoney_dll/ctocpp/ctocpp_ref_counted.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class HoneycombStreamReaderCToCpp : public HoneycombCToCppRefCounted<HoneycombStreamReaderCToCpp,
                                                         HoneycombStreamReader,
                                                         honey_stream_reader_t> {
 public:
  HoneycombStreamReaderCToCpp();
  virtual ~HoneycombStreamReaderCToCpp();

  // HoneycombStreamReader methods.
  size_t Read(void* ptr, size_t size, size_t n) override;
  int Seek(int64_t offset, int whence) override;
  int64_t Tell() override;
  int Eof() override;
  bool MayBlock() override;
};

#endif  // HONEYCOMB_LIBHONEY_DLL_CTOCPP_STREAM_READER_CTOCPP_H_