// Copyright 2017 The Honeycomb Authors.
// Portions copyright 2016 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoneycomb/browser/extensions/value_store/honey_value_store_factory.h"

#include "libhoneycomb/browser/extensions/value_store/honey_value_store.h"

#include "base/containers/contains.h"
#include "base/memory/ptr_util.h"
#include "components/value_store/leveldb_value_store.h"

namespace {

const char kUMAClientName[] = "Honeycomb";

}  // namespace

namespace value_store {

HoneycombValueStoreFactory::HoneycombValueStoreFactory() = default;

HoneycombValueStoreFactory::HoneycombValueStoreFactory(const base::FilePath& db_path)
    : db_path_(db_path) {}

HoneycombValueStoreFactory::~HoneycombValueStoreFactory() = default;

std::unique_ptr<ValueStore> HoneycombValueStoreFactory::CreateValueStore(
    const base::FilePath& directory,
    const std::string& uma_client_name) {
  std::unique_ptr<ValueStore> value_store(CreateStore());
  // This factory is purposely keeping the raw pointers to each ValueStore
  // created. Honeycombs using HoneycombValueStoreFactory must be careful to keep
  // those ValueStore's alive for the duration of their test.
  value_store_map_[directory] = value_store.get();
  return value_store;
}

ValueStore* HoneycombValueStoreFactory::LastCreatedStore() const {
  return last_created_store_;
}

void HoneycombValueStoreFactory::DeleteValueStore(const base::FilePath& directory) {
  value_store_map_.erase(directory);
}

bool HoneycombValueStoreFactory::HasValueStore(const base::FilePath& directory) {
  return base::Contains(value_store_map_, directory);
}

ValueStore* HoneycombValueStoreFactory::GetExisting(
    const base::FilePath& directory) const {
  auto it = value_store_map_.find(directory);
  DCHECK(it != value_store_map_.end());
  return it->second;
}

void HoneycombValueStoreFactory::Reset() {
  last_created_store_ = nullptr;
  value_store_map_.clear();
}

std::unique_ptr<ValueStore> HoneycombValueStoreFactory::CreateStore() {
  std::unique_ptr<ValueStore> store;
  if (db_path_.empty()) {
    store = std::make_unique<HoneycombValueStore>();
  } else {
    store = std::make_unique<LeveldbValueStore>(kUMAClientName, db_path_);
  }
  last_created_store_ = store.get();
  return store;
}

}  // namespace value_store
