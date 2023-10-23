// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_COMMON_DRAG_DATA_IMPL_H_
#define HONEYCOMB_LIBHONEY_COMMON_DRAG_DATA_IMPL_H_
#pragma once

#include "include/honey_drag_data.h"
#include "include/honey_image.h"

#include <vector>

#include "base/synchronization/lock.h"
#include "content/public/common/drop_data.h"

// Implementation of HoneycombDragData.
class HoneycombDragDataImpl : public HoneycombDragData {
 public:
  HoneycombDragDataImpl();
  explicit HoneycombDragDataImpl(const content::DropData& data);
  HoneycombDragDataImpl(const content::DropData& data,
                  HoneycombRefPtr<HoneycombImage> image,
                  const HoneycombPoint& image_hotspot);

  HoneycombRefPtr<HoneycombDragData> Clone() override;
  bool IsReadOnly() override;
  bool IsLink() override;
  bool IsFragment() override;
  bool IsFile() override;
  HoneycombString GetLinkURL() override;
  HoneycombString GetLinkTitle() override;
  HoneycombString GetLinkMetadata() override;
  HoneycombString GetFragmentText() override;
  HoneycombString GetFragmentHtml() override;
  HoneycombString GetFragmentBaseURL() override;
  HoneycombString GetFileName() override;
  size_t GetFileContents(HoneycombRefPtr<HoneycombStreamWriter> writer) override;
  bool GetFileNames(std::vector<HoneycombString>& names) override;
  bool GetFilePaths(std::vector<HoneycombString>& paths) override;
  void SetLinkURL(const HoneycombString& url) override;
  void SetLinkTitle(const HoneycombString& title) override;
  void SetLinkMetadata(const HoneycombString& data) override;
  void SetFragmentText(const HoneycombString& text) override;
  void SetFragmentHtml(const HoneycombString& fragment) override;
  void SetFragmentBaseURL(const HoneycombString& fragment) override;
  void ResetFileContents() override;
  void AddFile(const HoneycombString& path, const HoneycombString& display_name) override;
  void ClearFilenames() override;
  HoneycombRefPtr<HoneycombImage> GetImage() override;
  HoneycombPoint GetImageHotspot() override;
  bool HasImage() override;

  // This method is not safe. Use Lock/Unlock to get mutually exclusive access.
  content::DropData* drop_data() { return &data_; }

  void SetReadOnly(bool read_only);

  base::Lock& lock() { return lock_; }

 private:
  content::DropData data_;
  HoneycombRefPtr<HoneycombImage> image_;
  HoneycombPoint image_hotspot_;

  // True if this object is read-only.
  bool read_only_;

  base::Lock lock_;

  IMPLEMENT_REFCOUNTING(HoneycombDragDataImpl);
};

#endif  // HONEYCOMB_LIBHONEY_COMMON_DRAG_DATA_IMPL_H_
