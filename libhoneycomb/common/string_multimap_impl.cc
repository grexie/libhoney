// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <map>
#include <vector>

#include "include/internal/honey_string_multimap.h"

#include "base/logging.h"

namespace {

class StringMultimap {
  using Map = std::multimap<HoneycombString, HoneycombString>;
  using value_type = Map::value_type;

 public:
  using const_iterator = Map::const_iterator;

  size_t size() const { return map_ref_.size(); }
  size_t count(const HoneycombString& key) const { return map_.count(key); }
  const value_type operator[](size_t pos) const { return *map_ref_[pos]; }

  std::pair<const_iterator, const_iterator> equal_range(
      const HoneycombString& key) const {
    return map_.equal_range(key);
  }
  void insert(value_type&& value) {
    auto it = map_.insert(std::move(value));  // does not invalidate iterators
    map_ref_.push_back(std::move(it));
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

HONEYCOMB_EXPORT honey_string_multimap_t honey_string_multimap_alloc() {
  return reinterpret_cast<honey_string_multimap_t>(new StringMultimap);
}

HONEYCOMB_EXPORT size_t honey_string_multimap_size(honey_string_multimap_t map) {
  DCHECK(map);
  StringMultimap* impl = reinterpret_cast<StringMultimap*>(map);
  return impl->size();
}

HONEYCOMB_EXPORT size_t honey_string_multimap_find_count(honey_string_multimap_t map,
                                                 const honey_string_t* key) {
  DCHECK(map);
  DCHECK(key);
  StringMultimap* impl = reinterpret_cast<StringMultimap*>(map);
  return impl->count(HoneycombString(key));
}

HONEYCOMB_EXPORT int honey_string_multimap_enumerate(honey_string_multimap_t map,
                                             const honey_string_t* key,
                                             size_t value_index,
                                             honey_string_t* value) {
  DCHECK(map);
  DCHECK(key);
  DCHECK(value);

  StringMultimap* impl = reinterpret_cast<StringMultimap*>(map);
  HoneycombString key_str(key);

  DCHECK_LT(value_index, impl->count(key_str));
  if (value_index >= impl->count(key_str)) {
    return 0;
  }

  std::pair<StringMultimap::const_iterator, StringMultimap::const_iterator>
      range_it = impl->equal_range(key_str);

  size_t count = value_index;
  while (count-- && range_it.first != range_it.second) {
    range_it.first++;
  }

  if (range_it.first == range_it.second) {
    return 0;
  }

  const HoneycombString& val = range_it.first->second;
  return honey_string_set(val.c_str(), val.length(), value, true);
}

HONEYCOMB_EXPORT int honey_string_multimap_key(honey_string_multimap_t map,
                                       size_t index,
                                       honey_string_t* key) {
  DCHECK(map);
  DCHECK(key);
  StringMultimap* impl = reinterpret_cast<StringMultimap*>(map);
  DCHECK_LT(index, impl->size());
  if (index >= impl->size()) {
    return 0;
  }

  const auto& [k, _] = (*impl)[index];
  return honey_string_set(k.c_str(), k.length(), key, true);
}

HONEYCOMB_EXPORT int honey_string_multimap_value(honey_string_multimap_t map,
                                         size_t index,
                                         honey_string_t* value) {
  DCHECK(map);
  DCHECK(value);
  StringMultimap* impl = reinterpret_cast<StringMultimap*>(map);
  DCHECK_LT(index, impl->size());
  if (index >= impl->size()) {
    return 0;
  }

  const auto& [_, v] = (*impl)[index];
  return honey_string_set(v.c_str(), v.length(), value, true);
}

HONEYCOMB_EXPORT int honey_string_multimap_append(honey_string_multimap_t map,
                                          const honey_string_t* key,
                                          const honey_string_t* value) {
  DCHECK(map);
  StringMultimap* impl = reinterpret_cast<StringMultimap*>(map);
  impl->insert(std::make_pair(HoneycombString(key), HoneycombString(value)));
  return 1;
}

HONEYCOMB_EXPORT void honey_string_multimap_clear(honey_string_multimap_t map) {
  DCHECK(map);
  StringMultimap* impl = reinterpret_cast<StringMultimap*>(map);
  impl->clear();
}

HONEYCOMB_EXPORT void honey_string_multimap_free(honey_string_multimap_t map) {
  DCHECK(map);
  StringMultimap* impl = reinterpret_cast<StringMultimap*>(map);
  delete impl;
}
