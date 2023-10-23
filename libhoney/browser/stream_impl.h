// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_STREAM_IMPL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_STREAM_IMPL_H_
#pragma once

#include "include/honey_stream.h"

#include <stdio.h>
#include <string>

#include "base/synchronization/lock.h"

// Implementation of HoneycombStreamReader for files.
class HoneycombFileReader : public HoneycombStreamReader {
 public:
  HoneycombFileReader(FILE* file, bool close);
  ~HoneycombFileReader() override;

  size_t Read(void* ptr, size_t size, size_t n) override;
  int Seek(int64_t offset, int whence) override;
  int64_t Tell() override;
  int Eof() override;
  bool MayBlock() override { return true; }

 protected:
  bool close_;
  FILE* file_;

  base::Lock lock_;

  IMPLEMENT_REFCOUNTING(HoneycombFileReader);
};

// Implementation of HoneycombStreamWriter for files.
class HoneycombFileWriter : public HoneycombStreamWriter {
 public:
  HoneycombFileWriter(FILE* file, bool close);
  ~HoneycombFileWriter() override;

  size_t Write(const void* ptr, size_t size, size_t n) override;
  int Seek(int64_t offset, int whence) override;
  int64_t Tell() override;
  int Flush() override;
  bool MayBlock() override { return true; }

 protected:
  FILE* file_;
  bool close_;

  base::Lock lock_;

  IMPLEMENT_REFCOUNTING(HoneycombFileWriter);
};

// Implementation of HoneycombStreamReader for byte buffers.
class HoneycombBytesReader : public HoneycombStreamReader {
 public:
  HoneycombBytesReader(void* data, int64_t datasize, bool copy);
  ~HoneycombBytesReader() override;

  size_t Read(void* ptr, size_t size, size_t n) override;
  int Seek(int64_t offset, int whence) override;
  int64_t Tell() override;
  int Eof() override;
  bool MayBlock() override { return false; }

  void SetData(void* data, int64_t datasize, bool copy);

  void* GetData() { return data_; }
  size_t GetDataSize() { return offset_; }

 protected:
  void* data_;
  int64_t datasize_;
  bool copy_;
  int64_t offset_;

  base::Lock lock_;

  IMPLEMENT_REFCOUNTING(HoneycombBytesReader);
};

// Implementation of HoneycombStreamWriter for byte buffers.
class HoneycombBytesWriter : public HoneycombStreamWriter {
 public:
  explicit HoneycombBytesWriter(size_t grow);
  ~HoneycombBytesWriter() override;

  size_t Write(const void* ptr, size_t size, size_t n) override;
  int Seek(int64_t offset, int whence) override;
  int64_t Tell() override;
  int Flush() override;
  bool MayBlock() override { return false; }

  void* GetData() { return data_; }
  int64_t GetDataSize() { return offset_; }
  std::string GetDataString();

 protected:
  size_t Grow(size_t size);

  size_t grow_;
  void* data_;
  int64_t datasize_;
  int64_t offset_;

  base::Lock lock_;

  IMPLEMENT_REFCOUNTING(HoneycombBytesWriter);
};

// Implementation of HoneycombStreamReader for handlers.
class HoneycombHandlerReader : public HoneycombStreamReader {
 public:
  explicit HoneycombHandlerReader(HoneycombRefPtr<HoneycombReadHandler> handler)
      : handler_(handler) {}

  size_t Read(void* ptr, size_t size, size_t n) override {
    return handler_->Read(ptr, size, n);
  }
  int Seek(int64_t offset, int whence) override {
    return handler_->Seek(offset, whence);
  }
  int64_t Tell() override { return handler_->Tell(); }
  int Eof() override { return handler_->Eof(); }
  bool MayBlock() override { return handler_->MayBlock(); }

 protected:
  HoneycombRefPtr<HoneycombReadHandler> handler_;

  IMPLEMENT_REFCOUNTING(HoneycombHandlerReader);
};

// Implementation of HoneycombStreamWriter for handlers.
class HoneycombHandlerWriter : public HoneycombStreamWriter {
 public:
  explicit HoneycombHandlerWriter(HoneycombRefPtr<HoneycombWriteHandler> handler)
      : handler_(handler) {}

  size_t Write(const void* ptr, size_t size, size_t n) override {
    return handler_->Write(ptr, size, n);
  }
  int Seek(int64_t offset, int whence) override {
    return handler_->Seek(offset, whence);
  }
  int64_t Tell() override { return handler_->Tell(); }
  int Flush() override { return handler_->Flush(); }
  bool MayBlock() override { return handler_->MayBlock(); }

 protected:
  HoneycombRefPtr<HoneycombWriteHandler> handler_;

  IMPLEMENT_REFCOUNTING(HoneycombHandlerWriter);
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_STREAM_IMPL_H_
