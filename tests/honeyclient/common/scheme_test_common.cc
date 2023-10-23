// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeyclient/common/scheme_test_common.h"

#include "include/honey_scheme.h"

namespace client {
namespace scheme_test {

void RegisterCustomSchemes(HoneycombRawPtr<HoneycombSchemeRegistrar> registrar) {
  registrar->AddCustomScheme(
      "client", HONEYCOMB_SCHEME_OPTION_STANDARD | HONEYCOMB_SCHEME_OPTION_CORS_ENABLED);
}

}  // namespace scheme_test
}  // namespace client
