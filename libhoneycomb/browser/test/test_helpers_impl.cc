// Copyright (c) 2022 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/test/honey_test_helpers.h"

#include "base/files/file_path.h"
#include "base/path_service.h"

void HoneycombSetDataDirectoryForTests(const HoneycombString& dir) {
  base::PathService::OverrideAndCreateIfNeeded(
      base::DIR_SRC_TEST_DATA_ROOT, base::FilePath(dir), /*is_absolute=*/true,
      /*create=*/false);
}
