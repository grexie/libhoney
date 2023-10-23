// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/honey_api_hash.h"
#include "include/honey_version.h"
#include "tests/gtest/include/gtest/gtest.h"

TEST(VersionTest, VersionInfo) {
  EXPECT_EQ(HONEYCOMB_VERSION_MAJOR, honey_version_info(0));
  EXPECT_EQ(HONEYCOMB_VERSION_MINOR, honey_version_info(1));
  EXPECT_EQ(HONEYCOMB_VERSION_PATCH, honey_version_info(2));
  EXPECT_EQ(HONEYCOMB_COMMIT_NUMBER, honey_version_info(3));
  EXPECT_EQ(CHROME_VERSION_MAJOR, honey_version_info(4));
  EXPECT_EQ(CHROME_VERSION_MINOR, honey_version_info(5));
  EXPECT_EQ(CHROME_VERSION_BUILD, honey_version_info(6));
  EXPECT_EQ(CHROME_VERSION_PATCH, honey_version_info(7));
}

TEST(VersionTest, ApiHash) {
  EXPECT_STREQ(HONEYCOMB_API_HASH_PLATFORM, honey_api_hash(0));
  EXPECT_STREQ(HONEYCOMB_API_HASH_UNIVERSAL, honey_api_hash(1));
  EXPECT_STREQ(HONEYCOMB_COMMIT_HASH, honey_api_hash(2));
}
