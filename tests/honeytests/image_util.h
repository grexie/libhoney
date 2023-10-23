// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBTESTS_IMAGE_UTIL_H_
#define HONEYCOMB_TESTS_HONEYCOMBTESTS_IMAGE_UTIL_H_
#pragma once

#include "include/honey_image.h"

namespace image_util {

// Load an PNG image. Tests that the size is |expected_size| in DIPs. Call
// multiple times to load the same image at different scale factors.
void LoadImage(HoneycombRefPtr<HoneycombImage> image,
               double scale_factor,
               const std::string& name,
               const HoneycombSize& expected_size);

// Load an icon image. Expected size is 16x16 DIPs.
void LoadIconImage(HoneycombRefPtr<HoneycombImage> image,
                   double scale_factor,
                   const std::string& name = "window_icon");

}  // namespace image_util

#endif  // HONEYCOMB_TESTS_HONEYCOMBTESTS_IMAGE_UTIL_H_
