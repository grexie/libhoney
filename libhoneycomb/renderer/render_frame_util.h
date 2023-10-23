// Copyright (c) 2012 The Honeycomb Authors.
// Portions copyright (c) 2011 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_RENDERER_RENDER_FRAME_UTIL_H_
#define HONEYCOMB_LIBHONEYCOMB_RENDERER_RENDER_FRAME_UTIL_H_

#include <stdint.h>

#include <string>

namespace blink {
class WebLocalFrame;
}

namespace render_frame_util {

int64_t GetIdentifier(blink::WebLocalFrame* frame);
std::string GetName(blink::WebLocalFrame* frame);

}  // namespace render_frame_util

#endif  // HONEYCOMB_LIBHONEYCOMB_RENDERER_RENDER_FRAME_UTIL_H_
