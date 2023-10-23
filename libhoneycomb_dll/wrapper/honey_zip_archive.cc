// Copyright (c) 2010 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/wrapper/honey_zip_archive.h"

#include <algorithm>
#include <memory>

#include "include/base/honey_logging.h"
#include "include/honey_stream.h"
#include "include/honey_zip_reader.h"
#include "include/wrapper/honey_byte_read_handler.h"

#if defined(OS_LINUX)
#include <wctype.h>
#endif

namespace {

// Convert |str| to lowercase in a Unicode-friendly manner.
HoneycombString ToLower(const HoneycombString& str) {
  std::wstring wstr = str;
  std::transform(wstr.begin(), wstr.end(), wstr.begin(), towlower);
  return wstr;
}

class HoneycombZipFile : public HoneycombZipArchive::File {
 public:
  HoneycombZipFile() : data_size_(0) {}

  HoneycombZipFile(const HoneycombZipFile&) = delete;
  HoneycombZipFile& operator=(const HoneycombZipFile&) = delete;

  bool Initialize(size_t data_size) {
    data_.reset(new (std::nothrow) unsigned char[data_size]);
    if (data_) {
      data_size_ = data_size;
      return true;
    } else {
      DLOG(ERROR) << "Failed to allocate " << data_size << " bytes of memory";
      data_size_ = 0;
      return false;
    }
  }

  virtual const unsigned char* GetData() const override { return data_.get(); }

  virtual size_t GetDataSize() const override { return data_size_; }

  virtual HoneycombRefPtr<HoneycombStreamReader> GetStreamReader() const override {
    HoneycombRefPtr<HoneycombReadHandler> handler(new HoneycombByteReadHandler(
        data_.get(), data_size_, const_cast<HoneycombZipFile*>(this)));
    return HoneycombStreamReader::CreateForHandler(handler);
  }

  unsigned char* data() { return data_.get(); }

 private:
  size_t data_size_;
  std::unique_ptr<unsigned char[]> data_;

  IMPLEMENT_REFCOUNTING(HoneycombZipFile);
};

}  // namespace

// HoneycombZipArchive implementation

HoneycombZipArchive::HoneycombZipArchive() {}

HoneycombZipArchive::~HoneycombZipArchive() {}

size_t HoneycombZipArchive::Load(HoneycombRefPtr<HoneycombStreamReader> stream,
                           const HoneycombString& password,
                           bool overwriteExisting) {
  base::AutoLock lock_scope(lock_);

  HoneycombRefPtr<HoneycombZipReader> reader(HoneycombZipReader::Create(stream));
  if (!reader.get()) {
    return 0;
  }

  if (!reader->MoveToFirstFile()) {
    return 0;
  }

  FileMap::iterator it;
  size_t count = 0;

  do {
    const size_t size = static_cast<size_t>(reader->GetFileSize());
    if (size == 0) {
      // Skip directories and empty files.
      continue;
    }

    if (!reader->OpenFile(password)) {
      break;
    }

    const HoneycombString& name = ToLower(reader->GetFileName());

    it = contents_.find(name);
    if (it != contents_.end()) {
      if (overwriteExisting) {
        contents_.erase(it);
      } else {  // Skip files that already exist.
        continue;
      }
    }

    HoneycombRefPtr<HoneycombZipFile> contents = new HoneycombZipFile();
    if (!contents->Initialize(size)) {
      continue;
    }
    unsigned char* data = contents->data();
    size_t offset = 0;

    // Read the file contents.
    do {
      offset += reader->ReadFile(data + offset, size - offset);
    } while (offset < size && !reader->Eof());

    DCHECK(offset == size);

    reader->CloseFile();
    count++;

    // Add the file to the map.
    contents_.insert(std::make_pair(name, contents.get()));
  } while (reader->MoveToNextFile());

  return count;
}

void HoneycombZipArchive::Clear() {
  base::AutoLock lock_scope(lock_);
  contents_.clear();
}

size_t HoneycombZipArchive::GetFileCount() const {
  base::AutoLock lock_scope(lock_);
  return contents_.size();
}

bool HoneycombZipArchive::HasFile(const HoneycombString& fileName) const {
  base::AutoLock lock_scope(lock_);
  FileMap::const_iterator it = contents_.find(ToLower(fileName));
  return (it != contents_.end());
}

HoneycombRefPtr<HoneycombZipArchive::File> HoneycombZipArchive::GetFile(
    const HoneycombString& fileName) const {
  base::AutoLock lock_scope(lock_);
  FileMap::const_iterator it = contents_.find(ToLower(fileName));
  if (it != contents_.end()) {
    return it->second;
  }
  return nullptr;
}

bool HoneycombZipArchive::RemoveFile(const HoneycombString& fileName) {
  base::AutoLock lock_scope(lock_);
  FileMap::iterator it = contents_.find(ToLower(fileName));
  if (it != contents_.end()) {
    contents_.erase(it);
    return true;
  }
  return false;
}

size_t HoneycombZipArchive::GetFiles(FileMap& map) const {
  base::AutoLock lock_scope(lock_);
  map = contents_;
  return contents_.size();
}
