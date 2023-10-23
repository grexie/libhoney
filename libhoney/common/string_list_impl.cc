// Copyright (c) 2009 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <vector>

#include "include/internal/honey_string_list.h"

#include "base/logging.h"

namespace {
using StringList = std::vector<HoneycombString>;
}  // namespace

HONEYCOMB_EXPORT honey_string_list_t honey_string_list_alloc() {
  return reinterpret_cast<honey_string_list_t>(new StringList);
}

HONEYCOMB_EXPORT size_t honey_string_list_size(honey_string_list_t list) {
  DCHECK(list);
  StringList* impl = reinterpret_cast<StringList*>(list);
  return impl->size();
}

HONEYCOMB_EXPORT int honey_string_list_value(honey_string_list_t list,
                                     size_t index,
                                     honey_string_t* value) {
  DCHECK(list);
  DCHECK(value);
  StringList* impl = reinterpret_cast<StringList*>(list);
  DCHECK_LT(index, impl->size());
  if (index >= impl->size()) {
    return false;
  }
  const HoneycombString& str = (*impl)[index];
  return honey_string_copy(str.c_str(), str.length(), value);
}

HONEYCOMB_EXPORT void honey_string_list_append(honey_string_list_t list,
                                       const honey_string_t* value) {
  DCHECK(list);
  StringList* impl = reinterpret_cast<StringList*>(list);
  impl->push_back(HoneycombString(value));
}

HONEYCOMB_EXPORT void honey_string_list_clear(honey_string_list_t list) {
  DCHECK(list);
  StringList* impl = reinterpret_cast<StringList*>(list);
  impl->clear();
}

HONEYCOMB_EXPORT void honey_string_list_free(honey_string_list_t list) {
  DCHECK(list);
  StringList* impl = reinterpret_cast<StringList*>(list);
  delete impl;
}

HONEYCOMB_EXPORT honey_string_list_t honey_string_list_copy(honey_string_list_t list) {
  DCHECK(list);
  StringList* impl = reinterpret_cast<StringList*>(list);
  return reinterpret_cast<honey_string_list_t>(new StringList(*impl));
}
