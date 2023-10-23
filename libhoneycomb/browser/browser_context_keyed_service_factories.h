// Copyright 2015 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_BROWSER_CONTEXT_KEYED_SERVICE_FACTORIES_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_BROWSER_CONTEXT_KEYED_SERVICE_FACTORIES_H_

namespace honey {

// Ensures the existence of any BrowserContextKeyedServiceFactory provided by
// the Honeycomb extensions code or otherwise required by Honeycomb. See
// libhoneycomb/common/extensions/api/README.txt for additional details.
void EnsureBrowserContextKeyedServiceFactoriesBuilt();

}  // namespace honey

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_BROWSER_CONTEXT_KEYED_SERVICE_FACTORIES_H_
