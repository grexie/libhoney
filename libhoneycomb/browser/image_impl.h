// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_IMAGE_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_IMAGE_IMPL_H_
#pragma once

#include "include/honey_image.h"
#include "libhoneycomb/browser/thread_util.h"

#include "third_party/skia/include/core/SkBitmap.h"
#include "ui/gfx/image/image.h"

class HoneycombImageImpl : public HoneycombImage {
 public:
  // Creates an empty image with no representations.
  HoneycombImageImpl() = default;

  // Creates a new image by copying the ImageSkia for use as the default
  // representation.
  explicit HoneycombImageImpl(const gfx::ImageSkia& image_skia);

  HoneycombImageImpl(const HoneycombImageImpl&) = delete;
  HoneycombImageImpl& operator=(const HoneycombImageImpl&) = delete;

  // Deletes the image and, if the only owner of the storage, all of its cached
  // representations.
  ~HoneycombImageImpl() override = default;

  // HoneycombImage methods:
  bool IsEmpty() override;
  bool IsSame(HoneycombRefPtr<HoneycombImage> that) override;
  bool AddBitmap(float scale_factor,
                 int pixel_width,
                 int pixel_height,
                 honey_color_type_t color_type,
                 honey_alpha_type_t alpha_type,
                 const void* pixel_data,
                 size_t pixel_data_size) override;
  bool AddPNG(float scale_factor,
              const void* png_data,
              size_t png_data_size) override;
  bool AddJPEG(float scale_factor,
               const void* jpeg_data,
               size_t jpeg_data_size) override;
  size_t GetWidth() override;
  size_t GetHeight() override;
  bool HasRepresentation(float scale_factor) override;
  bool RemoveRepresentation(float scale_factor) override;
  bool GetRepresentationInfo(float scale_factor,
                             float& actual_scale_factor,
                             int& pixel_width,
                             int& pixel_height) override;
  HoneycombRefPtr<HoneycombBinaryValue> GetAsBitmap(float scale_factor,
                                        honey_color_type_t color_type,
                                        honey_alpha_type_t alpha_type,
                                        int& pixel_width,
                                        int& pixel_height) override;
  HoneycombRefPtr<HoneycombBinaryValue> GetAsPNG(float scale_factor,
                                     bool with_transparency,
                                     int& pixel_width,
                                     int& pixel_height) override;
  HoneycombRefPtr<HoneycombBinaryValue> GetAsJPEG(float scale_factor,
                                      int quality,
                                      int& pixel_width,
                                      int& pixel_height) override;

  // Add |bitmaps| which should be the same image at different scale factors.
  // |scale_1x_size| is the size in pixels of the 1x factor image. If
  // |scale_1x_size| is 0 the smallest image size in pixels will be used as the
  // 1x factor size.
  void AddBitmaps(int32_t scale_1x_size, const std::vector<SkBitmap>& bitmaps);

  // Return a representation of this Image that contains only the bitmap nearest
  // |scale_factor| as the 1x scale representation. Conceptually this is an
  // incorrect representation but is necessary to work around bugs in the views
  // architecture.
  // TODO(honey): Remove once https://crbug.com/597732 is resolved.
  gfx::ImageSkia GetForced1xScaleRepresentation(float scale_factor) const;

  // Returns the skia representation of this Image.
  gfx::ImageSkia AsImageSkia() const;

 private:
  // Add a bitmap.
  bool AddBitmap(float scale_factor, const SkBitmap& bitmap);

  // Returns the bitmap that most closely matches |scale_factor| or nullptr if
  // one doesn't exist.
  const SkBitmap* GetBitmap(float scale_factor) const;

  // Convert |src_bitmap| to |target_bitmap| with |target_ct| and |target_at|.
  static bool ConvertBitmap(const SkBitmap& src_bitmap,
                            SkBitmap* target_bitmap,
                            SkColorType target_ct,
                            SkAlphaType target_at);

  // The |bitmap| argument will be RGBA or BGRA and either opaque or transparent
  // with post-multiplied alpha. Writes the compressed output into |compressed|.
  using CompressionMethod =
      base::OnceCallback<bool(const SkBitmap& /*bitmap*/,
                              std::vector<unsigned char>* /*compressed*/)>;

  // Write |bitmap| into |compressed| using |method|.
  static bool WriteCompressedFormat(const SkBitmap& bitmap,
                                    std::vector<unsigned char>* compressed,
                                    CompressionMethod method);

  // Write |bitmap| into |compressed| using PNG encoding.
  static bool WritePNG(const SkBitmap& bitmap,
                       std::vector<unsigned char>* compressed,
                       bool with_transparency);

  // Write |bitmap| into |compressed| using JPEG encoding. The alpha channel
  // will be ignored.
  static bool WriteJPEG(const SkBitmap& bitmap,
                        std::vector<unsigned char>* compressed,
                        int quality);

  mutable base::Lock lock_;

  // Access to |image_| must be protected by |lock_|.
  gfx::Image image_;

  IMPLEMENT_REFCOUNTING_DELETE_ON_UIT(HoneycombImageImpl);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_IMAGE_IMPL_H_
