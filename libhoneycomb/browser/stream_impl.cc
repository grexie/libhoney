// Copyright (c) 2008 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoneycomb/browser/stream_impl.h"
#include <stdlib.h>
#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/threading/thread_restrictions.h"

// Static functions

HoneycombRefPtr<HoneycombStreamReader> HoneycombStreamReader::CreateForFile(
    const HoneycombString& fileName) {
  DCHECK(!fileName.empty());

  // TODO(honey): Do not allow file IO on all threads (issue #1187).
  base::ScopedAllowBlockingForTesting allow_blocking;

  HoneycombRefPtr<HoneycombStreamReader> reader;
  FILE* file = base::OpenFile(base::FilePath(fileName), "rb");
  if (file) {
    reader = new HoneycombFileReader(file, true);
  }
  return reader;
}

HoneycombRefPtr<HoneycombStreamReader> HoneycombStreamReader::CreateForData(void* data,
                                                          size_t size) {
  DCHECK(data != nullptr);
  DCHECK(size > 0);
  HoneycombRefPtr<HoneycombStreamReader> reader;
  if (data && size > 0) {
    reader = new HoneycombBytesReader(data, size, true);
  }
  return reader;
}

HoneycombRefPtr<HoneycombStreamReader> HoneycombStreamReader::CreateForHandler(
    HoneycombRefPtr<HoneycombReadHandler> handler) {
  DCHECK(handler.get());
  HoneycombRefPtr<HoneycombStreamReader> reader;
  if (handler.get()) {
    reader = new HoneycombHandlerReader(handler);
  }
  return reader;
}

HoneycombRefPtr<HoneycombStreamWriter> HoneycombStreamWriter::CreateForFile(
    const HoneycombString& fileName) {
  DCHECK(!fileName.empty());

  // TODO(honey): Do not allow file IO on all threads (issue #1187).
  base::ScopedAllowBlockingForTesting allow_blocking;

  HoneycombRefPtr<HoneycombStreamWriter> writer;
  FILE* file = base::OpenFile(base::FilePath(fileName), "wb");
  if (file) {
    writer = new HoneycombFileWriter(file, true);
  }
  return writer;
}

HoneycombRefPtr<HoneycombStreamWriter> HoneycombStreamWriter::CreateForHandler(
    HoneycombRefPtr<HoneycombWriteHandler> handler) {
  DCHECK(handler.get());
  HoneycombRefPtr<HoneycombStreamWriter> writer;
  if (handler.get()) {
    writer = new HoneycombHandlerWriter(handler);
  }
  return writer;
}

// HoneycombFileReader

HoneycombFileReader::HoneycombFileReader(FILE* file, bool close)
    : close_(close), file_(file) {}

HoneycombFileReader::~HoneycombFileReader() {
  base::AutoLock lock_scope(lock_);
  if (close_) {
    base::CloseFile(file_);
  }
}

size_t HoneycombFileReader::Read(void* ptr, size_t size, size_t n) {
  base::AutoLock lock_scope(lock_);
  return fread(ptr, size, n, file_);
}

int HoneycombFileReader::Seek(int64_t offset, int whence) {
  base::AutoLock lock_scope(lock_);
#if BUILDFLAG(IS_WIN)
  return _fseeki64(file_, offset, whence);
#else
  return fseek(file_, offset, whence);
#endif
}

int64_t HoneycombFileReader::Tell() {
  base::AutoLock lock_scope(lock_);
#if BUILDFLAG(IS_WIN)
  return _ftelli64(file_);
#else
  return ftell(file_);
#endif
}

int HoneycombFileReader::Eof() {
  base::AutoLock lock_scope(lock_);
  return feof(file_);
}

// HoneycombFileWriter

HoneycombFileWriter::HoneycombFileWriter(FILE* file, bool close)
    : file_(file), close_(close) {}

HoneycombFileWriter::~HoneycombFileWriter() {
  base::AutoLock lock_scope(lock_);
  if (close_) {
    base::CloseFile(file_);
  }
}

size_t HoneycombFileWriter::Write(const void* ptr, size_t size, size_t n) {
  base::AutoLock lock_scope(lock_);
  return (size_t)fwrite(ptr, size, n, file_);
}

int HoneycombFileWriter::Seek(int64_t offset, int whence) {
  base::AutoLock lock_scope(lock_);
  return fseek(file_, offset, whence);
}

int64_t HoneycombFileWriter::Tell() {
  base::AutoLock lock_scope(lock_);
  return ftell(file_);
}

int HoneycombFileWriter::Flush() {
  base::AutoLock lock_scope(lock_);
  return fflush(file_);
}

// HoneycombBytesReader

HoneycombBytesReader::HoneycombBytesReader(void* data, int64_t datasize, bool copy)
    : data_(nullptr), datasize_(0), copy_(false), offset_(0) {
  SetData(data, datasize, copy);
}

HoneycombBytesReader::~HoneycombBytesReader() {
  SetData(nullptr, 0, false);
}

size_t HoneycombBytesReader::Read(void* ptr, size_t size, size_t n) {
  base::AutoLock lock_scope(lock_);
  size_t s = (datasize_ - offset_) / size;
  size_t ret = (n < s ? n : s);
  memcpy(ptr, (reinterpret_cast<char*>(data_)) + offset_, ret * size);
  offset_ += ret * size;
  return ret;
}

int HoneycombBytesReader::Seek(int64_t offset, int whence) {
  int rv = -1L;
  base::AutoLock lock_scope(lock_);
  switch (whence) {
    case SEEK_CUR:
      if (offset_ + offset > datasize_ || offset_ + offset < 0) {
        break;
      }
      offset_ += offset;
      rv = 0;
      break;
    case SEEK_END: {
      int64_t offset_abs = std::abs(offset);
      if (offset_abs > datasize_) {
        break;
      }
      offset_ = datasize_ - offset_abs;
      rv = 0;
      break;
    }
    case SEEK_SET:
      if (offset > datasize_ || offset < 0) {
        break;
      }
      offset_ = offset;
      rv = 0;
      break;
  }

  return rv;
}

int64_t HoneycombBytesReader::Tell() {
  base::AutoLock lock_scope(lock_);
  return offset_;
}

int HoneycombBytesReader::Eof() {
  base::AutoLock lock_scope(lock_);
  return (offset_ >= datasize_);
}

void HoneycombBytesReader::SetData(void* data, int64_t datasize, bool copy) {
  base::AutoLock lock_scope(lock_);
  if (copy_) {
    free(data_);
  }

  copy_ = copy;
  offset_ = 0;
  datasize_ = datasize;

  if (copy) {
    data_ = malloc(datasize);
    DCHECK(data_ != nullptr);
    if (data_) {
      memcpy(data_, data, datasize);
    }
  } else {
    data_ = data;
  }
}

// HoneycombBytesWriter

HoneycombBytesWriter::HoneycombBytesWriter(size_t grow)
    : grow_(grow), datasize_(grow), offset_(0) {
  DCHECK(grow > 0);
  data_ = malloc(grow);
  DCHECK(data_ != nullptr);
}

HoneycombBytesWriter::~HoneycombBytesWriter() {
  base::AutoLock lock_scope(lock_);
  if (data_) {
    free(data_);
  }
}

size_t HoneycombBytesWriter::Write(const void* ptr, size_t size, size_t n) {
  base::AutoLock lock_scope(lock_);
  size_t rv;
  if (offset_ + static_cast<int64_t>(size * n) >= datasize_ &&
      Grow(size * n) == 0) {
    rv = 0;
  } else {
    memcpy(reinterpret_cast<char*>(data_) + offset_, ptr, size * n);
    offset_ += size * n;
    rv = n;
  }

  return rv;
}

int HoneycombBytesWriter::Seek(int64_t offset, int whence) {
  int rv = -1L;
  base::AutoLock lock_scope(lock_);
  switch (whence) {
    case SEEK_CUR:
      if (offset_ + offset > datasize_ || offset_ + offset < 0) {
        break;
      }
      offset_ += offset;
      rv = 0;
      break;
    case SEEK_END: {
      int64_t offset_abs = std::abs(offset);
      if (offset_abs > datasize_) {
        break;
      }
      offset_ = datasize_ - offset_abs;
      rv = 0;
      break;
    }
    case SEEK_SET:
      if (offset > datasize_ || offset < 0) {
        break;
      }
      offset_ = offset;
      rv = 0;
      break;
  }

  return rv;
}

int64_t HoneycombBytesWriter::Tell() {
  base::AutoLock lock_scope(lock_);
  return offset_;
}

int HoneycombBytesWriter::Flush() {
  return 0;
}

std::string HoneycombBytesWriter::GetDataString() {
  base::AutoLock lock_scope(lock_);
  std::string str(reinterpret_cast<char*>(data_), offset_);
  return str;
}

size_t HoneycombBytesWriter::Grow(size_t size) {
  base::AutoLock lock_scope(lock_);
  size_t rv;
  size_t s = (size > grow_ ? size : grow_);
  void* tmp = realloc(data_, datasize_ + s);
  DCHECK(tmp != nullptr);
  if (tmp) {
    data_ = tmp;
    datasize_ += s;
    rv = datasize_;
  } else {
    rv = 0;
  }

  return rv;
}
