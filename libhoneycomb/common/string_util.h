// Copyright 2021 The Honeycomb Authors. Portions copyright
// 2011 The Chromium Authors and 2016 the CEF Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_COMMON_STRING_UTIL_H_
#define HONEYCOMB_LIBHONEYCOMB_COMMON_STRING_UTIL_H_
#pragma once

#include "include/honey_base.h"

#include "base/functional/callback.h"
#include "base/memory/scoped_refptr.h"

namespace base {
class ReadOnlySharedMemoryRegion;
class RefCountedMemory;
}  // namespace base

namespace blink {
class WebString;
}

namespace string_util {

// Convert |source| to |honey_string|, avoiding UTF conversions if possible.
void GetHoneycombString(const blink::WebString& source, HoneycombString& honey_string);
void GetHoneycombString(scoped_refptr<base::RefCountedMemory> source,
                  HoneycombString& honey_string);

// Read |source| into shared memory, avoiding UTF conversions if possible.
// Use ExecuteWithScopedHoneycombString() to retrieve the value on the receiving end
// with zero UTF conversions and zero copies if possible.
base::ReadOnlySharedMemoryRegion CreateSharedMemoryRegion(
    const blink::WebString& source);

using ScopedHoneycombStringCallback = base::OnceCallback<void(const HoneycombString&)>;

// Helper for executing |callback| with |region| as a scoped HoneycombString.
void ExecuteWithScopedHoneycombString(base::ReadOnlySharedMemoryRegion region,
                                ScopedHoneycombStringCallback callback);

}  // namespace string_util

#endif  // HONEYCOMB_LIBHONEYCOMB_COMMON_STRING_UTIL_H_
