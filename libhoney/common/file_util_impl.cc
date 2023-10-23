// Copyright (C) 2023 Grexie. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "include/honey_file_util.h"

#include "include/honey_task.h"

#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#include "third_party/zlib/google/zip.h"

namespace {

bool AllowFileIO() {
  if (HoneycombCurrentlyOn(TID_UI) || HoneycombCurrentlyOn(TID_IO)) {
    DCHECK(false) << "file IO is not allowed on the current thread";
    return false;
  }
  return true;
}

}  // namespace

bool HoneycombCreateDirectory(const HoneycombString& full_path) {
  if (!AllowFileIO()) {
    return false;
  }
  return base::CreateDirectory(full_path);
}

bool HoneycombGetTempDirectory(HoneycombString& temp_dir) {
  if (!AllowFileIO()) {
    return false;
  }
  base::FilePath result;
  if (base::GetTempDir(&result)) {
    temp_dir = result.value();
    return true;
  }
  return false;
}

bool HoneycombCreateNewTempDirectory(const HoneycombString& prefix,
                               HoneycombString& new_temp_path) {
  if (!AllowFileIO()) {
    return false;
  }
  base::FilePath result;
  if (base::CreateNewTempDirectory(prefix, &result)) {
    new_temp_path = result.value();
    return true;
  }
  return false;
}

bool HoneycombCreateTempDirectoryInDirectory(const HoneycombString& base_dir,
                                       const HoneycombString& prefix,
                                       HoneycombString& new_dir) {
  if (!AllowFileIO()) {
    return false;
  }
  base::FilePath result;
  if (base::CreateTemporaryDirInDir(base_dir, prefix, &result)) {
    new_dir = result.value();
    return true;
  }
  return false;
}

bool HoneycombDirectoryExists(const HoneycombString& path) {
  if (!AllowFileIO()) {
    return false;
  }
  return base::DirectoryExists(path);
}

bool HoneycombDeleteFile(const HoneycombString& path, bool recursive) {
  if (!AllowFileIO()) {
    return false;
  }
  if (recursive) {
    return base::DeletePathRecursively(path);
  } else {
    return base::DeleteFile(path);
  }
}

bool HoneycombZipDirectory(const HoneycombString& src_dir,
                     const HoneycombString& dest_file,
                     bool include_hidden_files) {
  if (!AllowFileIO()) {
    return false;
  }
  return zip::Zip(src_dir, dest_file, include_hidden_files);
}
