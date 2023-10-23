// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoney/browser/image_impl.h"

#include <algorithm>

#include "skia/ext/skia_utils_base.h"
#include "ui/gfx/codec/jpeg_codec.h"
#include "ui/gfx/codec/png_codec.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/image/image_png_rep.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/gfx/image/image_skia_rep.h"

namespace {

SkColorType GetSkColorType(honey_color_type_t color_type) {
  switch (color_type) {
    case HONEYCOMB_COLOR_TYPE_RGBA_8888:
      return kRGBA_8888_SkColorType;
    case HONEYCOMB_COLOR_TYPE_BGRA_8888:
      return kBGRA_8888_SkColorType;
    default:
      break;
  }

  DCHECK(false);
  return kUnknown_SkColorType;
}

SkAlphaType GetSkAlphaType(honey_alpha_type_t alpha_type) {
  switch (alpha_type) {
    case HONEYCOMB_ALPHA_TYPE_OPAQUE:
      return kOpaque_SkAlphaType;
    case HONEYCOMB_ALPHA_TYPE_PREMULTIPLIED:
      return kPremul_SkAlphaType;
    case HONEYCOMB_ALPHA_TYPE_POSTMULTIPLIED:
      return kUnpremul_SkAlphaType;
    default:
      break;
  }

  DCHECK(false);
  return kUnknown_SkAlphaType;
}

// Compress as PNG. Requires post-multiplied alpha.
bool PNGMethod(bool with_transparency,
               const SkBitmap& bitmap,
               std::vector<unsigned char>* compressed) {
  return gfx::PNGCodec::Encode(
      reinterpret_cast<unsigned char*>(bitmap.getPixels()),
      bitmap.colorType() == kBGRA_8888_SkColorType ? gfx::PNGCodec::FORMAT_BGRA
                                                   : gfx::PNGCodec::FORMAT_RGBA,
      gfx::Size(bitmap.width(), bitmap.height()),
      static_cast<int>(bitmap.rowBytes()),
      bitmap.alphaType() == kOpaque_SkAlphaType || !with_transparency,
      std::vector<gfx::PNGCodec::Comment>(), compressed);
}

// Compress as JPEG. This internally uses JCS_EXT_RGBX or JCS_EXT_BGRX which
// causes the alpha channel to be ignored. Requires post-multiplied alpha.
bool JPEGMethod(int quality,
                const SkBitmap& bitmap,
                std::vector<unsigned char>* compressed) {
  return gfx::JPEGCodec::Encode(bitmap, quality, compressed);
}

}  // namespace

// static
HoneycombRefPtr<HoneycombImage> HoneycombImage::CreateImage() {
  return new HoneycombImageImpl();
}

HoneycombImageImpl::HoneycombImageImpl(const gfx::ImageSkia& image_skia)
    : image_(image_skia) {}

bool HoneycombImageImpl::IsEmpty() {
  base::AutoLock lock_scope(lock_);
  return image_.IsEmpty();
}

bool HoneycombImageImpl::IsSame(HoneycombRefPtr<HoneycombImage> that) {
  HoneycombImageImpl* that_impl = static_cast<HoneycombImageImpl*>(that.get());
  if (!that_impl) {
    return false;
  }

  // Quick check for the same object.
  if (this == that_impl) {
    return true;
  }

  base::AutoLock lock_scope(lock_);
  return image_.AsImageSkia().BackedBySameObjectAs(
      that_impl->image_.AsImageSkia());
}

bool HoneycombImageImpl::AddBitmap(float scale_factor,
                             int pixel_width,
                             int pixel_height,
                             honey_color_type_t color_type,
                             honey_alpha_type_t alpha_type,
                             const void* pixel_data,
                             size_t pixel_data_size) {
  const SkColorType ct = GetSkColorType(color_type);
  const SkAlphaType at = GetSkAlphaType(alpha_type);

  // Make sure the client passed in the expected values.
  if (ct != kBGRA_8888_SkColorType && ct != kRGBA_8888_SkColorType) {
    return false;
  }
  if (pixel_data_size != pixel_width * pixel_height * 4U) {
    return false;
  }

  SkBitmap bitmap;
  if (!bitmap.tryAllocPixels(
          SkImageInfo::Make(pixel_width, pixel_height, ct, at))) {
    return false;
  }

  DCHECK_EQ(pixel_data_size, bitmap.computeByteSize());
  memcpy(bitmap.getPixels(), pixel_data, pixel_data_size);

  return AddBitmap(scale_factor, bitmap);
}

bool HoneycombImageImpl::AddPNG(float scale_factor,
                          const void* png_data,
                          size_t png_data_size) {
  SkBitmap bitmap;
  if (!gfx::PNGCodec::Decode(static_cast<const unsigned char*>(png_data),
                             png_data_size, &bitmap)) {
    return false;
  }

  return AddBitmap(scale_factor, bitmap);
}

bool HoneycombImageImpl::AddJPEG(float scale_factor,
                           const void* jpeg_data,
                           size_t jpeg_data_size) {
  std::unique_ptr<SkBitmap> bitmap(gfx::JPEGCodec::Decode(
      static_cast<const unsigned char*>(jpeg_data), jpeg_data_size));
  if (!bitmap.get()) {
    return false;
  }

  return AddBitmap(scale_factor, *bitmap);
}

size_t HoneycombImageImpl::GetWidth() {
  base::AutoLock lock_scope(lock_);
  return image_.Width();
}

size_t HoneycombImageImpl::GetHeight() {
  base::AutoLock lock_scope(lock_);
  return image_.Height();
}

bool HoneycombImageImpl::HasRepresentation(float scale_factor) {
  base::AutoLock lock_scope(lock_);
  return image_.AsImageSkia().HasRepresentation(scale_factor);
}

bool HoneycombImageImpl::RemoveRepresentation(float scale_factor) {
  base::AutoLock lock_scope(lock_);
  gfx::ImageSkia image_skia = image_.AsImageSkia();
  if (image_skia.HasRepresentation(scale_factor)) {
    image_skia.RemoveRepresentation(scale_factor);
    return true;
  }
  return false;
}

bool HoneycombImageImpl::GetRepresentationInfo(float scale_factor,
                                         float& actual_scale_factor,
                                         int& pixel_width,
                                         int& pixel_height) {
  base::AutoLock lock_scope(lock_);
  gfx::ImageSkia image_skia = image_.AsImageSkia();
  if (image_skia.isNull()) {
    return false;
  }

  const gfx::ImageSkiaRep& rep = image_skia.GetRepresentation(scale_factor);
  if (rep.is_null()) {
    return false;
  }

  actual_scale_factor = rep.scale();
  pixel_width = rep.GetBitmap().width();
  pixel_height = rep.GetBitmap().height();
  return true;
}

HoneycombRefPtr<HoneycombBinaryValue> HoneycombImageImpl::GetAsBitmap(float scale_factor,
                                                    honey_color_type_t color_type,
                                                    honey_alpha_type_t alpha_type,
                                                    int& pixel_width,
                                                    int& pixel_height) {
  const SkColorType desired_ct = GetSkColorType(color_type);
  const SkAlphaType desired_at = GetSkAlphaType(alpha_type);

  base::AutoLock lock_scope(lock_);
  const SkBitmap* bitmap = GetBitmap(scale_factor);
  if (!bitmap) {
    return nullptr;
  }

  DCHECK(bitmap->readyToDraw());

  pixel_width = bitmap->width();
  pixel_height = bitmap->height();

  if (bitmap->colorType() == desired_ct && bitmap->alphaType() == desired_at) {
    // No conversion necessary.
    return HoneycombBinaryValue::Create(bitmap->getPixels(),
                                  bitmap->computeByteSize());
  } else {
    SkBitmap desired_bitmap;
    if (!ConvertBitmap(*bitmap, &desired_bitmap, desired_ct, desired_at)) {
      return nullptr;
    }
    DCHECK(desired_bitmap.readyToDraw());
    return HoneycombBinaryValue::Create(desired_bitmap.getPixels(),
                                  desired_bitmap.computeByteSize());
  }
}

HoneycombRefPtr<HoneycombBinaryValue> HoneycombImageImpl::GetAsPNG(float scale_factor,
                                                 bool with_transparency,
                                                 int& pixel_width,
                                                 int& pixel_height) {
  base::AutoLock lock_scope(lock_);
  const SkBitmap* bitmap = GetBitmap(scale_factor);
  if (!bitmap) {
    return nullptr;
  }

  std::vector<unsigned char> compressed;
  if (!WritePNG(*bitmap, &compressed, with_transparency)) {
    return nullptr;
  }

  pixel_width = bitmap->width();
  pixel_height = bitmap->height();

  return HoneycombBinaryValue::Create(&compressed.front(), compressed.size());
}

HoneycombRefPtr<HoneycombBinaryValue> HoneycombImageImpl::GetAsJPEG(float scale_factor,
                                                  int quality,
                                                  int& pixel_width,
                                                  int& pixel_height) {
  base::AutoLock lock_scope(lock_);
  const SkBitmap* bitmap = GetBitmap(scale_factor);
  if (!bitmap) {
    return nullptr;
  }

  std::vector<unsigned char> compressed;
  if (!WriteJPEG(*bitmap, &compressed, quality)) {
    return nullptr;
  }

  pixel_width = bitmap->width();
  pixel_height = bitmap->height();

  return HoneycombBinaryValue::Create(&compressed.front(), compressed.size());
}

void HoneycombImageImpl::AddBitmaps(int32_t scale_1x_size,
                              const std::vector<SkBitmap>& bitmaps) {
  if (scale_1x_size == 0) {
    // Set the scale 1x size to the smallest bitmap pixel size.
    int32_t min_size = std::numeric_limits<int32_t>::max();
    for (const SkBitmap& bitmap : bitmaps) {
      const int32_t size = std::max(bitmap.width(), bitmap.height());
      if (size < min_size) {
        min_size = size;
      }
    }
    scale_1x_size = min_size;
  }

  for (const SkBitmap& bitmap : bitmaps) {
    const int32_t size = std::max(bitmap.width(), bitmap.height());
    const float scale_factor =
        static_cast<float>(size) / static_cast<float>(scale_1x_size);
    AddBitmap(scale_factor, bitmap);
  }
}

gfx::ImageSkia HoneycombImageImpl::GetForced1xScaleRepresentation(
    float scale_factor) const {
  base::AutoLock lock_scope(lock_);
  if (scale_factor == 1.0f) {
    // We can use the existing image without modification.
    return image_.AsImageSkia();
  }

  const SkBitmap* bitmap = GetBitmap(scale_factor);
  gfx::ImageSkia image_skia;
  if (bitmap) {
    image_skia.AddRepresentation(gfx::ImageSkiaRep(*bitmap, 1.0f));
  }
  return image_skia;
}

gfx::ImageSkia HoneycombImageImpl::AsImageSkia() const {
  base::AutoLock lock_scope(lock_);
  return image_.AsImageSkia();
}

bool HoneycombImageImpl::AddBitmap(float scale_factor, const SkBitmap& bitmap) {
#if DCHECK_IS_ON()
  DCHECK(bitmap.readyToDraw());
#endif
  DCHECK(bitmap.colorType() == kBGRA_8888_SkColorType ||
         bitmap.colorType() == kRGBA_8888_SkColorType);

  // Convert to N32 (e.g. native encoding) format if not already in that format.
  // N32 is expected by the Views framework and this early conversion avoids
  // CHECKs in ImageSkiaRep and eventual conversion to N32 at some later point
  // in the compositing pipeline.
  SkBitmap n32_bitmap;
  if (!skia::SkBitmapToN32OpaqueOrPremul(bitmap, &n32_bitmap)) {
    return false;
  }

  gfx::ImageSkiaRep skia_rep(n32_bitmap, scale_factor);
  base::AutoLock lock_scope(lock_);
  if (image_.IsEmpty()) {
    image_ = gfx::Image(gfx::ImageSkia(skia_rep));
  } else {
    image_.AsImageSkia().AddRepresentation(skia_rep);
  }
  return true;
}

const SkBitmap* HoneycombImageImpl::GetBitmap(float scale_factor) const {
  lock_.AssertAcquired();
  gfx::ImageSkia image_skia = image_.AsImageSkia();
  if (image_skia.isNull()) {
    return nullptr;
  }

  const gfx::ImageSkiaRep& rep = image_skia.GetRepresentation(scale_factor);
  if (rep.is_null()) {
    return nullptr;
  }

  return &rep.GetBitmap();
}

// static
bool HoneycombImageImpl::ConvertBitmap(const SkBitmap& src_bitmap,
                                 SkBitmap* target_bitmap,
                                 SkColorType target_ct,
                                 SkAlphaType target_at) {
  DCHECK(src_bitmap.readyToDraw());
  DCHECK(src_bitmap.colorType() != target_ct ||
         src_bitmap.alphaType() != target_at);
  DCHECK(target_bitmap);

  SkImageInfo target_info = SkImageInfo::Make(
      src_bitmap.width(), src_bitmap.height(), target_ct, target_at);
  if (!target_bitmap->tryAllocPixels(target_info)) {
    return false;
  }

  if (!src_bitmap.readPixels(target_info, target_bitmap->getPixels(),
                             target_bitmap->rowBytes(), 0, 0)) {
    return false;
  }

  DCHECK(target_bitmap->readyToDraw());
  return true;
}

// static
bool HoneycombImageImpl::WriteCompressedFormat(const SkBitmap& bitmap,
                                         std::vector<unsigned char>* compressed,
                                         CompressionMethod method) {
  const SkBitmap* bitmap_ptr = nullptr;
  SkBitmap bitmap_postalpha;
  if (bitmap.alphaType() == kPremul_SkAlphaType) {
    // Compression methods require post-multiplied alpha values.
    if (!ConvertBitmap(bitmap, &bitmap_postalpha, bitmap.colorType(),
                       kUnpremul_SkAlphaType)) {
      return false;
    }
    bitmap_ptr = &bitmap_postalpha;
  } else {
    bitmap_ptr = &bitmap;
  }

  DCHECK(bitmap_ptr->readyToDraw());
  DCHECK(bitmap_ptr->colorType() == kBGRA_8888_SkColorType ||
         bitmap_ptr->colorType() == kRGBA_8888_SkColorType);
  DCHECK(bitmap_ptr->alphaType() == kOpaque_SkAlphaType ||
         bitmap_ptr->alphaType() == kUnpremul_SkAlphaType);

  return std::move(method).Run(*bitmap_ptr, compressed);
}

// static
bool HoneycombImageImpl::WritePNG(const SkBitmap& bitmap,
                            std::vector<unsigned char>* compressed,
                            bool with_transparency) {
  return WriteCompressedFormat(bitmap, compressed,
                               base::BindOnce(PNGMethod, with_transparency));
}

// static
bool HoneycombImageImpl::WriteJPEG(const SkBitmap& bitmap,
                             std::vector<unsigned char>* compressed,
                             int quality) {
  return WriteCompressedFormat(bitmap, compressed,
                               base::BindOnce(JPEGMethod, quality));
}
