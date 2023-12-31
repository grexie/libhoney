// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_ORIGIN_WHITELIST_IMPL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_ORIGIN_WHITELIST_IMPL_H_

#include <vector>

#include "third_party/abseil-cpp/absl/types/optional.h"

#include "honey/libhoney/common/mojom/honey.mojom-forward.h"

namespace content {
class RenderProcessHost;
}

namespace url {
class Origin;
}

using CrossOriginWhiteList =
    std::vector<honey::mojom::CrossOriginWhiteListEntryPtr>;

// Called to retrieve the current list of cross-origin white list entries. This
// method is thread safe.
void GetCrossOriginWhitelistEntries(
    absl::optional<CrossOriginWhiteList>* entries);

// Returns true if |source| can access |target| based on the cross-origin white
// list settings.
bool HasCrossOriginWhitelistEntry(const url::Origin& source,
                                  const url::Origin& target);

#endif  // HONEYCOMB_LIBHONEY_BROWSER_ORIGIN_WHITELIST_IMPL_H_
