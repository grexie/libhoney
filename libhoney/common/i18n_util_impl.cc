// Copyright 2021 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "include/honey_i18n_util.h"

#include "base/i18n/rtl.h"

bool HoneycombIsRTL() {
  return base::i18n::IsRTL();
}
