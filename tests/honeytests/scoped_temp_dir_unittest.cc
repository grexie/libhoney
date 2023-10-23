// Copyright (C) 2023 Grexie. Portions copyright
// 2011 The Chromium Authors and 2016 the CEF Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#include <string>

#include "include/honey_file_util.h"
#include "include/wrapper/honey_scoped_temp_dir.h"
#include "tests/gtest/include/gtest/gtest.h"

TEST(ScopedTempDir, FullPath) {
  HoneycombString test_path;
  HoneycombCreateNewTempDirectory("scoped_temp_dir", test_path);

  // Against an existing dir, it should get destroyed when leaving scope.
  EXPECT_TRUE(HoneycombDirectoryExists(test_path));
  {
    HoneycombScopedTempDir dir;
    EXPECT_TRUE(dir.Set(test_path));
    EXPECT_TRUE(dir.IsValid());
  }
  EXPECT_FALSE(HoneycombDirectoryExists(test_path));

  {
    HoneycombScopedTempDir dir;
    EXPECT_TRUE(dir.Set(test_path));
    // Now the dir doesn't exist, so ensure that it gets created.
    EXPECT_TRUE(HoneycombDirectoryExists(test_path));
    // When we call Take(), it shouldn't get destroyed when leaving scope.
    HoneycombString path = dir.Take();
    EXPECT_STREQ(path.ToString().c_str(), test_path.ToString().c_str());
    EXPECT_FALSE(dir.IsValid());
  }
  EXPECT_TRUE(HoneycombDirectoryExists(test_path));

  // Clean up.
  {
    HoneycombScopedTempDir dir;
    EXPECT_TRUE(dir.Set(test_path));
  }
  EXPECT_FALSE(HoneycombDirectoryExists(test_path));
}

TEST(ScopedTempDir, TempDir) {
  // In this case, just verify that a directory was created and that it's a
  // child of TempDir.
  HoneycombString test_path;
  {
    HoneycombScopedTempDir dir;
    EXPECT_TRUE(dir.CreateUniqueTempDir());
    test_path = dir.GetPath();
    EXPECT_TRUE(HoneycombDirectoryExists(test_path));
    HoneycombString tmp_dir;
    EXPECT_TRUE(HoneycombGetTempDirectory(tmp_dir));
    EXPECT_TRUE(test_path.ToString().find(tmp_dir.ToString()) !=
                std::string::npos);
  }
  EXPECT_FALSE(HoneycombDirectoryExists(test_path));
}

TEST(ScopedTempDir, UniqueTempDirUnderPath) {
  // Create a path which will contain a unique temp path.
  HoneycombString base_path;
  ASSERT_TRUE(HoneycombCreateNewTempDirectory("base_dir", base_path));

  HoneycombString test_path;
  {
    HoneycombScopedTempDir dir;
    EXPECT_TRUE(dir.CreateUniqueTempDirUnderPath(base_path));
    test_path = dir.GetPath();
    EXPECT_TRUE(HoneycombDirectoryExists(test_path));
    EXPECT_TRUE(test_path.ToString().find(base_path.ToString()) == 0);
  }
  EXPECT_FALSE(HoneycombDirectoryExists(test_path));
  HoneycombDeleteFile(base_path, true);
}

TEST(ScopedTempDir, MultipleInvocations) {
  HoneycombScopedTempDir dir;
  EXPECT_TRUE(dir.CreateUniqueTempDir());
  EXPECT_FALSE(dir.CreateUniqueTempDir());
  EXPECT_TRUE(dir.Delete());
  EXPECT_TRUE(dir.CreateUniqueTempDir());
  EXPECT_FALSE(dir.CreateUniqueTempDir());
  HoneycombScopedTempDir other_dir;
  EXPECT_TRUE(other_dir.Set(dir.Take()));
  EXPECT_TRUE(dir.CreateUniqueTempDir());
  EXPECT_FALSE(dir.CreateUniqueTempDir());
  EXPECT_FALSE(other_dir.CreateUniqueTempDir());
}
