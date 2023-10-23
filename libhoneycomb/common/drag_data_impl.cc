// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <string>

#include "base/files/file_path.h"
#include "libhoneycomb/browser/stream_impl.h"
#include "libhoneycomb/common/drag_data_impl.h"

#define CHECK_READONLY_RETURN_VOID()        \
  if (read_only_) {                         \
    DCHECK(false) << "object is read only"; \
    return;                                 \
  }

HoneycombDragDataImpl::HoneycombDragDataImpl(const content::DropData& data)
    : data_(data), read_only_(false) {}

HoneycombDragDataImpl::HoneycombDragDataImpl(const content::DropData& data,
                                 HoneycombRefPtr<HoneycombImage> image,
                                 const HoneycombPoint& image_hotspot)
    : data_(data),
      image_(image),
      image_hotspot_(image_hotspot),
      read_only_(false) {}

HoneycombDragDataImpl::HoneycombDragDataImpl() : read_only_(false) {}

HoneycombRefPtr<HoneycombDragData> HoneycombDragData::Create() {
  return new HoneycombDragDataImpl();
}

HoneycombRefPtr<HoneycombDragData> HoneycombDragDataImpl::Clone() {
  HoneycombDragDataImpl* drag_data = nullptr;
  {
    base::AutoLock lock_scope(lock_);
    drag_data = new HoneycombDragDataImpl(data_, image_, image_hotspot_);
  }
  return drag_data;
}

bool HoneycombDragDataImpl::IsReadOnly() {
  base::AutoLock lock_scope(lock_);
  return read_only_;
}

bool HoneycombDragDataImpl::IsLink() {
  base::AutoLock lock_scope(lock_);
  return (data_.url.is_valid() &&
          data_.file_contents_content_disposition.empty());
}

bool HoneycombDragDataImpl::IsFragment() {
  base::AutoLock lock_scope(lock_);
  return (!data_.url.is_valid() &&
          data_.file_contents_content_disposition.empty() &&
          data_.filenames.empty());
}

bool HoneycombDragDataImpl::IsFile() {
  base::AutoLock lock_scope(lock_);
  return (!data_.file_contents_content_disposition.empty() ||
          !data_.filenames.empty());
}

HoneycombString HoneycombDragDataImpl::GetLinkURL() {
  base::AutoLock lock_scope(lock_);
  return data_.url.spec();
}

HoneycombString HoneycombDragDataImpl::GetLinkTitle() {
  base::AutoLock lock_scope(lock_);
  return data_.url_title;
}

HoneycombString HoneycombDragDataImpl::GetLinkMetadata() {
  base::AutoLock lock_scope(lock_);
  return data_.download_metadata;
}

HoneycombString HoneycombDragDataImpl::GetFragmentText() {
  base::AutoLock lock_scope(lock_);
  return data_.text ? HoneycombString(*data_.text) : HoneycombString();
}

HoneycombString HoneycombDragDataImpl::GetFragmentHtml() {
  base::AutoLock lock_scope(lock_);
  return data_.html ? HoneycombString(*data_.html) : HoneycombString();
}

HoneycombString HoneycombDragDataImpl::GetFragmentBaseURL() {
  base::AutoLock lock_scope(lock_);
  return data_.html_base_url.spec();
}

HoneycombString HoneycombDragDataImpl::GetFileName() {
  base::AutoLock lock_scope(lock_);
  auto filename = data_.GetSafeFilenameForImageFileContents();
  return filename ? HoneycombString(filename->value()) : HoneycombString();
}

size_t HoneycombDragDataImpl::GetFileContents(HoneycombRefPtr<HoneycombStreamWriter> writer) {
  base::AutoLock lock_scope(lock_);
  if (data_.file_contents.empty()) {
    return 0;
  }

  char* data = const_cast<char*>(data_.file_contents.c_str());
  size_t size = data_.file_contents.size();

  if (!writer.get()) {
    return size;
  }

  return writer->Write(data, 1, size);
}

bool HoneycombDragDataImpl::GetFileNames(std::vector<HoneycombString>& names) {
  base::AutoLock lock_scope(lock_);
  if (data_.filenames.empty()) {
    return false;
  }

  std::vector<ui::FileInfo>::const_iterator it = data_.filenames.begin();
  for (; it != data_.filenames.end(); ++it) {
    auto name = it->display_name.value();
    if (name.empty()) {
      name = it->path.BaseName().value();
    }
    names.push_back(name);
  }

  return true;
}

bool HoneycombDragDataImpl::GetFilePaths(std::vector<HoneycombString>& paths) {
  base::AutoLock lock_scope(lock_);
  if (data_.filenames.empty()) {
    return false;
  }

  std::vector<ui::FileInfo>::const_iterator it = data_.filenames.begin();
  for (; it != data_.filenames.end(); ++it) {
    auto path = it->path.value();
    paths.push_back(path);
  }

  return true;
}

void HoneycombDragDataImpl::SetLinkURL(const HoneycombString& url) {
  base::AutoLock lock_scope(lock_);
  CHECK_READONLY_RETURN_VOID();
  data_.url = GURL(url.ToString());
}

void HoneycombDragDataImpl::SetLinkTitle(const HoneycombString& title) {
  base::AutoLock lock_scope(lock_);
  CHECK_READONLY_RETURN_VOID();
  data_.url_title = title.ToString16();
}

void HoneycombDragDataImpl::SetLinkMetadata(const HoneycombString& data) {
  base::AutoLock lock_scope(lock_);
  CHECK_READONLY_RETURN_VOID();
  data_.download_metadata = data.ToString16();
}

void HoneycombDragDataImpl::SetFragmentText(const HoneycombString& text) {
  base::AutoLock lock_scope(lock_);
  CHECK_READONLY_RETURN_VOID();
  data_.text = text.ToString16();
}

void HoneycombDragDataImpl::SetFragmentHtml(const HoneycombString& fragment) {
  base::AutoLock lock_scope(lock_);
  CHECK_READONLY_RETURN_VOID();
  data_.html = fragment.ToString16();
}

void HoneycombDragDataImpl::SetFragmentBaseURL(const HoneycombString& fragment) {
  base::AutoLock lock_scope(lock_);
  CHECK_READONLY_RETURN_VOID();
  data_.html_base_url = GURL(fragment.ToString());
}

void HoneycombDragDataImpl::ResetFileContents() {
  base::AutoLock lock_scope(lock_);
  CHECK_READONLY_RETURN_VOID();
  data_.file_contents.erase();
  data_.file_contents_source_url = GURL();
  data_.file_contents_filename_extension.erase();
  data_.file_contents_content_disposition.erase();
}

void HoneycombDragDataImpl::AddFile(const HoneycombString& path,
                              const HoneycombString& display_name) {
  base::AutoLock lock_scope(lock_);
  CHECK_READONLY_RETURN_VOID();
  data_.filenames.push_back(
      ui::FileInfo(base::FilePath(path), base::FilePath(display_name)));
}

void HoneycombDragDataImpl::ClearFilenames() {
  base::AutoLock lock_scope(lock_);
  data_.filenames.clear();
}

void HoneycombDragDataImpl::SetReadOnly(bool read_only) {
  base::AutoLock lock_scope(lock_);
  if (read_only_ == read_only) {
    return;
  }

  read_only_ = read_only;
}

HoneycombRefPtr<HoneycombImage> HoneycombDragDataImpl::GetImage() {
  base::AutoLock lock_scope(lock_);
  return image_;
}

HoneycombPoint HoneycombDragDataImpl::GetImageHotspot() {
  base::AutoLock lock_scope(lock_);
  return image_hotspot_;
}

bool HoneycombDragDataImpl::HasImage() {
  base::AutoLock lock_scope(lock_);
  return image_ ? true : false;
}
