// Copyright 2021 The Honeycomb Authors. Portions copyright
// 2011 The Chromium Authors and 2016 the CEF Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#include "libhoney/common/string_util.h"

#include "base/memory/read_only_shared_memory_region.h"
#include "base/memory/ref_counted_memory.h"
#include "third_party/blink/public/platform/web_string.h"

namespace string_util {

void GetHoneycombString(const blink::WebString& source, HoneycombString& honey_string) {
#if defined(HONEYCOMB_STRING_TYPE_UTF8)
  honey_string.FromString(source.Utf8());
#else
  honey_string.FromString16(source.Utf16());
#endif
}

void GetHoneycombString(scoped_refptr<base::RefCountedMemory> source,
                  HoneycombString& honey_string) {
  if (source && source->size() > 0U) {
#if defined(HONEYCOMB_STRING_TYPE_UTF8) || defined(HONEYCOMB_STRING_TYPE_UTF16)
    // Reference existing UTF8 or UTF16 data.
    honey_string.FromString(source->front_as<HoneycombString::char_type>(),
                          source->size() / sizeof(HoneycombString::char_type),
                          /*copy=*/false);
#else
    // Must convert from UTF16.
    honey_string.FromString16(
        source->front_as<std::u16string::value_type>(),
        source->size() / sizeof(std::u16string::value_type));
#endif
  } else {
    honey_string.clear();
  }
}

base::ReadOnlySharedMemoryRegion CreateSharedMemoryRegion(
    const blink::WebString& source) {
  base::ReadOnlySharedMemoryRegion region;

  if (!source.IsEmpty()) {
#if defined(HONEYCOMB_STRING_TYPE_UTF8)
    const std::string& string = source.Utf8();
    const size_t byte_size = string.length();
#else
    const std::u16string& string = source.Utf16();
    const size_t byte_size =
        string.length() * sizeof(std::u16string::value_type);
#endif
    auto mapped_region = base::ReadOnlySharedMemoryRegion::Create(byte_size);
    if (mapped_region.IsValid()) {
      memcpy(mapped_region.mapping.memory(), string.data(), byte_size);
      region = std::move(mapped_region.region);
    }
  }

  return region;
}

void ExecuteWithScopedHoneycombString(base::ReadOnlySharedMemoryRegion region,
                                ScopedHoneycombStringCallback callback) {
  auto shared_buf =
      base::RefCountedSharedMemoryMapping::CreateFromWholeRegion(region);
  HoneycombString str;
  GetHoneycombString(shared_buf, str);
  std::move(callback).Run(str);
}

}  // namespace string_util