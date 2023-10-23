// Copyright (c) 2009 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <map>
#include <vector>

#include "include/internal/honey_string_map.h"

#include "base/logging.h"

namespace {

class StringMap {
  using Map = std::map<HoneycombString, HoneycombString>;
  using value_type = Map::value_type;

 public:
  using const_iterator = Map::const_iterator;

  size_t size() const { return map_ref_.size(); }
  size_t count(const HoneycombString& key) const { return map_.count(key); }
  const_iterator find(const HoneycombString& value) const { return map_.find(value); }
  const_iterator cend() const { return map_.cend(); }
  const value_type& operator[](size_t pos) const { return *map_ref_[pos]; }

  void insert(value_type&& value) {
    // does not invalidate iterators
    const auto [it, inserted] = map_.insert(std::move(value));
    if (inserted) {
      map_ref_.push_back(std::move(it));
    }
  }
  void clear() {
    map_ref_.clear();
    map_.clear();
  }

 private:
  Map map_;
  std::vector<Map::const_iterator> map_ref_;
};

}  // namespace

HONEYCOMB_EXPORT honey_string_map_t honey_string_map_alloc() {
  return reinterpret_cast<honey_string_map_t>(new StringMap);
}

HONEYCOMB_EXPORT size_t honey_string_map_size(honey_string_map_t map) {
  DCHECK(map);
  StringMap* impl = reinterpret_cast<StringMap*>(map);
  return impl->size();
}

HONEYCOMB_EXPORT int honey_string_map_find(honey_string_map_t map,
                                   const honey_string_t* key,
                                   honey_string_t* value) {
  DCHECK(map);
  DCHECK(value);
  StringMap* impl = reinterpret_cast<StringMap*>(map);
  StringMap::const_iterator it = impl->find(HoneycombString(key));
  if (it == impl->cend()) {
    return 0;
  }

  const HoneycombString& val = it->second;
  return honey_string_set(val.c_str(), val.length(), value, true);
}

HONEYCOMB_EXPORT int honey_string_map_key(honey_string_map_t map,
                                  size_t index,
                                  honey_string_t* key) {
  DCHECK(map);
  DCHECK(key);
  StringMap* impl = reinterpret_cast<StringMap*>(map);
  DCHECK_LT(index, impl->size());
  if (index >= impl->size()) {
    return 0;
  }

  const auto& [k, _] = (*impl)[index];
  return honey_string_set(k.c_str(), k.length(), key, true);
}

HONEYCOMB_EXPORT int honey_string_map_value(honey_string_map_t map,
                                    size_t index,
                                    honey_string_t* value) {
  DCHECK(map);
  DCHECK(value);
  StringMap* impl = reinterpret_cast<StringMap*>(map);
  DCHECK_LT(index, impl->size());
  if (index >= impl->size()) {
    return 0;
  }

  const auto& [_, v] = (*impl)[index];
  return honey_string_set(v.c_str(), v.length(), value, true);
}

HONEYCOMB_EXPORT int honey_string_map_append(honey_string_map_t map,
                                     const honey_string_t* key,
                                     const honey_string_t* value) {
  DCHECK(map);
  StringMap* impl = reinterpret_cast<StringMap*>(map);
  impl->insert(std::make_pair(HoneycombString(key), HoneycombString(value)));
  return 1;
}

HONEYCOMB_EXPORT void honey_string_map_clear(honey_string_map_t map) {
  DCHECK(map);
  StringMap* impl = reinterpret_cast<StringMap*>(map);
  impl->clear();
}

HONEYCOMB_EXPORT void honey_string_map_free(honey_string_map_t map) {
  DCHECK(map);
  StringMap* impl = reinterpret_cast<StringMap*>(map);
  delete impl;
}
