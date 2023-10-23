// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_ZIP_READER_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_ZIP_READER_IMPL_H_
#pragma once

#include <sstream>

#include "base/threading/platform_thread.h"
#include "include/honey_zip_reader.h"
#include "third_party/zlib/contrib/minizip/unzip.h"

// Implementation of HoneycombZipReader
class HoneycombZipReaderImpl : public HoneycombZipReader {
 public:
  HoneycombZipReaderImpl();
  ~HoneycombZipReaderImpl() override;

  // Initialize the reader context.
  bool Initialize(HoneycombRefPtr<HoneycombStreamReader> stream);

  bool MoveToFirstFile() override;
  bool MoveToNextFile() override;
  bool MoveToFile(const HoneycombString& fileName, bool caseSensitive) override;
  bool Close() override;
  HoneycombString GetFileName() override;
  int64_t GetFileSize() override;
  HoneycombBaseTime GetFileLastModified() override;
  bool OpenFile(const HoneycombString& password) override;
  bool CloseFile() override;
  int ReadFile(void* buffer, size_t bufferSize) override;
  int64_t Tell() override;
  bool Eof() override;

  bool GetFileInfo();

  // Verify that the reader exists and is being accessed from the correct
  // thread.
  bool VerifyContext();

 protected:
  base::PlatformThreadId supported_thread_id_;
  unzFile reader_;
  bool has_fileopen_;
  bool has_fileinfo_;
  HoneycombString filename_;
  int64_t filesize_;
  time_t filemodified_;

  IMPLEMENT_REFCOUNTING(HoneycombZipReaderImpl);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_ZIP_READER_IMPL_H_
